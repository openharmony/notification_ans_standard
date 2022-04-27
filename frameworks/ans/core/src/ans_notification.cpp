/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ans_notification.h"
#include "ans_const_define.h"
#include "ans_inner_errors.h"
#include "ans_log_wrapper.h"
#include "iservice_registry.h"
#include "reminder_request_alarm.h"
#include "reminder_request_calendar.h"
#include "reminder_request_timer.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Notification {
ErrCode AnsNotification::AddNotificationSlot(const NotificationSlot &slot)
{
    std::vector<NotificationSlot> slots;
    slots.push_back(slot);
    return AddNotificationSlots(slots);
}

ErrCode AnsNotification::AddSlotByType(const NotificationConstant::SlotType &slotType)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->AddSlotByType(slotType);
}

ErrCode AnsNotification::AddNotificationSlots(const std::vector<NotificationSlot> &slots)
{
    if (slots.size() == 0) {
        ANS_LOGE("Failed to add notification slots because input slots size is 0.");
        return ERR_ANS_INVALID_PARAM;
    }
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    std::vector<sptr<NotificationSlot>> slotsSptr;
    for (auto it = slots.begin(); it != slots.end(); ++it) {
        sptr<NotificationSlot> slot = new (std::nothrow) NotificationSlot(*it);
        if (slot == nullptr) {
            ANS_LOGE("Failed to create NotificationSlot ptr.");
            return ERR_ANS_NO_MEMORY;
        }
        slotsSptr.emplace_back(slot);
    }

    return ansManagerProxy_->AddSlots(slotsSptr);
}

ErrCode AnsNotification::RemoveNotificationSlot(const NotificationConstant::SlotType &slotType)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->RemoveSlotByType(slotType);
}

ErrCode AnsNotification::RemoveAllSlots()
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->RemoveAllSlots();
}

ErrCode AnsNotification::GetNotificationSlot(
    const NotificationConstant::SlotType &slotType, sptr<NotificationSlot> &slot)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetSlotByType(slotType, slot);
}

ErrCode AnsNotification::GetNotificationSlots(std::vector<sptr<NotificationSlot>> &slots)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetSlots(slots);
}

ErrCode AnsNotification::AddNotificationSlotGroup(const NotificationSlotGroup &slotGroup)
{
    std::vector<NotificationSlotGroup> slotGroups;
    slotGroups.emplace_back(slotGroup);
    return AddNotificationSlotGroups(slotGroups);
}

ErrCode AnsNotification::AddNotificationSlotGroups(const std::vector<NotificationSlotGroup> &slotGroups)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    std::vector<sptr<NotificationSlotGroup>> slotGroupsSptr;
    for (auto it = slotGroups.begin(); it != slotGroups.end(); ++it) {
        sptr<NotificationSlotGroup> slotGroup = new (std::nothrow) NotificationSlotGroup(*it);
        if (slotGroup == nullptr) {
            ANS_LOGE("Failed to add notification slot groups with NotificationSlotGroup nullptr");
            return ERR_ANS_NO_MEMORY;
        }
        slotGroupsSptr.emplace_back(slotGroup);
    }

    return ansManagerProxy_->AddSlotGroups(slotGroupsSptr);
}

ErrCode AnsNotification::RemoveNotificationSlotGroup(const std::string &slotGroupId)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    std::vector<std::string> slotGroupIds;
    slotGroupIds.emplace_back(slotGroupId);
    return ansManagerProxy_->RemoveSlotGroups(slotGroupIds);
}

ErrCode AnsNotification::GetNotificationSlotGroup(const std::string &groupId, sptr<NotificationSlotGroup> &group)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetSlotGroup(groupId, group);
}

ErrCode AnsNotification::GetNotificationSlotGroups(std::vector<sptr<NotificationSlotGroup>> &groups)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetSlotGroups(groups);
}

ErrCode AnsNotification::GetNotificationSlotNumAsBundle(const NotificationBundleOption &bundleOption, int &num)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->GetSlotNumAsBundle(bo, num);
}

