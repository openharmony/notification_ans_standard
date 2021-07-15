/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "advanced_notification_service.h"

#include <functional>

#include "ans_inner_errors.h"
#include "ans_log_wrapper.h"
#include "bundle_manager_helper.h"
#include "disturb_filter.h"
#include "ipc_skeleton.h"
#include "notification_constant.h"
#include "notification_filter.h"
#include "notification_preferences.h"
#include "notification_slot.h"
#include "notification_slot_filter.h"
#include "notification_subscriber_manager.h"
#include "permission_filter.h"

namespace OHOS {
namespace Notification {

sptr<AdvancedNotificationService> AdvancedNotificationService::instance_;
std::mutex AdvancedNotificationService::instanceMutex_;

static const std::shared_ptr<INotificationFilter> NOTIFICATION_FILTERS[] = {
    std::make_shared<PermissionFilter>(),
    std::make_shared<NotificationSlotFilter>(),
    std::make_shared<DisturbFilter>(),
};

sptr<AdvancedNotificationService> AdvancedNotificationService::GetInstance()
{
    std::lock_guard<std::mutex> lock(instanceMutex_);

    if (instance_ == nullptr) {
        instance_ = new AdvancedNotificationService();
    }
    return instance_;
}

AdvancedNotificationService::AdvancedNotificationService()
{
    runner_ = OHOS::AppExecFwk::EventRunner::Create();
    handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner_);

    StartFilters();
}

AdvancedNotificationService::~AdvancedNotificationService()
{
    StopFilters();
}

ErrCode AdvancedNotificationService::Publish(const std::string &label, const sptr<NotificationRequest> &request)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if ((request->GetSlotType() == NotificationConstant::SlotType::CUSTOM) && !IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;

    std::string bundle = GetClientBundleName();
    request->SetOwnerBundleName(bundle);
    request->SetCreatorBundleName(bundle);

    int32_t uid = IPCSkeleton::GetCallingUid();
    int32_t pid = IPCSkeleton::GetCallingPid();
    request->SetCreatorUid(uid);
    request->SetCreatorPid(pid);

    sptr<Notification> notification = new Notification(request);
    handler_->PostSyncTask(std::bind([this, &label, &notification, &result]() {
        result = Filter(notification);
        if (result != ERR_OK) {
            return;
        }

        if (!IsNotificationExists(notification->GetKey())) {
            AddToNotificationList(notification);
        } else {
            if (notification->request_->IsAlertOneTime()) {
                notification->request_->SetColorEnabled(false);
                notification->SetSound(Uri(std::string()));
                notification->SetVibrationStyle(std::vector<int64_t>());
            }
            UpdateInNotificationList(notification);
        }

        sptr<NotificationSortingMap> sortingMap = GenerateSortingMap();
        NotificationSubscriberManager::GetInstance()->NotifyConsumed(notification, sortingMap);
    }));

    return result;
}

bool AdvancedNotificationService::IsNotificationExists(const std::string &key)
{
    bool isExists = false;

    for (auto item : notificationList_) {
        if (item->GetKey() == key) {
            isExists = true;
            break;
        }
    }

    return isExists;
}

ErrCode AdvancedNotificationService::Filter(const sptr<Notification> &notification)
{
    ErrCode result = ERR_OK;

    for (auto filter : NOTIFICATION_FILTERS) {
        result = filter->OnPublish(notification);
        if (result != ERR_OK) {
            break;
        }
    }

    return result;
}

void AdvancedNotificationService::AddToNotificationList(const sptr<Notification> &notification)
{
    notificationList_.push_back(notification);
    SortNotificationList();
}

void AdvancedNotificationService::UpdateInNotificationList(const sptr<Notification> &notification)
{
    auto iter = notificationList_.begin();
    while (iter != notificationList_.end()) {
        if ((*iter)->GetKey() == notification->GetKey()) {
            *iter = notification;
            break;
        }
        iter++;
    }

    SortNotificationList();
}

void AdvancedNotificationService::SortNotificationList()
{
    notificationList_.sort(AdvancedNotificationService::NotificationCompare);
}

bool AdvancedNotificationService::NotificationCompare(const sptr<Notification> &first, const sptr<Notification> &second)
{
    // sorting notifications by create time
    return (first->request_->GetCreateTime() < second->request_->GetCreateTime());
}

sptr<NotificationSortingMap> AdvancedNotificationService::GenerateSortingMap()
{
    std::vector<NotificationSorting> sortingList;
    for (auto notification : notificationList_) {
        NotificationSorting sorting;
        sorting.SetKey(notification->GetKey());
        sptr<NotificationSlot> slot;
        if (NotificationPreferences::GetInstance().GetNotificationSlot(
                notification->GetBundleName(), notification->request_->GetSlotType(), slot) == ERR_OK) {
            sorting.SetRanking((int32_t)sortingList.size());
            sorting.SetSlot(slot);
        }
        sortingList.push_back(sorting);
    }

    sptr<NotificationSortingMap> sortingMap = new NotificationSortingMap(sortingList);

    return sortingMap;
}

void AdvancedNotificationService::StartFilters()
{
    for (auto filter : NOTIFICATION_FILTERS) {
        filter->OnStart();
    }
}

void AdvancedNotificationService::StopFilters()
{
    for (auto filter : NOTIFICATION_FILTERS) {
        filter->OnStop();
    }
}

ErrCode AdvancedNotificationService::Cancel(int notificationId, const std::string &label)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &label, notificationId, &result]() {
        sptr<Notification> notification = nullptr;
        result = RemoveFromNotificationList(bundle, label, notificationId, notification);
        if (result != ERR_OK) {
            return;
        }

        if (notification != nullptr) {
            sptr<NotificationSortingMap> sortingMap = GenerateSortingMap();
            NotificationSubscriberManager::GetInstance()->NotifyCanceled(
                notification, sortingMap, NotificationConstant::CANCEL_REASON_DELETE);
        }
    }));
    return result;
}

