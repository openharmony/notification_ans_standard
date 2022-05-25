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

#ifndef ANS_MOCK_SINGLE_KV_STORE_H
#define ANS_MOCK_SINGLE_KV_STORE_H

#include <map>
#include <vector>

#include "data_query.h"
#include "kvstore.h"
#include "kvstore_observer.h"
#include "kvstore_result_set.h"
#include "kvstore_sync_callback.h"
#include "single_kvstore.h"
#include "types.h"

namespace OHOS {
namespace DistributedKv {
// This is a public interface. Implementation of this class is in AppKvStoreImpl.
// This class provides put, delete, search, sync and subscribe functions of a key-value store.
class AnsTestSingleKvStore : public SingleKvStore {
public:
    /**
     * @brief Get all entries in this store which key start with prefixKey.
     *
     * @param perfixkey Indicates the prefix to be searched.
     * @param entries Indicates the entries.
     * @return Indicates the status of this GetEntries operation.
     */
    Status GetEntries(const Key &prefixKey, std::vector<Entry> &entries) const override;

    /**
     * @brief Get all entries in this store by query.
     *
     * @param query Indicates the query object.
     * @param entries Indicates the entries.
     * @return Indicates the status of this GetEntries operation.
     */
    Status GetEntries(const DataQuery &query, std::vector<Entry> &entries) const override;

    /**
     * @brief Get ResultSet in this store which key start with prefixKey.
     *
     * @param perfixkey Indicates the prefix to be searched.
     * @param resultSet Indicates the resultSet.
     * @return Indicates the status of this GetResultSet operation.
     */
    Status GetResultSet(const Key &prefixKey, std::shared_ptr<KvStoreResultSet> &resultSet) const override;

    /**
     * @brief Get ResultSet in this store by Query.
     *
     * @param query Indicates the query object.
     * @param resultSet Indicates the resultSet.
     * @return Indicates the status of this GetResultSet operation.
     */
    Status GetResultSet(const DataQuery &query, std::shared_ptr<KvStoreResultSet> &resultSet) const override;

    /**
     * @brief Close the ResultSet returned by GetResultSet.
     *
     * @param resultSet Indicates the resultSet.
     * @return Indicates the status of this CloseResultSet operation.
     */
    Status CloseResultSet(std::shared_ptr<KvStoreResultSet> &resultSet) override;

    /**
     * @brief Get the number of result by query.
     *
     * @param query Indicates the query object.
     * @param result Indicates the result will be returned in this parameter.
     * @return Indicates the status of this CloseResultSet operation.
     */
    Status GetCount(const DataQuery &query, int &result) const override;

    /**
     * @brief Sync store with other devices. This is an asynchronous method,
     *        sync will fail if there is a syncing operation in progress.
     *
     * @param deviceIds Indicates the device list to sync.
     * @param mode Indicates the sync mode, can be set to SyncMode::PUSH, SyncMode::PULL and SyncMode::PUTH_PULL.
     *          PUSH_PULL will firstly push all not-local store to listed devices, then pull these stores back.
     * @param allowedDelayMs Indicates the allowed delay milli-second to sync. default value is 0 for compatibility.
     * @return Indicates the status of this Sync operation.
     */
    Status Sync(const std::vector<std::string> &deviceIds, SyncMode mode, uint32_t delayMs) override;

    /**
     * @brief Remove the device data synced from remote.
     *
     * @param device Indicates the device id.
     * @return Indicates the status of this remove operation.
     */
    Status RemoveDeviceData(const std::string &device) override;

    /**
     * @brief Get id of this AppKvStore.
     *
     * @return Indicates the id of store.
     */
    StoreId GetStoreId() const override;

    /**
     * @brief Delete an entry by its key.
     *
     * @param key Indicates the key of the entry to be deleted.
     * @return Indicates the status of this delete operation.
     */
    Status Delete(const Key &key) override;

    /**
     * @brief Write a pair of key and value to this store.
     *
     * @param key Indicates the key of this entry. Should be less than 256 bytes. key will be trimmed before store.
     * @param value Indicates the value of this entry. Should be less than (1024 * 1024) bytes.
     * @return Indicates the status of this put operation.
     */
    Status Put(const Key &key, const Value &value) override;

    /**
     * @brief Get value from AppKvStore by its key.
     *
     * @param key Indicates the key of this entry.
     * @param value Indicates the value will be returned in this parameter.
     * @return Indicates the status of this get operation.
     */
    Status Get(const Key &key, Value &value) override;

    /**
     * @brief Register change of this kvstore to a client-defined observer.
     *        observer->OnChange method will be called when store changes.
     *        One observer can subscribe more than one AppKvStore.
     *
     * @param subscribeType Indicates the subscribe type.
     *                      OBSERVER_CHANGES_NATIVE means native changes of syncable kv store,
     *                      OBSERVER_CHANGES_FOREIGN means synced data changes from remote devices,
     *                      OBSERVER_CHANGES_ALL means both native changes and synced data changes.
     * @param observer Indicates the observer to subscribe changes.
     * @return Indicates the status of this subscribe operation.
     */
    Status SubscribeKvStore(SubscribeType subscribeType, std::shared_ptr<KvStoreObserver> observer) override;

