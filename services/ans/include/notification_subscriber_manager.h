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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_INCLUDE_NOTIFICATION_SUBSCRIBER_MANAGER_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_INCLUDE_NOTIFICATION_SUBSCRIBER_MANAGER_H

#include <list>
#include <memory>
#include <mutex>

#include "errors.h"
#include "event_handler.h"
#include "event_runner.h"
#include "nocopyable.h"
#include "refbase.h"
#include "singleton.h"

#include "ans_subscriber_interface.h"
#include "notification_bundle_option.h"
#include "notification_constant.h"
#include "notification_request.h"
#include "notification_sorting_map.h"
#include "notification_subscribe_info.h"

namespace OHOS {
namespace Notification {
class NotificationSubscriberManager : public DelayedSingleton<NotificationSubscriberManager> {
public:
    /**
     * @brief Add a subscriber.
     *
     * @param subscriber Indicates the AnsSubscriberInterface object.
     * @param subscribeInfo Indicates the NotificationSubscribeInfo object.
     * @return Indicates the result code.
     */
    ErrCode AddSubscriber(const sptr<AnsSubscriberInterface> &subscriber,
        const sptr<NotificationSubscribeInfo> &subscribeInfo);

    /**
     * @brief Remove a subscriber.
     *
     * @param subscriber Indicates the AnsSubscriberInterface object.
     * @param subscribeInfo Indicates the NotificationSubscribeInfo object.
     * @return Indicates the result code.
     */
    ErrCode RemoveSubscriber(
        const sptr<AnsSubscriberInterface> &subscriber, const sptr<NotificationSubscribeInfo> &subscribeInfo);

    /**
     * @brief Notify all subscribers on counsumed.
     *
     * @param notification Indicates the Notification object.
     * @param notificationMap Indicates the NotificationSortingMap object.
     */
    void NotifyConsumed(const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap);

    /**
     * @brief Notify all subscribers on canceled.
     *
     * @param notification Indicates the Notification object.
     * @param notificationMap Indicates the NotificationSortingMap object.
     * @param deleteReason Indicates the delete reason.
     */
    void NotifyCanceled(const sptr<Notification> &notification,
        const sptr<NotificationSortingMap> &notificationMap, int32_t deleteReason);

    /**
     * @brief Notify all subscribers on updated.
     *
     * @param notificationMap Indicates the NotificationSortingMap object.
     */
    void NotifyUpdated(const sptr<NotificationSortingMap> &notificationMap);

    /**
     * @brief Notify all subscribers on dnd date changed.
     *
     * @param date Indicates the NotificationDoNotDisturbDate object.
     */
    void NotifyDoNotDisturbDateChanged(const sptr<NotificationDoNotDisturbDate> &date);

    void NotifyEnabledNotificationChanged(const sptr<EnabledNotificationCallbackData> &callbackData);

    /**
     * @brief Obtains the death event.
     *
     * @param object Indicates the death object.
     */
    void OnRemoteDied(const wptr<IRemoteObject> &object);

private:
    struct SubscriberRecord;

    std::shared_ptr<SubscriberRecord> FindSubscriberRecord(const wptr<IRemoteObject> &object);
    std::shared_ptr<SubscriberRecord> FindSubscriberRecord(const sptr<AnsSubscriberInterface> &subscriber);
    std::shared_ptr<SubscriberRecord> CreateSubscriberRecord(const sptr<AnsSubscriberInterface> &subscriber);
    void AddRecordInfo(
        std::shared_ptr<SubscriberRecord> &record, const sptr<NotificationSubscribeInfo> &subscribeInfo);
    void RemoveRecordInfo(
        std::shared_ptr<SubscriberRecord> &record, const sptr<NotificationSubscribeInfo> &subscribeInfo);
    ErrCode AddSubscriberInner(
        const sptr<AnsSubscriberInterface> &subscriber, const sptr<NotificationSubscribeInfo> &subscribeInfo);
    ErrCode RemoveSubscriberInner(
        const sptr<AnsSubscriberInterface> &subscriber, const sptr<NotificationSubscribeInfo> &subscribeInfo);

    void NotifyConsumedInner(
        const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap);
    void NotifyCanceledInner(const sptr<Notification> &notification,
        const sptr<NotificationSortingMap> &notificationMap, int32_t deleteReason);
    void NotifyUpdatedInner(const sptr<NotificationSortingMap> &notificationMap);
    void NotifyDoNotDisturbDateChangedInner(const sptr<NotificationDoNotDisturbDate> &date);
    void NotifyEnabledNotificationChangedInner(const sptr<EnabledNotificationCallbackData> &callbackData);
    bool IsSystemUser(int32_t userId);

private:
    std::list<std::shared_ptr<SubscriberRecord>> subscriberRecordList_ {};
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_ {};
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_ {};
    sptr<AnsSubscriberInterface> ansSubscriberProxy_ {};
    sptr<IRemoteObject::DeathRecipient> recipient_ {};

    DECLARE_DELAYED_SINGLETON(NotificationSubscriberManager);
    DISALLOW_COPY_AND_MOVE(NotificationSubscriberManager);
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_INCLUDE_NOTIFICATION_SUBSCRIBER_MANAGER_H