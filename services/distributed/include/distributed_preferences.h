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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_H
#define BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_H

#include <memory>
#include <string>

#include "singleton.h"
#include "refbase.h"

#include "ans_inner_errors.h"
#include "distributed_preferences_database.h"
#include "distributed_preferences_info.h"
#include "notification_bundle_option.h"

namespace OHOS {
namespace Notification {
class DistributedPreferences : public DelayedSingleton<DistributedPreferences> {
public:
    ErrCode SetDistributedEnable(bool isEnable);
    ErrCode GetDistributedEnable(bool &isEnable);
    ErrCode SetDistributedBundleEnable(const sptr<NotificationBundleOption> &bundleOption, bool isEnable);
    ErrCode GetDistributedBundleEnable(const sptr<NotificationBundleOption> &bundleOption, bool &isEnable);
    ErrCode ClearDataInRestoreFactorySettings();

private:
    struct ResolveKey {
        bool isMainKey = false;
        std::string bundleName;
        int32_t uid = 0;
    };
    bool InitDistributedAllInfo(void);
    void GetDistributedMainKey(std::string &key);
    void GetDistributedBundleKey(const sptr<NotificationBundleOption> &bundleOption, std::string &key);
    bool ResolveDistributedKey(const std::string &key, ResolveKey &resolveKey);

private:
    std::unique_ptr<DistributedPreferencesInfo> preferencesInfo_ = nullptr;
    std::unique_ptr<DistributedPreferencesDatabase> database_ = nullptr;

    DECLARE_DELAYED_SINGLETON(DistributedPreferences);
    DISALLOW_COPY_AND_MOVE(DistributedPreferences);
};
}  // namespace Notification
}  // namespace OHOS
#endif /* BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_H */