ErrCode AnsNotification::PublishNotification(const NotificationRequest &request)
{
    ANS_LOGI("enter");
    return PublishNotification(std::string(), request);
}

ErrCode AnsNotification::PublishNotification(const std::string &label, const NotificationRequest &request)
{
    ANS_LOGI("enter");

    if (request.GetContent() == nullptr || request.GetNotificationType() == NotificationContent::Type::NONE) {
        ANS_LOGE("Refuse to publish the notification without valid content");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!CanPublishMediaContent(request)) {
        ANS_LOGE("Refuse to publish the notification because the sequence numbers actions not match those assigned to "
                 "added action buttons.");
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode checkErr = CheckImageSize(request);
    if (checkErr != ERR_OK) {
        ANS_LOGE("The size of one picture exceeds the limit");
        return checkErr;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationRequest> reqPtr = new (std::nothrow) NotificationRequest(request);
    if (reqPtr == nullptr) {
        ANS_LOGE("Failed to create NotificationRequest ptr");
        return ERR_ANS_NO_MEMORY;
    }
    if (IsNonDistributedNotificationType(reqPtr->GetNotificationType())) {
        reqPtr->SetDistributed(false);
    }
    return ansManagerProxy_->Publish(label, reqPtr);
}

ErrCode AnsNotification::PublishNotification(const NotificationRequest &request, const std::string &deviceId)
{
    if (request.GetContent() == nullptr || request.GetNotificationType() == NotificationContent::Type::NONE) {
        ANS_LOGE("Refuse to publish the notification without valid content");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!deviceId.empty() &&
        (IsNonDistributedNotificationType(request.GetNotificationType()))) {
        ANS_LOGE("Refuse to publish the conversational and picture notification to the remote device");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!CanPublishMediaContent(request)) {
        ANS_LOGE("Refuse to publish the notification because the sequence numbers actions not match those assigned to "
                 "added action buttons.");
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode checkErr = CheckImageSize(request);
    if (checkErr != ERR_OK) {
        ANS_LOGE("The size of one picture exceeds the limit");
        return checkErr;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationRequest> reqPtr = new (std::nothrow) NotificationRequest(request);
    if (reqPtr == nullptr) {
        ANS_LOGE("Failed to create NotificationRequest ptr");
        return ERR_ANS_NO_MEMORY;
    }
    return ansManagerProxy_->PublishToDevice(reqPtr, deviceId);
}

ErrCode AnsNotification::CancelNotification(int32_t notificationId)
{
    return CancelNotification("", notificationId);
}

ErrCode AnsNotification::CancelNotification(const std::string &label, int32_t notificationId)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->Cancel(notificationId, label);
}

ErrCode AnsNotification::CancelAllNotifications()
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->CancelAll();
}

ErrCode AnsNotification::GetActiveNotificationNums(int32_t &num)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetActiveNotificationNums(num);
}

ErrCode AnsNotification::GetActiveNotifications(std::vector<sptr<NotificationRequest>> &request)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetActiveNotifications(request);
}

ErrCode AnsNotification::GetCurrentAppSorting(sptr<NotificationSortingMap> &sortingMap)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetCurrentAppSorting(sortingMap);
}

ErrCode AnsNotification::SetNotificationAgent(const std::string &agent)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->SetNotificationAgent(agent);
}

ErrCode AnsNotification::GetNotificationAgent(std::string &agent)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetNotificationAgent(agent);
}

