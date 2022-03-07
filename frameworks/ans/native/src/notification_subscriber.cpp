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

#include "notification_subscriber.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Notification {
NotificationSubscriber::NotificationSubscriber()
{
    impl_ = new (std::nothrow) SubscriberImpl(*this);
};

NotificationSubscriber::~NotificationSubscriber()
{}

const sptr<NotificationSubscriber::SubscriberImpl> NotificationSubscriber::GetImpl() const
{
    return impl_;
}

NotificationSubscriber::SubscriberImpl::SubscriberImpl(NotificationSubscriber &subscriber) : subscriber_(subscriber)
{
    recipient_ = new (std::nothrow) DeathRecipient(*this);
};

void NotificationSubscriber::SubscriberImpl::OnConnected()
{
    if (GetAnsManagerProxy()) {
        proxy_->AsObject()->AddDeathRecipient(recipient_);
        ANS_LOGD("%s, Add death recipient.", __func__);
    }
    subscriber_.OnConnected();
}

void NotificationSubscriber::SubscriberImpl::OnDisconnected()
{
    if (GetAnsManagerProxy()) {
        proxy_->AsObject()->RemoveDeathRecipient(recipient_);
        ANS_LOGD("%s, Remove death recipient.", __func__);
    }
    subscriber_.OnDisconnected();
}

void NotificationSubscriber::SubscriberImpl::OnConsumed(const sptr<Notification> &notification)
{
    subscriber_.OnConsumed(std::make_shared<Notification>(*notification));
}

void NotificationSubscriber::SubscriberImpl::OnConsumed(
    const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap)
{
    subscriber_.OnConsumed(
        std::make_shared<Notification>(*notification), std::make_shared<NotificationSortingMap>(*notificationMap));
}

void NotificationSubscriber::SubscriberImpl::OnCanceled(const sptr<Notification> &notification)
{
    subscriber_.OnCanceled(std::make_shared<Notification>(*notification));
}

void NotificationSubscriber::SubscriberImpl::OnCanceled(
    const sptr<Notification> &notification, const sptr<NotificationSortingMap> &notificationMap, int deleteReason)
{
    subscriber_.OnCanceled(std::make_shared<Notification>(*notification),
        std::make_shared<NotificationSortingMap>(*notificationMap),
        deleteReason);
}

void NotificationSubscriber::SubscriberImpl::OnUpdated(const sptr<NotificationSortingMap> &notificationMap)
{
    subscriber_.OnUpdate(std::make_shared<NotificationSortingMap>(*notificationMap));
}

void NotificationSubscriber::SubscriberImpl::OnDoNotDisturbDateChange(const sptr<NotificationDoNotDisturbDate> &date)
{
    subscriber_.OnDoNotDisturbDateChange(std::make_shared<NotificationDoNotDisturbDate>(*date));
}

void NotificationSubscriber::SubscriberImpl::OnEnabledNotificationChanged(
    const sptr<EnabledNotificationCallbackData> &callbackData)
{
    subscriber_.OnEnabledNotificationChanged(std::make_shared<EnabledNotificationCallbackData>(*callbackData));
}

bool NotificationSubscriber::SubscriberImpl::GetAnsManagerProxy()
{
    if (proxy_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (proxy_ == nullptr) {
            sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (!systemAbilityManager) {
                return false;
            }

            sptr<IRemoteObject> remoteObject =
                systemAbilityManager->GetSystemAbility(ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID);
            if (!remoteObject) {
                return false;
            }

            proxy_ = iface_cast<IAnsManager>(remoteObject);
            if ((proxy_ == nullptr) || (proxy_->AsObject() == nullptr)) {
                return false;
            }
        }
    }

    return true;
}

NotificationSubscriber::SubscriberImpl::DeathRecipient::DeathRecipient(SubscriberImpl &subscriberImpl)
    : subscriberImpl_(subscriberImpl) {};

NotificationSubscriber::SubscriberImpl::DeathRecipient::~DeathRecipient() {};

void NotificationSubscriber::SubscriberImpl::DeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    subscriberImpl_.proxy_ = nullptr;
    subscriberImpl_.subscriber_.OnDied();
}
}  // namespace Notification
}  // namespace OHOS