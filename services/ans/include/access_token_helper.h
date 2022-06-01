/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_ACCESS_TOKEN_HELPER_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_ACCESS_TOKEN_HELPER_H

#include <string>

#include "accesstoken_kit.h"

namespace OHOS {
namespace Notification {
class AccessTokenHelper {
public:
    /**
     * @brief Verifies that the caller has the specified permission.
     *
     * @param tokenCaller The AccessTokenID of caller.
     * @param permission The permission to be verified.
     * @return Returns true if the caller has the specified permission, false otherwise.
     */
    static bool VerifyCallerPermission(
        const Security::AccessToken::AccessTokenID &tokenCaller, const std::string &permission);
    static bool VerifyNativeToken(const Security::AccessToken::AccessTokenID &callerToken);
    static bool IsSystemHap();
};
}  // namespace Notification
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_ACCESS_TOKEN_HELPER_H