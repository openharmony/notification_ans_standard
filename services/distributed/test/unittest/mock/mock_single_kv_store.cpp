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

#include "mock_single_kv_store.h"
#include "types.h"

namespace OHOS {
namespace DistributedKv {
Status MockSingleKvStore::GetEntries(const Key &prefixKey, std::vector<Entry> &entries) const
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::GetEntries(const DataQuery &query, std::vector<Entry> &entries) const
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::GetResultSet(const Key &prefixKey, std::shared_ptr<KvStoreResultSet> &resultSet) const
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::GetResultSet(
    const DataQuery &query, std::shared_ptr<KvStoreResultSet> &resultSet) const
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::CloseResultSet(std::shared_ptr<KvStoreResultSet> &resultSet)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::GetCount(const DataQuery &query, int &result) const
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::Sync(const std::vector<std::string> &deviceIds, SyncMode mode, uint32_t allowedDelayMs)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::RemoveDeviceData(const std::string &device)
{
    return Status::SUCCESS;
}

StoreId MockSingleKvStore::GetStoreId() const
{
    StoreId storeId;
    storeId.storeId = "";
    return storeId;
}

Status MockSingleKvStore::Delete(const Key &key)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::Put(const Key &key, const Value &value)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::Get(const Key &key, Value &value)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::SubscribeKvStore(SubscribeType subscribeType, std::shared_ptr<KvStoreObserver> observer)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::UnSubscribeKvStore(SubscribeType subscribeType, std::shared_ptr<KvStoreObserver> observer)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::RegisterSyncCallback(std::shared_ptr<KvStoreSyncCallback> callback)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::UnRegisterSyncCallback()
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::PutBatch(const std::vector<Entry> &entries)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::DeleteBatch(const std::vector<Key> &keys)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::StartTransaction()
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::Commit()
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::Rollback()
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::SetSyncParam(const KvSyncParam &syncParam)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::GetSyncParam(KvSyncParam &syncParam)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::SetCapabilityEnabled(bool enabled) const
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::SetCapabilityRange(
    const std::vector<std::string> &localLabels, const std::vector<std::string> &remoteSupportLabels) const
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::GetSecurityLevel(SecurityLevel &securityLevel) const
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::Sync(const std::vector<std::string> &deviceIds, SyncMode mode,
    const DataQuery &query, std::shared_ptr<KvStoreSyncCallback> syncCallback)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::SubscribeWithQuery(const std::vector<std::string> &deviceIds, const DataQuery &query)
{
    return Status::SUCCESS;
}

Status MockSingleKvStore::UnsubscribeWithQuery(const std::vector<std::string> &deviceIds, const DataQuery &query)
{
    return Status::SUCCESS;
}
}  // namespace DistributedKv
}  // namespace OHOS
