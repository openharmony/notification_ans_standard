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

#include "distributed_database.h"

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
namespace {
const std::string APP_ID = "advanced_notification_service";
const std::string STORE_ID = "distributed_notification";
}  // namespace

DistributedDatabase::DistributedDatabase(
    std::shared_ptr<DistributedDatabaseCallback> databaseCb, std::shared_ptr<DistributedDeviceCallback> deviceCb)
    : DistributedFlowControl(), databaseCb_(databaseCb), deviceCb_(deviceCb)
{
    GetKvDataManager();
    GetKvStore();
}

DistributedDatabase::~DistributedDatabase()
{}

void DistributedDatabase::GetKvDataManager(void)
{
    kvDataManager_ = std::make_unique<DistributedKv::DistributedKvDataManager>();
    if (kvDataManager_ != nullptr) {
        DistributedKv::Status status = kvDataManager_->StartWatchDeviceChange(deviceCb_);
        if (status != DistributedKv::Status::SUCCESS) {
            ANS_LOGW("kvDataManager StartWatchDeviceChange failed ret = 0x%{public}x", status);
            kvDataManager_.reset();
        }
    }

    KvManagerFlowControlClear();
}

bool DistributedDatabase::CheckKvDataManager(void)
{
    if (kvDataManager_ == nullptr) {
        GetKvDataManager();
    }
    if (kvDataManager_ == nullptr) {
        ANS_LOGE("kvDataManager is nullptr.");
        return false;
    }
    return true;
}

void DistributedDatabase::GetKvStore(void)
{
    if (!CheckKvDataManager()) {
        return;
    }

    DistributedKv::Options options;
    options.createIfMissing = true;
    options.autoSync = true;
    options.kvStoreType = DistributedKv::KvStoreType::SINGLE_VERSION;
    DistributedKv::AppId appId = {.appId = APP_ID};
    DistributedKv::StoreId storeId = {.storeId = STORE_ID};
    DistributedKv::Status status = kvDataManager_->GetSingleKvStore(options, appId, storeId, kvStore_);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager GetSingleKvStore failed ret = 0x%{public}x", status);
        kvStore_.reset();
        kvDataManager_->StopWatchDeviceChange(deviceCb_);
        kvDataManager_.reset();
        return;
    }

    if (kvStore_ != nullptr) {
        status = kvStore_->SubscribeKvStore(DistributedKv::SubscribeType::SUBSCRIBE_TYPE_REMOTE, databaseCb_);
        if (status != DistributedKv::Status::SUCCESS) {
            ANS_LOGE("kvStore SubscribeKvStore failed ret = 0x%{public}x", status);
            kvStore_.reset();
        }
    }

    KvStoreFlowControlClear();
}

bool DistributedDatabase::CheckKvStore(void)
{
    if (kvStore_ == nullptr) {
        GetKvStore();
    }
    if (kvStore_ == nullptr) {
        ANS_LOGE("kvStore is nullptr.");
        return false;
    }
    return true;
}

bool DistributedDatabase::PutToDistributedDB(const std::string &key, const std::string &value)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!CheckKvStore()) {
        return false;
    }

    if (!KvStoreFlowControl()) {
        ANS_LOGE("KvStore flow control.");
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

bool DistributedDatabase::GetFromDistributedDB(const std::string &key, std::string &value)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!CheckKvStore()) {
        return false;
    }

    if (!KvStoreFlowControl()) {
        ANS_LOGE("KvStore flow control.");
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

bool DistributedDatabase::GetEntriesFromDistributedDB(const std::string &prefixKey, std::vector<Entry> &entries)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!CheckKvStore()) {
        return false;
    }

    if (!KvStoreFlowControl()) {
        ANS_LOGE("KvStore flow control.");
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

bool DistributedDatabase::DeleteToDistributedDB(const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!CheckKvStore()) {
        return false;
    }

    if (!KvStoreFlowControl()) {
        ANS_LOGE("KvStore flow control.");
        return false;
    }

    DistributedKv::Key kvStoreKey(key);
    DistributedKv::Status status = kvStore_->Delete(kvStoreKey);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvStore Delete() failed ret = 0x%{public}x", status);
        return false;
    }

    return true;
}

bool DistributedDatabase::ClearDataByDevice(const std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!CheckKvStore()) {
        return false;
    }

    if (!KvStoreFlowControl()) {
        ANS_LOGE("KvStore flow control.");
        return false;
    }

    DistributedKv::Status status = kvStore_->RemoveDeviceData(deviceId);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvStore RemoveDeviceData() failed ret = 0x%{public}x", status);
        return false;
    }

    return true;
}

bool DistributedDatabase::GetLocalDeviceId(std::string &deviceId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!CheckKvDataManager()) {
        return false;
    }

    if (KvManagerFlowControl()) {
        DistributedKv::DeviceInfo deviceInfo;
        DistributedKv::Status status = kvDataManager_->GetLocalDevice(deviceInfo);
        if (status != DistributedKv::Status::SUCCESS) {
            ANS_LOGE("kvDataManager GetLocalDevice() failed ret = 0x%{public}x", status);
            return false;
        }

        localDeviceId_ = deviceInfo.deviceId;
    }

    if (localDeviceId_.empty()) {
        return false;
    }

    deviceId = localDeviceId_;

    return true;
}

bool DistributedDatabase::GetLocalDeviceInfo(DeviceInfo &localInfo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!CheckKvDataManager()) {
        return false;
    }

    if (!KvManagerFlowControl()) {
        ANS_LOGE("KvManager flow control.");
        return false;
    }

    DistributedKv::Status status = kvDataManager_->GetLocalDevice(localInfo);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager GetLocalDevice() failed ret = 0x%{public}x", status);
        return false;
    }

    return true;
}

bool DistributedDatabase::GetDeviceInfoList(std::vector<DeviceInfo> &deviceList)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!CheckKvDataManager()) {
        return false;
    }

    if (!KvManagerFlowControl()) {
        ANS_LOGE("KvManager flow control.");
        return false;
    }

    DistributedKv::Status status =
        kvDataManager_->GetDeviceList(deviceList, DistributedKv::DeviceFilterStrategy::NO_FILTER);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager GetDeviceList() failed ret = 0x%{public}x", status);
        return false;
    }

    return true;
}

bool DistributedDatabase::RecreateDistributedDB()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!CheckKvDataManager()) {
        return false;
    }

    if (!KvManagerFlowControl()) {
        ANS_LOGE("KvManager flow control.");
        return false;
    }

    kvStore_.reset();

    DistributedKv::AppId appId = {.appId = APP_ID};
    DistributedKv::StoreId storeId = {.storeId = STORE_ID};
    DistributedKv::Status status = kvDataManager_->DeleteKvStore(appId, storeId);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager DeleteKvStore() failed ret = 0x%{public}x", status);
        return false;
    }

    GetKvStore();
    return true;
}
}  // namespace Notification
}  // namespace OHOS