ErrCode AdvancedNotificationService::CancelAll()
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &result]() {
        sptr<Notification> notification = nullptr;

        std::vector<std::string> removableList = GetRemovableNotificationKeys(bundle);
        for (auto key : removableList) {
            result = RemoveFromNotificationList(key, notification);
            if (result != ERR_OK) {
                continue;
            }

            if (notification != nullptr) {
                sptr<NotificationSortingMap> sortingMap = GenerateSortingMap();
                NotificationSubscriberManager::GetInstance()->NotifyCanceled(
                    notification, sortingMap, NotificationConstant::CANCEL_REASON_DELETE);
            }
        }
    }));
    return result;
}

inline bool IsCustomSlotContained(const std::vector<sptr<NotificationSlot>> &slots)
{
    bool isContained = false;
    for (auto slot : slots) {
        if (slot->GetType() == NotificationConstant::SlotType::CUSTOM) {
            isContained = true;
            break;
        }
    }
    return isContained;
}

ErrCode AdvancedNotificationService::AddSlots(const std::vector<sptr<NotificationSlot>> &slots)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (IsCustomSlotContained(slots) && !IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &slots, &result]() {
        result = NotificationPreferences::GetInstance().AddNotificationSlots(bundle, slots);
    }));
    return result;
}

ErrCode AdvancedNotificationService::AddSlotGroups(std::vector<sptr<NotificationSlotGroup>> groups)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &groups, &result]() {
        result = NotificationPreferences::GetInstance().AddNotificationSlotGroups(bundle, groups);
    }));
    return result;
}

ErrCode AdvancedNotificationService::GetSlots(std::vector<sptr<NotificationSlot>> &slots)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &slots, &result]() {
        result = NotificationPreferences::GetInstance().GetNotificationAllSlots(bundle, slots);
    }));
    return result;
}

ErrCode AdvancedNotificationService::GetSlotGroup(const std::string &groupId, sptr<NotificationSlotGroup> &group)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &groupId, &group, &result]() {
        result = NotificationPreferences::GetInstance().GetNotificationSlotGroup(bundle, groupId, group);
    }));
    return result;
}

ErrCode AdvancedNotificationService::GetSlotGroups(std::vector<sptr<NotificationSlotGroup>> &groups)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &groups, &result]() {
        result = NotificationPreferences::GetInstance().GetNotificationAllSlotGroups(bundle, groups);
    }));
    return result;
}

ErrCode AdvancedNotificationService::RemoveSlotGroups(const std::vector<std::string> &groupIds)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &groupIds, &result]() {
        result = NotificationPreferences::GetInstance().RemoveNotificationSlotGroups(bundle, groupIds);
    }));
    return result;
}

ErrCode AdvancedNotificationService::GetActiveNotifications(std::vector<sptr<NotificationRequest>> &notifications)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &notifications, &result]() {
        for (auto notification : notificationList_) {
            if (notification->GetBundleName() == bundle) {
                notifications.push_back(notification->request_);
            }
        }
    }));
    return result;
}

