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

#include "remove.h"

namespace OHOS {
namespace NotificationNapi {
const int REMOVE_OR_BUNDLE_MAX_PARA = 2;
const int REMOVE_ALL_MAX_PARA = 1;

struct AsyncCallbackInfoRemove {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = nullptr;
    napi_deferred deferred;
    std::string strValue;
    bool isCallback = false;
    int errorCode = 0;
};

napi_value ParseParameters(const napi_env &env, const napi_value (&argv)[REMOVE_OR_BUNDLE_MAX_PARA], const size_t &argc,
    std::string &strValue, napi_ref &callback)
{

    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
    // argv[0]: hashCode/bundleName
    size_t strLen;
    char str[STR_MAX_SIZE] = {0};
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], str, STR_MAX_SIZE - 1, &strLen));
    strValue = str;

    // argv[1]:callback
    if (argc >= REMOVE_OR_BUNDLE_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &callback);
    }
    return Common::NapiGetNull(env);
}

napi_value ParseParametersByRemoveAll(
    const napi_env &env, const napi_value (&argv)[REMOVE_ALL_MAX_PARA], const size_t &argc, napi_ref &callback)
{
    // argv[0]:callback
    napi_valuetype valuetype;
    if (argc >= REMOVE_ALL_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[0], 1, &callback);
    }
    return Common::NapiGetNull(env);
}
void PaddingAsyncCallbackInfoIs(
    const napi_env &env, AsyncCallbackInfoRemove *&asynccallbackinfo, const napi_ref &callback, napi_value &promise)
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

napi_value Remove(napi_env env, napi_callback_info info)
{
    ANS_LOGI("Remove start");

    size_t argc = REMOVE_OR_BUNDLE_MAX_PARA;
    napi_value argv[REMOVE_OR_BUNDLE_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    napi_ref callback = nullptr;

    std::string hashCode;
    if (ParseParameters(env, argv, argc, hashCode, callback) == nullptr) {
        return Common::JSParaError(env, callback);
    }
    ANS_LOGI("Remove hashCode = %{public}s", hashCode.c_str());

    AsyncCallbackInfoRemove *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoRemove{.env = env, .asyncWork = nullptr, .strValue = hashCode};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIs(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "remove", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Remove napi_create_async_work start");
            AsyncCallbackInfoRemove *asynccallbackinfo = (AsyncCallbackInfoRemove *)data;
            asynccallbackinfo->errorCode = NotificationHelper::RemoveNotification(asynccallbackinfo->strValue);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Remove napi_create_async_work end");
            AsyncCallbackInfoRemove *asynccallbackinfo = (AsyncCallbackInfoRemove *)data;

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
napi_value RemoveAsBundle(napi_env env, napi_callback_info info)
{
    ANS_LOGI("RemoveAsBundle start");

    size_t argc = REMOVE_OR_BUNDLE_MAX_PARA;
    napi_value argv[REMOVE_OR_BUNDLE_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));

    napi_ref callback = nullptr;

    std::string bundleName;
    if (ParseParameters(env, argv, argc, bundleName, callback) == nullptr) {
        return Common::JSParaError(env, callback);
    }
    ANS_LOGI("RemoveAsBundle bundleName = %{public}s", bundleName.c_str());

    AsyncCallbackInfoRemove *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoRemove{.env = env, .asyncWork = nullptr, .strValue = bundleName};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIs(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "removeAsBundle", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("removeAsBundle napi_create_async_work start");
            AsyncCallbackInfoRemove *asynccallbackinfo = (AsyncCallbackInfoRemove *)data;
            asynccallbackinfo->errorCode = NotificationHelper::RemoveNotifications(asynccallbackinfo->strValue);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("removeAsBundle napi_create_async_work end");
            AsyncCallbackInfoRemove *asynccallbackinfo = (AsyncCallbackInfoRemove *)data;

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
napi_value RemoveAll(napi_env env, napi_callback_info info)
{
    ANS_LOGI("RemoveAll start");

    size_t argc = REMOVE_ALL_MAX_PARA;
    napi_value argv[REMOVE_ALL_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    napi_ref callback = nullptr;

    if (ParseParametersByRemoveAll(env, argv, argc, callback) == nullptr) {
        return Common::JSParaError(env, callback);
    }

    AsyncCallbackInfoRemove *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoRemove{.env = env, .asyncWork = nullptr};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIs(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "removeAll", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("RemoveAll napi_create_async_work start");
            AsyncCallbackInfoRemove *asynccallbackinfo = (AsyncCallbackInfoRemove *)data;
            asynccallbackinfo->errorCode = NotificationHelper::RemoveNotifications();
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("RemoveAll napi_create_async_work end");
            AsyncCallbackInfoRemove *asynccallbackinfo = (AsyncCallbackInfoRemove *)data;

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