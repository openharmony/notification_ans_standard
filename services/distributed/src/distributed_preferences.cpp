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

#include "distributed_preferences.h"

#include <map>

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
namespace {
const std::string DISTRIBUTED_LABEL = "distributed";
const std::string DELIMITER = "|";
const std::string MAIN_LABEL = "ans_main";
const std::string BUNDLE_LABEL = "bundle";
}  // namespace

inline bool GetBoolFromString(const std::string &str)
{
    return static_cast<bool>(atoi(str.data()));
}

DistributedPreferences::DistributedPreferences()
{
    database_ = std::make_unique<DistributedPreferencesDatabase>();

    preferencesInfo_ = std::make_unique<DistributedPreferencesInfo>();
    InitDistributedAllInfo();
}

DistributedPreferences::~DistributedPreferences()
{}

bool DistributedPreferences::InitDistributedAllInfo(void)
{
    std::vector<DistributedPreferencesDatabase::Entry> entries;
    if (!database_->GetEntriesFromDistributedDB(DISTRIBUTED_LABEL, entries)) {
        return false;
    }

    for (auto entry : entries) {
        ResolveKey resolveKey;
        if (!ResolveDistributedKey(entry.key.ToString(), resolveKey)) {
            ANS_LOGE("key <%{public}s> is invalid.", entry.key.ToString().c_str());
            continue;
        }

        if (resolveKey.isMainKey) {
            int value = atoi(entry.value.ToString().data());
            preferencesInfo_->SetDistributedEnable(static_cast<bool>(value));
        } else {
            preferencesInfo_->SetDistributedBundleEnable(
                resolveKey.bundleName, resolveKey.uid, GetBoolFromString(entry.value.ToString()));
        }
    }

    return true;
}

void DistributedPreferences::GetDistributedMainKey(std::string &key)
{
    key = DISTRIBUTED_LABEL + DELIMITER + MAIN_LABEL + DELIMITER;
}

void DistributedPreferences::GetDistributedBundleKey(
    const sptr<NotificationBundleOption> &bundleOption, std::string &key)
{
    if (bundleOption) {
        key = DISTRIBUTED_LABEL + DELIMITER + BUNDLE_LABEL + DELIMITER + bundleOption->GetBundleName() + DELIMITER +
            std::to_string(bundleOption->GetUid());
    }
}

bool DistributedPreferences::ResolveDistributedKey(const std::string &key, ResolveKey &resolveKey)
{
    std::size_t distributedLabelPosition = 0;
    std::size_t distributedLabelEndPosition = key.find(DELIMITER, distributedLabelPosition);
    if (distributedLabelEndPosition == std::string::npos) {
        return false;
    }
    std::size_t typeLabelPosition = distributedLabelEndPosition + DELIMITER.size();
    std::size_t typeLabelEndPosition = key.find(DELIMITER, typeLabelPosition);
    if (typeLabelPosition == std::string::npos) {
        return false;
    }

    if (key.substr(typeLabelPosition, typeLabelEndPosition - typeLabelPosition).compare(MAIN_LABEL) == 0) {
        resolveKey.isMainKey = true;
        return true;
    }

    std::size_t bundleNamePosition = typeLabelEndPosition + DELIMITER.size();
    std::size_t bundleNameEndPosition = key.find(DELIMITER, bundleNamePosition);
    if (bundleNameEndPosition == std::string::npos) {
        return false;
    }

    std::size_t uidPosition = key.find_last_of(DELIMITER) + DELIMITER.size();
    if (uidPosition < bundleNameEndPosition) {
        return false;
    }

    resolveKey.isMainKey = false;
    resolveKey.bundleName = key.substr(bundleNamePosition, bundleNameEndPosition - bundleNamePosition);
    resolveKey.uid = atoi(&key[uidPosition]);

    return true;
}

ErrCode DistributedPreferences::SetDistributedEnable(bool isEnable)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    std::string key;
    GetDistributedMainKey(key);

    if (!database_->PutToDistributedDB(key, std::to_string(isEnable))) {
        ANS_LOGE("put to distributed DB failed. key:%{public}s", key.c_str());
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    preferencesInfo_->SetDistributedEnable(isEnable);

    return ERR_OK;
}

ErrCode DistributedPreferences::GetDistributedEnable(bool &isEnable)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);

    isEnable = preferencesInfo_->GetDistributedEnable();

    return ERR_OK;
}

ErrCode DistributedPreferences::SetDistributedBundleEnable(
    const sptr<NotificationBundleOption> &bundleOption, bool isEnable)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    if (bundleOption == nullptr) {
        ANS_LOGE("bundleOption is nullptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    std::string key;
    GetDistributedBundleKey(bundleOption, key);

    if (!database_->PutToDistributedDB(key, std::to_string(isEnable))) {
        ANS_LOGE("put to distributed DB failed. key:%{public}s", key.c_str());
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    preferencesInfo_->SetDistributedBundleEnable(bundleOption->GetBundleName(), bundleOption->GetUid(), isEnable);

    return ERR_OK;
}

ErrCode DistributedPreferences::GetDistributedBundleEnable(
    const sptr<NotificationBundleOption> &bundleOption, bool &isEnable)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    if (bundleOption == nullptr) {
        ANS_LOGE("bundleOption is nullptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    isEnable = preferencesInfo_->GetDistributedBundleEnable(bundleOption->GetBundleName(), bundleOption->GetUid());

    return ERR_OK;
}

ErrCode DistributedPreferences::DeleteDistributedBundleInfo(const sptr<NotificationBundleOption> &bundleOption)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    if (bundleOption == nullptr) {
        ANS_LOGE("bundleOption is nullptr.");
        return ERR_ANS_INVALID_PARAM;
    }

    std::string key;
    GetDistributedBundleKey(bundleOption, key);

    if (!database_->DeleteToDistributedDB(key)) {
        ANS_LOGE("delete to distributed DB failed. key:%{public}s", key.c_str());
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    preferencesInfo_->DeleteDistributedBundleInfo(bundleOption->GetBundleName(), bundleOption->GetUid());

    return ERR_OK;
}

ErrCode DistributedPreferences::ClearDataInRestoreFactorySettings()
{
    if (!database_->ClearDatabase()) {
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    SetDistributedEnable(false);

    preferencesInfo_ = std::make_unique<DistributedPreferencesInfo>();

    return ERR_OK;
}
}  // namespace Notification
}  // namespace OHOS