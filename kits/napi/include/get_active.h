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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_GET_ACTIVE_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_GET_ACTIVE_H

#include "common.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;

const int ALL_ACTIVE_MAX_PARA = 2;
const int ACTIVE_OR_NUMS_MAX_PARA = 1;

struct AsyncCallbackInfoActive {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = nullptr;
    napi_deferred deferred;
    std::vector<std::string> strValue;
    bool isCallback = false;
    napi_value result = nullptr;
    int errorCode = 0;
};

napi_value GetAllActiveNotifications(napi_env env, napi_callback_info info);
napi_value GetActiveNotifications(napi_env env, napi_callback_info info);
napi_value GetActiveNotificationNums(napi_env env, napi_callback_info info);
napi_value ParseParametersByAllActive(const napi_env &env, const napi_value (&argv)[ALL_ACTIVE_MAX_PARA],
    const size_t &argc, std::vector<std::string> &strValue, napi_ref &callback);
napi_value ParseParameters(
    const napi_env &env, const napi_value (&argv)[ACTIVE_OR_NUMS_MAX_PARA], const size_t &argc, napi_ref &callback);
void PaddingAsyncCallbackInfoIs(
    const napi_env &env, AsyncCallbackInfoActive *&asynccallbackinfo, const napi_ref &callback, napi_value &promise);

}  // namespace NotificationNapi
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_GET_ACTIVE_H