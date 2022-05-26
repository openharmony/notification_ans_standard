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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_FLOW_CONTROL_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_FLOW_CONTROL_H

#include <chrono>
#include <list>

namespace OHOS {
namespace Notification {
class DistributedFlowControl {
public:
    /**
     * @brief The constructor.
     *
     * @param kvManagerSecondMaxinum The maximum number of calls to DistributedKvDataManager interface per second.
     * @param kvManagerMinuteMaxinum The maximum number of calls to DistributedKvDataManager interface per minute.
     * @param kvStoreSecondMaxinum The maximum number of calls to SingleKvStore interface per second.
     * @param kvStoreMinuteMaxinum The maximum number of calls to SingleKvStore interface per minute.
     */
    DistributedFlowControl(size_t kvManagerSecondMaxinum = KVMANAGER_MAXINUM_PER_SECOND,
        size_t kvManagerMinuteMaxinum = KVMANAGER_MAXINUM_PER_MINUTE,
        size_t kvStoreSecondMaxinum = KVSTORE_MAXINUM_PER_SECOND,
        size_t kvStoreMinuteMaxinum = KVSTORE_MAXINUM_PER_MINUTE);

    /**
     * @brief Check if DistributedKvDataManager interface flow control can pass.
     *
     * @return True on passed, otherwise false.
     */
    bool KvManagerFlowControl(void);

    /**
     * @brief Check if SingleKvStore interface flow control can pass.
     *
     * @return True on passed, otherwise false.
     */
    bool KvStoreFlowControl(void);

    /**
     * @brief Clear DistributedKvDataManager interface flow control count.
     */
    void KvManagerFlowControlClear(void);

    /**
     * @brief Clear SingleKvStore interface flow control count.
     */
    void KvStoreFlowControlClear(void);

protected:
    static const size_t KVMANAGER_MAXINUM_PER_SECOND = 50;
    static const size_t KVMANAGER_MAXINUM_PER_MINUTE = 500;
    static const size_t KVSTORE_MAXINUM_PER_SECOND = 1000;
    static const size_t KVSTORE_MAXINUM_PER_MINUTE = 10000;

private:
    size_t kvManagerSecondMaxinum_;
    size_t kvManagerMinuteMaxinum_;
    size_t kvStoreSecondMaxinum_;
    size_t kvStoreMinuteMaxinum_;
    std::list<std::chrono::system_clock::time_point> kvDataManagerTimestampList_;
    std::list<std::chrono::system_clock::time_point> kvStoreTimestampList_;
};
}  // namespace Notification
}  // namespace OHOS

#endif // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_FLOW_CONTROL_H