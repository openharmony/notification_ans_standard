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

#include "notification_subscriber_manager.h"

#include <algorithm>
#include <memory>
#include <set>

#include "ans_const_define.h"
#include "ans_inner_errors.h"
#include "ans_log_wrapper.h"
#include "ipc_skeleton.h"
#include "os_account_manager.h"
#include "remote_death_recipient.h"

namespace OHOS {
namespace Notification {
struct NotificationSubscriberManager::SubscriberRecord {
    sptr<IAnsSubscriber> subscriber {nullptr};
    std::set<std::string> bundleList_ {};
    bool subscribedAll {false};
    int32_t userId {SUBSCRIBE_USER_INIT};
};

NotificationSubscriberManager::NotificationSubscriberManager()
{
    runner_ = OHOS::AppExecFwk::EventRunner::Create();
    handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner_);
    recipient_ =
        new RemoteDeathRecipient(std::bind(&NotificationSubscriberManager::OnRemoteDied, this, std::placeholders::_1));
}

NotificationSubscriberManager::~NotificationSubscriberManager()
{
    subscriberRecordList_.clear();
}

ErrCode NotificationSubscriberManager::AddSubscriber(
    const sptr<IAnsSubscriber> &subscriber, const sptr<NotificationSubscribeInfo> &subscribeInfo)
{
    if (subscriber == nullptr) {
        ANS_LOGE("subscriber is null.");
        return ERR_ANS_INVALID_PARAM;
    }

    sptr<NotificationSubscribeInfo> subInfo = subscribeInfo;
    if (subInfo == nullptr) {
        subInfo = new (std::nothrow) NotificationSubscribeInfo();
        if (subInfo == nullptr) {
            ANS_LOGE("Failed to create NotificationSubscribeInfo ptr.");
            return ERR_ANS_NO_MEMORY;
        }

        int userId = SUBSCRIBE_USER_ALL;
        subInfo->AddAppUserId(userId);
    }

    ErrCode result = ERR_ANS_TASK_ERR;
    handler_->PostSyncTask(std::bind([this, &subscriber, &subInfo, &result]() {
        result = this->AddSubscriberInner(subscriber, subInfo);
    }),
        AppExecFwk::EventQueue::Priority::HIGH);
    return result;
}

ErrCode NotificationSubscriberManager::RemoveSubscriber(
    const sptr<IAnsSubscriber> &subscriber, const sptr<NotificationSubscribeInfo> &subscribeInfo)
{
    if (subscriber == nullptr) {
        ANS_LOGE("subscriber is null.");
        return ERR_ANS_INVALID_PARAM;
    }

    ErrCode result = ERR_ANS_TASK_ERR;
    handler_->PostSyncTask(std::bind([this, &subscriber, &subscribeInfo, &result]() {
        result = this->RemoveSubscriberInner(subscriber, subscribeInfo);
    }),
        AppExecFwk::EventQueue::Priority::HIGH);
    return result;
}

void NotificationSubscriberManager::NotifyConsumed(
    const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap)
{
    if (handler_ == nullptr) {
        ANS_LOGE("handler is nullptr");
        return;
    }

    AppExecFwk::EventHandler::Callback NotifyConsumedFunc =
        std::bind(&NotificationSubscriberManager::NotifyConsumedInner, this, notification, notificationMap);

    handler_->PostTask(NotifyConsumedFunc);
}

void NotificationSubscriberManager::NotifyCanceled(
    const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap, int deleteReason)
{
    if (handler_ == nullptr) {
        ANS_LOGE("handler is nullptr");
        return;
    }

    AppExecFwk::EventHandler::Callback NotifyCanceledFunc = std::bind(
        &NotificationSubscriberManager::NotifyCanceledInner, this, notification, notificationMap, deleteReason);

    handler_->PostTask(NotifyCanceledFunc);
}

void NotificationSubscriberManager::NotifyUpdated(const sptr<NotificationSortingMap> &notificationMap)
{
    if (handler_ == nullptr) {
        ANS_LOGE("handler is nullptr");
        return;
    }

    AppExecFwk::EventHandler::Callback NotifyUpdatedFunc =
        std::bind(&NotificationSubscriberManager::NotifyUpdatedInner, this, notificationMap);

    handler_->PostTask(NotifyUpdatedFunc);
}

void NotificationSubscriberManager::NotifyDoNotDisturbDateChanged(const sptr<NotificationDoNotDisturbDate> &date)
{
    if (handler_ == nullptr) {
        ANS_LOGE("handler is nullptr");
        return;
    }

    AppExecFwk::EventHandler::Callback func =
        std::bind(&NotificationSubscriberManager::NotifyDoNotDisturbDateChangedInner, this, date);

    handler_->PostTask(func);
}

void NotificationSubscriberManager::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    ANS_LOGI("OnRemoteDied");
    handler_->PostSyncTask(std::bind([this, object]() {
        std::shared_ptr<SubscriberRecord> record = FindSubscriberRecord(object);
        if (record != nullptr) {
            ANS_LOGW("subscriber removed.");
            subscriberRecordList_.remove(record);
        }
    }),
        AppExecFwk::EventQueue::Priority::HIGH);
}

std::shared_ptr<NotificationSubscriberManager::SubscriberRecord> NotificationSubscriberManager::FindSubscriberRecord(
    const wptr<IRemoteObject> &object)
{
    auto iter = subscriberRecordList_.begin();

    for (; iter != subscriberRecordList_.end(); iter++) {
        if ((*iter)->subscriber->AsObject() == object) {
            return (*iter);
        }
    }
    return nullptr;
}

