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
#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_SUBSCRIBE_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_SUBSCRIBE_H

#include "common.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;

class SubscriberInstance : public NotificationSubscriber {
public:
    SubscriberInstance();
    virtual ~SubscriberInstance();

    /**
     * @brief Called back when a notification is canceled.
     *
     * @param request Indicates the canceled NotificationRequest object.
     */
    virtual void OnCanceled(const std::shared_ptr<OHOS::Notification::Notification> &request) override;

    /**
     * @brief Called back when a notification is canceled.
     *
     * @param request Indicates the canceled NotificationRequest object.
     * @param sortingMap Indicates the sorting map used by the current subscriber to obtain notification ranking
     * information.
     * @param deleteReason Indicates the reason for the deletion. For details, see NotificationConstant.
     */
    virtual void OnCanceled(const std::shared_ptr<OHOS::Notification::Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap, int32_t deleteReason) override;

    /**
     * @brief Called back when a notification is canceled.
     *
     * @param request Indicates the received NotificationRequest object.
     */
    virtual void OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &request) override;

    /**
     * @brief Called back when a notification is canceled.
     *
     * @param request Indicates the received NotificationRequest object.
     * @param sortingMap Indicates the sorting map used by the current subscriber to obtain notification ranking
     * information.
     */
    virtual void OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap) override;

    /**
     * @brief Called back when a notification is canceled.
     *
     * @param sortingMap Indicates the sorting map used to obtain notification ranking information.
     */
    virtual void OnUpdate(const std::shared_ptr<NotificationSortingMap> &sortingMap) override;

    /**
     * @brief Called back when a notification is canceled.
     *
     */
    virtual void OnConnected() override;

    /**
     * @brief Called back when the subscriber is disconnected from the ANS.
     *
     */
    virtual void OnDisconnected() override;

    /**
     * @brief Called back when connection to the ANS has died.
     *
     */
    virtual void OnDied() override;

    /**
     * @brief Called when the Do Not Disturb mode type changes.
     *
     * @param date Indicates the NotificationDoNotDisturbDate object.
     */
    virtual void OnDoNotDisturbDateChange(const std::shared_ptr<NotificationDoNotDisturbDate> &date) override;

    /**
     * @brief Called when the enabled notification changes.
     *
     * @param callbackData Indicates the EnabledNotificationCallbackData object.
     */
    virtual void OnEnabledNotificationChanged(
        const std::shared_ptr<EnabledNotificationCallbackData> &callbackData) override;

    /**
     * @brief Sets the callback information by type.
     *
     * @param env Indicates the environment that the API is invoked under.
     * @param type Indicates the type of callback.
     * @param ref Indicates the napi_ref of callback.
     */
    void SetCallbackInfo(const napi_env &env, const std::string &type, const napi_ref &ref);

private:
    void SetCancelCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetConsumeCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetUpdateCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetSubscribeCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetUnsubscribeCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetDieCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetDisturbModeCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetDisturbDateCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetEnabledNotificationCallbackInfo(const napi_env &env, const napi_ref &ref);

private:
    struct CallbackInfo {
        napi_env env = nullptr;
        napi_ref ref = nullptr;
    };

    CallbackInfo canceCallbackInfo_;
    CallbackInfo consumeCallbackInfo_;
    CallbackInfo updateCallbackInfo_;
    CallbackInfo subscribeCallbackInfo_;
    CallbackInfo unsubscribeCallbackInfo_;
    CallbackInfo dieCallbackInfo_;
    CallbackInfo disturbModeCallbackInfo_;
    CallbackInfo disturbDateCallbackInfo_;
    CallbackInfo enabledNotificationCallbackInfo_;
};

struct SubscriberInstancesInfo {
    napi_ref ref = nullptr;
    SubscriberInstance *subscriber = nullptr;
};

static std::mutex mutex_;
static thread_local std::vector<SubscriberInstancesInfo> subscriberInstances_;

static std::mutex delMutex_;
static std::vector<SubscriberInstance*> DeletingSubscriber;

bool HasNotificationSubscriber(const napi_env &env, const napi_value &value, SubscriberInstancesInfo &subscriberInfo);
bool AddSubscriberInstancesInfo(const napi_env &env, const SubscriberInstancesInfo &subscriberInfo);
bool DelSubscriberInstancesInfo(const napi_env &env, SubscriberInstance *subscriber);

bool AddDeletingSubscriber(SubscriberInstance *subscriber);
void DelDeletingSubscriber(SubscriberInstance *subscriber);

napi_value Subscribe(napi_env env, napi_callback_info info);
}  // namespace NotificationNapi
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_SUBSCRIBE_H