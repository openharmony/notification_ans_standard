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

#include "enable_notification.h"

namespace OHOS {
namespace NotificationNapi {
const int ENABLE_NOTIFICATION_MAX_PARA = 3;
const int ENABLE_NOTIFICATION_MIN_PARA = 2;
const int IS_NOTIFICATION_ENABLE_MAX_PARA = 2;

struct EnableParams {
    NotificationBundleOption option;
    bool enable = false;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoEnable {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    EnableParams params;
    CallbackPromiseInfo info;
};

struct IsEnableParams {
    NotificationBundleOption option;
    napi_ref callback = nullptr;
    bool hasBundleOption = false;
    int32_t userId = SUBSCRIBE_USER_INIT;
    bool hasUserId = false;
};

struct AsyncCallbackInfoIsEnable {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    IsEnableParams params;
    CallbackPromiseInfo info;
    bool allowed = false;
};

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, EnableParams &params)
{
    ANS_LOGI("enter");

    size_t argc = ENABLE_NOTIFICATION_MAX_PARA;
    napi_value argv[ENABLE_NOTIFICATION_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc < ENABLE_NOTIFICATION_MIN_PARA) {
        ANS_LOGW("Wrong number of arguments.");
        return nullptr;
    }

    // argv[0]: bundle
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_object) {
        ANS_LOGW("Wrong argument type. Object expected.");
        return nullptr;
    }
    auto retValue = Common::GetBundleOption(env, argv[PARAM0], params.option);
    if (retValue == nullptr) {
        ANS_LOGE("GetBundleOption failed.");
        return nullptr;
    }

    // argv[1]: enable
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
    if (valuetype != napi_boolean) {
        ANS_LOGW("Wrong argument type. Bool expected.");
        return nullptr;
    }
    napi_get_value_bool(env, argv[PARAM1], &params.enable);

