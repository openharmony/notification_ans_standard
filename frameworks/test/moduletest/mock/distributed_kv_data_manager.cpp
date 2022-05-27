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

#define LOG_TAG "DistributedKvDataManager"

#include "distributed_kv_data_manager.h"
#include "mock_single_kv_store.h"
#include "types.h"

namespace OHOS {
namespace DistributedKv {
DistributedKvDataManager::DistributedKvDataManager()
{}

DistributedKvDataManager::~DistributedKvDataManager()
{}

Status DistributedKvDataManager::GetSingleKvStore(const Options &options, const AppId &appId, const StoreId &storeId,
                                                  std::shared_ptr<SingleKvStore> &kvStore)
{
    std::string storeIdTmp = storeId.storeId;
    kvStore = std::make_shared<MockSingleKvStore>();
    MockSingleKvStore::InsertMockKvStore(appId, storeId, kvStore);
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
    MockSingleKvStore::RemoveMockKvStore(appId, storeId);
    return Status::SUCCESS;
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
    DeviceInfo remoteDevice = {
        .deviceId = "<remoteDeviceId>",
        .deviceName = "<remoteDeviceName>",
        .deviceType = "<remoteDeviceType>",
    };
    deviceInfoList.clear();
    deviceInfoList.push_back(remoteDevice);

    return Status::SUCCESS;
}

void DistributedKvDataManager::RegisterKvStoreServiceDeathRecipient(
    std::shared_ptr<KvStoreDeathRecipient> kvStoreDeathRecipient)
{}

void DistributedKvDataManager::UnRegisterKvStoreServiceDeathRecipient(
    std::shared_ptr<KvStoreDeathRecipient> kvStoreDeathRecipient)
{}
}  // namespace DistributedKv
}  // namespace OHOS
