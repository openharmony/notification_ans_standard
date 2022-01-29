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

#include "distributed_preferences_info.h"

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
DistributedPreferencesInfo::DistributedPreferencesInfo()
{}

DistributedPreferencesInfo::~DistributedPreferencesInfo()
{}

void DistributedPreferencesInfo::SetDistributedEnable(bool enable)
{
    distributedEnable_ = enable;
}

bool DistributedPreferencesInfo::GetDistributedEnable(void)
{
    return distributedEnable_;
}

void DistributedPreferencesInfo::SetDistributedBundleEnable(const std::string &bundleName, int32_t uid, bool enable)
{
    bundleEnable_[std::make_pair(bundleName, uid)] = enable;
}

bool DistributedPreferencesInfo::GetDistributedBundleEnable(const std::string &bundleName, int32_t uid)
{
    auto iter = bundleEnable_.find(std::make_pair(bundleName, uid));
    if (iter == bundleEnable_.end()) {
        ANS_LOGW("bundle %{public}s(%{public}d) not found.", bundleName.c_str(), uid);
        return true;
    }

    return iter->second;
}

void DistributedPreferencesInfo::DeleteDistributedBundleInfo(const std::string &bundleName, int32_t uid)
{
    bundleEnable_.erase(std::make_pair(bundleName, uid));
}
}  // namespace Notification
}  // namespace OHOS