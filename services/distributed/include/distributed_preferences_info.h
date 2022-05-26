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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_PREFERENCES_INFO_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_PREFERENCES_INFO_H

#include <map>
#include <string>

namespace OHOS {
namespace Notification {
class DistributedPreferencesInfo {
public:
    DistributedPreferencesInfo();
    ~DistributedPreferencesInfo();

    /**
     * @brief Set whether the device supports distributed notifications.
     *
     * @param enable Specifies whether to enable the device to support distributed notification.
     */
    void SetDistributedEnable(bool enable);

    /**
     * @brief  Check if the device supports distributed notification.
     *
     * @return True if the device supports distributed notification; false otherwise.
     */
    bool GetDistributedEnable(void);

    /**
     * @brief Set whether an application supports distributed notifications.
     *
     * @param bundleOption Indicates the bundle name and uid of an application.
     * @param enabled Specifies whether to enable an application to support distributed notification.
     * @return Returns enable distributed by bundle result.
     */

    /**
     * @brief Set whether an application supports distributed notifications.
     *
     * @param bundleName Indicates the bundle name of an application.
     * @param uid Indicates the uid of an application.
     * @param enable Specifies whether to enable an application to support distributed notification.
     */
    void SetDistributedBundleEnable(const std::string &bundleName, int32_t uid, bool enable);

    /**
     * @brief Check whether an application supports distributed notifications.
     *
     * @param bundleName Indicates the bundle name of an application.
     * @param uid Indicates the uid of an application.
     * @return True if the application supports distributed notification; false otherwise.
     */
    bool GetDistributedBundleEnable(const std::string &bundleName, int32_t uid);

    /**
     * @brief Remove the setting of whether the application supports distributed notification.
     *
     * @param bundleName Indicates the bundle name of an application.
     * @param uid Indicates the uid of an application.
     */
    void DeleteDistributedBundleInfo(const std::string &bundleName, int32_t uid);

private:
    bool distributedEnable_ = true;
    std::map<std::pair<const std::string, int32_t>, bool> bundleEnable_;
};
}  // namespace Notification
}  // namespace OHOS

#endif // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_PREFERENCES_INFO_H