ErrCode AdvancedNotificationService::GetActiveNotificationNums(int &num)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &num, &result]() {
        int count = 0;
        for (auto notification : notificationList_) {
            if (notification->GetBundleName() == bundle) {
                count += 1;
            }
        }
        num = count;
    }));
    return result;
}

ErrCode AdvancedNotificationService::SetNotificationAgent(const std::string &agent)
{
    return ERR_OK;
}
ErrCode AdvancedNotificationService::GetNotificationAgent(std::string &agent)
{
    return ERR_OK;
}
ErrCode AdvancedNotificationService::CanPublishAsBundle(const std::string &representativeBundle, bool &canPublish)
{
    return ERR_OK;
}
ErrCode AdvancedNotificationService::PublishAsBundle(
    const sptr<NotificationRequest> notification, const std::string &representativeBundle)
{
    return ERR_OK;
}

ErrCode AdvancedNotificationService::SetNotificationBadgeNum(int num)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &num, &result]() {
        result = NotificationPreferences::GetInstance().SetTotalBadgeNums(bundle, num);
    }));
    return result;
}

ErrCode AdvancedNotificationService::GetBundleImportance(int &importance)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &importance, &result]() {
        result = NotificationPreferences::GetInstance().GetImportance(bundle, importance);
    }));
    return result;
}

ErrCode AdvancedNotificationService::SetDisturbMode(NotificationConstant::DisturbMode mode)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind(
        [this, &bundle, mode, &result]() { result = NotificationPreferences::GetInstance().SetDisturbMode(mode); }));
    return result;
}
ErrCode AdvancedNotificationService::GetDisturbMode(NotificationConstant::DisturbMode &mode)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind(
        [this, &bundle, &mode, &result]() { result = NotificationPreferences::GetInstance().GetDisturbMode(mode); }));
    return result;
}
ErrCode AdvancedNotificationService::HasNotificationPolicyAccessPermission(bool &granted)
{
    return ERR_OK;
}

ErrCode AdvancedNotificationService::SetPrivateNotificationsAllowed(bool allow)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &allow, &result]() {
        result = NotificationPreferences::GetInstance().SetPrivateNotificationsAllowed(bundle, allow);
    }));
    return result;
}

ErrCode AdvancedNotificationService::GetPrivateNotificationsAllowed(bool &allow)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &allow, &result]() {
        result = NotificationPreferences::GetInstance().GetPrivateNotificationsAllowed(bundle, allow);
    }));
    return result;
}

ErrCode AdvancedNotificationService::Delete(const std::string &key)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;

    handler_->PostSyncTask(std::bind([this, &key, &result]() {
        sptr<Notification> notification = nullptr;
        result = RemoveFromNotificationList(key, notification);
        if (result != ERR_OK) {
            return;
        }

        if (notification != nullptr) {
            sptr<NotificationSortingMap> sortingMap = GenerateSortingMap();
            NotificationSubscriberManager::GetInstance()->NotifyCanceled(
                notification, sortingMap, NotificationConstant::CANCEL_REASON_DELETE);
        }
    }));

    return result;
}

ErrCode AdvancedNotificationService::DeleteByBundle(const std::string &bundle)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;

    handler_->PostSyncTask(std::bind([this, &bundle, &result]() {
        sptr<Notification> notification = nullptr;

        std::vector<std::string> removableList = GetRemovableNotificationKeys(bundle);
        for (auto key : removableList) {
            result = RemoveFromNotificationList(key, notification);
            if (result != ERR_OK) {
                continue;
            }

            if (notification != nullptr) {
                sptr<NotificationSortingMap> sortingMap = GenerateSortingMap();
                NotificationSubscriberManager::GetInstance()->NotifyCanceled(
                    notification, sortingMap, NotificationConstant::CANCEL_REASON_DELETE);
            }
        }
    }));

    return result;
}

ErrCode AdvancedNotificationService::DeleteAll()
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &result]() {
        sptr<Notification> notification = nullptr;

        std::vector<std::string> removableList = GetRemovableNotificationKeys(std::string());
        for (auto key : removableList) {
            result = RemoveFromNotificationList(key, notification);
            if (result != ERR_OK) {
                continue;
            }

            if (notification != nullptr) {
                sptr<NotificationSortingMap> sortingMap = GenerateSortingMap();
                NotificationSubscriberManager::GetInstance()->NotifyCanceled(
                    notification, sortingMap, NotificationConstant::CANCEL_REASON_DELETE);
            }
        }
    }));

    return result;
}