    // argv[2]:callback
    if (argc >= ENABLE_NOTIFICATION_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGW("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, argv[PARAM2], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, IsEnableParams &params)
{
    ANS_LOGI("enter");

    size_t argc = IS_NOTIFICATION_ENABLE_MAX_PARA;
    napi_value argv[IS_NOTIFICATION_ENABLE_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));

    if (argc == 0) {
        return Common::NapiGetNull(env);
    }

    // argv[0]: bundle / userId / callback
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if ((valuetype != napi_object) && (valuetype != napi_number) && (valuetype != napi_function)) {
        ANS_LOGW("Wrong argument type. Function or object expected.");
        return nullptr;
    }
    if (valuetype == napi_object) {
        auto retValue = Common::GetBundleOption(env, argv[PARAM0], params.option);
        if (retValue == nullptr) {
            ANS_LOGE("GetBundleOption failed.");
            return nullptr;
        }
        params.hasBundleOption = true;
    } else if (valuetype == napi_number) {
        NAPI_CALL(env, napi_get_value_int32(env, argv[PARAM0], &params.userId));
        params.hasUserId = true;
    } else {
        napi_create_reference(env, argv[PARAM0], 1, &params.callback);
    }

    // argv[1]:callback
    if (argc >= IS_NOTIFICATION_ENABLE_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGW("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, argv[PARAM1], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

void AsyncCompleteCallbackEnableNotification(napi_env env, napi_status status, void *data)
{
    ANS_LOGI("enter");
    if (!data) {
        ANS_LOGE("Invalid async callback data");
        return;
    }
    AsyncCallbackInfoEnable *asynccallbackinfo = (AsyncCallbackInfoEnable *)data;

    Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));

    if (asynccallbackinfo->info.callback != nullptr) {
        napi_delete_reference(env, asynccallbackinfo->info.callback);
    }

    napi_delete_async_work(env, asynccallbackinfo->asyncWork);
    if (asynccallbackinfo) {
        delete asynccallbackinfo;
        asynccallbackinfo = nullptr;
    }
}

napi_value EnableNotification(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    EnableParams params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoEnable *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoEnable {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "enableNotification", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("EnableNotification napi_create_async_work start");
            AsyncCallbackInfoEnable *asynccallbackinfo = (AsyncCallbackInfoEnable *)data;
            std::string deviceId {""};
            asynccallbackinfo->info.errorCode = NotificationHelper::SetNotificationsEnabledForSpecifiedBundle(
                asynccallbackinfo->params.option, deviceId, asynccallbackinfo->params.enable);
            ANS_LOGI("asynccallbackinfo->info.errorCode = %{public}d", asynccallbackinfo->info.errorCode);
        },
        AsyncCompleteCallbackEnableNotification,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

void AsyncCompleteCallbackIsNotificationEnabled(napi_env env, napi_status status, void *data)
{
    ANS_LOGI("enter");
    if (!data) {
        ANS_LOGE("Invalid async callback data");
        return;
    }
    AsyncCallbackInfoIsEnable *asynccallbackinfo = (AsyncCallbackInfoIsEnable *)data;

    napi_value result = nullptr;
    napi_get_boolean(env, asynccallbackinfo->allowed, &result);
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

napi_value IsNotificationEnabled(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    IsEnableParams params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoIsEnable *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoIsEnable {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "isNotificationEnabled", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("IsNotificationEnabled napi_create_async_work start");
            AsyncCallbackInfoIsEnable *asynccallbackinfo = (AsyncCallbackInfoIsEnable *)data;

            if (asynccallbackinfo->params.hasBundleOption) {
                ANS_LOGI("option.bundle = %{public}s option.uid = %{public}d",
                    asynccallbackinfo->params.option.GetBundleName().c_str(),
                    asynccallbackinfo->params.option.GetUid());
                asynccallbackinfo->info.errorCode =
                    NotificationHelper::IsAllowedNotify(asynccallbackinfo->params.option, asynccallbackinfo->allowed);
            } else if (asynccallbackinfo->params.hasUserId) {
                ANS_LOGI("userId = %{public}d", asynccallbackinfo->params.userId);
                asynccallbackinfo->info.errorCode =
                    NotificationHelper::IsAllowedNotify(asynccallbackinfo->params.userId, asynccallbackinfo->allowed);
            } else {
                asynccallbackinfo->info.errorCode = NotificationHelper::IsAllowedNotify(asynccallbackinfo->allowed);
            }
            ANS_LOGI("asynccallbackinfo->info.errorCode = %{public}d, allowed = %{public}d",
                asynccallbackinfo->info.errorCode, asynccallbackinfo->allowed);
        },
        AsyncCompleteCallbackIsNotificationEnabled,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value IsNotificationEnabledSelf(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    IsEnableParams params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoIsEnable *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoIsEnable {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "IsNotificationEnabledSelf", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("IsNotificationEnabledSelf napi_create_async_work start");
            AsyncCallbackInfoIsEnable *asynccallbackinfo = (AsyncCallbackInfoIsEnable *)data;

            if (asynccallbackinfo->params.hasBundleOption) {
                ANS_LOGE("Not allowed to query another application");
            } else {
                asynccallbackinfo->info.errorCode = NotificationHelper::IsAllowedNotifySelf(asynccallbackinfo->allowed);
            }
            ANS_LOGI("asynccallbackinfo->info.errorCode = %{public}d, allowed = %{public}d",
                asynccallbackinfo->info.errorCode, asynccallbackinfo->allowed);
        },
        AsyncCompleteCallbackIsNotificationEnabled,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value RequestEnableNotification(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    IsEnableParams params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoIsEnable *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoIsEnable {.env = env, .asyncWork = nullptr, .params = params};

    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "RequestEnableNotification", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("RequestEnableNotification napi_create_async_work start");
            AsyncCallbackInfoIsEnable *asynccallbackinfo = (AsyncCallbackInfoIsEnable *)data;

            std::string deviceId {""};
            asynccallbackinfo->info.errorCode = NotificationHelper::RequestEnableNotification(deviceId);
            ANS_LOGI("asynccallbackinfo->info.errorCode = %{public}d", asynccallbackinfo->info.errorCode);
        },
        AsyncCompleteCallbackIsNotificationEnabled,
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