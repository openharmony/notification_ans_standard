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

    bool PutToDistributedDB(const std::string &key, const std::string &value);
    bool GetFromDistributedDB(const std::string &key, std::string &value);
    bool GetEntriesFromDistributedDB(const std::string &prefixKey, std::vector<Entry> &entries);
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