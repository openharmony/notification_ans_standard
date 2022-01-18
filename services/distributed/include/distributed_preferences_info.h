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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_INFO_H
#define BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_INFO_H

#include <map>
#include <string>

namespace OHOS {
namespace Notification {
class DistributedPreferencesInfo {
public:
    DistributedPreferencesInfo();
    ~DistributedPreferencesInfo();
    void SetDistributedEnable(bool enable);
    bool GetDistributedEnable(void);
    void SetDistributedBundleEnable(const std::string &bundleName, int32_t uid, bool enable);
    bool GetDistributedBundleEnable(const std::string &bundleName, int32_t uid);

private:
    bool distributedEnable_ = true;
    std::map<std::pair<const std::string, int32_t>, bool> bundleEnable_;
};
}  // namespace Notification
}  // namespace OHOS

#endif /* BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_INFO_H */