ErrCode AnsNotification::CanPublishNotificationAsBundle(const std::string &representativeBundle, bool &canPublish)
{
    if (representativeBundle.empty()) {
        ANS_LOGW("Input representativeBundle is empty");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->CanPublishAsBundle(representativeBundle, canPublish);
}

ErrCode AnsNotification::PublishNotificationAsBundle(
    const std::string &representativeBundle, const NotificationRequest &request)
{
    if (representativeBundle.empty()) {
        ANS_LOGE("Refuse to publish the notification whit invalid representativeBundle");
        return ERR_ANS_INVALID_PARAM;
    }

    if (request.GetContent() == nullptr || request.GetNotificationType() == NotificationContent::Type::NONE) {
        ANS_LOGE("Refuse to publish the notification without valid content");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!CanPublishMediaContent(request)) {
        ANS_LOGE("Refuse to publish the notification because the sequence numbers actions not match those assigned to "
                 "added action buttons.");
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode checkErr = CheckImageSize(request);
    if (checkErr != ERR_OK) {
        ANS_LOGE("The size of one picture exceeds the limit");
        return checkErr;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationRequest> reqPtr = new (std::nothrow) NotificationRequest(request);
    if (reqPtr == nullptr) {
        ANS_LOGE("Failed to create NotificationRequest ptr");
        return ERR_ANS_NO_MEMORY;
    }
    if (IsNonDistributedNotificationType(reqPtr->GetNotificationType())) {
        reqPtr->SetDistributed(false);
    }
    return ansManagerProxy_->PublishAsBundle(reqPtr, representativeBundle);
}

ErrCode AnsNotification::SetNotificationBadgeNum()
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    int32_t num = -1;
    return ansManagerProxy_->SetNotificationBadgeNum(num);
}

ErrCode AnsNotification::SetNotificationBadgeNum(int32_t num)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->SetNotificationBadgeNum(num);
}

ErrCode AnsNotification::IsAllowedNotify(bool &allowed)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->IsAllowedNotify(allowed);
}

ErrCode AnsNotification::IsAllowedNotifySelf(bool &allowed)
{
    ANS_LOGD("enter");
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->IsAllowedNotifySelf(allowed);
}

ErrCode AnsNotification::RequestEnableNotification(std::string &deviceId)
{
    ANS_LOGD("enter");
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->RequestEnableNotification(deviceId);
}

ErrCode AnsNotification::AreNotificationsSuspended(bool &suspended)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->AreNotificationsSuspended(suspended);
}

ErrCode AnsNotification::HasNotificationPolicyAccessPermission(bool &hasPermission)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->HasNotificationPolicyAccessPermission(hasPermission);
}

ErrCode AnsNotification::GetBundleImportance(NotificationSlot::NotificationLevel &importance)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    int importanceTemp;
    ErrCode ret = ansManagerProxy_->GetBundleImportance(importanceTemp);
    if ((NotificationSlot::LEVEL_NONE <= importanceTemp) && (importanceTemp <= NotificationSlot::LEVEL_HIGH)) {
        importance = static_cast<NotificationSlot::NotificationLevel>(importanceTemp);
    } else {
        importance = NotificationSlot::LEVEL_UNDEFINED;
    }
    return ret;
}

ErrCode AnsNotification::SubscribeNotification(const NotificationSubscriber &subscriber)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationSubscriber::SubscriberImpl> subscriberSptr = subscriber.GetImpl();
    if (subscriberSptr == nullptr) {
        ANS_LOGE("Failed to subscribe with SubscriberImpl null ptr.");
        return ERR_ANS_INVALID_PARAM;
    }
    return ansManagerProxy_->Subscribe(subscriberSptr, nullptr);
}

ErrCode AnsNotification::SubscribeNotification(
    const NotificationSubscriber &subscriber, const NotificationSubscribeInfo &subscribeInfo)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationSubscribeInfo> sptrInfo = new (std::nothrow) NotificationSubscribeInfo(subscribeInfo);
    if (sptrInfo == nullptr) {
        ANS_LOGE("Failed to create NotificationSubscribeInfo ptr.");
        return ERR_ANS_NO_MEMORY;
    }

    sptr<NotificationSubscriber::SubscriberImpl> subscriberSptr = subscriber.GetImpl();
    if (subscriberSptr == nullptr) {
        ANS_LOGE("Failed to subscribe with SubscriberImpl null ptr.");
        return ERR_ANS_INVALID_PARAM;
    }
    return ansManagerProxy_->Subscribe(subscriberSptr, sptrInfo);
}

ErrCode AnsNotification::UnSubscribeNotification(NotificationSubscriber &subscriber)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationSubscriber::SubscriberImpl> subscriberSptr = subscriber.GetImpl();
    if (subscriberSptr == nullptr) {
        ANS_LOGE("Failed to unsubscribe with SubscriberImpl null ptr.");
        return ERR_ANS_INVALID_PARAM;
    }
    return ansManagerProxy_->Unsubscribe(subscriberSptr, nullptr);
}

