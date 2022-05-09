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

#include "distributed_flow_control.h"

namespace OHOS {
namespace Notification {
DistributedFlowControl::DistributedFlowControl(
    size_t kvManagerSecondMaxinum, size_t kvManagerMinuteMaxinum, size_t kvStoreSecondMaxinum,
    size_t kvStoreMinuteMaxinum)
    : kvManagerSecondMaxinum_(kvManagerSecondMaxinum),
      kvManagerMinuteMaxinum_(kvManagerMinuteMaxinum),
      kvStoreSecondMaxinum_(kvStoreSecondMaxinum),
      kvStoreMinuteMaxinum_(kvStoreMinuteMaxinum)
{}

bool DistributedFlowControl::KvManagerFlowControl(void)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    kvDataManagerTimestampList_.remove_if([&](const std::chrono::system_clock::time_point &value) -> bool {
        return now - value > std::chrono::minutes(1);
    });

    size_t listSize = kvStoreTimestampList_.size();
    if (listSize >= kvManagerMinuteMaxinum_) {
        return false;
    }

    size_t count = 0;
    for (auto value : kvDataManagerTimestampList_) {
        if (now - value > std::chrono::seconds(1)) {
            if (count >= kvManagerSecondMaxinum_) {
                return false;
            } else {
                break;
            }
        }
        count++;
    }

    kvDataManagerTimestampList_.push_front(now);
    return true;
}

bool DistributedFlowControl::KvStoreFlowControl(void)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    kvStoreTimestampList_.remove_if([&](const std::chrono::system_clock::time_point &value) -> bool {
        return now - value > std::chrono::minutes(1);
    });

    size_t listSize = kvStoreTimestampList_.size();
    if (listSize >= kvStoreMinuteMaxinum_) {
        return false;
    }

    size_t count = 0;
    for (auto value : kvStoreTimestampList_) {
        if (now - value > std::chrono::seconds(1)) {
            if (count >= kvStoreSecondMaxinum_) {
                return false;
            } else {
                break;
            }
        }
        count++;
    }

    kvStoreTimestampList_.push_front(now);
    return true;
}

void DistributedFlowControl::KvManagerFlowControlClear(void)
{
    kvDataManagerTimestampList_.clear();
}

void DistributedFlowControl::KvStoreFlowControlClear(void)
{
    kvStoreTimestampList_.clear();
}
}  // namespace Notification
}  // namespace OHOS
