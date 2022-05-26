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

#include "distributed_kv_data_manager.h"

#include "mock_single_kv_store.h"

namespace OHOS {
namespace DistributedKv {
DistributedKvDataManager::DistributedKvDataManager()
{}

DistributedKvDataManager::~DistributedKvDataManager()
{}

Status DistributedKvDataManager::StartWatchDeviceChange(std::shared_ptr<DeviceStatusChangeListener> observer)
{
    return Status::SUCCESS;
}

Status DistributedKvDataManager::StopWatchDeviceChange(std::shared_ptr<DeviceStatusChangeListener> observer)
{
    return Status::SUCCESS;
}

Status DistributedKvDataManager::GetSingleKvStore(const Options &options, const AppId &appId, const StoreId &storeId,
    std::shared_ptr<SingleKvStore> &singleKvStore)
{
    std::shared_ptr<MockSingleKvStore> kvStore = std::make_shared<MockSingleKvStore>();
    singleKvStore = std::static_pointer_cast<SingleKvStore>(kvStore);
    return DistributedKv::Status::SUCCESS;
}

Status DistributedKvDataManager::GetLocalDevice(DeviceInfo &localDevice)
{
    localDevice.deviceId = "<localDeviceId>";
    localDevice.deviceName = "<localDeviceName>";
    localDevice.deviceType = "<localDeviceType>";
    return Status::SUCCESS;
}

Status DistributedKvDataManager::GetDeviceList(std::vector<DeviceInfo> &deviceInfoList, DeviceFilterStrategy strategy)
{
    DeviceInfo localDevice = {
        .deviceId = "<localDeviceId>",
        .deviceName = "<localDeviceName>",
        .deviceType = "<localDeviceType>",
    };
    DeviceInfo remoteDevice = {
        .deviceId = "<remoteDeviceId>",
        .deviceName = "<remoteDeviceName>",
        .deviceType = "<remoteDeviceType>",
    };
    deviceInfoList.clear();
    deviceInfoList.push_back(localDevice);
    deviceInfoList.push_back(remoteDevice);

    return Status::SUCCESS;
}

Status DistributedKvDataManager::CloseKvStore(const AppId &appId, const StoreId &storeId)
{
    return Status::SUCCESS;
}

Status DistributedKvDataManager::CloseKvStore(const AppId &appId, std::shared_ptr<SingleKvStore> &kvStorePtr)
{
    return Status::SUCCESS;
}

Status DistributedKvDataManager::DeleteKvStore(const AppId &appId, const StoreId &storeId)
{
    return Status::SUCCESS;
}
}  // namespace DistributedKv
}  // namespace OHOS