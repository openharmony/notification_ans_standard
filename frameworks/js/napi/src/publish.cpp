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

#include "publish.h"

#include "want_agent_helper.h"

namespace OHOS {
namespace NotificationNapi {
using namespace AbilityRuntime::WantAgent;

namespace {
constexpr int8_t PUBLISH_NOTIFICATION_MAX = 3;
constexpr int8_t SHOW_NOTIFICATION_MAX = 1;
constexpr int8_t PUBLISH_AS_BUNDLE_MAX = 4;
}

struct AsyncCallbackInfoPublish {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    NotificationRequest request;
    CallbackPromiseInfo info;
};

struct ParametersInfoPublish {
    NotificationRequest request;
    napi_ref callback = nullptr;
};

napi_value GetCallback(const napi_env &env, const napi_value &value, ParametersInfoPublish &params)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    if (valuetype != napi_function) {
        ANS_LOGW("Wrong argument type. Function expected.");
        return nullptr;
    }
    napi_create_reference(env, value, 1, &params.callback);
    ANS_LOGI("end");
    return Common::NapiGetNull(env);
}

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, ParametersInfoPublish &params)
{
    ANS_LOGI("enter");

    size_t argc = PUBLISH_NOTIFICATION_MAX;
    napi_value argv[PUBLISH_NOTIFICATION_MAX] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if (argc < 1) {
        ANS_LOGW("Wrong number of arguments.");
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_object) {
        ANS_LOGW("Wrong argument type. Object expected.");
        return nullptr;
    }

    // argv[0] : NotificationRequest
    if (Common::GetNotificationRequest(env, argv[PARAM0], params.request) == nullptr) {
        return nullptr;
    }

    // argv[1] : userId / callback
    if (argc >= PUBLISH_NOTIFICATION_MAX - 1) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        if ((valuetype != napi_number) && (valuetype != napi_function)) {
            ANS_LOGW("Wrong argument type. Function or object expected.");
            return nullptr;
        }

        if (valuetype == napi_number) {
            int32_t recvUserId = SUBSCRIBE_USER_INIT;
            NAPI_CALL(env, napi_get_value_int32(env, argv[PARAM1], &recvUserId));
            params.request.SetReceiverUserId(recvUserId);
        } else {
            napi_create_reference(env, argv[PARAM1], 1, &params.callback);
        }
    }

    // argv[2] : callback
    if (argc >= PUBLISH_NOTIFICATION_MAX) {
        if (GetCallback(env, argv[PARAM2], params) == nullptr) {
            return nullptr;
        }
    }

    ANS_LOGI("end");
    return Common::NapiGetNull(env);
}

