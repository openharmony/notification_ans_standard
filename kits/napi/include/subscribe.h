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
#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_SUBSCRIBE_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_SUBSCRIBE_H

#include "create_subscriber.h"
#include "common.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;

const int SUBSRIBE_MAX_PARA = 3;

struct AsyncCallbackInfoSubscribe {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = 0;
    napi_deferred deferred;
    SubscriberInstance *objectInfo = nullptr;
    NotificationSubscriberInfo subscriberInfo;
    bool isCallback = false;
    int errorCode = 0;
};

napi_value ParseParameters(const napi_env &env, const napi_value (&argv)[SUBSRIBE_MAX_PARA], const size_t &argc,
    NotificationSubscriberInfo &subscriberInfo, SubscriberInstance *&objectInfo, napi_ref &callback);

void PaddingAsyncCallbackInfoIs(
    const napi_env &env, AsyncCallbackInfoSubscribe *&asynccallbackinfo, const napi_ref &callback, napi_value &promise);

napi_value Subscribe(napi_env env, napi_callback_info info);

}  // namespace NotificationNapi
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_SUBSCRIBE_H