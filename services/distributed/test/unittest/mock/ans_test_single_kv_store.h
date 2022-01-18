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
    virtual Status GetEntries(const Key &prefixKey, std::vector<Entry> &entries) const override;

    virtual Status GetEntriesWithQuery(const std::string &query, std::vector<Entry> &entries) const override;

    virtual Status GetEntriesWithQuery(const DataQuery &query, std::vector<Entry> &entries) const override;

    virtual Status GetResultSet(const Key &prefixKey, std::shared_ptr<KvStoreResultSet> &resultSet) const override;

    virtual Status GetResultSetWithQuery(
        const std::string &query, std::shared_ptr<KvStoreResultSet> &resultSet) const override;

    virtual Status GetResultSetWithQuery(
        const DataQuery &query, std::shared_ptr<KvStoreResultSet> &resultSet) const override;

    virtual Status CloseResultSet(std::shared_ptr<KvStoreResultSet> &resultSet) override;

    virtual Status GetCountWithQuery(const std::string &query, int &result) const override;

    virtual Status GetCountWithQuery(const DataQuery &query, int &result) const override;

    virtual Status Sync(
        const std::vector<std::string> &deviceIds, SyncMode mode, uint32_t allowedDelayMs = 0) override;

    virtual Status RemoveDeviceData(const std::string &device) override;

    virtual StoreId GetStoreId() const override;

    virtual Status Delete(const Key &key) override;

    virtual Status Put(const Key &key, const Value &value) override;

    virtual Status Get(const Key &key, Value &value) override;

    virtual Status SubscribeKvStore(SubscribeType subscribeType, std::shared_ptr<KvStoreObserver> observer) override;

    virtual Status UnSubscribeKvStore(SubscribeType subscribeType, std::shared_ptr<KvStoreObserver> observer) override;

    virtual Status RegisterSyncCallback(std::shared_ptr<KvStoreSyncCallback> callback) override;

    virtual Status UnRegisterSyncCallback() override;

    virtual Status PutBatch(const std::vector<Entry> &entries) override;

    virtual Status DeleteBatch(const std::vector<Key> &keys) override;

    virtual Status StartTransaction() override;

    virtual Status Commit() override;

    virtual Status Rollback() override;

    virtual Status SetSyncParam(const KvSyncParam &syncParam) override;

    virtual Status GetSyncParam(KvSyncParam &syncParam) override;

    virtual Status SetCapabilityEnabled(bool enabled) const override;

    virtual Status SetCapabilityRange(const std::vector<std::string> &localLabels,
        const std::vector<std::string> &remoteSupportLabels) const override;

    virtual Status GetSecurityLevel(SecurityLevel &securityLevel) const override;

    virtual Status SyncWithCondition(const std::vector<std::string> &deviceIds, SyncMode mode,
                                     const DataQuery &query) override;

    virtual Status SubscribeWithQuery(const std::vector<std::string> &deviceIds, const DataQuery &query) override;

    virtual Status UnSubscribeWithQuery(const std::vector<std::string> &deviceIds, const DataQuery &query) override;

    Status GetKvStoreSnapshot(std::shared_ptr<KvStoreObserver> observer,
                              std::shared_ptr<KvStoreSnapshot> &snapshot) const override;

    Status ReleaseKvStoreSnapshot(std::shared_ptr<KvStoreSnapshot> &snapshot) override;

    Status Clear() override;

protected:
    KVSTORE_API virtual Status Control(KvControlCmd cmd, const KvParam &inputParam, KvParam &output) override;
};
}  // namespace DistributedKv
}  // namespace OHOS
#endif  // SINGLE_KV_STORE_H
