/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "distributed.h"

namespace OHOS {
namespace NotificationNapi {
const int ENABLED_MAX_PARA = 2;
const int ENABLED_MIN_PARA = 1;
const int ENABLED_BUNDLE_MAX_PARA = 3;
const int ENABLED_BUNDLE_MIN_PARA = 2;
const int IS_ENABLED_BUNDLE_MAX_PARA = 2;
const int IS_ENABLED_BUNDLE_MIN_PARA = 1;

struct AsyncCallbackInfoIsEnabled {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    CallbackPromiseInfo info;
    bool enable = false;
};

struct EnabledParams {
    napi_ref callback = nullptr;
    bool enable = false;
};

struct AsyncCallbackInfoEnabled {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    CallbackPromiseInfo info;
    EnabledParams params;
};

struct EnabledByBundleParams {
    NotificationBundleOption option;
    napi_ref callback = nullptr;
    bool enable = false;
};

struct AsyncCallbackInfoEnabledByBundle {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    CallbackPromiseInfo info;
    EnabledByBundleParams params;
};

struct IsEnabledByBundleParams {
    NotificationBundleOption option;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoIsEnabledByBundle {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    CallbackPromiseInfo info;
    IsEnabledByBundleParams params;
    bool enable = false;
};

struct AsyncCallbackInfoGetRemindType {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    CallbackPromiseInfo info;
    NotificationConstant::RemindType remindType = NotificationConstant::RemindType::NONE;
};

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, EnabledParams &params)
{
    ANS_LOGI("enter");

    size_t argc = ENABLED_MAX_PARA;
    napi_value argv[ENABLED_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ENABLED_MIN_PARA, "Wrong number of arguments");
    napi_valuetype valuetype = napi_undefined;

    // argv[0]: enable
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
    napi_get_value_bool(env, argv[PARAM0], &params.enable);

    // argv[1]:callback
    if (argc >= ENABLED_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, EnabledByBundleParams &params)
{
    ANS_LOGI("enter");

    size_t argc = ENABLED_BUNDLE_MAX_PARA;
    napi_value argv[ENABLED_BUNDLE_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ENABLED_BUNDLE_MIN_PARA, "Wrong number of arguments");

    // argv[0]: bundle
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    auto retValue = Common::GetBundleOption(env, argv[PARAM0], params.option);
    if (retValue == nullptr) {
        ANS_LOGE("GetBundleOption failed.");
        return nullptr;
    }

    // argv[1]: enable
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
    napi_get_value_bool(env, argv[PARAM1], &params.enable);

    // argv[2]:callback
    if (argc >= ENABLED_BUNDLE_MAX_PARA) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM2], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, IsEnabledByBundleParams &params)
{
    ANS_LOGI("enter");

    size_t argc = IS_ENABLED_BUNDLE_MAX_PARA;
    napi_value argv[IS_ENABLED_BUNDLE_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= IS_ENABLED_BUNDLE_MIN_PARA, "Wrong number of arguments");

    // argv[0]: bundle
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    auto retValue = Common::GetBundleOption(env, argv[PARAM0], params.option);
    if (retValue == nullptr) {
        ANS_LOGE("GetBundleOption failed.");
        return nullptr;
    }

    // argv[1]:callback
    if (argc >= IS_ENABLED_BUNDLE_MAX_PARA) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

void AsyncCompleteCallbackIsDistributedEnabled(napi_env env, napi_status status, void *data)
{
    ANS_LOGI("enter");
    if (!data) {
        ANS_LOGE("Invalid async callback data");
        return;
    }
    ANS_LOGI("IsDistributedEnabled napi_create_async_work end");
    AsyncCallbackInfoIsEnabled *asynccallbackinfo = (AsyncCallbackInfoIsEnabled *)data;
    napi_value result = nullptr;
    if (asynccallbackinfo->info.errorCode != ERR_OK) {
        result = Common::NapiGetNull(env);
    } else {
        napi_get_boolean(env, asynccallbackinfo->enable, &result);
    }

    Common::ReturnCallbackPromise(env, asynccallbackinfo->info, result);

    if (asynccallbackinfo->info.callback != nullptr) {
        napi_delete_reference(env, asynccallbackinfo->info.callback);
    }

    napi_delete_async_work(env, asynccallbackinfo->asyncWork);
    if (asynccallbackinfo) {
        delete asynccallbackinfo;
        asynccallbackinfo = nullptr;
    }
}

napi_value IsDistributedEnabled(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    napi_ref callback = nullptr;
    if (Common::ParseParaOnlyCallback(env, info, callback) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoIsEnabled *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoIsEnabled {.env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "isDistributedEnabled", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("IsDistributedEnabled napi_create_async_work start");
            AsyncCallbackInfoIsEnabled *asynccallbackinfo = (AsyncCallbackInfoIsEnabled *)data;

            asynccallbackinfo->info.errorCode =
                NotificationHelper::IsDistributedEnabled(asynccallbackinfo->enable);
            ANS_LOGI("IsDistributedEnabled enable = %{public}d", asynccallbackinfo->enable);
        },
        AsyncCompleteCallbackIsDistributedEnabled,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value EnableDistributed(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    EnabledParams params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoEnabled *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoEnabled {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "enableDistributed", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("EnableDistributed napi_create_async_work start");
            AsyncCallbackInfoEnabled *asynccallbackinfo = (AsyncCallbackInfoEnabled *)data;

            asynccallbackinfo->info.errorCode =
                NotificationHelper::EnableDistributed(asynccallbackinfo->params.enable);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("EnableDistributed napi_create_async_work end");
            AsyncCallbackInfoEnabled *asynccallbackinfo = (AsyncCallbackInfoEnabled *)data;

            Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));

            if (asynccallbackinfo->info.callback != nullptr) {
                napi_delete_reference(env, asynccallbackinfo->info.callback);
            }

            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value EnableDistributedByBundle(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    EnabledByBundleParams params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoEnabledByBundle *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoEnabledByBundle {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "enableDistributedByBundle", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("EnableDistributedByBundle napi_create_async_work start");
            AsyncCallbackInfoEnabledByBundle *asynccallbackinfo = (AsyncCallbackInfoEnabledByBundle *)data;

            asynccallbackinfo->info.errorCode = NotificationHelper::EnableDistributedByBundle(
                asynccallbackinfo->params.option, asynccallbackinfo->params.enable);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("EnableDistributedByBundle napi_create_async_work end");
            AsyncCallbackInfoEnabledByBundle *asynccallbackinfo = (AsyncCallbackInfoEnabledByBundle *)data;

            Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));

            if (asynccallbackinfo->info.callback != nullptr) {
                napi_delete_reference(env, asynccallbackinfo->info.callback);
            }

            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value EnableDistributedSelf(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    EnabledParams params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoEnabled *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoEnabled {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "enableDistributedSelf", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("EnableDistributedSelf napi_create_async_work start");
            AsyncCallbackInfoEnabled *asynccallbackinfo = (AsyncCallbackInfoEnabled *)data;

            asynccallbackinfo->info.errorCode =
                NotificationHelper::EnableDistributedSelf(asynccallbackinfo->params.enable);
            ANS_LOGI("EnableDistributedSelf enable = %{public}d", asynccallbackinfo->params.enable);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("EnableDistributedSelf napi_create_async_work end");
            AsyncCallbackInfoEnabled *asynccallbackinfo = (AsyncCallbackInfoEnabled *)data;

            Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));

            if (asynccallbackinfo->info.callback != nullptr) {
                napi_delete_reference(env, asynccallbackinfo->info.callback);
            }

            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

void AsyncCompleteCallbackIsDistributedEnableByBundle(napi_env env, napi_status status, void *data)
{
    ANS_LOGI("enter");
    if (!data) {
        ANS_LOGE("Invalid async callback data");
        return;
    }
    ANS_LOGI("IsDistributedEnableByBundle napi_create_async_work end");
    AsyncCallbackInfoIsEnabledByBundle *asynccallbackinfo = (AsyncCallbackInfoIsEnabledByBundle *)data;
    napi_value result = nullptr;
    if (asynccallbackinfo->info.errorCode != ERR_OK) {
        result = Common::NapiGetNull(env);
    } else {
        napi_get_boolean(env, asynccallbackinfo->enable, &result);
    }
    Common::ReturnCallbackPromise(env, asynccallbackinfo->info, result);

    if (asynccallbackinfo->info.callback != nullptr) {
        napi_delete_reference(env, asynccallbackinfo->info.callback);
    }

    napi_delete_async_work(env, asynccallbackinfo->asyncWork);
    if (asynccallbackinfo) {
        delete asynccallbackinfo;
        asynccallbackinfo = nullptr;
    }
}

napi_value IsDistributedEnableByBundle(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    IsEnabledByBundleParams params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoIsEnabledByBundle *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoIsEnabledByBundle {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "isDistributedEnableByBundle", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("IsDistributedEnableByBundle napi_create_async_work start");
            AsyncCallbackInfoIsEnabledByBundle *asynccallbackinfo = (AsyncCallbackInfoIsEnabledByBundle *)data;

            asynccallbackinfo->info.errorCode = NotificationHelper::IsDistributedEnableByBundle(
                asynccallbackinfo->params.option, asynccallbackinfo->enable);
        },
        AsyncCompleteCallbackIsDistributedEnableByBundle,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

void AsyncCompleteCallbackGetDeviceRemindType(napi_env env, napi_status status, void *data)
{
    ANS_LOGI("enter");
    if (!data) {
        ANS_LOGE("Invalid async callback data");
        return;
    }
    ANS_LOGI("GetDeviceRemindType napi_create_async_work end");
    AsyncCallbackInfoGetRemindType *asynccallbackinfo = (AsyncCallbackInfoGetRemindType *)data;
    napi_value result = nullptr;
    if (asynccallbackinfo->info.errorCode != ERR_OK) {
        result = Common::NapiGetNull(env);
    } else {
        DeviceRemindType outType = DeviceRemindType::IDLE_DONOT_REMIND;
        if (!Common::DeviceRemindTypeCToJS(asynccallbackinfo->remindType, outType)) {
            asynccallbackinfo->info.errorCode = ERROR;
            result = Common::NapiGetNull(env);
        }
        napi_create_int32(env, (int32_t)outType, &result);
    }
    Common::ReturnCallbackPromise(env, asynccallbackinfo->info, result);

    if (asynccallbackinfo->info.callback != nullptr) {
        napi_delete_reference(env, asynccallbackinfo->info.callback);
    }

    napi_delete_async_work(env, asynccallbackinfo->asyncWork);
    if (asynccallbackinfo) {
        delete asynccallbackinfo;
        asynccallbackinfo = nullptr;
    }
}

napi_value GetDeviceRemindType(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    napi_ref callback = nullptr;
    if (Common::ParseParaOnlyCallback(env, info, callback) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoGetRemindType *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoGetRemindType {.env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getDeviceRemindType", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetDeviceRemindType napi_create_async_work start");
            AsyncCallbackInfoGetRemindType *asynccallbackinfo = (AsyncCallbackInfoGetRemindType *)data;
            asynccallbackinfo->info.errorCode =
                NotificationHelper::GetDeviceRemindType(asynccallbackinfo->remindType);
        },
        AsyncCompleteCallbackGetDeviceRemindType,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}
}  // namespace NotificationNapi
}  // namespace OHOS