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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_REMOVE_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_REMOVE_H

#include "common.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;

const int REMOVE_OR_BUNDLE_MAX_PARA = 2;
const int REMOVE_ALL_MAX_PARA = 1;

struct AsyncCallbackInfoRemove {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = nullptr;
    napi_deferred deferred;
    std::string strValue;
    bool isCallback = false;
    int errorCode = 0;
};

napi_value Remove(napi_env env, napi_callback_info info);
napi_value RemoveAsBundle(napi_env env, napi_callback_info info);
napi_value RemoveAll(napi_env env, napi_callback_info info);

napi_value ParseParameters(const napi_env &env, const napi_value (&argv)[REMOVE_OR_BUNDLE_MAX_PARA], const size_t &argc,
    std::string &strValue, napi_ref &callback);

napi_value ParseParametersByRemoveAll(
    const napi_env &env, const napi_value (&argv)[REMOVE_ALL_MAX_PARA], const size_t &argc, napi_ref &callback);

void PaddingAsyncCallbackInfoIs(
    const napi_env &env, AsyncCallbackInfoRemove *&asynccallbackinfo, const napi_ref &callback, napi_value &promise);

}  // namespace NotificationNapi
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_REMOVE_H