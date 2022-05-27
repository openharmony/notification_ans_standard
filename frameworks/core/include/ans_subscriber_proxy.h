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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_ANS_SUBSCRIBER_PROXY_H
#define BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_ANS_SUBSCRIBER_PROXY_H

#include "ans_subscriber_interface.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Notification {
class AnsSubscriberProxy : public IRemoteProxy<AnsSubscriberInterface> {
public:
    AnsSubscriberProxy() = delete;
    explicit AnsSubscriberProxy(const sptr<IRemoteObject> &impl);
    ~AnsSubscriberProxy() override;
    DISALLOW_COPY_AND_MOVE(AnsSubscriberProxy);

    /**
     * @brief The callback function for the subscriber to establish a connection.
     */
    void OnConnected() override;

    /**
     * @brief The callback function for subscriber disconnected.
     */
    void OnDisconnected() override;

    /**
     * @brief The callback function on a notification published.
     *
     * @param notification Indicates the consumed notification.
     */
    void OnConsumed(const sptr<Notification> &notification) override;

    /**
     * @brief The callback function on a notification published.
     *
     * @param notification Indicates the consumed notification.
     * @param notificationMap Indicates the NotificationSortingMap object.
     */
    void OnConsumed(
        const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap) override;

    /**
     * @brief The callback function on a notification canceled.
     *
     * @param notification Indicates the canceled notification.
     */
    void OnCanceled(const sptr<Notification> &notification) override;

    /**
     * @brief The callback function on a notification canceled.
     *
     * @param notification Indicates the canceled notification.
     * @param notificationMap Indicates the NotificationSortingMap object.
     * @param deleteReason Indicates the delete reason.
     */
    void OnCanceled(const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap,
        int32_t deleteReason) override;

    /**
     * @brief The callback function on the notifications updated.
     *
     * @param notificationMap Indicates the NotificationSortingMap object.
     */
    void OnUpdated(const sptr<NotificationSortingMap> &notificationMap) override;

    /**
     * @brief The callback function on the do not disturb date changed.
     *
     * @param date Indicates the NotificationDoNotDisturbDate object.
     */
    void OnDoNotDisturbDateChange(const sptr<NotificationDoNotDisturbDate> &date) override;

    /**
     * @brief The callback function on the notification enabled flag changed.
     *
     * @param callbackData Indicates the EnabledNotificationCallbackData object.
     */
    void OnEnabledNotificationChanged(const sptr<EnabledNotificationCallbackData> &callbackData) override;

private:
    ErrCode InnerTransact(uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply);
    static inline BrokerDelegator<AnsSubscriberProxy> delegator_;
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_ANS_SUBSCRIBER_PROXY_H
