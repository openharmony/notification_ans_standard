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
#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_PUBLISH_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_PUBLISH_H

#include <string>
#include "common.h"

static const int32_t PUBLISH_NOTIFICATION_MAX = 2;

namespace OHOS {
namespace NotificationNapi {
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

napi_value ParseParametersByPublish(ParseParametersPublish &parseParameters,
    const napi_value (&argv)[PUBLISH_NOTIFICATION_MAX], NotificationRequest &notificationRequest);
void PaddingCallbackInfoInfoPublish(AsyncCallbackInfoPublish *&asynccallbackinfo,
    const ParseParametersPublish &parseParameters, const NotificationRequest &notificationRequest);
void PaddingPublishCallbackPromise(const napi_env &env, const ParseParametersPublish &parseParameters,
    AsyncCallbackInfoPublish *&asynccallbackinfo, napi_value &promise);
napi_value Publish(napi_env env, napi_callback_info info);
}  // namespace NotificationNapi
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_PUBLISH_H