    /**
     * @brief Unregister a kvstore to an observer.
     *
     * @param subscribeType Indicates the subscribe type.
     *                      Reserved parameter. Current is always SubscribeType::DEFAULT.
     * @param observer Indicates the observer to unsubscribe this store.
     * @return Indicates the status of this unsubscribe operation.
     */
    Status UnSubscribeKvStore(SubscribeType subscribeType, std::shared_ptr<KvStoreObserver> observer) override;

    /**
     * @brief Register message for sync operation.
     *
     * @param callback Indicates the callback to register.
     * @return Indicates the status of this register operation.
     */
    Status RegisterSyncCallback(std::shared_ptr<KvStoreSyncCallback> callback) override;

    /**
     * @brief Unregister message for sync operation.
     *
     * @param callback Indicates the callback to register.
     * @return Indicates the status of this register operation.
     */
    Status UnRegisterSyncCallback() override;

    /**
     * @brief See Put, PutBatch put a list of entries to kvstore,
     *        all entries will be put in a transaction,
     *        if entries contains invalid entry, PutBatch will all fail.
     *
     * @param entries Indicates the entries.
     *              The entries's size should be less than 128 and memory size must be less than IPC transport limit.
     * @return Indicates the status of this register operation.
     */
    Status PutBatch(const std::vector<Entry> &entries) override;

    /**
     * @brief Delete a list of entries in the kvstore,
     *        delete key not exist still return success,
     *        key length should not be greater than 256, and can not be empty.
     *        If keys contains invalid key, all delete will fail.
     *
     * @param keys Indicates the list of keys.
     *              The keys memory size should not be greater than IPC transport limit, and can not be empty.
     * @return Indicates the status of this register operation.
     */
    Status DeleteBatch(const std::vector<Key> &keys) override;

    /**
     * @brief Start transaction.
     *        All changes to this kvstore will be in a same transaction and will not change the store until Commit() or
     *        Rollback() is called.
     *        Before this transaction is committed or rollbacked, all attemption to close this store will fail.
     *
     * @return Indicates the status of this operation.
     */
    virtual Status StartTransaction() override;

    /**
     * @brief Commit current transaction. All changes to this store will be done after calling this method.
     *        Any calling of this method outside a transaction will fail.
     *
     * @return Indicates the status of this operation.
     */
    virtual Status Commit() override;

    /**
     * @brief Rollback current transaction.
     *        All changes to this store during this transaction will be rollback after calling this method.
     *        Any calling of this method outside a transaction will fail.
     *
     * @return Indicates the status of this operation.
     */
    virtual Status Rollback() override;

    /**
     * @brief Set synchronization parameters of this store.
     *
     * @param syncParam Indicates the sync policy parameter.
     * @return Indicates the status of this operation.
     */
    virtual Status SetSyncParam(const KvSyncParam &syncParam) override;

    /**
     * @brief Get synchronization parameters of this store.
     *
     * @param syncParam Indicates the sync policy parameter.
     * @return Indicates the status of this operation.
     */
    virtual Status GetSyncParam(KvSyncParam &syncParam) override;

    /**
     * @brief Set capability parameters of this store.
     *
     * @param enabled Indicates the capability Enabled status.
     * @return Indicates the status of this operation.
     */
    virtual Status SetCapabilityEnabled(bool enabled) const override;

    /**
     * @brief Set capability parameters of this store.
     *
     * @param localLabels Indicates the labels of local.
     * @param remoteSupportLabels Indicates the labels of remote support.
     * @return Indicates the status of this operation.
     */
    virtual Status SetCapabilityRange(const std::vector<std::string> &localLabels,
        const std::vector<std::string> &remoteSupportLabels) const override;

    /**
     * @brief Get security level.
     *
     * @param securityLevel Indicates the security level.
     * @return Indicates the status of this operation.
     */
    virtual Status GetSecurityLevel(SecurityLevel &securityLevel) const override;

    /**
     * @brief Sync store with other devices only syncing the data which is satisfied with the condition.
     *        This is an asynchronous method, sync will fail if there is a syncing operation in progress.
     *
     * @param deviceIds Indicates the device list to sync, this is network id from soft bus.
     * @param query Indicates the query condition.
     * @param mode Indicates the sync mode, can be set to SyncMode::PUSH, SyncMode::PULL and SyncMode::PUSH_PULL.
     *              PUSH_PULL will firstly push all not-local store to listed devices, then pull these stores back.
     * @return Indicates the status of this operation.
     */
    virtual Status Sync(const std::vector<std::string> &deviceIds, SyncMode mode, const DataQuery &query,
        std::shared_ptr<KvStoreSyncCallback> syncCallback) override;

    /**
     * @brief Subscribe store with other devices consistently Synchronize the data which is satisfied
     *        with the condition.
     *
     * @param deviceIds Indicates the device list to sync, this is network id from soft bus.
     * @param query Indicates the query condition.
     *
     * @return Indicates the status of this operation.
     */
    virtual Status SubscribeWithQuery(const std::vector<std::string> &deviceIds, const DataQuery &query) override;

    /**
     * @brief UnSubscribe store with other devices which is satisfied with the condition.
     *
     * @param deviceIds Indicates the device list to sync, this is network id from soft bus.
     * @param query Indicates the query condition.
     * @return Indicates the status of this UnSubscribe operation.
     */
    virtual Status UnsubscribeWithQuery(const std::vector<std::string> &deviceIds, const DataQuery &query) override;
};
}  // namespace DistributedKv
}  // namespace OHOS
#endif  // SINGLE_KV_STORE_H
