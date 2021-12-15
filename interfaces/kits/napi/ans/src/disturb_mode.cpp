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

#include "disturb_mode.h"

namespace OHOS {
namespace NotificationNapi {
const int SET_DISTURB_MAX_PARA = 2;
const int SET_DISTURB_MIN_PARA = 1;

struct SetDoNotDisturbDateParams {
    NotificationDoNotDisturbDate date;
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
    napi_ref callback = nullptr;
    NotificationDoNotDisturbDate date;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoSupportDoNotDisturb {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    napi_ref callback = nullptr;
    bool disturbMode =  false;
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
    NAPI_ASSERT(env, hasProperty, "Property type expected.");
    napi_get_named_property(env, argv, "type", &value);
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
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
    NAPI_ASSERT(env, hasProperty, "Property type expected.");
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
    NAPI_ASSERT(env, hasProperty, "Property type expected.");
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
    NAPI_ASSERT(env, argc >= SET_DISTURB_MIN_PARA, "Wrong number of arguments");

    // argv[0]: date
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    if (GetDoNotDisturbDate(env, argv[0], params) == nullptr) {
        return nullptr;
    }

    // argv[1]:callback
    if (argc >= SET_DISTURB_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[SET_DISTURB_MIN_PARA], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[SET_DISTURB_MIN_PARA], 1, &params.callback);
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
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("SetDoNotDisturbDate napi_create_async_work start");
            AsyncCallbackInfoSetDoNotDisturb *asynccallbackinfo = (AsyncCallbackInfoSetDoNotDisturb *)data;
            asynccallbackinfo->info.errorCode = NotificationHelper::SetDoNotDisturbDate(asynccallbackinfo->params.date);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("SetDoNotDisturbDate napi_create_async_work end");
            AsyncCallbackInfoSetDoNotDisturb *asynccallbackinfo = (AsyncCallbackInfoSetDoNotDisturb *)data;

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

void AsyncCompleteCallbackGetDoNotDisturbDate(napi_env env, napi_status status, void *data)
{
    ANS_LOGI("enter");
    if (!data) {
        ANS_LOGE("Invalid async callback data");
        return;
    }
    AsyncCallbackInfoGetDoNotDisturb *asynccallbackinfo = (AsyncCallbackInfoGetDoNotDisturb *)data;
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
    if (asynccallbackinfo) {
        delete asynccallbackinfo;
        asynccallbackinfo = nullptr;
    }
}

napi_value GetDoNotDisturbDate(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    napi_ref callback = nullptr;
    if (Common::ParseParaOnlyCallback(env, info, callback) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoGetDoNotDisturb *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoGetDoNotDisturb {.env = env, .asyncWork = nullptr, .callback = callback};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getDoNotDisturbDate", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetDoNotDisturbDate napi_create_async_work start");
            AsyncCallbackInfoGetDoNotDisturb *asynccallbackinfo = (AsyncCallbackInfoGetDoNotDisturb *)data;
            asynccallbackinfo->info.errorCode = NotificationHelper::GetDoNotDisturbDate(asynccallbackinfo->date);
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
            AsyncCallbackInfoSupportDoNotDisturb *asynccallbackinfo = (AsyncCallbackInfoSupportDoNotDisturb *)data;
            asynccallbackinfo->info.errorCode =
                NotificationHelper::DoesSupportDoNotDisturbMode(asynccallbackinfo->disturbMode);
            ANS_LOGI("asynccallbackinfo->disturbMode = %{public}d", asynccallbackinfo->disturbMode);
            ANS_LOGI("asynccallbackinfo->info.errorCode = %{public}d", asynccallbackinfo->info.errorCode);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("SupportDoNotDisturbMode napi_create_async_work end");
            AsyncCallbackInfoSupportDoNotDisturb *asynccallbackinfo = (AsyncCallbackInfoSupportDoNotDisturb *)data;

            napi_value result = nullptr;
            napi_get_boolean(env, asynccallbackinfo->disturbMode, &result);
            Common::ReturnCallbackPromise(env, asynccallbackinfo->info, result);

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
}  // namespace NotificationNapi
}  // namespace OHOS