std::vector<std::string> AdvancedNotificationService::GetRemovableNotificationKeys(const std::string &bundle)
{
    std::vector<std::string> keys;

    for (auto item : notificationList_) {
        if (!bundle.empty() && (item->GetBundleName() != bundle)) {
            continue;
        }
        if (!item->GetNotificationRequest().IsUnremovable()) {
            keys.push_back(item->GetKey());
        }
    }

    return keys;
}

ErrCode AdvancedNotificationService::GetSlotsByBundle(
    const std::string &bundle, std::vector<sptr<NotificationSlot>> &slots)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &bundle, &slots, &result]() {
        result = NotificationPreferences::GetInstance().GetNotificationAllSlots(bundle, slots);
    }));
    return result;
}

ErrCode AdvancedNotificationService::UpdateSlots(
    const std::string &bundle, const std::vector<sptr<NotificationSlot>> &slots)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &bundle, &slots, &result]() {
        result = NotificationPreferences::GetInstance().UpdateNotificationSlots(bundle, slots);
    }));
    return result;
}

ErrCode AdvancedNotificationService::UpdateSlotGroups(
    const std::string &bundle, const std::vector<sptr<NotificationSlotGroup>> &groups)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &bundle, &groups, &result]() {
        result = NotificationPreferences::GetInstance().UpdateNotificationSlotGroups(bundle, groups);
    }));
    return result;
}

ErrCode AdvancedNotificationService::SetNotificationsEnabledForBundle(const std::string &bundle, bool enabled)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &bundle, &enabled, &result]() {
        result = NotificationPreferences::GetInstance().SetNotificationsEnabledForBundle(bundle, enabled);
    }));
    return result;
}

ErrCode AdvancedNotificationService::SetShowBadgeEnabledForBundle(const std::string &bundle, bool enabled)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &bundle, &enabled, &result]() {
        result = NotificationPreferences::GetInstance().SetShowBadge(bundle, enabled);
    }));
    return result;
}

ErrCode AdvancedNotificationService::GetShowBadgeEnabledForBundle(const std::string &bundle, bool &enabled)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &bundle, &enabled, &result]() {
        result = NotificationPreferences::GetInstance().IsShowBadge(bundle, enabled);
    }));
    return result;
}

ErrCode AdvancedNotificationService::RemoveFromNotificationList(
    const std::string &bundle, const std::string &label, int notificationId, sptr<Notification> &notification)
{
    for (auto item : notificationList_) {
        if (item->GetBundleName() == bundle && item->GetLabel() == label && item->GetId() == notificationId) {
            if (item->GetNotificationRequest().IsUnremovable()) {
                return ERR_ANS_NOTIFICATION_IS_UNREMOVABLE;
            }
            notification = item;
            notificationList_.remove(item);
            return ERR_OK;
        }
    }

    return ERR_ANS_NOTIFICATION_NOT_EXISTS;
}

ErrCode AdvancedNotificationService::RemoveFromNotificationList(
    const std::string &key, sptr<Notification> &notification)
{
    for (auto item : notificationList_) {
        if (item->GetKey() == key) {
            if (item->GetNotificationRequest().IsUnremovable()) {
                return ERR_ANS_NOTIFICATION_IS_UNREMOVABLE;
            }
            notification = item;
            notificationList_.remove(item);
            return ERR_OK;
        }
    }

    return ERR_ANS_NOTIFICATION_NOT_EXISTS;
}

std::string AdvancedNotificationService::GetClientBundleName()
{
    std::string bundle;

    uid_t callingUid = IPCSkeleton::GetCallingUid();

    std::shared_ptr<BundleManagerHelper> bundleManager = BundleManagerHelper::GetInstance();
    if (bundleManager != nullptr) {
        bundle = bundleManager->GetBundleNameByUid(callingUid);
    }

    return bundle;
}

bool AdvancedNotificationService::IsSystemApp()
{
    bool isSystemApp = false;

    uid_t callingUid = IPCSkeleton::GetCallingUid();

    std::shared_ptr<BundleManagerHelper> bundleManager = BundleManagerHelper::GetInstance();
    if (bundleManager != nullptr) {
        isSystemApp = bundleManager->IsSystemApp(callingUid);
    }

    return isSystemApp;
}