ErrCode AnsNotification::UnSubscribeNotification(
    NotificationSubscriber &subscriber, NotificationSubscribeInfo subscribeInfo)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationSubscribeInfo> sptrInfo = new (std::nothrow) NotificationSubscribeInfo(subscribeInfo);
    if (sptrInfo == nullptr) {
        ANS_LOGE("Failed to create NotificationSubscribeInfo ptr.");
        return ERR_ANS_NO_MEMORY;
    }

    sptr<NotificationSubscriber::SubscriberImpl> subscriberSptr = subscriber.GetImpl();
    if (subscriberSptr == nullptr) {
        ANS_LOGE("Failed to unsubscribe with SubscriberImpl null ptr.");
        return ERR_ANS_INVALID_PARAM;
    }
    return ansManagerProxy_->Unsubscribe(subscriberSptr, sptrInfo);
}

ErrCode AnsNotification::RemoveNotification(const std::string &key)
{
    if (key.empty()) {
        ANS_LOGW("Input key is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->Delete(key);
}

ErrCode AnsNotification::RemoveNotification(
    const NotificationBundleOption &bundleOption, const int32_t notificationId, const std::string &label)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->RemoveNotification(bo, notificationId, label);
}

ErrCode AnsNotification::RemoveAllNotifications(const NotificationBundleOption &bundleOption)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->RemoveAllNotifications(bo);
}

ErrCode AnsNotification::RemoveNotificationsByBundle(const NotificationBundleOption &bundleOption)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->DeleteByBundle(bo);
}

ErrCode AnsNotification::RemoveNotifications()
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->DeleteAll();
}

ErrCode AnsNotification::GetNotificationSlotsForBundle(
    const NotificationBundleOption &bundleOption, std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Input bundleName is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->GetSlotsByBundle(bo, slots);
}

ErrCode AnsNotification::UpdateNotificationSlots(
    const NotificationBundleOption &bundleOption, const std::vector<sptr<NotificationSlot>> &slots)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->UpdateSlots(bo, slots);
}

ErrCode AnsNotification::UpdateNotificationSlotGroups(
    const NotificationBundleOption &bundleOption, const std::vector<sptr<NotificationSlotGroup>> &groups)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->UpdateSlotGroups(bo, groups);
}

ErrCode AnsNotification::GetAllActiveNotifications(std::vector<sptr<Notification>> &notification)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetAllActiveNotifications(notification);
}

ErrCode AnsNotification::GetAllActiveNotifications(
    const std::vector<std::string> key, std::vector<sptr<Notification>> &notification)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetSpecialActiveNotifications(key, notification);
}

ErrCode AnsNotification::IsAllowedNotify(const NotificationBundleOption &bundleOption, bool &allowed)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Input bundle is empty.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->IsSpecialBundleAllowedNotify(bo, allowed);
}

ErrCode AnsNotification::SetNotificationsEnabledForAllBundles(const std::string &deviceId, bool enabled)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->SetNotificationsEnabledForAllBundles(deviceId, enabled);
}

ErrCode AnsNotification::SetNotificationsEnabledForDefaultBundle(const std::string &deviceId, bool enabled)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->SetNotificationsEnabledForBundle(deviceId, enabled);
}

ErrCode AnsNotification::SetNotificationsEnabledForSpecifiedBundle(
    const NotificationBundleOption &bundleOption, const std::string &deviceId, bool enabled)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->SetNotificationsEnabledForSpecialBundle(deviceId, bo, enabled);
}

ErrCode AnsNotification::SetShowBadgeEnabledForBundle(const NotificationBundleOption &bundleOption, bool enabled)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->SetShowBadgeEnabledForBundle(bo, enabled);
}

ErrCode AnsNotification::GetShowBadgeEnabledForBundle(const NotificationBundleOption &bundleOption, bool &enabled)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->GetShowBadgeEnabledForBundle(bo, enabled);
}

