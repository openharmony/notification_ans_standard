/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_BUNDLE_MANAGER_HELPER_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_BUNDLE_MANAGER_HELPER_H

#include <memory>
#include <mutex>
#include <string>

#include "bundle_mgr_interface.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "notification_bundle_option.h"
#include "refbase.h"
#include "remote_death_recipient.h"
#include "singleton.h"

namespace OHOS {
namespace Notification {
class BundleManagerHelper : public DelayedSingleton<BundleManagerHelper> {
public:
    /**
     * @brief Obtains the bundle name base on the specified uid.
     *
     * @param uid Indicates the specified uid.
     * @return Returns the bundle name.
     */
    std::string GetBundleNameByUid(int32_t uid);

    /**
     * @brief Check whether the caller is a system application base on the specified uid.
     *
     * @param uid Indicates the specified uid.
     * @return Returns the check result.
     */
    bool IsSystemApp(int32_t uid);

    /**
     * @brief Check API compatibility.
     *
     * @param bundleOption Indicates the bundle option.
     * @return Returns the check result.
     */
    bool CheckApiCompatibility(const sptr<NotificationBundleOption> &bundleOption);

    /**
     * @brief Obtains the default uid.
     *
     * @param bundle Indicates the bundle name.
     * @param userId Indicates the user id.
     * @return Returns the uid.
     */
    int32_t GetDefaultUidByBundleName(const std::string &bundle, const int32_t userId);

    /**
     * @brief Obtains the bundle info.
     *
     * @param bundle Indicates the bundle name.
     * @param userId Indicates the user id.
     * @param bundleInfo Indicates the bundle info.
     * @return Returns the uid.
     */
    bool GetBundleInfoByBundleName(const std::string bundle, const int32_t userId, AppExecFwk::BundleInfo &bundleInfo);

#ifdef DISTRIBUTED_NOTIFICATION_SUPPORTED
    /**
     * @brief Check whether the specified bundle has the distributed notification supported.
     *
     * @param bundleName Indicates the bundle name.
     * @param userId Indicates the user id.
     * @return Returns the check result.
     */
    bool GetDistributedNotificationEnabled(const std::string &bundleName, const int32_t userId);
#endif

private:
    void Connect();
    void Disconnect();

    void OnRemoteDied(const wptr<IRemoteObject> &object);

private:
    sptr<AppExecFwk::IBundleMgr> bundleMgr_ = nullptr;
    std::mutex connectionMutex_;
    sptr<RemoteDeathRecipient> deathRecipient_ = nullptr;

    DECLARE_DELAYED_SINGLETON(BundleManagerHelper)
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_BUNDLE_MANAGER_HELPER_H