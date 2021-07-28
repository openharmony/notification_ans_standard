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

#include "subscribe.h"

namespace OHOS {
namespace NotificationNapi {

const int SUBSRIBE_MAX_PARA = 3;

struct AsyncCallbackInfoSubscribe {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = 0;
    napi_deferred deferred;
    SubscriberInstance *objectInfo = nullptr;
    NotificationSubscriberInfo subscriberInfo;
    bool isCallback = false;
    int errorCode = 0;
};

napi_value ParseParameters(const napi_env &env, const napi_value (&argv)[SUBSRIBE_MAX_PARA], const size_t &argc,
    NotificationSubscriberInfo &subscriberInfo, SubscriberInstance *&objectInfo, napi_ref &callback)
{

    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
    napi_valuetype valuetype;

    // argv[0]:Subscriber
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type for arg0. Subscribe expected.");

    napi_unwrap(env, argv[0], (void **)&objectInfo);
    if (!objectInfo) {
        ANS_LOGE("ParseParameters subscriberInstance is null");
        return nullptr;
    }

    ANS_LOGI("ParseParameters subscriberInstance = %{public}p start", objectInfo);

    if (!HasNotificationSubscriber(objectInfo)) {
        return nullptr;
    }
    ANS_LOGI("================ParseParameters 1111111111111111111111=================");
    // argv[1]:callback/NotificationSubscriberInfo
    if (argc >= 2) {
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        if (valuetype == napi_function) {
            napi_create_reference(env, argv[1], 1, &callback);
        } else if (valuetype == napi_object) {
            if (Common::GetNotificationSubscriberInfo(env, argv[1], subscriberInfo) == nullptr) {
                ANS_LOGE("ParseParameters SubscriberInfo parse failed");
                return nullptr;
            }
        }
    }
    ANS_LOGI("================ParseParameters 22222222222222222222222222=================");
    // argv[2]:callback
    if (argc >= SUBSRIBE_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[2], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        ANS_LOGI("================ParseParameters 33333333333333333333333=================");
        napi_create_reference(env, argv[2], 1, &callback);
    }
    ANS_LOGI("================ParseParameters 444444444444444444444444444=================");
    return Common::NapiGetNull(env);
}

void PaddingAsyncCallbackInfoIs(
    const napi_env &env, AsyncCallbackInfoSubscribe *&asynccallbackinfo, const napi_ref &callback, napi_value &promise)
{
    ANS_LOGI("PaddingAsyncCallbackInfoIs start");

    if (callback) {
        asynccallbackinfo->callback = callback;
        asynccallbackinfo->isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        asynccallbackinfo->deferred = deferred;
        asynccallbackinfo->isCallback = false;
    }
}

napi_value Subscribe(napi_env env, napi_callback_info info)
{
    ANS_LOGI("Subscribe start");

    size_t argc = SUBSRIBE_MAX_PARA;
    napi_value argv[SUBSRIBE_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));

    napi_ref callback = nullptr;

    SubscriberInstance *objectInfo = nullptr;
    NotificationSubscriberInfo subscriberInfo;
    if (ParseParameters(env, argv, argc, subscriberInfo, objectInfo, callback) == nullptr) {
        return Common::JSParaError(env, callback);
    }
    ANS_LOGI("Subscribe objectInfo = %{public}p", objectInfo);

    AsyncCallbackInfoSubscribe *asynccallbackinfo = new (std::nothrow) AsyncCallbackInfoSubscribe{
        .env = env, .asyncWork = nullptr, .objectInfo = objectInfo, .subscriberInfo = subscriberInfo};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIs(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "subscribeNotification", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Subscribe napi_create_async_work start");
            AsyncCallbackInfoSubscribe *asynccallbackinfo = (AsyncCallbackInfoSubscribe *)data;
            if (asynccallbackinfo->subscriberInfo.hasSubscriberInfo) {
                ANS_LOGI("Subscribe There are NotificationSubscribeInfo");
                NotificationSubscribeInfo subscribeInfo;
                subscribeInfo.AddAppNames(asynccallbackinfo->subscriberInfo.bundleNames);
                asynccallbackinfo->errorCode =
                    NotificationHelper::SubscribeNotification(*(asynccallbackinfo->objectInfo), subscribeInfo);
            } else {
                asynccallbackinfo->errorCode =
                    NotificationHelper::SubscribeNotification(*(asynccallbackinfo->objectInfo));
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Subscribe napi_create_async_work end");
            AsyncCallbackInfoSubscribe *asynccallbackinfo = (AsyncCallbackInfoSubscribe *)data;

            CallbackPromiseInfo info;
            info.isCallback = asynccallbackinfo->isCallback;
            info.callback = asynccallbackinfo->callback;
            info.deferred = asynccallbackinfo->deferred;
            info.errorCode = asynccallbackinfo->errorCode;
            Common::ReturnCallbackPromise(env, info, Common::NapiGetNull(env));

            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

}  // namespace NotificationNapi
}  // namespace OHOS