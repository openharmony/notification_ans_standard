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
// Get all entries in this store which key start with prefixKey.
// Parameters:
//     perfixkey: the prefix to be searched.
//     entries: entries will be returned in this parameter.
// Return:
//     Status of this GetEntries operation.
Status AnsTestSingleKvStore::GetEntries(const Key &prefixKey, std::vector<Entry> &entries) const
{
    return Status::SUCCESS;
}

// Get all entries in this store by query.
// Parameters:
//     query: the query string.
//     entries: entries will be returned in this parameter.
// Return:
//     Status of this GetEntries operation.
Status AnsTestSingleKvStore::GetEntriesWithQuery(const std::string &query, std::vector<Entry> &entries) const
{
    return Status::SUCCESS;
}

// Get all entries in this store by query.
// Parameters:
//     query: the query object.
//     entries: entries will be returned in this parameter.
// Return:
//     Status of this GetEntries operation.
Status AnsTestSingleKvStore::GetEntriesWithQuery(const DataQuery &query, std::vector<Entry> &entries) const
{
    return Status::SUCCESS;
}

// Get ResultSet in this store which key start with prefixKey.
// Parameters:
//     perfixkey: the prefix to be searched.
//     resultSet: resultSet will be returned in this parameter.
// Return:
//     Status of this GetResultSet operation.
void AnsTestSingleKvStore::GetResultSet(
    const Key &prefixKey, std::function<void(Status, std::unique_ptr<KvStoreResultSet>)> callback) const
{}

// Get ResultSet in this store by Query.
// Parameters:
//     query: the query string.
//     resultSet: resultSet will be returned in this parameter.
// Return:
//     Status of this GetResultSet operation.
void AnsTestSingleKvStore::GetResultSetWithQuery(
    const std::string &query, std::function<void(Status, std::unique_ptr<KvStoreResultSet>)> callback) const
{}

// Get ResultSet in this store by Query.
// Parameters:
//     query: the query object.
//     resultSet: resultSet will be returned in this parameter.
// Return:
//     Status of this GetResultSet operation.
void AnsTestSingleKvStore::GetResultSetWithQuery(
    const DataQuery &query, std::function<void(Status, std::unique_ptr<KvStoreResultSet>)> callback) const
{}

// Close the ResultSet returned by GetResultSet.
// Parameters:
//     resultSet: resultSet will be returned in this parameter.
// Return:
//     Status of this CloseResultSet operation.
Status AnsTestSingleKvStore::CloseResultSet(std::unique_ptr<KvStoreResultSet> resultSet)
{
    return Status::SUCCESS;
}

// Get the number of result by query.
// Parameters:
//     query: the query string.
//     result: result will be returned in this parameter.
// Return:
//     Status of this CloseResultSet operation.
Status AnsTestSingleKvStore::GetCountWithQuery(const std::string &query, int &result) const
{
    return Status::SUCCESS;
}

// Get the number of result by query.
// Parameters:
//     query: the query object.
//     result: result will be returned in this parameter.
// Return:
//     Status of this CloseResultSet operation.
Status AnsTestSingleKvStore::GetCountWithQuery(const DataQuery &query, int &result) const
{
    return Status::SUCCESS;
}

// Sync store with other devices. This is an asynchronous method,
// sync will fail if there is a syncing operation in progress.
// Parameters:
//     deviceIdList: device list to sync.
//     mode: mode can be set to SyncMode::PUSH, SyncMode::PULL and SyncMode::PUTH_PULL. PUSH_PULL will firstly
//           push all not-local store to listed devices, then pull these stores back.
//     allowedDelayMs: allowed delay milli-second to sync. default value is 0 for compatibility.
// Return:
//     Status of this Sync operation.
Status AnsTestSingleKvStore::Sync(
    const std::vector<std::string> &deviceIdList, const SyncMode &mode, uint32_t allowedDelayMs)
{
    return Status::SUCCESS;
}

// Remove the device data synced from remote.
// Parameters:
//     device: device id.
// Return:
//     Status of this remove operation.
Status AnsTestSingleKvStore::RemoveDeviceData(const std::string &device)
{
    return Status::SUCCESS;
}

