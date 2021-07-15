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

#include "cancel.h"

namespace OHOS {
namespace NotificationNapi {
napi_value ParseParameters(const napi_env &env, const napi_value (&argv)[CANCEL_MAX_PARA], const size_t &argc,
    paraInfoCancel &info, napi_ref &callback)
{
    ANS_LOGI("ParseParameters start");

    napi_valuetype valuetype;
    // argv[0]: id: number
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &info.id));

    // argv[1]: label: string / callback
    if (argc >= CANCEL_MAX_PARA - 1) {
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        if (valuetype == napi_string) {
            char str[STR_MAX_SIZE] = {0};
            size_t strLen = 0;
            NAPI_CALL(env, napi_get_value_string_utf8(env, argv[1], str, STR_MAX_SIZE - 1, &strLen));
            info.label = str;
            info.hasLabel = true;
        } else if (valuetype == napi_function) {
            napi_create_reference(env, argv[1], 1, &callback);
        } else {
            return nullptr;
        }
    }

    // argv[2]:callback
    if (argc >= CANCEL_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[CANCEL_MAX_PARA - 1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[CANCEL_MAX_PARA - 1], 1, &callback);
    }

    return Common::NapiGetNull(env);
}

napi_value ParseParametersByAll(
    const napi_env &env, const napi_value (&argv)[CANCEL_ALL_MAX_PARA], const size_t &argc, napi_ref &callback)
{
    ANS_LOGI("ParseParametersByAll start");

    if (argc >= CANCEL_ALL_MAX_PARA) {
        napi_valuetype valuetype;
        // argv[0]:callback
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[0], 1, &callback);
    }
    return Common::NapiGetNull(env);
}

void PaddingAsyncCallbackInfoIs(
    const napi_env &env, AsyncCallbackInfoCancel *&asynccallbackinfo, const napi_ref &callback, napi_value &promise)
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

napi_value Cancel(napi_env env, napi_callback_info info)
{
    ANS_LOGI("Cancel start");

    size_t argc = CANCEL_MAX_PARA;
    napi_value argv[CANCEL_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
    napi_ref callback = nullptr;

    paraInfoCancel paraInfo;
    if (ParseParameters(env, argv, argc, paraInfo, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }
    ANS_LOGI("Cancel id = %{public}d", paraInfo.id);
    ANS_LOGI("Cancel label = %{public}s", paraInfo.label.c_str());

    AsyncCallbackInfoCancel *asynccallbackinfo = new AsyncCallbackInfoCancel{
        .env = env, .asyncWork = nullptr, .id = paraInfo.id, .label = paraInfo.label, .hasLabel = paraInfo.hasLabel};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIs(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "cancel", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Cancel napi_create_async_work start");
            AsyncCallbackInfoCancel *asynccallbackinfo = (AsyncCallbackInfoCancel *)data;

            if (asynccallbackinfo->hasLabel) {
                asynccallbackinfo->errorCode = NotificationHelper::CancelNotification(asynccallbackinfo->id);
            } else {
                asynccallbackinfo->errorCode =
                    NotificationHelper::CancelNotification(asynccallbackinfo->label, asynccallbackinfo->id);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Cancel napi_create_async_work end");
            AsyncCallbackInfoCancel *asynccallbackinfo = (AsyncCallbackInfoCancel *)data;

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

napi_value CancelAll(napi_env env, napi_callback_info info)
{
    ANS_LOGI("CancelAll start");

    size_t argc = CANCEL_ALL_MAX_PARA;
    napi_value argv[CANCEL_ALL_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 0, "Wrong number of arguments");
    napi_ref callback = nullptr;

    if (ParseParametersByAll(env, argv, argc, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }

    AsyncCallbackInfoCancel *asynccallbackinfo = new AsyncCallbackInfoCancel{.env = env, .asyncWork = nullptr};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIs(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "cancelAll", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("CancelAll napi_create_async_work start");
            AsyncCallbackInfoCancel *asynccallbackinfo = (AsyncCallbackInfoCancel *)data;
            asynccallbackinfo->errorCode = NotificationHelper::CancelAllNotifications();
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("CancelAll napi_create_async_work end");
            AsyncCallbackInfoCancel *asynccallbackinfo = (AsyncCallbackInfoCancel *)data;

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