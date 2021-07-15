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
#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_CREATE_SUBSCRIBER_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_CREATE_SUBSCRIBER_H

#include <map>
#include <mutex>

#include "common.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;

const int ON_MAX_PARA = 2;
const int OFF_MAX_PARA = 1;
const std::string CONSUME = "consume";
const std::string CANCEL = "cancel";
const std::string UPDATE = "update";
const std::string SUBSCRIBE = "subscribe";
const std::string UNSUBSCRIBE = "unsubscribe";
const std::string DIE = "die";
const std::string DISTURB_MODE_CHANGE = "disturbModeChange";

class SubscriberInstance;

struct AsyncCallbackInfoOn {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = 0;
    SubscriberInstance *objectInfo;
    std::string type;
};

struct AsyncCallbackInfoOff {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = 0;
    SubscriberInstance *objectInfo;
    std::string type;
};

struct CallbackInfo {
    napi_env env;
    napi_ref ref;
};

struct NotificationReceiveDataWorker {
    napi_env env;
    napi_ref ref = 0;
    std::shared_ptr<OHOS::Notification::Notification> request = nullptr;
    std::shared_ptr<NotificationSortingMap> sortingMap = nullptr;
    int deleteReason = 0;
    int result = 0;
    int disturbMode = 0;
};

class SubscriberInstance : public NotificationSubscriber {
public:
    SubscriberInstance();

    virtual ~SubscriberInstance();

    virtual void OnCanceled(const std::shared_ptr<OHOS::Notification::Notification> &request) override;

    virtual void OnCanceled(const std::shared_ptr<OHOS::Notification::Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap, int deleteReason) override;

    virtual void OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &request) override;

    virtual void OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap) override;

    virtual void OnUpdate(const std::shared_ptr<NotificationSortingMap> &sortingMap) override;

    virtual void OnSubscribeResult(NotificationConstant::SubscribeResult result) override;

    virtual void OnUnsubscribeResult(NotificationConstant::SubscribeResult result) override;

    virtual void OnDied() override;

    virtual void OnDisturbModeChanged(int disturbMode) override;

    void SetCallbackInfo(const napi_env &env, const std::string &type, const napi_ref &ref);

private:
    void SetCancelCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetConsumeCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetUpdateCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetSubscribeCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetUnsubscribeCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetDieCallbackInfo(const napi_env &env, const napi_ref &ref);

    void SetDisturbModeCallbackInfo(const napi_env &env, const napi_ref &ref);

private:
    CallbackInfo canceCallbackInfo_;
    CallbackInfo consumeCallbackInfo_;
    CallbackInfo updateCallbackInfo_;
    CallbackInfo subscribeCallbackInfo_;
    CallbackInfo unsubscribeCallbackInfo_;
    CallbackInfo dieCallbackInfo_;
    CallbackInfo disturbModeCallbackInfo_;
};

napi_value SetSubscriberCallbackData(
    const napi_env &env, const std::shared_ptr<OHOS::Notification::Notification> &request, napi_value &result);

napi_value SetSubscriberCallbackData(const napi_env &env,
    const std::shared_ptr<OHOS::Notification::Notification> &request,
    const std::shared_ptr<NotificationSortingMap> &sortingMap, int deleteReason, napi_value &result);

napi_value NotificationSubscriberInit(napi_env env, napi_value exports);

napi_value On(napi_env env, napi_callback_info info);

napi_value ParseParametersByOn(const napi_env &env, const size_t &argc, const napi_value (&argv)[ON_MAX_PARA],
    std::string &type, napi_ref &callback);

napi_value Off(napi_env env, napi_callback_info info);

napi_value ParseParametersByOff(
    const napi_env &env, const size_t &argc, const napi_value (&argv)[OFF_MAX_PARA], std::string &type);

napi_value SubscriberConstructor(napi_env env, napi_callback_info info);

napi_value CreateSubscriber(napi_env env, napi_callback_info info);

bool CreateSubscriberInstanceRecord(SubscriberInstance **subscriber);

bool AddAsyncCallbackInfoOn(SubscriberInstance *subscriber, AsyncCallbackInfoOn *asynccallbackinfo);

bool DelAsyncCallbackInfoOn(SubscriberInstance *subscriber, const std::string &type = "");

bool HasNotificationSubscriber(SubscriberInstance *subscriber);

static std::mutex mutex_;
static napi_value g_NotificationSubscriber;
static std::map<SubscriberInstance *, std::vector<AsyncCallbackInfoOn *>> g_SubscriberInstances;

}  // namespace NotificationNapi
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_CREATE_SUBSCRIBER_H