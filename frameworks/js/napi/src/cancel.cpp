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

#include "cancel.h"

namespace OHOS {
namespace NotificationNapi {
constexpr int8_t CANCEL_MAX_PARA = 3;
constexpr int8_t CANCEL_GROUP_MAX_PARA = 2;
constexpr int8_t CANCEL_GROUP_MIN_PARA = 1;
constexpr int8_t CANCEL_AS_BUNDLE_MAX_PARA = 4;

struct ParametersInfoCancel {
    int32_t id = 0;
    std::string label = "";
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoCancel {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    int32_t id = 0;
    std::string label;
    CallbackPromiseInfo info;
};

struct ParametersInfoCancelGroup {
    std::string groupName = "";
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoCancelGroup {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    CallbackPromiseInfo info;
    ParametersInfoCancelGroup params {};
};

struct  ParametersInfoCancelAsBundle {
    int32_t id = 0;
    std::string representativeBundle = "";
    int32_t userId = 0;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoCancelAsBundle {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    int32_t id = 0;
    std::string representativeBundle = "";
    int32_t userId = 0;
    CallbackPromiseInfo info;
};

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, ParametersInfoCancel &paras)
{
    ANS_LOGI("enter");

    size_t argc = CANCEL_MAX_PARA;
    napi_value argv[CANCEL_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc < 1) {
        ANS_LOGW("Wrong number of arguments");
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    // argv[0]: id: number
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_number) {
        ANS_LOGW("Wrong argument type. Number expected.");
        return nullptr;
    }
    NAPI_CALL(env, napi_get_value_int32(env, argv[PARAM0], &paras.id));

    // argv[1]: label: string / callback
    if (argc >= CANCEL_MAX_PARA - 1) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        if (valuetype != napi_string && valuetype != napi_function) {
            ANS_LOGW("Wrong argument type. String or function expected.");
            return nullptr;
        }
        if (valuetype == napi_string) {
            char str[STR_MAX_SIZE] = {0};
            size_t strLen = 0;
            NAPI_CALL(env, napi_get_value_string_utf8(env, argv[PARAM1], str, STR_MAX_SIZE - 1, &strLen));
            paras.label = str;
        } else {
            napi_create_reference(env, argv[PARAM1], 1, &paras.callback);
        }
    }

