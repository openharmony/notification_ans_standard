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

#include "distributed_device_callback.h"

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
DistributedDeviceCallback::DistributedDeviceCallback(const IDeviceChange &callback) : callback_(callback)
{}
DistributedDeviceCallback::~DistributedDeviceCallback()
{}

void DistributedDeviceCallback::OnDeviceChanged(
    const DistributedKv::DeviceInfo &info, const DistributedKv::DeviceChangeType &type) const
{
    ANS_LOGI("%{public}s start", __FUNCTION__);
    if (type == DistributedKv::DeviceChangeType::DEVICE_ONLINE) {
        ANS_LOGI("device %{public}s is ONLINE", info.deviceId.c_str());
        if (callback_.OnConnected) {
            callback_.OnConnected(info.deviceId);
        }
    }

    if (type == DistributedKv::DeviceChangeType::DEVICE_OFFLINE) {
        ANS_LOGI("device %{public}s is OFFLINE", info.deviceId.c_str());
        if (callback_.OnDisconnected) {
            callback_.OnDisconnected(info.deviceId);
        }
    }
}
DistributedKv::DeviceFilterStrategy DistributedDeviceCallback::GetFilterStrategy() const
{
    return DistributedKv::DeviceFilterStrategy::NO_FILTER;
}
}  // namespace Notification
}  // namespace OHOS