ErrCode AnsNotification::GetShowBadgeEnabled(bool &enabled)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->GetShowBadgeEnabled(enabled);
}

ErrCode AnsNotification::CancelGroup(const std::string &groupName)
{
    if (groupName.empty()) {
        ANS_LOGE("Invalid group name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->CancelGroup(groupName);
}

ErrCode AnsNotification::RemoveGroupByBundle(
    const NotificationBundleOption &bundleOption, const std::string &groupName)
{
    if (bundleOption.GetBundleName().empty() || groupName.empty()) {
        ANS_LOGE("Invalid parameter.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->RemoveGroupByBundle(bo, groupName);
}

ErrCode AnsNotification::SetDoNotDisturbDate(const NotificationDoNotDisturbDate &doNotDisturbDate)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    auto dndDatePtr = new (std::nothrow) NotificationDoNotDisturbDate(doNotDisturbDate);
    if (dndDatePtr == nullptr) {
        ANS_LOGE("create DoNotDisturbDate failed.");
        return ERR_ANS_NO_MEMORY;
    }

    sptr<NotificationDoNotDisturbDate> dndDate(dndDatePtr);
    return ansManagerProxy_->SetDoNotDisturbDate(dndDate);
}

ErrCode AnsNotification::GetDoNotDisturbDate(NotificationDoNotDisturbDate &doNotDisturbDate)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationDoNotDisturbDate> dndDate = nullptr;
    auto ret = ansManagerProxy_->GetDoNotDisturbDate(dndDate);
    if (ret != ERR_OK) {
        ANS_LOGE("Get DoNotDisturbDate failed.");
        return ret;
    }

    if (!dndDate) {
        ANS_LOGE("Invalid DoNotDisturbDate.");
        return ERR_ANS_NO_MEMORY;
    }

    doNotDisturbDate = *dndDate;
    return ret;
}

ErrCode AnsNotification::DoesSupportDoNotDisturbMode(bool &doesSupport)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->DoesSupportDoNotDisturbMode(doesSupport);
}

ErrCode AnsNotification::PublishContinuousTaskNotification(const NotificationRequest &request)
{
    if (request.GetContent() == nullptr || request.GetNotificationType() == NotificationContent::Type::NONE) {
        ANS_LOGE("Refuse to publish the notification without valid content");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!CanPublishMediaContent(request)) {
        ANS_LOGE("Refuse to publish the notification because the sequence numbers actions not match those assigned to "
                 "added action buttons.");
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode checkErr = CheckImageSize(request);
    if (checkErr != ERR_OK) {
        ANS_LOGE("The size of one picture exceeds the limit");
        return checkErr;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    auto pReq = new (std::nothrow) NotificationRequest(request);
    if (pReq == nullptr) {
        ANS_LOGE("Failed to create NotificationRequest ptr.");
        return ERR_ANS_NO_MEMORY;
    }

    sptr<NotificationRequest> sptrReq(pReq);
    if (IsNonDistributedNotificationType(sptrReq->GetNotificationType())) {
        sptrReq->SetDistributed(false);
    }
    return ansManagerProxy_->PublishContinuousTaskNotification(sptrReq);
}

ErrCode AnsNotification::CancelContinuousTaskNotification(const std::string &label, int32_t notificationId)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->CancelContinuousTaskNotification(label, notificationId);
}

ErrCode AnsNotification::IsDistributedEnabled(bool &enabled)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->IsDistributedEnabled(enabled);
}

ErrCode AnsNotification::EnableDistributed(const bool enabled)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->EnableDistributed(enabled);
}

ErrCode AnsNotification::EnableDistributedByBundle(const NotificationBundleOption &bundleOption, const bool enabled)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->EnableDistributedByBundle(bo, enabled);
}

ErrCode AnsNotification::EnableDistributedSelf(const bool enabled)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->EnableDistributedSelf(enabled);
}

ErrCode AnsNotification::IsDistributedEnableByBundle(const NotificationBundleOption &bundleOption, bool &enabled)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->IsDistributedEnableByBundle(bo, enabled);
}

