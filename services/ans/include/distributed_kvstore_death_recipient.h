/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_DISTRIBUTED_KVSTORE_DEATH_RECIPIENT_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_DISTRIBUTED_KVSTORE_DEATH_RECIPIENT_H

#include <functional>

#include "kvstore_death_recipient.h"

namespace OHOS {
namespace Notification {
class DistributedKvStoreDeathRecipient : public DistributedKv::KvStoreDeathRecipient {
public:
    /**
     * @brief The constructor.
     *
     * @param callback Indicates the callback.
     */
    explicit DistributedKvStoreDeathRecipient(const std::function<void()> &callback)
    {
        callback_ = callback;
    }

    /**
     * @brief The deconstructor.
     */
    virtual ~DistributedKvStoreDeathRecipient()
    {}

    /**
     * @brief Obtains the death event. Inherited from DistributedKv::KvStoreDeathRecipient.
     */
    void OnRemoteDied() override
    {
        if (callback_ != nullptr) {
            callback_();
        }
    }

private:
    std::function<void()> callback_;
};
}  // namespace Notification
}  // namespace OHOS
#endif // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_ANS_INCLUDE_DISTRIBUTED_KVSTORE_DEATH_RECIPIENT_H