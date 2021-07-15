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

#include "get_active.h"

namespace OHOS {
namespace NotificationNapi {
napi_value ParseParametersByAllActive(const napi_env &env, const napi_value (&argv)[ALL_ACTIVE_MAX_PARA],
    const size_t &argc, std::vector<std::string> &strValue, napi_ref &callback)
{
    ANS_LOGI("ParseParametersByAllActive start");

    if (argc >= 1) {
        napi_valuetype valuetype;
        // argv[0]: hashcode: Array<string> / callback
        bool isArray = false;
        napi_is_array(env, argv[0], &isArray);
        if (isArray) {
            uint32_t length = 0;
            napi_get_array_length(env, argv[0], &length);
            if (length > 0) {
                for (uint32_t i = 0; i < length; ++i) {
                    napi_value nHashcode;
                    char str[STR_MAX_SIZE] = {0};
                    size_t strLen = 0;
                    napi_get_element(env, argv[0], i, &nHashcode);
                    NAPI_CALL(env, napi_typeof(env, nHashcode, &valuetype));
                    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
                    NAPI_CALL(env, napi_get_value_string_utf8(env, nHashcode, str, STR_MAX_SIZE - 1, &strLen));
                    strValue.emplace_back(str);
                }
            } else {
                ANS_LOGI("hashcode: Array lenth is 0");
                return nullptr;
            }
        } else {
            NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
            NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
            napi_create_reference(env, argv[0], 1, &callback);
        }
        if (argc >= ALL_ACTIVE_MAX_PARA) {
            // argv[1]:callback
            NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
            NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
            napi_create_reference(env, argv[1], 1, &callback);
        }
    }

    return Common::NapiGetNull(env);
}

napi_value ParseParameters(
    const napi_env &env, const napi_value (&argv)[ACTIVE_OR_NUMS_MAX_PARA], const size_t &argc, napi_ref &callback)
{
    ANS_LOGI("ParseParameters start");

    if (argc >= ACTIVE_OR_NUMS_MAX_PARA) {
        napi_valuetype valuetype;
        // argv[0]:callback
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[0], 1, &callback);
    }
    return Common::NapiGetNull(env);
}

void PaddingAsyncCallbackInfoIs(
    const napi_env &env, AsyncCallbackInfoActive *&asynccallbackinfo, const napi_ref &callback, napi_value &promise)
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

napi_value GetAllActiveNotifications(napi_env env, napi_callback_info info)
{
    ANS_LOGI("GetAllActiveNotifications start");

    size_t argc = ALL_ACTIVE_MAX_PARA;
    napi_value argv[ALL_ACTIVE_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 0, "Wrong number of arguments");
    napi_ref callback = nullptr;

    std::vector<std::string> hashcode;
    if (ParseParametersByAllActive(env, argv, argc, hashcode, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }
    ANS_LOGI("GetAllActiveNotifications hashcode.size = %{public}d", hashcode.size());

    AsyncCallbackInfoActive *asynccallbackinfo =
        new AsyncCallbackInfoActive{.env = env, .asyncWork = nullptr, .strValue = hashcode};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIs(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "getAllActiveNotifications", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetAllActiveNotifications napi_create_async_work start");
            AsyncCallbackInfoActive *asynccallbackinfo = (AsyncCallbackInfoActive *)data;

            std::vector<sptr<OHOS::Notification::Notification>> notification;
            if (asynccallbackinfo->strValue.size() > 0) {
                asynccallbackinfo->errorCode =
                    NotificationHelper::GetAllActiveNotifications(asynccallbackinfo->strValue, notification);
            } else {
                asynccallbackinfo->errorCode = NotificationHelper::GetAllActiveNotifications(notification);
            }

            napi_value arr;
            int count = 0;
            napi_create_array(env, &arr);
            for (auto vec : notification) {
                if (vec) {
                    napi_value result;
                    std::shared_ptr<OHOS::Notification::Notification> notificationSharedPtr(
                        static_cast<OHOS::Notification::Notification *>(vec.GetRefPtr()));
                    if (Common::SetNotificationRequest(env, notificationSharedPtr, result)) {
                        napi_set_element(env, arr, count, result);
                        count++;
                    }
                }
            }
            if (count > 0) {
                ANS_LOGI("GetAllActiveNotifications count = %{public}d", count);
                asynccallbackinfo->result = arr;
            } else {
                asynccallbackinfo->result = Common::NapiGetNull(env);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("GetAllActiveNotifications napi_create_async_work end");
            AsyncCallbackInfoActive *asynccallbackinfo = (AsyncCallbackInfoActive *)data;

            CallbackPromiseInfo info;
            info.isCallback = asynccallbackinfo->isCallback;
            info.callback = asynccallbackinfo->callback;
            info.deferred = asynccallbackinfo->deferred;
            info.errorCode = asynccallbackinfo->errorCode;
            Common::ReturnCallbackPromise(env, info, asynccallbackinfo->result);

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

napi_value GetActiveNotifications(napi_env env, napi_callback_info info)
{
    ANS_LOGI("GetActiveNotifications start");

    size_t argc = ACTIVE_OR_NUMS_MAX_PARA;
    napi_value argv[ACTIVE_OR_NUMS_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 0, "Wrong number of arguments");
    napi_ref callback = nullptr;

    if (ParseParameters(env, argv, argc, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }

    AsyncCallbackInfoActive *asynccallbackinfo = new AsyncCallbackInfoActive{.env = env, .asyncWork = nullptr};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIs(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "getActiveNotifications", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetActiveNotifications napi_create_async_work start");
            AsyncCallbackInfoActive *asynccallbackinfo = (AsyncCallbackInfoActive *)data;

            std::vector<sptr<OHOS::Notification::NotificationRequest>> request;
            asynccallbackinfo->errorCode = NotificationHelper::GetActiveNotifications(request);

            napi_value arr;
            int count = 0;
            napi_create_array(env, &arr);
            for (auto vec : request) {
                if (vec) {
                    napi_value result;
                    std::shared_ptr<OHOS::Notification::NotificationRequest> requestSharedPtr(
                        static_cast<OHOS::Notification::NotificationRequest *>(vec.GetRefPtr()));
                    if (Common::SetNotificationRequest2(env, requestSharedPtr, result)) {
                        napi_set_element(env, arr, count, result);
                        count++;
                    }
                }
            }
            if (count > 0) {
                ANS_LOGI("GetActiveNotifications count = %{public}d", count);
                asynccallbackinfo->result = arr;
            } else {
                asynccallbackinfo->result = Common::NapiGetNull(env);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("GetActiveNotifications napi_create_async_work end");
            AsyncCallbackInfoActive *asynccallbackinfo = (AsyncCallbackInfoActive *)data;

            CallbackPromiseInfo info;
            info.isCallback = asynccallbackinfo->isCallback;
            info.callback = asynccallbackinfo->callback;
            info.deferred = asynccallbackinfo->deferred;
            info.errorCode = asynccallbackinfo->errorCode;
            Common::ReturnCallbackPromise(env, info, asynccallbackinfo->result);

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
napi_value GetActiveNotificationNums(napi_env env, napi_callback_info info)
{
    ANS_LOGI("GetActiveNotificationNums start");

    size_t argc = ACTIVE_OR_NUMS_MAX_PARA;
    napi_value argv[ACTIVE_OR_NUMS_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 0, "Wrong number of arguments");
    napi_ref callback = nullptr;

    if (ParseParameters(env, argv, argc, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }

    AsyncCallbackInfoActive *asynccallbackinfo = new AsyncCallbackInfoActive{.env = env, .asyncWork = nullptr};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIs(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "getActiveNotificationNums", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetActiveNotificationNums napi_create_async_work start");
            AsyncCallbackInfoActive *asynccallbackinfo = (AsyncCallbackInfoActive *)data;

            int32_t nums = 0;
            asynccallbackinfo->errorCode = NotificationHelper::GetActiveNotificationNums(nums);
            ANS_LOGI("GetActiveNotificationNums nums = %{public}d", nums);
            napi_create_int32(env, nums, &asynccallbackinfo->result);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("GetActiveNotificationNums napi_create_async_work end");
            AsyncCallbackInfoActive *asynccallbackinfo = (AsyncCallbackInfoActive *)data;

            CallbackPromiseInfo info;
            info.isCallback = asynccallbackinfo->isCallback;
            info.callback = asynccallbackinfo->callback;
            info.deferred = asynccallbackinfo->deferred;
            info.errorCode = asynccallbackinfo->errorCode;
            Common::ReturnCallbackPromise(env, info, asynccallbackinfo->result);

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