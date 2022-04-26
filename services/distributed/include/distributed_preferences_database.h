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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_DATABASE_H
#define BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_DATABASE_H

#include <mutex>
#include <string>
#include <vector>

#include "distributed_kv_data_manager.h"
#include "singleton.h"

#include "distributed_database_callback.h"
#include "distributed_device_callback.h"
#include "distributed_flow_control.h"

namespace OHOS {
namespace Notification {
class DistributedPreferencesDatabase : private DistributedFlowControl {
public:
    using Entry = DistributedKv::Entry;

    DistributedPreferencesDatabase();
    ~DistributedPreferencesDatabase();

    /**
     * @brief Put a key-value to database.
     *
     * @param key Indicates the key.
     * @param value Indicates the value.
     * @return Whether to put key-value success.
     */
    bool PutToDistributedDB(const std::string &key, const std::string &value);

    /**
     * @brief Get the value of its key from database.
     *
     * @param key Indicates key.
     * @param value Indicates value.
     * @return Whether to get key-value success.
     */
    bool GetFromDistributedDB(const std::string &key, std::string &value);

    /**
     * @brief Get all entries which key start with prefixKey.
     *
     * @param perfixkey Indicates the prefix to be searched.
     * @param entries Indicates the entries will be returned in this parameter.
     * @return Whether to get entries success.
     */
    bool GetEntriesFromDistributedDB(const std::string &prefixKey, std::vector<Entry> &entries);

    /**
     * @brief Delete a key-value of its key from database.
     *
     * @param key Indicates the key.
     * @return Whether to delete key-value success.
     */
    bool DeleteToDistributedDB(const std::string &key);

    /**
     * @brief Clear all data in database and Delete the database.
     *
     * @return Whether to clear database success.
     */
    bool ClearDatabase(void);

private:
    void GetKvDataManager(void);
    bool CheckKvDataManager(void);
    void GetKvStore(void);
    bool CheckKvStore(void);

private:
    std::mutex mutex_;
    std::unique_ptr<DistributedKv::DistributedKvDataManager> kvDataManager_;
    std::shared_ptr<DistributedKv::SingleKvStore> kvStore_;

    DISALLOW_COPY_AND_MOVE(DistributedPreferencesDatabase);
};
}  // namespace Notification
}  // namespace OHOS

#endif /* BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_DATABASE_H */