napi_value Publish(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoPublish params;
    if (ParseParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    napi_value promise = nullptr;
    AsyncCallbackInfoPublish *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoPublish {.env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }

    asynccallbackinfo->request = params.request;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "publish", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Publish napi_create_async_work start");
            AsyncCallbackInfoPublish *asynccallbackinfo = static_cast<AsyncCallbackInfoPublish *>(data);
            ANS_LOGI("Publish napi_create_async_work start notificationId = %{public}d, contentType = "
                     "%{public}d",
                asynccallbackinfo->request.GetNotificationId(),
                asynccallbackinfo->request.GetContent()->GetContentType());

            asynccallbackinfo->info.errorCode =
                NotificationHelper::PublishNotification(asynccallbackinfo->request);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Publish napi_create_async_work complete start");
            AsyncCallbackInfoPublish *asynccallbackinfo = static_cast<AsyncCallbackInfoPublish *>(data);
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
            ANS_LOGI("Publish napi_create_async_work complete end");
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

bool CheckProperty(const napi_env &env, const napi_value &content, const std::string &property)
{
    ANS_LOGI("enter");

    bool hasProperty = false;

    NAPI_CALL_BASE(env, napi_has_named_property(env, content, property.data(), &hasProperty), false);
    if (!hasProperty) {
        ANS_LOGW("Property %{public}s expected.", property.c_str());
    }
    return hasProperty;
}

napi_value GetStringProperty(
    const napi_env &env, const napi_value &content, const std::string &property, std::string &result)
{
    ANS_LOGI("enter");

    if (!CheckProperty(env, content, property)) {
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    napi_value value = nullptr;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    napi_get_named_property(env, content, property.data(), &value);
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    if (valuetype != napi_string) {
        ANS_LOGW("Wrong argument type. String expected.");
        return nullptr;
    }
    NAPI_CALL(env, napi_get_value_string_utf8(env, value, str, STR_MAX_SIZE - 1, &strLen));
    ANS_LOGI("normal::%{public}s = %{public}s", property.c_str(), str);
    result = str;
    return Common::NapiGetNull(env);
}

napi_value GetObjectProperty(
    const napi_env &env, const napi_value &content, const std::string &property, napi_value &result)
{
    ANS_LOGI("enter");

    if (!CheckProperty(env, content, property)) {
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    napi_get_named_property(env, content, property.data(), &result);
    NAPI_CALL(env, napi_typeof(env, result, &valuetype));
    if (valuetype != napi_object) {
        ANS_LOGW("Wrong argument type. Object expected.");
        return nullptr;
    }
    return Common::NapiGetNull(env);
}

napi_value ParseShowOptions(const napi_env &env, const napi_callback_info &info, ParametersInfoPublish &params)
{
    ANS_LOGI("enter");

    size_t argc = SHOW_NOTIFICATION_MAX;
    napi_value argv[SHOW_NOTIFICATION_MAX] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if (argc == 0) {
        ANS_LOGW("Wrong number of arguments.");
        return nullptr;
    }

    // argv[0] : ShowNotificationOptions
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_object) {
        ANS_LOGW("Wrong argument type. Object expected.");
        return nullptr;
    }

    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();

    // contentTitle
    std::string contentTitle;
    if (GetStringProperty(env, argv[PARAM0], "contentTitle", contentTitle) != nullptr) {
        normalContent->SetTitle(contentTitle);
    }

    // contentText
    std::string contentText;
    if (GetStringProperty(env, argv[PARAM0], "contentText", contentText) != nullptr) {
        normalContent->SetText(contentText);
    }

    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    params.request.SetContent(content);

    // clickAction
    napi_value clickAction = nullptr;
    if (GetObjectProperty(env, argv[PARAM0], "clickAction", clickAction) != nullptr) {
        ANS_LOGD("create wantagent");
        // bundleName & abilityName
        std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
        std::string bundleName;
        std::string abilityName;
        if (GetStringProperty(env, clickAction, "bundleName", bundleName) == nullptr) {
            return nullptr;
        }
        if (GetStringProperty(env, clickAction, "abilityName", abilityName) == nullptr) {
            return nullptr;
        }
        want->SetElementName(bundleName, abilityName);
        // uri
        std::string uri;
        if (GetStringProperty(env, clickAction, "uri", uri) == nullptr) {
            return nullptr;
        }
        want->SetUri(uri);

        std::vector<std::shared_ptr<AAFwk::Want>> wants = {};
        std::vector<WantAgentConstant::Flags> wantAgentFlags = {};
        std::shared_ptr<AAFwk::WantParams> extraInfo = std::make_shared<AAFwk::WantParams>();
        wants.emplace_back(want);
        WantAgentInfo wantAgentInfo(-1, WantAgentConstant::OperationType::START_ABILITY, wantAgentFlags,
            wants, extraInfo);
        std::shared_ptr<AbilityRuntime::ApplicationContext> context = AbilityRuntime::Context::GetApplicationContext();

        std::shared_ptr<WantAgent> wantAgent =
            WantAgentHelper::GetWantAgent(context, wantAgentInfo);
        params.request.SetWantAgent(wantAgent);
    }

    ANS_LOGI("end");
    return Common::NapiGetNull(env);
}

napi_value ShowNotification(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoPublish params;
    if (ParseShowOptions(env, info, params) == nullptr) {
        ANS_LOGW("parse showOptions failed");
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoPublish *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoPublish {.env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        ANS_LOGW("failed to create asynccallbackinfo");
        return Common::JSParaError(env, params.callback);
    }

    asynccallbackinfo->request = params.request;

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "show", NAPI_AUTO_LENGTH, &resourceName);

    ANS_LOGI("before napi_create_async_work");
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Show napi_create_async_work start");
            AsyncCallbackInfoPublish *asynccallbackinfo = static_cast<AsyncCallbackInfoPublish *>(data);
            ANS_LOGI("Show napi_create_async_work start notificationId = %{public}d, contentType = "
                     "%{public}d",
                asynccallbackinfo->request.GetNotificationId(),
                asynccallbackinfo->request.GetContent()->GetContentType());

            asynccallbackinfo->info.errorCode =
                NotificationHelper::PublishNotification(asynccallbackinfo->request);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Show napi_create_async_work complete start");
            AsyncCallbackInfoPublish *asynccallbackinfo = static_cast<AsyncCallbackInfoPublish *>(data);
            if (asynccallbackinfo) {
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
            ANS_LOGI("Show napi_create_async_work complete end");
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));
    return nullptr;
}

napi_value ParsePublishAsBundleParameters(
    const napi_env &env, const napi_callback_info &info, ParametersInfoPublish &params)
{
    ANS_LOGI("enter");

    size_t argc = PUBLISH_AS_BUNDLE_MAX;
    napi_value argv[PUBLISH_AS_BUNDLE_MAX] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if (argc < 1) {
        ANS_LOGW("Wrong number of arguments");
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_object) {
        ANS_LOGW("Wrong argument type. Object expected.");
        return nullptr;
    }

    // argv[0] : NotificationRequest
    if (Common::GetNotificationRequest(env, argv[PARAM0], params.request) == nullptr) {
        return nullptr;
    }

    // argv[1] : bundleName
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
    if (valuetype != napi_string) {
        ANS_LOGW("Wrong argument type. String expected.");
        return nullptr;
    }

    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    napi_get_value_string_utf8(env, argv[PARAM1], str, STR_MAX_SIZE - 1, &strLen);
    params.request.SetOwnerBundleName(str);

    // argv[2] : userId
    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valuetype));
    if (valuetype != napi_number) {
        ANS_LOGW("Wrong argument type. Number expected.");
        return nullptr;
    }
    int32_t userId = 0;
    napi_get_value_int32(env, argv[PARAM2], &userId);
    params.request.SetOwnerUserId(userId);
    params.request.SetIsAgentNotification(true);

    // argv[3] : callback
    if (argc >= PUBLISH_AS_BUNDLE_MAX) {
        if (GetCallback(env, argv[PARAM3], params) == nullptr) {
            return nullptr;
        }
    }

    ANS_LOGI("end");
    return Common::NapiGetNull(env);
}

napi_value PublishAsBundle(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoPublish params;
    if (ParsePublishAsBundleParameters(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    napi_value promise = nullptr;
    AsyncCallbackInfoPublish *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoPublish {.env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }

    asynccallbackinfo->request = params.request;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "publishasbundle", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("PublishAsBundle napi_create_async_work start");
            AsyncCallbackInfoPublish *asynccallbackinfo = static_cast<AsyncCallbackInfoPublish *>(data);
            ANS_LOGI("PublishAsBundle napi_create_async_work start notificationId = %{public}d, contentType = "
                     "%{public}d",
                asynccallbackinfo->request.GetNotificationId(),
                asynccallbackinfo->request.GetContent()->GetContentType());

            asynccallbackinfo->info.errorCode =
                NotificationHelper::PublishNotification(asynccallbackinfo->request);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("PublishAsBundle napi_create_async_work complete start");
            AsyncCallbackInfoPublish *asynccallbackinfo = static_cast<AsyncCallbackInfoPublish *>(data);
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
            ANS_LOGI("PublishAsBundle napi_create_async_work complete end");
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