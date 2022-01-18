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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_DATABASE_CALLBACK_H
#define BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_DATABASE_CALLBACK_H

#include <string>

#include "kvstore_observer.h"

namespace OHOS {
namespace Notification {
class DistributedDatabaseCallback : public DistributedKv::KvStoreObserver {
public:
    struct IDatabaseChange {
        std::function<void(const std::string &deviceId, const std::string &key, const std::string &value)> OnInsert;
        std::function<void(const std::string &deviceId, const std::string &key, const std::string &value)> OnUpdate;
        std::function<void(const std::string &deviceId, const std::string &key, const std::string &value)> OnDelete;
    };
    explicit DistributedDatabaseCallback(const IDatabaseChange &callback);
    ~DistributedDatabaseCallback();
private:
    void OnChange(const DistributedKv::ChangeNotification &changeNotification,
        std::shared_ptr<DistributedKv::KvStoreSnapshot> snapshot) override;
    void OnChange(const DistributedKv::ChangeNotification &changeNotification) override;

private:
    IDatabaseChange callback_;
};
}  // namespace Notification
}  // namespace OHOS

#endif /* BASE_NOTIFICATION_ANS_STANDARD_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_DATABASE_CALLBACK_H */