std::shared_ptr<NotificationSubscriberManager::SubscriberRecord> NotificationSubscriberManager::FindSubscriberRecord(
    const sptr<IAnsSubscriber> &subscriber)
{
    auto iter = subscriberRecordList_.begin();

    for (; iter != subscriberRecordList_.end(); iter++) {
        if ((*iter)->subscriber->AsObject() == subscriber->AsObject()) {
            return (*iter);
        }
    }
    return nullptr;
}

std::shared_ptr<NotificationSubscriberManager::SubscriberRecord> NotificationSubscriberManager::CreateSubscriberRecord(
    const sptr<IAnsSubscriber> &subscriber)
{
    std::shared_ptr<SubscriberRecord> record = std::make_shared<SubscriberRecord>();
    if (record != nullptr) {
        record->subscriber = subscriber;
    }
    return record;
}

void NotificationSubscriberManager::AddRecordInfo(
    std::shared_ptr<SubscriberRecord> &record, const sptr<NotificationSubscribeInfo> &subscribeInfo)
{
    if (subscribeInfo != nullptr) {
        record->bundleList_.clear();
        record->subscribedAll = true;
        for (auto bundle : subscribeInfo->GetAppNames()) {
            record->bundleList_.insert(bundle);
            record->subscribedAll = false;
        }
        record->userId = subscribeInfo->GetAppUserId();
        if (record->userId == SUBSCRIBE_USER_INIT ||
            (record->userId >= SUBSCRIBE_USER_SYSTEM_BEGIN && record->userId <= SUBSCRIBE_USER_SYSTEM_END)) {
            record->userId = SUBSCRIBE_USER_ALL;
        }
    } else {
        record->bundleList_.clear();
        record->subscribedAll = true;
    }
}

void NotificationSubscriberManager::RemoveRecordInfo(
    std::shared_ptr<SubscriberRecord> &record, const sptr<NotificationSubscribeInfo> &subscribeInfo)
{
    if (subscribeInfo != nullptr) {
        for (auto bundle : subscribeInfo->GetAppNames()) {
            if (record->subscribedAll) {
                record->bundleList_.insert(bundle);
            } else {
                record->bundleList_.erase(bundle);
            }
        }
    } else {
        record->bundleList_.clear();
        record->subscribedAll = false;
    }
}

ErrCode NotificationSubscriberManager::AddSubscriberInner(
    const sptr<IAnsSubscriber> &subscriber, const sptr<NotificationSubscribeInfo> &subscribeInfo)
{
    std::shared_ptr<SubscriberRecord> record = FindSubscriberRecord(subscriber);
    if (record == nullptr) {
        record = CreateSubscriberRecord(subscriber);
        if (record == nullptr) {
            ANS_LOGE("CreateSubscriberRecord failed.");
            return ERR_ANS_NO_MEMORY;
        }
        subscriberRecordList_.push_back(record);

        record->subscriber->AsObject()->AddDeathRecipient(recipient_);

        record->subscriber->OnConnected();
        ANS_LOGI("subscriber is connected.");
    }

    AddRecordInfo(record, subscribeInfo);

    return ERR_OK;
}

ErrCode NotificationSubscriberManager::RemoveSubscriberInner(
    const sptr<IAnsSubscriber> &subscriber, const sptr<NotificationSubscribeInfo> &subscribeInfo)
{
    std::shared_ptr<SubscriberRecord> record = FindSubscriberRecord(subscriber);

    if (record == nullptr) {
        ANS_LOGE("subscriber not found.");
        return ERR_ANS_INVALID_PARAM;
    }

    RemoveRecordInfo(record, subscribeInfo);

    if (record->subscribedAll == false && record->bundleList_.empty()) {
        record->subscriber->AsObject()->RemoveDeathRecipient(recipient_);

        subscriberRecordList_.remove(record);

        record->subscriber->OnDisconnected();
        ANS_LOGI("subscriber is disconnected.");
    }

    return ERR_OK;
}

void NotificationSubscriberManager::NotifyConsumedInner(
    const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap)
{
    for (auto record : subscriberRecordList_) {
        auto BundleNames = notification->GetBundleName();
        auto iter = std::find(record->bundleList_.begin(), record->bundleList_.end(), BundleNames);
        if (!record->subscribedAll == (iter != record->bundleList_.end()) &&
            (notification->GetUserId() == record->userId ||
                notification->GetUserId() == SUBSCRIBE_USER_ALL ||
                record->userId == SUBSCRIBE_USER_ALL)) {
            record->subscriber->OnConsumed(notification, notificationMap);
            record->subscriber->OnConsumed(notification);
        }
    }
}

void NotificationSubscriberManager::NotifyCanceledInner(
    const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap, int deleteReason)
{
    for (auto record : subscriberRecordList_) {
        auto BundleNames = notification->GetBundleName();

        auto iter = std::find(record->bundleList_.begin(), record->bundleList_.end(), BundleNames);
        if (!record->subscribedAll == (iter != record->bundleList_.end()) &&
            (notification->GetUserId() == record->userId || notification->GetUserId() == SUBSCRIBE_USER_ALL)) {
            record->subscriber->OnCanceled(notification, notificationMap, deleteReason);
            record->subscriber->OnCanceled(notification);
        }
    }
}

void NotificationSubscriberManager::NotifyUpdatedInner(const sptr<NotificationSortingMap> &notificationMap)
{
    for (auto record : subscriberRecordList_) {
        record->subscriber->OnUpdated(notificationMap);
    }
}

void NotificationSubscriberManager::NotifyDoNotDisturbDateChangedInner(const sptr<NotificationDoNotDisturbDate> &date)
{
    for (auto record : subscriberRecordList_) {
        record->subscriber->OnDoNotDisturbDateChange(date);
    }
}
}  // namespace Notification
}  // namespace OHOS
