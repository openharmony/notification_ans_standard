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

#include "unsubscribe.h"
#include "create_subscriber.h"

namespace OHOS {
namespace NotificationNapi {
const int UNSUBSCRIBE_MAX_PARA = 2;

struct AsyncCallbackInfoUnsubscribe {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = 0;
    napi_deferred deferred;
    SubscriberInstance *objectInfo = nullptr;
    bool isCallback = false;
    int errorCode = 0;
};

napi_value ParseParameters(const napi_env &env, const size_t &argc, const napi_value (&argv)[UNSUBSCRIBE_MAX_PARA],
    SubscriberInstance *&objectInfo, napi_ref &callback)
{
    ANS_LOGI("ParseParameters start");

    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");

    napi_valuetype valuetype;
    // argv[0]:subscriber
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type for arg0. Subscribe expected.");

    napi_unwrap(env, argv[0], (void **)&objectInfo);
    if (!objectInfo) {
        ANS_LOGE("ParseParameters objectInfo is null");
        return nullptr;
    }

    ANS_LOGI("ParseParameters objectInfo = %{public}p start", objectInfo);

    if (!HasNotificationSubscriber(objectInfo)) {
        return nullptr;
    }

    // argv[1]:callback
    if (argc >= UNSUBSCRIBE_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &callback);
    }

    return Common::NapiGetNull(env);
}

void PaddingAsyncCallbackInfoIs(const napi_env &env, const size_t &argc,
    AsyncCallbackInfoUnsubscribe *&asynccallbackinfo, const napi_ref &callback, napi_value &promise)
{
    ANS_LOGI("PaddingAsyncCallbackInfoIs start");

    if (argc >= UNSUBSCRIBE_MAX_PARA) {
        asynccallbackinfo->callback = callback;
        asynccallbackinfo->isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        asynccallbackinfo->deferred = deferred;
        asynccallbackinfo->isCallback = false;
    }
}

napi_value Unsubscribe(napi_env env, napi_callback_info info)
{
    ANS_LOGI("Unsubscribe start");

    // Argument parsing
    size_t argc = UNSUBSCRIBE_MAX_PARA;
    napi_value argv[UNSUBSCRIBE_MAX_PARA];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));

    napi_ref callback = 0;
    SubscriberInstance *subscriberInstance = nullptr;
    if (ParseParameters(env, argc, argv, subscriberInstance, callback) == nullptr) {
        return Common::JSParaError(env, callback);
    }

    AsyncCallbackInfoUnsubscribe *asynccallbackinfo = new (std::nothrow)
        AsyncCallbackInfoUnsubscribe{.env = env, .asyncWork = nullptr, .objectInfo = subscriberInstance};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIs(env, argc, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "Unsubscribe", NAPI_AUTO_LENGTH, &resourceName);

    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Unsubscribe napi_create_async_work start");
            AsyncCallbackInfoUnsubscribe *asynccallbackinfo = (AsyncCallbackInfoUnsubscribe *)data;

            asynccallbackinfo->errorCode =
                NotificationHelper::UnSubscribeNotification(*(asynccallbackinfo->objectInfo));
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Unsubscribe napi_create_async_work end");
            AsyncCallbackInfoUnsubscribe *asynccallbackinfo = (AsyncCallbackInfoUnsubscribe *)data;

            CallbackPromiseInfo info;
            info.isCallback = asynccallbackinfo->isCallback;
            info.callback = asynccallbackinfo->callback;
            info.deferred = asynccallbackinfo->deferred;
            info.errorCode = asynccallbackinfo->errorCode;
            Common::ReturnCallbackPromise(env, info, Common::NapiGetNull(env));

            DelAsyncCallbackInfo(asynccallbackinfo->objectInfo);

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