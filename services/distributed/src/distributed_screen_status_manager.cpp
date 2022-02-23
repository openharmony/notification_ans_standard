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

#include "distributed_screen_status_manager.h"

#include "ans_inner_errors.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
namespace {
const std::string APP_ID = "advanced_notification_service";
const std::string STORE_ID = "distributed_screen_status";
const std::string DELIMITER = "|";
const std::string SCREEN_STATUS_LABEL = "screen_status";
const std::string SCREEN_STATUS_VALUE_ON = "on";
const std::string SCREEN_STATUS_VALUE_OFF = "off";
}  // namespace

DistributedScreenStatusManager::DistributedScreenStatusManager() : DistributedFlowControl()
{
    DistributedDeviceCallback::IDeviceChange callback = {
        .OnConnected = std::bind(&DistributedScreenStatusManager::OnDeviceConnected, this, std::placeholders::_1),
        .OnDisconnected = std::bind(&DistributedScreenStatusManager::OnDeviceDisconnected, this, std::placeholders::_1),
    };
    deviceCb_ = std::make_shared<DistributedDeviceCallback>(callback);
    GetKvDataManager();
    GetKvStore();
}

DistributedScreenStatusManager::~DistributedScreenStatusManager()
{}

void DistributedScreenStatusManager::OnDeviceConnected(const std::string &deviceId)
{
    ANS_LOGI("deviceId:%{public}s", deviceId.c_str());
}

void DistributedScreenStatusManager::OnDeviceDisconnected(const std::string &deviceId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!CheckKvDataManager()) {
        return;
    }

    std::vector<DistributedKv::DeviceInfo> devInfoList;
    DistributedKv::Status status =
        kvDataManager_->GetDeviceList(devInfoList, DistributedKv::DeviceFilterStrategy::NO_FILTER);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager GetDeviceList() failed ret = 0x%{public}x", status);
        kvDataManager_.reset();
        return;
    }

    if (!devInfoList.empty()) {
        return;
    }

    kvStore_.reset();

    DistributedKv::AppId appId = {.appId = APP_ID};
    DistributedKv::StoreId storeId = {.storeId = STORE_ID};
    kvDataManager_->DeleteKvStore(appId, storeId);

    if (!CheckKvStore()) {
        return;
    }

    SetLocalScreenStatus(localScreenOn_);
}

void DistributedScreenStatusManager::GetKvDataManager(void)
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

bool DistributedScreenStatusManager::CheckKvDataManager(void)
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

void DistributedScreenStatusManager::GetKvStore(void)
{
    if (!CheckKvDataManager()) {
        return;
    }

    DistributedKv::Status status;
    DistributedKv::Options options;
    options.createIfMissing = true;
    options.autoSync = true;
    options.kvStoreType = DistributedKv::KvStoreType::SINGLE_VERSION;

    DistributedKv::AppId appId = {.appId = APP_ID};
    DistributedKv::StoreId storeId = {.storeId = STORE_ID};
    status = kvDataManager_->GetSingleKvStore(options, appId, storeId, kvStore_);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager GetSingleKvStore failed ret = 0x%{public}x", status);
        kvStore_.reset();
        kvDataManager_->StopWatchDeviceChange(deviceCb_);
        kvDataManager_.reset();
        return;
    }

    KvStoreFlowControlClear();
}

bool DistributedScreenStatusManager::CheckKvStore(void)
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

std::string DistributedScreenStatusManager::GenerateDistributedKey(const std::string &deviceId)
{
    return deviceId + DELIMITER + SCREEN_STATUS_LABEL;
}

ErrCode DistributedScreenStatusManager::CheckRemoteDevicesIsUsing(bool &isUsing)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!CheckKvDataManager() || !CheckKvStore()) {
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    if (!KvManagerFlowControl() || !KvStoreFlowControl()) {
        ANS_LOGE("flow control.");
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    std::vector<DistributedKv::DeviceInfo> devInfoList;
    DistributedKv::Status status =
        kvDataManager_->GetDeviceList(devInfoList, DistributedKv::DeviceFilterStrategy::NO_FILTER);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager GetDeviceList() failed ret = 0x%{public}x", status);
        kvDataManager_.reset();
        return ERR_ANS_DISTRIBUTED_GET_INFO_FAILED;
    }

    DistributedKv::Key prefixKey("");
    std::vector<DistributedKv::Entry> entries;
    status = kvStore_->GetEntries(prefixKey, entries);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvStore GetEntries() failed ret = 0x%{public}x", status);
        kvStore_.reset();
        return ERR_ANS_DISTRIBUTED_GET_INFO_FAILED;
    }

    for (auto entry : entries) {
        std::string key = entry.key.ToString();
        std::string deviceId = key.substr(0, key.find_first_of(DELIMITER));
        ANS_LOGD("key-deviceId:%{public}s, value:%{public}s", deviceId.c_str(), entry.value.ToString().c_str());
        for (auto devInfo : devInfoList) {
            ANS_LOGD("list-deviceId:%{public}s", devInfo.deviceId.c_str());
            if (devInfo.deviceId == deviceId) {
                isUsing = isUsing || (entry.value.ToString() == SCREEN_STATUS_VALUE_ON);
                break;
            }
        }
        if (isUsing) {
            break;
        }
    }

    ANS_LOGI("%{public}s, isUsing:%{public}s", __FUNCTION__, isUsing ? "true" : "false");
    return ERR_OK;
}

ErrCode DistributedScreenStatusManager::SetLocalScreenStatus(bool screenOn)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ANS_LOGI("%{public}s, screenOn:%{public}s", __FUNCTION__, screenOn ? "true" : "false");
    localScreenOn_ = screenOn;
    if (!CheckKvStore()) {
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    if (!KvManagerFlowControl() || !KvStoreFlowControl()) {
        ANS_LOGE("flow control.");
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    DistributedKv::DeviceInfo localDevice;
    DistributedKv::Status status = kvDataManager_->GetLocalDevice(localDevice);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvDataManager GetLocalDevice() failed ret = 0x%{public}x", status);
        return ERR_ANS_DISTRIBUTED_GET_INFO_FAILED;
    }

    DistributedKv::Key kvStoreKey = GenerateDistributedKey(localDevice.deviceId);
    DistributedKv::Value kvStoreValue = screenOn ? SCREEN_STATUS_VALUE_ON : SCREEN_STATUS_VALUE_OFF;
    status = kvStore_->Put(kvStoreKey, kvStoreValue);
    if (status != DistributedKv::Status::SUCCESS) {
        ANS_LOGE("kvStore Put() failed ret = 0x%{public}x", status);
        return ERR_ANS_DISTRIBUTED_OPERATION_FAILED;
    }

    return ERR_OK;
}
}  // namespace Notification
}  // namespace OHOS