ErrCode AnsNotification::GetDeviceRemindType(NotificationConstant::RemindType &remindType)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->GetDeviceRemindType(remindType);
}

void AnsNotification::ResetAnsManagerProxy()
{
    ANS_LOGI("enter");
    std::lock_guard<std::mutex> lock(mutex_);
    if ((ansManagerProxy_ != nullptr) && (ansManagerProxy_->AsObject() != nullptr)) {
        ansManagerProxy_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    ansManagerProxy_ = nullptr;
}

ErrCode AnsNotification::ShellDump(const std::string &dumpOption, std::vector<std::string> &dumpInfo)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->ShellDump(dumpOption, dumpInfo);
}

ErrCode AnsNotification::PublishReminder(ReminderRequest &reminder)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    sptr<ReminderRequest> tarReminder = nullptr;
    switch (reminder.GetReminderType()) {
        case (ReminderRequest::ReminderType::TIMER): {
            ANSR_LOGI("Publish timer");
            ReminderRequestTimer &timer = (ReminderRequestTimer &)reminder;
            tarReminder = new (std::nothrow) ReminderRequestTimer(timer);
            break;
        }
        case (ReminderRequest::ReminderType::ALARM): {
            ANSR_LOGI("Publish alarm");
            ReminderRequestAlarm &alarm = (ReminderRequestAlarm &)reminder;
            tarReminder = new (std::nothrow) ReminderRequestAlarm(alarm);
            break;
        }
        case (ReminderRequest::ReminderType::CALENDAR): {
            ANSR_LOGI("Publish calendar");
            ReminderRequestCalendar &calendar = (ReminderRequestCalendar &)reminder;
            tarReminder = new (std::nothrow) ReminderRequestCalendar(calendar);
            break;
        }
        default: {
            ANSR_LOGW("PublishReminder fail.");
            return ERR_ANS_INVALID_PARAM;
        }
    }
    ErrCode code = ansManagerProxy_->PublishReminder(tarReminder);
    reminder.SetReminderId(tarReminder->GetReminderId());
    return code;
}

ErrCode AnsNotification::CancelReminder(const int32_t reminderId)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->CancelReminder(reminderId);
}

ErrCode AnsNotification::CancelAllReminders()
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->CancelAllReminders();
}

ErrCode AnsNotification::GetValidReminders(std::vector<sptr<ReminderRequest>> &validReminders)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->GetValidReminders(validReminders);
}

bool AnsNotification::GetAnsManagerProxy()
{
    if (!ansManagerProxy_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!ansManagerProxy_) {
            sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (!systemAbilityManager) {
                ANS_LOGE("Failed to get system ability mgr.");
                return false;
            }

            sptr<IRemoteObject> remoteObject =
                systemAbilityManager->GetSystemAbility(ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID);
            if (!remoteObject) {
                ANS_LOGE("Failed to get notification Manager.");
                return false;
            }

            ansManagerProxy_ = iface_cast<IAnsManager>(remoteObject);
            if ((!ansManagerProxy_) || (!ansManagerProxy_->AsObject())) {
                ANS_LOGE("Failed to get notification Manager's proxy");
                return false;
            }

            recipient_ = new (std::nothrow) AnsManagerDeathRecipient();
            if (!recipient_) {
                ANS_LOGE("Failed to create death recipient");
                return false;
            }
            ansManagerProxy_->AsObject()->AddDeathRecipient(recipient_);
        }
    }

    return true;
}

bool AnsNotification::CanPublishMediaContent(const NotificationRequest &request) const
{
    if (NotificationContent::Type::MEDIA != request.GetNotificationType()) {
        return true;
    }

    if (request.GetContent() == nullptr) {
        ANS_LOGE("Failed to publish notification with null content.");
        return false;
    }

    auto media = std::static_pointer_cast<NotificationMediaContent>(request.GetContent()->GetNotificationContent());
    if (media == nullptr) {
        ANS_LOGE("Failed to get media content.");
        return false;
    }

    auto showActions = media->GetShownActions();
    uint32_t size = request.GetActionButtons().size();
    for (auto it = showActions.begin(); it != showActions.end(); ++it) {
        if (*it > size) {
            ANS_LOGE("The sequence numbers actions is: %{public}d, the assigned to added action buttons size is: "
                     "%{public}d.",
                *it,
                size);
            return false;
        }
    }

    return true;
}