// Get id of this SingleKvStore.
StoreId AnsTestSingleKvStore::GetStoreId() const
{
    StoreId storeId;
    storeId.storeId = "";
    return storeId;
}

// Delete an entry by its key.
// Parameters:
//     key: key of the entry to be deleted.
// Return:
//     Status of this delete operation.
Status AnsTestSingleKvStore::Delete(const Key &key)
{
    return Status::SUCCESS;
}

// Write a pair of key and value to this store.
// Parameters:
//     key: key of this entry. Should be less than 1024 bytes. key will be trimmed before store.
//     value: value of this entry. Should be less than (4 * 1024 * 1024) bytes.
// Return:
//     Status of this put operation.
Status AnsTestSingleKvStore::Put(const Key &key, const Value &value)
{
    return Status::SUCCESS;
}

// Get value from AppKvStore by its key.
// Parameters:
//     key: key of this entry.
//     value: value will be returned in this parameter.
// Return:
//     Status of this get operation.
Status AnsTestSingleKvStore::Get(const Key &key, Value &value)
{
    return Status::SUCCESS;
}

// subscribe change of this kvstore to a client-defined observer. observer->OnChange method will be called when store
// changes.
// Parameters:
//     subscribeType: SUBSCRIBE_TYPE_LOCAL means local changes of syncable kv store,
//                  : SUBSCRIBE_TYPE_REMOTE means synced data changes from remote devices,
//                  : SUBSCRIBE_TYPE_ALL means both local changes and synced data changes.
//     observer: observer to subscribe changes.
// Return:
//     Status of this subscribe operation.
Status AnsTestSingleKvStore::SubscribeKvStore(SubscribeType subscribeType, std::shared_ptr<KvStoreObserver> observer)
{
    return Status::SUCCESS;
}

// un-subscribe change of this kvstore to a client-defined observer.
// Parameters:
//     subscribeType: SUBSCRIBE_TYPE_LOCAL means local changes of syncable kv store,
//                  : SUBSCRIBE_TYPE_REMOTE means synced data changes from remote devices,
//                  : SUBSCRIBE_TYPE_ALL means both local changes and synced data changes.
//     observer: observer to subscribe changes.
// Return:
//     Status of this subscribe operation.
Status AnsTestSingleKvStore::UnSubscribeKvStore(SubscribeType subscribeType, std::shared_ptr<KvStoreObserver> observer)
{
    return Status::SUCCESS;
}

// register message for sync operation.
// Parameters:
//     callback: callback to register.
// Return:
//     Status of this register operation.
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

// delete a list of entries in the kvstore,
// delete key not exist still return success,
// key length should not be greater than 256, and can not be empty.
// if keys contains invalid key, all delete will fail.
// keys memory size should not be greater than IPC transport limit, and can not be empty.
Status AnsTestSingleKvStore::DeleteBatch(const std::vector<Key> &keys)
{
    return Status::SUCCESS;
}

// start transaction.
// all changes to this kvstore will be in a same transaction and will not change the store until Commit() or
// Rollback() is called.
// before this transaction is committed or rollbacked, all attemption to close this store will fail.
Status AnsTestSingleKvStore::StartTransaction()
{
    return Status::SUCCESS;
}

// commit current transaction. all changes to this store will be done after calling this method.
// any calling of this method outside a transaction will fail.
Status AnsTestSingleKvStore::Commit()
{
    return Status::SUCCESS;
}

// rollback current transaction.
// all changes to this store during this transaction will be rollback after calling this method.
// any calling of this method outside a transaction will fail.
Status AnsTestSingleKvStore::Rollback()
{
    return Status::SUCCESS;
}

// set synchronization parameters of this store.
// Parameters:
//     syncParam: sync policy parameter.
// Return:
//     Status of this operation.
Status AnsTestSingleKvStore::SetSyncParam(const KvSyncParam &syncParam)
{
    return Status::SUCCESS;
}

// get synchronization parameters of this store.
// Parameters:
//     syncParam: sync policy parameter.
// Return:
//     Status of this operation.
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

Status AnsTestSingleKvStore::Control(KvControlCmd cmd, const KvParam &inputParam, sptr<KvParam> &output)
{
    return Status::SUCCESS;
}

}  // namespace DistributedKv
}  // namespace OHOS