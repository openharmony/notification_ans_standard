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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_FLOW_CONTROL_H
#define BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_FLOW_CONTROL_H

#include <chrono>
#include <list>

namespace OHOS {
namespace Notification {
class DistributedFlowControl {
public:
    DistributedFlowControl(int kvManagerSecondMaxinum = KVMANAGER_MAXINUM_PER_SECOND,
        int kvManagerMinuteMaxinum = KVMANAGER_MAXINUM_PER_MINUTE,
        int kvStoreSecondMaxinum = KVSTORE_MAXINUM_PER_SECOND,
        int kvStoreMinuteMaxinum = KVSTORE_MAXINUM_PER_MINUTE);
    bool KvManagerFlowControl(void);
    bool KvStoreFlowControl(void);
    void KvManagerFlowControlClear(void);
    void KvStoreFlowControlClear(void);

protected:
    static const int KVMANAGER_MAXINUM_PER_SECOND = 50;
    static const int KVMANAGER_MAXINUM_PER_MINUTE = 500;
    static const int KVSTORE_MAXINUM_PER_SECOND = 1000;
    static const int KVSTORE_MAXINUM_PER_MINUTE = 10000;

private:
    int kvManagerSecondMaxinum_;
    int kvManagerMinuteMaxinum_;
    int kvStoreSecondMaxinum_;
    int kvStoreMinuteMaxinum_;
    std::list<std::chrono::system_clock::time_point> kvDataManagerTimestampList_;
    std::list<std::chrono::system_clock::time_point> kvStoreTimestampList_;
};
}  // namespace Notification
}  // namespace OHOS

#endif /* BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_FLOW_CONTROL_H */