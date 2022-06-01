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

#include "access_token_helper.h"

#include "ans_log_wrapper.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace Notification {
bool AccessTokenHelper::VerifyCallerPermission(
    const Security::AccessToken::AccessTokenID &tokenCaller, const std::string &permission)
{
    int result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission);
    return (result == Security::AccessToken::PERMISSION_GRANTED);
}

bool AccessTokenHelper::VerifyNativeToken(const Security::AccessToken::AccessTokenID &callerToken)
{
    Security::AccessToken::ATokenTypeEnum tokenType =
        Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    return tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE;
}

bool AccessTokenHelper::IsSystemHap()
{
    AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    ATokenTypeEnum type = AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (type == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        return true;
    }
    if (type == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        HapTokenInfo info;
        AccessTokenKit::GetHapTokenInfo(tokenId, info);
        if (info.apl == ATokenAplEnum::APL_SYSTEM_CORE || info.apl == ATokenAplEnum::APL_SYSTEM_BASIC) {
            return true;
        }
        pid_t pid = IPCSkeleton::GetCallingPid();
        pid_t uid = IPCSkeleton::GetCallingUid();
        ANS_LOGW("apl not match, info.apl=%{public}d, type=%{public}d, pid=%{public}d, uid=%{public}d",
            static_cast<int32_t>(info.apl), static_cast<int32_t>(type), pid, uid);
    }
    return false;
}

}  // namespace Notification
}  // namespace OHOS