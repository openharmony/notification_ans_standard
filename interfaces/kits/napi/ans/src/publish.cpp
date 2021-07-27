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
#include "publish.h"
#include "notification_basic_content.h"
#include "notification_constant.h"
#include "notification_content.h"
#include "notification_normal_content.h"

namespace OHOS {
namespace NotificationNapi {
static const int32_t PUBLISH_NOTIFICATION_MAX = 2;

struct AsyncCallbackInfoPublish {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = 0;
    napi_deferred deferred;
    NotificationRequest notificationRequest;
    std::string lable;
    bool isCallback = false;
    bool isPublishNotificationTwoPara = false;
    int errorCode = 0;
};

struct NotificationLongTextContent {
    std::string longText;
    std::string briefText;
    std::string expandedTitle;
};

struct NotificationPictureContent {};

struct NotificationConversationalContent {};

struct NotificationMultiLineContent {};

struct NotificationContentPublish {
    int32_t type;
    NotificationBasicContent notificationBasicContent;
    NotificationLongTextContent notificationLongTextContent;
    NotificationPictureContent notificationPictureContent;
    NotificationConversationalContent notificationConversationalContent;
    NotificationMultiLineContent notificationMultiLineContent;
};

struct ParseParametersPublish {
    napi_env env;
    size_t argc;
    bool isCallback;
    std::string lable;
    napi_ref callback;
};

napi_value GetCallbackByPublish(
    ParseParametersPublish &parseParameters, const napi_value &value, napi_valuetype &valuetype)
{
    ANS_LOGI("enter");

    NAPI_CALL(parseParameters.env, napi_typeof(parseParameters.env, value, &valuetype));
    NAPI_ASSERT(parseParameters.env, valuetype = napi_function, "Wrong argument type. Function expected.");
    napi_create_reference(parseParameters.env, value, 1, &parseParameters.callback);
    ANS_LOGI("end");
    return Common::NapiGetNull(parseParameters.env);
}

napi_value ParseParametersByPublish(ParseParametersPublish &parseParameters,
    const napi_value (&argv)[PUBLISH_NOTIFICATION_MAX], NotificationRequest &notificationRequest)
{
    ANS_LOGI("enter");

    NAPI_ASSERT(parseParameters.env, parseParameters.argc >= 1, "Wrong number of arguments");
    napi_valuetype valuetype;

    NAPI_CALL(parseParameters.env, napi_typeof(parseParameters.env, argv[0], &valuetype));
    NAPI_ASSERT(parseParameters.env, valuetype == napi_object, "Wrong argument type. Object expected.");

    // argv[0]----NotificationRequest
    if (Common::GetNotificationRequest(parseParameters.env, argv[0], notificationRequest) == nullptr) {
        return nullptr;
    }

    // argv[1] : callback
    if (parseParameters.argc >= PUBLISH_NOTIFICATION_MAX) {
        GetCallbackByPublish(parseParameters, argv[1], valuetype);
    }

    ANS_LOGI("end");
    return Common::NapiGetNull(parseParameters.env);
}

void PaddingCallbackInfoInfoPublish(AsyncCallbackInfoPublish *&asynccallbackinfo,
    const ParseParametersPublish &parseParameters, const NotificationRequest &notificationRequest)
{
    ANS_LOGI("enter");

    asynccallbackinfo->lable = parseParameters.lable;
    asynccallbackinfo->notificationRequest = notificationRequest;

    return;
}

void PaddingPublishCallbackPromise(const napi_env &env, const ParseParametersPublish &parseParameters,
    AsyncCallbackInfoPublish *&asynccallbackinfo, napi_value &promise)
{
    ANS_LOGI("enter");

    if (parseParameters.callback != nullptr) {
        asynccallbackinfo->callback = parseParameters.callback;
        asynccallbackinfo->isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        napi_create_promise(env, &deferred, &promise);
        asynccallbackinfo->deferred = deferred;
        asynccallbackinfo->isCallback = false;
    }

    return;
}

napi_value Publish(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    size_t argc = PUBLISH_NOTIFICATION_MAX;
    napi_value argv[PUBLISH_NOTIFICATION_MAX];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));

    ParseParametersPublish parseParameters = {env, argc, false, "", nullptr};
    NotificationRequest notificationRequest;
    if (ParseParametersByPublish(parseParameters, argv, notificationRequest) == nullptr) {
        return Common::JSParaError(env, parseParameters.callback);
    }
    AsyncCallbackInfoPublish *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoPublish{.env = env, .asyncWork = nullptr};
    napi_value promise = 0;

    PaddingCallbackInfoInfoPublish(asynccallbackinfo, parseParameters, notificationRequest);

    PaddingPublishCallbackPromise(env, parseParameters, asynccallbackinfo, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "publish", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Publish napi_create_async_work start");
            AsyncCallbackInfoPublish *asynccallbackinfo = (AsyncCallbackInfoPublish *)data;
            ANS_LOGI("==============Publish napi_create_async_work start notificationId = %{public}d,contentType = "
                     "%{public}d",
                asynccallbackinfo->notificationRequest.GetNotificationId(),
                asynccallbackinfo->notificationRequest.GetContent()->GetContentType());

            asynccallbackinfo->errorCode = NotificationHelper::PublishNotification(
                asynccallbackinfo->lable, asynccallbackinfo->notificationRequest);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Publish napi_create_async_work complete start");
            AsyncCallbackInfoPublish *asynccallbackinfo = (AsyncCallbackInfoPublish *)data;

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
            ANS_LOGI("Publish napi_create_async_work complete end");
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