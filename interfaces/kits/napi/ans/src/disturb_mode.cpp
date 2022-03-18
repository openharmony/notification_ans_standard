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

#include "disturb_mode.h"

namespace OHOS {
namespace NotificationNapi {
const int SET_DISTURB_MAX_PARA = 3;
const int SET_DISTURB_MIN_PARA = 1;
const int GET_DISTURB_MAX_PARA = 2;

struct SetDoNotDisturbDateParams {
    NotificationDoNotDisturbDate date;
    bool hasUserId = false;
    int32_t userId = SUBSCRIBE_USER_INIT;
    napi_ref callback = nullptr;
};

struct GetDoNotDisturbDateParams {
    bool hasUserId = false;
    int32_t userId = SUBSCRIBE_USER_INIT;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoSetDoNotDisturb {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    SetDoNotDisturbDateParams params;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoGetDoNotDisturb {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    GetDoNotDisturbDateParams params;
    NotificationDoNotDisturbDate date;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoSupportDoNotDisturb {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    napi_ref callback = nullptr;
    bool isSupported = false;
    CallbackPromiseInfo info;
};

napi_value GetDoNotDisturbDate(const napi_env &env, const napi_value &argv, SetDoNotDisturbDateParams &params)
{
    ANS_LOGI("enter");
    napi_value value = nullptr;
    bool hasProperty = false;
    napi_valuetype valuetype = napi_undefined;
    // argv[0]: date:type
    NAPI_CALL(env, napi_has_named_property(env, argv, "type", &hasProperty));
    if (!hasProperty) {
        ANS_LOGW("Wrong argument type. Property type expected.");
        return nullptr;
    }
    napi_get_named_property(env, argv, "type", &value);
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    if (valuetype != napi_number) {
        ANS_LOGW("Wrong argument type. Number expected.");
        return nullptr;
    }
    int type = 0;
    NotificationConstant::DoNotDisturbType outType = NotificationConstant::DoNotDisturbType::NONE;
    napi_get_value_int32(env, value, &type);
    ANS_LOGI("type is: %{public}d", type);
    if (!Common::DoNotDisturbTypeJSToC(DoNotDisturbType(type), outType)) {
        return nullptr;
    }
    params.date.SetDoNotDisturbType(outType);

    // argv[0]: date:begin
    NAPI_CALL(env, napi_has_named_property(env, argv, "begin", &hasProperty));
    if (!hasProperty) {
        ANS_LOGW("Wrong argument type. Property type expected.");
        return nullptr;
    }
    double begin = 0;
    napi_get_named_property(env, argv, "begin", &value);
    bool isDate = false;
    napi_is_date(env, value, &isDate);
    if (!isDate) {
        ANS_LOGE("Wrong argument type. Date expected.");
        return nullptr;
    }
    napi_get_date_value(env, value, &begin);
    params.date.SetBeginDate(int64_t(begin));

    // argv[0]: date:end
    NAPI_CALL(env, napi_has_named_property(env, argv, "end", &hasProperty));
    if (!hasProperty) {
        ANS_LOGW("Wrong argument type. Property type expected.");
        return nullptr;
    }
    double end = 0;
    napi_get_named_property(env, argv, "end", &value);
    isDate = false;
    napi_is_date(env, value, &isDate);
    if (!isDate) {
        ANS_LOGE("Wrong argument type. Date expected.");
        return nullptr;
    }
    napi_get_date_value(env, value, &end);
    params.date.SetEndDate(int64_t(end));

    return Common::NapiGetNull(env);
}

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, SetDoNotDisturbDateParams &params)
{
    ANS_LOGI("enter");

    size_t argc = SET_DISTURB_MAX_PARA;
    napi_value argv[SET_DISTURB_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc < SET_DISTURB_MIN_PARA) {
        ANS_LOGW("Wrong argument type. Property type expected.");
        return nullptr;
    }

    // argv[0]: date
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_object) {
        ANS_LOGW("Wrong argument type. Property type expected.");
        return nullptr;
    }
    if (GetDoNotDisturbDate(env, argv[PARAM0], params) == nullptr) {
        return nullptr;
    }

    // argv[1] : userId / callback
    if (argc >= SET_DISTURB_MAX_PARA - 1) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        if ((valuetype != napi_number) && (valuetype != napi_function)) {
            ANS_LOGW("Wrong argument type. Function or object expected.");
            return nullptr;
        }

        if (valuetype == napi_number) {
            params.hasUserId = true;
            NAPI_CALL(env, napi_get_value_int32(env, argv[PARAM1], &params.userId));
        } else {
            napi_create_reference(env, argv[PARAM1], 1, &params.callback);
        }
    }

