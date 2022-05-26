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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_DATABASE_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_DATABASE_H

#include <chrono>
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
class DistributedDatabase : private DistributedFlowControl {
public:
    using Entry = DistributedKv::Entry;
    using DeviceInfo = DistributedKv::DeviceInfo;

    /**
     * @brief The constructor.
     *
     * @param databaseCb Distributed notification info changed callback object.
     * @param deviceCb Device connection info changed callback object.
     */
    DistributedDatabase(
        std::shared_ptr<DistributedDatabaseCallback> databaseCb, std::shared_ptr<DistributedDeviceCallback> deviceCb);

    /**
     * @brief The deconstructor.
     */
    ~DistributedDatabase();

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
     * @param key Indicates the key.
     * @param value Indicates the value.
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
     * @brief Clear all entries which put by specified device.
     *
     * @param deviceId Indicates the id of specified device.
     * @return Whether to clear device entries success.
     */
    bool ClearDataByDevice(const std::string &deviceId);

    /**
     * @brief Get local device id.
     *
     * @param deviceId Indicates the id of local device.
     * @return Whether to get device id success.
     */
    bool GetLocalDeviceId(std::string &deviceId);

    /**
     * @brief Get local device info.
     *
     * @param localInfo Indicates the infomation of local device.
     * @return Whether to get device infomation success.
     */
    bool GetLocalDeviceInfo(DeviceInfo &localInfo);

    /**
     * @brief Get all devices info on the network.
     *
     * @param deviceList Indicates the infomation list of devices.
     * @return Whether to get devices infomation success.
     */
    bool GetDeviceInfoList(std::vector<DeviceInfo> &deviceList);

    /**
     * @brief Recreate the database of distributed notification
     *
     * @return Whether to recreate the database success.
     */
    bool RecreateDistributedDB();

private:
    void GetKvDataManager(void);
    bool CheckKvDataManager(void);
    void GetKvStore(void);
    bool CheckKvStore(void);

private:
    std::mutex mutex_;
    std::unique_ptr<DistributedKv::DistributedKvDataManager> kvDataManager_;
    std::shared_ptr<DistributedKv::SingleKvStore> kvStore_;
    std::shared_ptr<DistributedDatabaseCallback> databaseCb_;
    std::shared_ptr<DistributedDeviceCallback> deviceCb_;

    std::string localDeviceId_;

    DISALLOW_COPY_AND_MOVE(DistributedDatabase);
};
}  // namespace Notification
}  // namespace OHOS

#endif // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_DATABASE_H