    // argv[2]: callback
    if (argc >= CANCEL_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGW("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, argv[PARAM2], 1, &paras.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, ParametersInfoCancelGroup &paras)
{
    ANS_LOGI("enter");

    size_t argc = CANCEL_GROUP_MAX_PARA;
    napi_value argv[CANCEL_GROUP_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc < CANCEL_GROUP_MIN_PARA) {
        ANS_LOGW("Wrong number of arguments");
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    // argv[0]: groupName: string
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_string) {
        ANS_LOGW("Wrong argument type. String expected.");
        return nullptr;
    }
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    NAPI_CALL(env, napi_get_value_string_utf8(env, argv[PARAM0], str, STR_MAX_SIZE - 1, &strLen));
    paras.groupName = str;

    // argv[1]: callback
    if (argc >= CANCEL_GROUP_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGW("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, argv[PARAM1], 1, &paras.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value Cancel(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoCancel paras;
    if (ParseParameters(env, info, paras) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoCancel *asynccallbackinfo = new (std::nothrow)
        AsyncCallbackInfoCancel {.env = env, .asyncWork = nullptr, .id = paras.id, .label = paras.label};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, paras.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, paras.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "cancel", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Cancel napi_create_async_work start");
            AsyncCallbackInfoCancel *asynccallbackinfo = static_cast<AsyncCallbackInfoCancel *>(data);

            if (asynccallbackinfo) {
                asynccallbackinfo->info.errorCode =
                    NotificationHelper::CancelNotification(asynccallbackinfo->label, asynccallbackinfo->id);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Cancel napi_create_async_work end");
            AsyncCallbackInfoCancel *asynccallbackinfo = static_cast<AsyncCallbackInfoCancel *>(data);
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
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value CancelAll(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    napi_ref callback = nullptr;
    if (Common::ParseParaOnlyCallback(env, info, callback) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoCancel *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoCancel {.env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "cancelAll", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("CancelAll napi_create_async_work start");
            AsyncCallbackInfoCancel *asynccallbackinfo = static_cast<AsyncCallbackInfoCancel *>(data);
            if (asynccallbackinfo) {
                asynccallbackinfo->info.errorCode = NotificationHelper::CancelAllNotifications();
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("CancelAll napi_create_async_work end");
            AsyncCallbackInfoCancel *asynccallbackinfo = static_cast<AsyncCallbackInfoCancel *>(data);
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
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value CancelGroup(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoCancelGroup params {};
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoCancelGroup *asynccallbackinfo = new (std::nothrow)
        AsyncCallbackInfoCancelGroup {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "cancelGroup", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("CancelGroup napi_create_async_work start");
            AsyncCallbackInfoCancelGroup *asynccallbackinfo = static_cast<AsyncCallbackInfoCancelGroup *>(data);
            if (asynccallbackinfo) {
                ANS_LOGI("asynccallbackinfo->params.groupName = %{public}s",
                    asynccallbackinfo->params.groupName.c_str());
                asynccallbackinfo->info.errorCode =
                    NotificationHelper::CancelGroup(asynccallbackinfo->params.groupName);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("CancelGroup napi_create_async_work end");
            AsyncCallbackInfoCancelGroup *asynccallbackinfo = static_cast<AsyncCallbackInfoCancelGroup *>(data);
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
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, ParametersInfoCancelAsBundle &paras)
{
    ANS_LOGI("enter");

    size_t argc = CANCEL_AS_BUNDLE_MAX_PARA;
    napi_value argv[CANCEL_AS_BUNDLE_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc < 1) {
        ANS_LOGW("Wrong number of arguments");
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    // argv[0]: id: number
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_number) {
        ANS_LOGW("Wrong argument type. Number expected.");
        return nullptr;
    }
    NAPI_CALL(env, napi_get_value_int32(env, argv[PARAM0], &paras.id));

    // argv[1]: representativeBundle: string
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
    if (valuetype != napi_string) {
        ANS_LOGW("Wrong argument type. String expected.");
        return nullptr;
    }

    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    napi_get_value_string_utf8(env, argv[PARAM1], str, STR_MAX_SIZE - 1, &strLen);
    paras.representativeBundle = str;

    // argv[2] : userId
    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valuetype));
    if (valuetype != napi_number) {
        ANS_LOGW("Wrong argument type. Number expected.");
        return nullptr;
    }
    napi_get_value_int32(env, argv[PARAM2], &paras.userId);

    // argv[3]: callback
    if (argc >= CANCEL_AS_BUNDLE_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM3], &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGW("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, argv[PARAM2], 1, &paras.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value CancelAsBundle(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoCancelAsBundle paras;
    if (ParseParameters(env, info, paras) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoCancelAsBundle *asynccallbackinfo = new (std::nothrow) AsyncCallbackInfoCancelAsBundle {
        .env = env, .asyncWork = nullptr,
        .id = paras.id,
        .representativeBundle = paras.representativeBundle,
        .userId = paras.userId
    };
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, paras.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, paras.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "cancelasbundle", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Cancel napi_create_async_work start");
            AsyncCallbackInfoCancelAsBundle *asynccallbackinfo = static_cast<AsyncCallbackInfoCancelAsBundle *>(data);

            if (asynccallbackinfo) {
                asynccallbackinfo->info.errorCode = NotificationHelper::CancelAsBundle(
                    asynccallbackinfo->id, asynccallbackinfo->representativeBundle, asynccallbackinfo->userId);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Cancel napi_create_async_work end");
            AsyncCallbackInfoCancelAsBundle *asynccallbackinfo = static_cast<AsyncCallbackInfoCancelAsBundle *>(data);
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