bool AnsNotification::CheckImageOverSizeForPixelMap(const std::shared_ptr<Media::PixelMap> &pixelMap, uint32_t maxSize)
{
    if (!pixelMap) {
        return false;
    }

    uint32_t size = static_cast<uint32_t>(pixelMap->GetByteCount());
    if (size > maxSize) {
        return true;
    }
    return false;
}

ErrCode AnsNotification::CheckImageSizeForContent(const NotificationRequest &request)
{
    auto content = request.GetContent();
    if (!content) {
        ANS_LOGW("Invalid content in NotificationRequest");
        return ERR_OK;
    }

    auto basicContent = request.GetContent()->GetNotificationContent();
    if (!basicContent) {
        ANS_LOGW("Invalid content in NotificationRequest");
        return ERR_OK;
    }

    auto contentType = request.GetNotificationType();
    switch (contentType) {
        case NotificationContent::Type::CONVERSATION: {
            auto conversationalContent = std::static_pointer_cast<NotificationConversationalContent>(basicContent);

            auto picture = conversationalContent->GetMessageUser().GetPixelMap();
            if (CheckImageOverSizeForPixelMap(picture, MAX_ICON_SIZE)) {
                ANS_LOGE("The size of picture in ConversationalContent's message user exceeds limit");
                return ERR_ANS_ICON_OVER_SIZE;
            }

            auto messages = conversationalContent->GetAllConversationalMessages();
            for (auto &msg : messages) {
                if (!msg) {
                    continue;
                }

                auto img = msg->GetSender().GetPixelMap();
                if (CheckImageOverSizeForPixelMap(img, MAX_ICON_SIZE)) {
                    ANS_LOGE("The size of picture in ConversationalContent's message exceeds limit");
                    return ERR_ANS_ICON_OVER_SIZE;
                }
            }
            break;
        }
        case NotificationContent::Type::PICTURE: {
            auto pictureContent = std::static_pointer_cast<NotificationPictureContent>(basicContent);

            auto bigPicture = pictureContent->GetBigPicture();
            if (CheckImageOverSizeForPixelMap(bigPicture, MAX_PICTURE_SIZE)) {
                ANS_LOGE("The size of big picture in PictureContent exceeds limit");
                return ERR_ANS_PICTURE_OVER_SIZE;
            }
            break;
        }
        default:
            break;
    }

    return ERR_OK;
}

ErrCode AnsNotification::CheckImageSize(const NotificationRequest &request)
{
    auto littleIcon = request.GetLittleIcon();
    if (CheckImageOverSizeForPixelMap(littleIcon, MAX_ICON_SIZE)) {
        ANS_LOGE("The size of little icon exceeds limit");
        return ERR_ANS_ICON_OVER_SIZE;
    }

    auto bigIcon = request.GetBigIcon();
    if (CheckImageOverSizeForPixelMap(bigIcon, MAX_ICON_SIZE)) {
        ANS_LOGE("The size of big icon exceeds limit");
        return ERR_ANS_ICON_OVER_SIZE;
    }

    ErrCode err = CheckImageSizeForContent(request);
    if (err != ERR_OK) {
        return err;
    }

    auto buttons = request.GetActionButtons();
    for (auto &btn : buttons) {
        if (!btn) {
            continue;
        }
        auto icon = btn->GetIcon();
        if (CheckImageOverSizeForPixelMap(icon, MAX_ICON_SIZE)) {
            ANS_LOGE("The size of icon in ActionButton exceeds limit");
            return ERR_ANS_ICON_OVER_SIZE;
        }
    }

    auto users = request.GetMessageUsers();
    for (auto &user : users) {
        if (!user) {
            continue;
        }
        auto icon = user->GetPixelMap();
        if (CheckImageOverSizeForPixelMap(icon, MAX_ICON_SIZE)) {
            ANS_LOGE("The size of picture in MessageUser exceeds limit");
            return ERR_ANS_ICON_OVER_SIZE;
        }
    }

    return ERR_OK;
}

