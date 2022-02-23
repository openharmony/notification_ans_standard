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

#include "bundle_manager_helper.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "ans_const_define.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
BundleManagerHelper::BundleManagerHelper()
{
    deathRecipient_ =
        new RemoteDeathRecipient(std::bind(&BundleManagerHelper::OnRemoteDied, this, std::placeholders::_1));
}

BundleManagerHelper::~BundleManagerHelper()
{
    std::lock_guard<std::mutex> lock(connectionMutex_);
    Disconnect();
}

void BundleManagerHelper::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    std::lock_guard<std::mutex> lock(connectionMutex_);
    Disconnect();
}

std::string BundleManagerHelper::GetBundleNameByUid(int uid)
{
    std::string bundle;

    std::lock_guard<std::mutex> lock(connectionMutex_);

    Connect();

    if (bundleMgr_ != nullptr) {
        bundleMgr_->GetBundleNameForUid(uid, bundle);
    }

    return bundle;
}
bool BundleManagerHelper::IsSystemApp(int uid)
{
    bool isSystemApp = false;

    std::lock_guard<std::mutex> lock(connectionMutex_);

    Connect();

    if (bundleMgr_ != nullptr) {
        isSystemApp = bundleMgr_->CheckIsSystemAppByUid(uid);
    }

    return isSystemApp;
}

bool BundleManagerHelper::GetBundleInfoByBundleName(
    const std::string bundle, const int32_t userId, AppExecFwk::BundleInfo &bundleInfo)
{
    if (bundleMgr_ == nullptr) {
        return false;
    }
    return bundleMgr_->GetBundleInfo(bundle, AppExecFwk::BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo, userId);
}

void BundleManagerHelper::Connect()
{
    if (bundleMgr_ != nullptr) {
        return;
    }

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        return;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        return;
    }

    bundleMgr_ = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (bundleMgr_ != nullptr) {
        bundleMgr_->AsObject()->AddDeathRecipient(deathRecipient_);
    }
}

void BundleManagerHelper::Disconnect()
{
    if (bundleMgr_ != nullptr) {
        bundleMgr_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        bundleMgr_ = nullptr;
    }
}

int BundleManagerHelper::GetDefaultUidByBundleName(const std::string &bundle, const int32_t userId)
{
    int uid = -1;

    std::lock_guard<std::mutex> lock(connectionMutex_);

    Connect();

    if (bundleMgr_ != nullptr) {
        AppExecFwk::BundleInfo bundleInfo;
        if (bundleMgr_->GetBundleInfo(bundle, AppExecFwk::BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo, userId)) {
            uid = bundleInfo.uid;
        }
    }

    return uid;
}

#ifdef DISTRIBUTED_NOTIFICATION_SUPPORTED
bool BundleManagerHelper::GetDistributedNotificationEnabled(const std::string &bundleName, const int userId)
{
    std::lock_guard<std::mutex> lock(connectionMutex_);

    Connect();

    if (bundleMgr_ != nullptr) {
        AppExecFwk::ApplicationInfo appInfo;
        if (bundleMgr_->GetApplicationInfo(
            bundleName, AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo)) {
            ANS_LOGD("APPLICATION_INFO distributed enabled %{public}d", appInfo.distributedNotificationEnabled);
            return appInfo.distributedNotificationEnabled;
        }
    }

    ANS_LOGD("APPLICATION_INFO distributed enabled is default");
    return DEFAULT_DISTRIBUTED_ENABLE_IN_APPLICATION_INFO;
}
#endif
}  // namespace Notification
}  // namespace OHOS