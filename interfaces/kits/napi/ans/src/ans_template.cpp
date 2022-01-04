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

#include "ans_template.h"
#include "cancel.h"

namespace OHOS {
namespace NotificationNapi {
const int IS_TEMPLATE_MAX_PARA = 2;

struct TemplateName {
    std::string templateName = "";
    bool support = false;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoTemplate {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    TemplateName params;
    CallbackPromiseInfo info;
};

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, TemplateName& params)
{
    ANS_LOGI("enter");

    size_t argc = IS_TEMPLATE_MAX_PARA;
    napi_value argv[IS_TEMPLATE_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc < IS_TEMPLATE_MAX_PARA, "Wrong number of arguments");

    napi_valuetype valuetype = napi_undefined;
    // argv[0]: name: string
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], str, STR_MAX_SIZE - 1, &strLen));
    params.templateName = str;

    // argv[1]: callback
    if (argc >= IS_TEMPLATE_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value IsSupportTemplate(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    TemplateName params;
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoTemplate *asyncCallbackinfo = new (std::nothrow)
        AsyncCallbackInfoTemplate {.env = env, .asyncWork = nullptr, .params = params};
    if (!asyncCallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asyncCallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "IsSupportTemplate", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("IsSupportTemplate napi_create_async_work start");
            AsyncCallbackInfoTemplate *asyncCallbackinfo = (AsyncCallbackInfoTemplate *)data;

            asyncCallbackinfo->info.errorCode = NotificationHelper::CancelNotification(
                asyncCallbackinfo->params.templateName, asyncCallbackinfo->params.support);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("IsSupportTemplate napi_create_async_work end");
            AsyncCallbackInfoTemplate *asyncCallbackinfo = (AsyncCallbackInfoTemplate *)data;

            napi_value result = nullptr;
            napi_get_boolean(env, asyncCallbackinfo->params.support, &result);
            Common::ReturnCallbackPromise(env, asyncCallbackinfo->info, result);

            if (asyncCallbackinfo->info.callback != nullptr) {
                napi_delete_reference(env, asyncCallbackinfo->info.callback);
            }

            napi_delete_async_work(env, asyncCallbackinfo->asyncWork);
            if (asyncCallbackinfo) {
                delete asyncCallbackinfo;
                asyncCallbackinfo = nullptr;
            }
        },
        (void *)asyncCallbackinfo,
        &asyncCallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackinfo->asyncWork));

    if (asyncCallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}
}  // namespace NotificationNapi
}  // namespace OHOS