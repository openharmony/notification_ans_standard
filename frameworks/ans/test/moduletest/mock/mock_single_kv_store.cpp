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

#include "mock_single_kv_store.h"

#include "types.h"

namespace OHOS {
namespace DistributedKv {
std::mutex AnsTestSingleKvStore::mutex_;
std::map<std::pair<std::string, std::string>, std::shared_ptr<SingleKvStore>> AnsTestSingleKvStore::kvStoreMap_;

void AnsTestSingleKvStore::InsertMockKvStore(AppId appId, StoreId storeId, std::shared_ptr<SingleKvStore> store)
{
    std::lock_guard<std::mutex> lock(mutex_);

    kvStoreMap_[std::pair<std::string, std::string>(appId.appId, storeId.storeId)] = store;
}

void AnsTestSingleKvStore::RemoveMockKvStore(AppId appId, StoreId storeId)
{
    kvStoreMap_.erase(std::pair<std::string, std::string>(appId.appId, storeId.storeId));
}

std::shared_ptr<AnsTestSingleKvStore> AnsTestSingleKvStore::GetMockKvStorePointer(AppId appId, StoreId storeId)
{
    for (auto kvstore : kvStoreMap_) {
        if (kvstore.first == std::pair<std::string, std::string>(appId.appId, storeId.storeId)) {
            return std::static_pointer_cast<AnsTestSingleKvStore>(kvstore.second);
        }
    }

    return nullptr;
}

void AnsTestSingleKvStore::InsertDataToDoCallback(const Key &key, const Value &value)
{
    Put(key, value);
    if (observer_ != nullptr) {
        Entry entry;
        entry.key = key;
        entry.value = value;
        std::vector<Entry> insertEntries;
        std::vector<Entry> updateEntries;
        std::vector<Entry> deleteEntries;
        insertEntries.push_back(entry);
        ChangeNotification change(
            std::move(insertEntries), std::move(updateEntries), std::move(deleteEntries), "<remoteDeviceId>", false);
        observer_->OnChange(change);
    }
}

void AnsTestSingleKvStore::UpdateDataToDoCallback(const Key &key, const Value &value)
{
    Put(key, value);
    if (observer_ != nullptr) {
        Entry entry;
        entry.key = key;
        entry.value = value;
        std::vector<Entry> insertEntries;
        std::vector<Entry> updateEntries;
        std::vector<Entry> deleteEntries;
        updateEntries.push_back(entry);
        ChangeNotification change(
            std::move(insertEntries), std::move(updateEntries), std::move(deleteEntries), "<remoteDeviceId>", false);
        observer_->OnChange(change);
    }
}

void AnsTestSingleKvStore::DeleteDataToDoCallback(const Key &key)
{
    Value value;
    Get(key, value);
    Delete(key);
    if (observer_ != nullptr) {
        Entry entry;
        entry.key = key;
        entry.value = value;
        std::vector<Entry> insertEntries;
        std::vector<Entry> updateEntries;
        std::vector<Entry> deleteEntries;
        deleteEntries.push_back(entry);
        ChangeNotification change(
            std::move(insertEntries), std::move(updateEntries), std::move(deleteEntries), "<remoteDeviceId>", false);
        observer_->OnChange(change);
    }
}

Status AnsTestSingleKvStore::GetEntries(const Key &prefixKey, std::vector<Entry> &entries) const
{
    entries.clear();

    for (auto iter : kvstore_) {
        if (iter.first.StartsWith(prefixKey)) {
            Entry entry;
            entry.key = iter.first;
            entry.value = iter.second;
            entries.push_back(entry);
        }
    }

    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::GetEntriesWithQuery(const std::string &query, std::vector<Entry> &entries) const
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::GetEntriesWithQuery(const DataQuery &query, std::vector<Entry> &entries) const
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::GetResultSet(const Key &prefixKey, std::shared_ptr<KvStoreResultSet> &resultSet) const
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::GetResultSetWithQuery(
    const std::string &query, std::shared_ptr<KvStoreResultSet> &resultSet) const
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::GetResultSetWithQuery(
    const DataQuery &query, std::shared_ptr<KvStoreResultSet> &resultSet) const
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::CloseResultSet(std::shared_ptr<KvStoreResultSet> &resultSet)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::GetCountWithQuery(const std::string &query, int &result) const
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::GetCountWithQuery(const DataQuery &query, int &result) const
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::Sync(
    const std::vector<std::string> &deviceIds, SyncMode mode, uint32_t allowedDelayMs)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::RemoveDeviceData(const std::string &device)
{
    kvstore_.clear();

    return Status::SUCCESS;
}

StoreId AnsTestSingleKvStore::GetStoreId() const
{
    StoreId storeId;
    storeId.storeId = "";
    return storeId;
}

Status AnsTestSingleKvStore::Delete(const Key &key)
{
    if (kvstore_.erase(key) != 0) {
        return Status::SUCCESS;
    }

    return Status::KEY_NOT_FOUND;
}

Status AnsTestSingleKvStore::Put(const Key &key, const Value &value)
{
    kvstore_[key] = value;

    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::Get(const Key &key, Value &value)
{
    for (auto iter : kvstore_) {
        if (iter.first == key) {
            value = iter.second;
            return Status::SUCCESS;
        }
    }

    return Status::KEY_NOT_FOUND;
}

Status AnsTestSingleKvStore::SubscribeKvStore(SubscribeType subscribeType, std::shared_ptr<KvStoreObserver> observer)
{
    observer_ = observer;
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::UnSubscribeKvStore(SubscribeType subscribeType, std::shared_ptr<KvStoreObserver> observer)
{
    observer_ = nullptr;
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::RegisterSyncCallback(std::shared_ptr<KvStoreSyncCallback> callback)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::UnRegisterSyncCallback()
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::PutBatch(const std::vector<Entry> &entries)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::DeleteBatch(const std::vector<Key> &keys)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::StartTransaction()
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::Commit()
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::Rollback()
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::SetSyncParam(const KvSyncParam &syncParam)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::GetSyncParam(KvSyncParam &syncParam)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::SetCapabilityEnabled(bool enabled) const
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::SetCapabilityRange(
    const std::vector<std::string> &localLabels, const std::vector<std::string> &remoteSupportLabels) const
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::GetSecurityLevel(SecurityLevel &securityLevel) const
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::Control(KvControlCmd cmd, const KvParam &inputParam, KvParam &output)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::SyncWithCondition(const std::vector<std::string> &deviceIds, SyncMode mode,
                                               const DataQuery &query)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::SubscribeWithQuery(const std::vector<std::string> &deviceIds, const DataQuery &query)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::UnSubscribeWithQuery(const std::vector<std::string> &deviceIds, const DataQuery &query)
{
    return Status::SUCCESS;
}

Status AnsTestSingleKvStore::GetKvStoreSnapshot(
    std::shared_ptr<KvStoreObserver> observer, std::shared_ptr<KvStoreSnapshot> &snapshot) const
{
    return Status::NOT_SUPPORT;
}

Status AnsTestSingleKvStore::ReleaseKvStoreSnapshot(std::shared_ptr<KvStoreSnapshot> &snapshot)
{
    return Status::NOT_SUPPORT;
}

Status AnsTestSingleKvStore::Clear()
{
    return Status::NOT_SUPPORT;
}
}  // namespace DistributedKv
}  // namespace OHOS
