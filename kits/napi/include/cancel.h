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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_CANCEL_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_CANCEL_H

#include "common.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;

const int CANCEL_MAX_PARA = 3;
const int CANCEL_ALL_MAX_PARA = 1;

struct AsyncCallbackInfoCancel {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = nullptr;
    napi_deferred deferred;
    std::string label;
    int id;
    bool hasLabel = false;
    bool isCallback = false;
    int errorCode = 0;
};

struct paraInfoCancel {
    int id = 0;
    std::string label;
    bool hasLabel = false;
};

napi_value Cancel(napi_env env, napi_callback_info info);
napi_value CancelAll(napi_env env, napi_callback_info info);
napi_value ParseParameters(const napi_env &env, const napi_value (&argv)[CANCEL_MAX_PARA], const size_t &argc,
    paraInfoCancel &info, napi_ref &callback);
napi_value ParseParametersByAll(
    const napi_env &env, const napi_value (&argv)[CANCEL_ALL_MAX_PARA], const size_t &argc, napi_ref &callback);
void PaddingAsyncCallbackInfoIs(
    const napi_env &env, AsyncCallbackInfoCancel *&asynccallbackinfo, const napi_ref &callback, napi_value &promise);

}  // namespace NotificationNapi
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_CANCEL_H