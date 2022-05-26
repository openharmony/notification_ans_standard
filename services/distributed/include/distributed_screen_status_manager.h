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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_SCREEN_STATUS_MANAGER_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_SCREEN_STATUS_MANAGER_H

#include <mutex>

#include "distributed_kv_data_manager.h"
#include "event_handler.h"
#include "event_runner.h"
#include "singleton.h"

#include "distributed_device_callback.h"
#include "distributed_flow_control.h"

namespace OHOS {
namespace Notification {
class DistributedScreenStatusManager : private DistributedFlowControl,
                                       public DelayedSingleton<DistributedScreenStatusManager> {
public:
    /**
     * @brief Check if any other device screen is on.
     *
     * @param isUsing True for any other device screen is on, otherwise false.
     * @return Returns the error code.
     */
    ErrCode CheckRemoteDevicesIsUsing(bool &isUsing);

    /**
     * @brief Set screen status of local device.
     *
     * @param screenOn Indicates the local device screen status.
     * @return Returns the error code.
     */
    ErrCode SetLocalScreenStatus(bool screenOn);

private:
    void OnDeviceConnected(const std::string &deviceId);
    void OnDeviceDisconnected(const std::string &deviceId);

    void GetKvDataManager(void);
    bool CheckKvDataManager(void);
    void GetKvStore(void);
    bool CheckKvStore(void);

    std::string GenerateDistributedKey(const std::string &deviceId);

private:
    std::recursive_mutex mutex_;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_ = nullptr;
    std::unique_ptr<DistributedKv::DistributedKvDataManager> kvDataManager_ = nullptr;
    std::shared_ptr<DistributedKv::SingleKvStore> kvStore_ = nullptr;
    std::shared_ptr<DistributedDeviceCallback> deviceCb_ = nullptr;

    bool localScreenOn_ = false;

    DECLARE_DELAYED_SINGLETON(DistributedScreenStatusManager);
    DISALLOW_COPY_AND_MOVE(DistributedScreenStatusManager);
};
}  // namespace Notification
}  // namespace OHOS

#endif // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_SCREEN_STATUS_MANAGER_H
