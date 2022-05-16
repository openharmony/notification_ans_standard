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

#include "ans_subscriber_proxy.h"

#include "ans_inner_errors.h"
#include "ans_log_wrapper.h"
#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
namespace Notification {
AnsSubscriberProxy::AnsSubscriberProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IAnsSubscriber>(impl)
{}

AnsSubscriberProxy::~AnsSubscriberProxy()
{}

ErrCode AnsSubscriberProxy::InnerTransact(
    uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply)
{
    auto remote = Remote();
    if (remote == nullptr) {
        ANS_LOGE("[InnerTransact] fail: get Remote fail code %{public}u", code);
        return ERR_DEAD_OBJECT;
    }

    int32_t err = remote->SendRequest(code, data, reply, flags);
    switch (err) {
        case NO_ERROR: {
            return ERR_OK;
        }
        case DEAD_OBJECT: {
            ANS_LOGE("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return ERR_DEAD_OBJECT;
        }
        default: {
            ANS_LOGE("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return ERR_ANS_TRANSACT_FAILED;
        }
    }
}

void AnsSubscriberProxy::OnConnected()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsSubscriberProxy::GetDescriptor())) {
        ANS_LOGE("[OnConnected] fail: write interface token failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(ON_CONNECTED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[OnConnected] fail: transact ErrCode=ERR_ANS_TRANSACT_FAILED");
        return;
    }
}

void AnsSubscriberProxy::OnDisconnected()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsSubscriberProxy::GetDescriptor())) {
        ANS_LOGE("[OnDisconnected] fail: write interface token failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(ON_DISCONNECTED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[OnDisconnected] fail: transact ErrCode=ERR_ANS_TRANSACT_FAILED");
        return;
    }
}

void AnsSubscriberProxy::OnConsumed(const sptr<Notification> &notification)
{
    if (notification == nullptr) {
        ANS_LOGE("[OnConsumed] fail: notification is nullptr.");
        return;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsSubscriberProxy::GetDescriptor())) {
        ANS_LOGE("[OnConsumed] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(notification)) {
        ANS_LOGE("[OnConsumed] fail: write notification failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(ON_CONSUMED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[OnConsumed] fail: transact ErrCode=ERR_ANS_TRANSACT_FAILED");
        return;
    }
}

void AnsSubscriberProxy::OnConsumed(
    const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap)
{
    if (notification == nullptr) {
        ANS_LOGE("[OnConsumed] fail: notification is nullptr.");
        return;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsSubscriberProxy::GetDescriptor())) {
        ANS_LOGE("[OnConsumed] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(notification)) {
        ANS_LOGE("[OnConsumed] fail: write notification failed.");
        return;
    }

    if (!data.WriteBool(notificationMap != nullptr)) {
        ANS_LOGE("[OnConsumed] fail: write existMap failed");
        return;
    }

    if (notificationMap != nullptr) {
        if (!data.WriteParcelable(notificationMap)) {
            ANS_LOGE("[OnConsumed] fail: write notificationMap failed");
            return;
        }
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(ON_CONSUMED_MAP, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[OnConsumed] fail: transact ErrCode=ERR_ANS_TRANSACT_FAILED");
        return;
    }
}

void AnsSubscriberProxy::OnCanceled(const sptr<Notification> &notification)
{
    if (notification == nullptr) {
        ANS_LOGE("[OnCanceled] fail: notification is nullptr.");
        return;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsSubscriberProxy::GetDescriptor())) {
        ANS_LOGE("[OnCanceled] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(notification)) {
        ANS_LOGE("[OnCanceled] fail: write notification failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(ON_CANCELED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[OnCanceled] fail: transact ErrCode=ERR_ANS_TRANSACT_FAILED");
        return;
    }
}

void AnsSubscriberProxy::OnCanceled(
    const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap, int32_t deleteReason)
{
    if (notification == nullptr) {
        ANS_LOGE("[OnCanceled] fail: notification is nullptr.");
        return;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsSubscriberProxy::GetDescriptor())) {
        ANS_LOGE("[OnCanceled] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(notification)) {
        ANS_LOGE("[OnCanceled] fail: write notification failed.");
        return;
    }

    if (!data.WriteBool(notificationMap != nullptr)) {
        ANS_LOGE("[OnCanceled] fail: write existMap failed");
        return;
    }

    if (notificationMap != nullptr) {
        if (!data.WriteParcelable(notificationMap)) {
            ANS_LOGE("[OnCanceled] fail: write notificationMap failed");
            return;
        }
    }

    if (!data.WriteInt32(deleteReason)) {
        ANS_LOGE("[OnCanceled] fail: write deleteReason failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(ON_CANCELED_MAP, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[OnCanceled] fail: transact ErrCode=ERR_ANS_TRANSACT_FAILED");
        return;
    }
}

void AnsSubscriberProxy::OnUpdated(const sptr<NotificationSortingMap> &notificationMap)
{
    if (notificationMap == nullptr) {
        ANS_LOGE("[OnUpdated] fail: notificationMap is empty.");
        return;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsSubscriberProxy::GetDescriptor())) {
        ANS_LOGE("[OnUpdated] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(notificationMap)) {
        ANS_LOGE("[OnUpdated] fail: write notificationMap failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(ON_UPDATED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[OnUpdated] fail: transact ErrCode=ERR_ANS_TRANSACT_FAILED");
        return;
    }
}

void AnsSubscriberProxy::OnDoNotDisturbDateChange(const sptr<NotificationDoNotDisturbDate> &date)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsSubscriberProxy::GetDescriptor())) {
        ANS_LOGE("[OnDoNotDisturbDateChange] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(date)) {
        ANS_LOGE("[OnDoNotDisturbDateChange] fail: write date failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(ON_DND_DATE_CHANGED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[OnDoNotDisturbDateChange] fail: transact ErrCode=ERR_ANS_TRANSACT_FAILED");
        return;
    }
}

void AnsSubscriberProxy::OnEnabledNotificationChanged(const sptr<EnabledNotificationCallbackData> &callbackData)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(AnsSubscriberProxy::GetDescriptor())) {
        ANS_LOGE("[OnEnabledNotificationChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(callbackData)) {
        ANS_LOGE("[OnEnabledNotificationChanged] fail: write callbackData failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(ON_ENABLED_NOTIFICATION_CHANGED, option, data, reply);
    if (result != ERR_OK) {
        ANS_LOGE("[OnEnabledNotificationChanged] fail: transact ErrCode=ERR_ANS_TRANSACT_FAILED");
        return;
    }
}
}  // namespace Notification
}  // namespace OHOS