ErrCode AdvancedNotificationService::Subscribe(
    const sptr<IAnsSubscriber> &subscriber, const sptr<NotificationSubscribeInfo> &info)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (subscriber == nullptr) {
        return ERR_ANS_INVALID_PARAM;
    }

    if (!IsSystemApp()) {
        ANS_LOGE("Client is not a system app");
        return ERR_ANS_NON_SYSTEM_APP;
    }

    return NotificationSubscriberManager::GetInstance()->AddSubscriber(subscriber, info);
}

ErrCode AdvancedNotificationService::Unsubscribe(
    const sptr<IAnsSubscriber> &subscriber, const sptr<NotificationSubscribeInfo> &info)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (subscriber == nullptr) {
        ANS_LOGE("Client is not a system app");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    return NotificationSubscriberManager::GetInstance()->RemoveSubscriber(subscriber, info);
}

ErrCode AdvancedNotificationService::GetSlotByType(
    const NotificationConstant::SlotType slotType, sptr<NotificationSlot> &slot)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &slotType, &slot, &result]() {
        result = NotificationPreferences::GetInstance().GetNotificationSlot(bundle, slotType, slot);
    }));
    return result;
}

ErrCode AdvancedNotificationService::RemoveSlotByType(const NotificationConstant::SlotType slotType)
{
    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &slotType, &result]() {
        result = NotificationPreferences::GetInstance().RemoveNotificationSlot(bundle, slotType);
    }));
    return result;
}

ErrCode AdvancedNotificationService::GetAllActiveNotifications(std::vector<sptr<Notification>> &notifications)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &notifications, &result]() {
        notifications.assign(notificationList_.begin(), notificationList_.end());
    }));
    return result;
}

inline bool IsContained(const std::vector<std::string> &vec, const std::string &target)
{
    bool isContained = false;

    auto iter = vec.begin();
    while (iter != vec.end()) {
        if (*iter == target) {
            isContained = true;
            break;
        }
        iter++;
    }

    return isContained;
}

ErrCode AdvancedNotificationService::GetSpecialActiveNotifications(
    const std::vector<std::string> &key, std::vector<sptr<Notification>> &notifications)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &key, &notifications, &result]() {
        for (auto notification : notificationList_) {
            if (IsContained(key, notification->GetKey())) {
                notifications.push_back(notification);
            }
        }
    }));
    return result;
}

ErrCode AdvancedNotificationService::SetNotificationsEnabledForAllBundles(const std::string &deviceId, bool enabled)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &deviceId, &enabled, &result]() {
        if (deviceId.empty()) {
            // Local device
            result = NotificationPreferences::GetInstance().SetNotificationsEnabled(enabled);
        } else {
            // Remote device
        }
    }));
    return result;
}

ErrCode AdvancedNotificationService::SetNotificationsEnabledForSpecialBundle(
    const std::string &deviceId, const std::string &bundleName, bool enabled)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &deviceId, &bundleName, &enabled, &result]() {
        if (deviceId.empty()) {
            // Local device
            result = NotificationPreferences::GetInstance().SetNotificationsEnabledForBundle(bundleName, enabled);
        } else {
            // Remote revice
        }
    }));
    return result;
}

ErrCode AdvancedNotificationService::IsAllowedNotify(bool &allowed)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    ErrCode result = ERR_OK;
    std::string bundle = GetClientBundleName();
    handler_->PostSyncTask(std::bind([this, &bundle, &allowed, &result]() {
        allowed = false;
        result = NotificationPreferences::GetInstance().GetNotificationsEnabled(allowed);
        if (result == ERR_OK && allowed) {
            result = NotificationPreferences::GetInstance().GetNotificationsEnabledForBundle(bundle, allowed);
        }
    }));
    return result;
}

ErrCode AdvancedNotificationService::IsSpecialBundleAllowedNotify(const std::string &bundle, bool &allowed)
{
    ANS_LOGD("%{public}s", __FUNCTION__);

    if (!IsSystemApp()) {
        return ERR_ANS_NON_SYSTEM_APP;
    }

    ErrCode result = ERR_OK;
    handler_->PostSyncTask(std::bind([this, &bundle, &allowed, &result]() {
        allowed = false;
        result = NotificationPreferences::GetInstance().GetNotificationsEnabled(allowed);
        if (result == ERR_OK && allowed) {
            result = NotificationPreferences::GetInstance().GetNotificationsEnabledForBundle(bundle, allowed);
        }
    }));
    return result;
}

}  // namespace Notification
}  // namespace OHOS
