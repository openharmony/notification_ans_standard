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

#include "distributed_preferences_database.h"

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
namespace {
const std::string APP_ID = "advanced_notification_service";
const std::string STORE_ID = "distributed_preferences";
}  // namespace

DistributedPreferencesDatabase::DistributedPreferencesDatabase() : DistributedFlowControl()
{
    GetKvDataManager();
    GetKvStore();
}

DistributedPreferencesDatabase::~DistributedPreferencesDatabase()
{}

void DistributedPreferencesDatabase::GetKvDataManager(void)
{
    kvDataManager_ = std::make_unique<DistributedKv::DistributedKvDataManager>();

    KvManagerFlowControlClear();
}

bool DistributedPreferencesDatabase::CheckKvDataManager(void)
{
    if (kvDataManager_ == nullptr) {
        GetKvDataManager();
    }
    if (kvDataManager_ == nullptr) {
        return false;
    }
    return true;
}

void DistributedPreferencesDatabase::GetKvStore(void)
{
    if (!CheckKvDataManager()) {
        return;
    }

    DistributedKv::Status status;
    DistributedKv::Options options;
    options.createIfMissing = true;
    options.autoSync = false;
    options.kvStoreType = DistributedKv::KvStoreType::SINGLE_VERSION;

    DistributedKv::AppId appId = {.appId = APP_ID};
    DistributedKv::StoreId storeId = {.storeId = STORE_ID};
    status = kvDataManager_->GetSingleKvStore(options, appId, storeId, kvStore_);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager GetSingleKvStore failed ret = 0x%{public}x", status);
        kvStore_.reset();
        kvDataManager_.reset();
    }

    KvStoreFlowControlClear();
}

bool DistributedPreferencesDatabase::CheckKvStore(void)
{
    if (kvStore_ == nullptr) {
        GetKvStore();
    }
    if (kvStore_ == nullptr) {
        return false;
    }
    return true;
}

bool DistributedPreferencesDatabase::PutToDistributedDB(const std::string &key, const std::string &value)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!CheckKvStore()) {
        return false;
    }

    if (!KvStoreFlowControl()) {
        ANS_LOGE("kvStore flow control.");
        return false;
    }

    DistributedKv::Key kvStoreKey(key);
    DistributedKv::Value kvStoreValue(value);
    DistributedKv::Status status = kvStore_->Put(kvStoreKey, kvStoreValue);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvStore Put() failed ret = 0x%{public}x", status);
        return false;
    }

    return true;
}

bool DistributedPreferencesDatabase::GetFromDistributedDB(const std::string &key, std::string &value)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!CheckKvStore()) {
        return false;
    }

    if (!KvStoreFlowControl()) {
        ANS_LOGE("kvStore flow control.");
        return false;
    }

    DistributedKv::Key kvStoreKey(key);
    DistributedKv::Value kvStoreValue;
    DistributedKv::Status status = kvStore_->Get(kvStoreKey, kvStoreValue);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvStore Get() failed ret = 0x%{public}x", status);
        return false;
    }

    value = kvStoreValue.ToString();

    return true;
}

bool DistributedPreferencesDatabase::GetEntriesFromDistributedDB(
    const std::string &prefixKey, std::vector<Entry> &entries)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!CheckKvStore()) {
        return false;
    }

    if (!KvStoreFlowControl()) {
        ANS_LOGE("kvStore flow control.");
        return false;
    }

    DistributedKv::Key kvStoreKey(prefixKey);
    DistributedKv::Status status = kvStore_->GetEntries(kvStoreKey, entries);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvStore GetEntries() failed ret = 0x%{public}x", status);
        return false;
    }

    return true;
}

bool DistributedPreferencesDatabase::ClearDatabase(void)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!CheckKvDataManager()) {
        return false;
    }

    if (!KvManagerFlowControl()) {
        ANS_LOGE("kvDataManager flow control.");
        return false;
    }

    DistributedKv::AppId appId = {.appId = APP_ID};
    DistributedKv::StoreId storeId = {.storeId = STORE_ID};
    DistributedKv::Status status = kvDataManager_->CloseKvStore(appId, storeId);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager CloseKvStore() failed ret = 0x%{public}x", status);
        return false;
    }

    status = kvDataManager_->DeleteKvStore(appId, storeId);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager DeleteKvStore() failed ret = 0x%{public}x", status);
        return false;
    }
    return true;
}
}  // namespace Notification
}  // namespace OHOS