    // argv[2]:callback
    if (argc >= SET_DISTURB_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGW("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, argv[PARAM2], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value SetDoNotDisturbDate(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    SetDoNotDisturbDateParams params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoSetDoNotDisturb *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoSetDoNotDisturb {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "setDoNotDisturbDate", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr, resourceName, [](napi_env env, void *data) {
            ANS_LOGI("SetDoNotDisturbDate napi_create_async_work start");
            AsyncCallbackInfoSetDoNotDisturb *asynccallbackinfo = static_cast<AsyncCallbackInfoSetDoNotDisturb *>(data);
            if (asynccallbackinfo->params.hasUserId) {
                asynccallbackinfo->info.errorCode = NotificationHelper::SetDoNotDisturbDate(
                    asynccallbackinfo->params.userId, asynccallbackinfo->params.date);
            } else {
                asynccallbackinfo->info.errorCode = NotificationHelper::SetDoNotDisturbDate(
                    asynccallbackinfo->params.date);
            }

            ANS_LOGI("SetDoNotDisturbDate date=%{public}s errorCode=%{public}d, hasUserId=%{public}d",
                asynccallbackinfo->params.date.Dump().c_str(), asynccallbackinfo->info.errorCode,
                asynccallbackinfo->params.hasUserId);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("SetDoNotDisturbDate napi_create_async_work end");
            AsyncCallbackInfoSetDoNotDisturb *asynccallbackinfo = static_cast<AsyncCallbackInfoSetDoNotDisturb *>(data);
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo, &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

void AsyncCompleteCallbackGetDoNotDisturbDate(napi_env env, napi_status status, void *data)
{
    ANS_LOGI("enter");
    if (!data) {
        ANS_LOGE("Invalid async callback data");
        return;
    }
    AsyncCallbackInfoGetDoNotDisturb *asynccallbackinfo = static_cast<AsyncCallbackInfoGetDoNotDisturb *>(data);
    napi_value result = Common::NapiGetNull(env);
    if (asynccallbackinfo->info.errorCode == ERR_OK) {
        napi_create_object(env, &result);
        if (!Common::SetDoNotDisturbDate(env, asynccallbackinfo->date, result)) {
            asynccallbackinfo->info.errorCode = ERROR;
        }
    }
    Common::ReturnCallbackPromise(env, asynccallbackinfo->info, result);
    if (asynccallbackinfo->info.callback != nullptr) {
        napi_delete_reference(env, asynccallbackinfo->info.callback);
    }
    napi_delete_async_work(env, asynccallbackinfo->asyncWork);
    delete asynccallbackinfo;
    asynccallbackinfo = nullptr;
}

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, GetDoNotDisturbDateParams &params)
{
    ANS_LOGI("enter");

    size_t argc = GET_DISTURB_MAX_PARA;
    napi_value argv[GET_DISTURB_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));

    napi_valuetype valuetype = napi_undefined;
    // argv[0]: userId / callback
    if (argc >= GET_DISTURB_MAX_PARA - 1) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
        if ((valuetype != napi_number) && (valuetype != napi_function)) {
            ANS_LOGW("Wrong argument type. Function or object expected.");
            return nullptr;
        }
        if (valuetype == napi_number) {
            params.hasUserId = true;
            NAPI_CALL(env, napi_get_value_int32(env, argv[PARAM0], &params.userId));
        } else {
            napi_create_reference(env, argv[PARAM0], 1, &params.callback);
        }
    }

    // argv[1]:callback
    if (argc >= GET_DISTURB_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGW("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, argv[PARAM1], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value GetDoNotDisturbDate(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    GetDoNotDisturbDateParams params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoGetDoNotDisturb *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoGetDoNotDisturb {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getDoNotDisturbDate", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetDoNotDisturbDate napi_create_async_work start");
            AsyncCallbackInfoGetDoNotDisturb *asynccallbackinfo = static_cast<AsyncCallbackInfoGetDoNotDisturb *>(data);
            if (asynccallbackinfo->params.hasUserId) {
                asynccallbackinfo->info.errorCode = NotificationHelper::GetDoNotDisturbDate(
                    asynccallbackinfo->params.userId, asynccallbackinfo->date);
            } else {
                asynccallbackinfo->info.errorCode = NotificationHelper::GetDoNotDisturbDate(asynccallbackinfo->date);
            }

            ANS_LOGI("GetDoNotDisturbDate errorCode=%{public}d date=%{public}s, hasUserId=%{public}d",
                asynccallbackinfo->info.errorCode, asynccallbackinfo->date.Dump().c_str(),
                asynccallbackinfo->params.hasUserId);
        },
        AsyncCompleteCallbackGetDoNotDisturbDate,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value SupportDoNotDisturbMode(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    napi_ref callback = nullptr;
    if (Common::ParseParaOnlyCallback(env, info, callback) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoSupportDoNotDisturb *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoSupportDoNotDisturb {
        .env = env, .asyncWork = nullptr, .callback = callback};

    if (!asynccallbackinfo) {
        return Common::JSParaError(env, callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "supportDoNotDisturbMode", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("SupportDoNotDisturbMode napi_create_async_work start");
            AsyncCallbackInfoSupportDoNotDisturb *asynccallbackinfo =
                static_cast<AsyncCallbackInfoSupportDoNotDisturb *>(data);
            asynccallbackinfo->info.errorCode =
                NotificationHelper::DoesSupportDoNotDisturbMode(asynccallbackinfo->isSupported);
            ANS_LOGI("SupportDoNotDisturbMode errorCode=%{public}d isSupported=%{public}d",
                asynccallbackinfo->info.errorCode, asynccallbackinfo->isSupported);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("SupportDoNotDisturbMode napi_create_async_work end");
            AsyncCallbackInfoSupportDoNotDisturb *asynccallbackinfo =
                static_cast<AsyncCallbackInfoSupportDoNotDisturb *>(data);
            if (asynccallbackinfo) {
                napi_value result = nullptr;
                napi_get_boolean(env, asynccallbackinfo->isSupported, &result);
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, result);
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
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
}  // namespace NotificationNapi
}  // namespace OHOS