ErrCode AnsNotification::IsSupportTemplate(const std::string &templateName, bool &support)
{
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->IsSupportTemplate(templateName, support);
}

bool AnsNotification::IsNonDistributedNotificationType(const NotificationContent::Type &type)
{
    return ((type == NotificationContent::Type::CONVERSATION) || (type == NotificationContent::Type::PICTURE));
}

ErrCode AnsNotification::IsAllowedNotify(const int32_t &userId, bool &allowed)
{
    if (userId <= SUBSCRIBE_USER_INIT) {
        ANS_LOGE("Input userId is invalid.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->IsSpecialUserAllowedNotify(userId, allowed);
}

ErrCode AnsNotification::SetNotificationsEnabledForAllBundles(const int32_t &userId, bool enabled)
{
    if (userId <= SUBSCRIBE_USER_INIT) {
        ANS_LOGE("Input userId is invalid.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }
    return ansManagerProxy_->SetNotificationsEnabledByUser(userId, enabled);
}

ErrCode AnsNotification::RemoveNotifications(const int32_t &userId)
{
    if (userId <= SUBSCRIBE_USER_INIT) {
        ANS_LOGE("Input userId is invalid.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    return ansManagerProxy_->DeleteAllByUser(userId);
}

ErrCode AnsNotification::SetDoNotDisturbDate(const int32_t &userId,
    const NotificationDoNotDisturbDate &doNotDisturbDate)
{
    if (userId <= SUBSCRIBE_USER_INIT) {
        ANS_LOGE("Input userId is invalid.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    auto dndDatePtr = new (std::nothrow) NotificationDoNotDisturbDate(doNotDisturbDate);
    if (dndDatePtr == nullptr) {
        ANS_LOGE("create DoNotDisturbDate failed.");
        return ERR_ANS_NO_MEMORY;
    }

    sptr<NotificationDoNotDisturbDate> dndDate(dndDatePtr);
    return ansManagerProxy_->SetDoNotDisturbDate(dndDate);
}

ErrCode AnsNotification::GetDoNotDisturbDate(const int32_t &userId, NotificationDoNotDisturbDate &doNotDisturbDate)
{
    if (userId <= SUBSCRIBE_USER_INIT) {
        ANS_LOGE("Input userId is invalid.");
        return ERR_ANS_INVALID_PARAM;
    }
    
    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetAnsManagerProxy fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationDoNotDisturbDate> dndDate = nullptr;
    auto ret = ansManagerProxy_->GetDoNotDisturbDate(dndDate);
    if (ret != ERR_OK) {
        ANS_LOGE("Get DoNotDisturbDate failed.");
        return ret;
    }

    if (!dndDate) {
        ANS_LOGE("Invalid DoNotDisturbDate.");
        return ERR_ANS_NO_MEMORY;
    }

    doNotDisturbDate = *dndDate;
    return ret;
}

ErrCode AnsNotification::SetEnabledForBundleSlot(
    const NotificationBundleOption &bundleOption, const NotificationConstant::SlotType &slotType, bool enabled)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("SetEnabledForBundleSlot fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->SetEnabledForBundleSlot(bo, slotType, enabled);
}

ErrCode AnsNotification::GetEnabledForBundleSlot(
    const NotificationBundleOption &bundleOption, const NotificationConstant::SlotType &slotType, bool &enabled)
{
    if (bundleOption.GetBundleName().empty()) {
        ANS_LOGE("Invalid bundle name.");
        return ERR_ANS_INVALID_PARAM;
    }

    if (!GetAnsManagerProxy()) {
        ANS_LOGE("GetEnabledForBundleSlot fail.");
        return ERR_ANS_SERVICE_NOT_CONNECTED;
    }

    sptr<NotificationBundleOption> bo(new (std::nothrow) NotificationBundleOption(bundleOption));
    return ansManagerProxy_->GetEnabledForBundleSlot(bo, slotType, enabled);
}
}  // namespace Notification
}  // namespace OHOS