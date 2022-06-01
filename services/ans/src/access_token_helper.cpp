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

using namespace OHOS::Security::AccessToken;

namespace OHOS {
namespace Notification {
bool AccessTokenHelper::VerifyCallerPermission(const AccessTokenID &tokenCaller, const std::string &permission)
{
    int result = AccessTokenKit::VerifyAccessToken(tokenCaller, permission);
    return (result == PERMISSION_GRANTED);
}

bool AccessTokenHelper::VerifyNativeToken(const AccessTokenID &callerToken)
{
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    return tokenType == ATokenTypeEnum::TOKEN_NATIVE;
}

bool AccessTokenHelper::IsSystemHap()
{
    AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    ATokenTypeEnum type = AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (type == ATokenTypeEnum::TOKEN_NATIVE) {
        return true;
    }
    if (type == ATokenTypeEnum::TOKEN_HAP) {
        HapTokenInfo info;
        AccessTokenKit::GetHapTokenInfo(tokenId, info);
        if (info.apl == ATokenAplEnum::APL_SYSTEM_CORE || info.apl == ATokenAplEnum::APL_SYSTEM_BASIC) {
            return true;
        }
        pid_t pid = IPCSkeleton::GetCallingPid();
        pid_t uid = IPCSkeleton::GetCallingUid();
        ANS_LOGW("apl not match, info.apl=%{public}d, type=%{public}d, pid=%{public}d, uid=%{public}d",
                 info.apl, type, pid, uid);
    }
    return false;
}
}  // namespace Notification
}  // namespace OHOS