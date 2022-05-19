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

#include "distributed_database_callback.h"

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
DistributedDatabaseCallback::DistributedDatabaseCallback(const IDatabaseChange &callback) : callback_(callback)
{}

DistributedDatabaseCallback::~DistributedDatabaseCallback()
{}

void DistributedDatabaseCallback::OnChange(const DistributedKv::ChangeNotification &changeNotification)
{
    ANS_LOGI("%{public}s start", __FUNCTION__);

    if (callback_.OnInsert) {
        const std::vector<DistributedKv::Entry> &entryList = changeNotification.GetInsertEntries();
        ANS_LOGI("GetInsertEntries count %{public}zu", entryList.size());
        for (auto entry : entryList) {
            callback_.OnInsert(changeNotification.GetDeviceId(), entry.key.ToString(), entry.value.ToString());
        }
    }

    if (callback_.OnUpdate) {
        const std::vector<DistributedKv::Entry> &entryList = changeNotification.GetUpdateEntries();
        ANS_LOGI("GetUpdateEntries count %{public}zu", entryList.size());
        for (auto entry : entryList) {
            callback_.OnUpdate(changeNotification.GetDeviceId(), entry.key.ToString(), entry.value.ToString());
        }
    }

    if (callback_.OnDelete) {
        const std::vector<DistributedKv::Entry> &entryList = changeNotification.GetDeleteEntries();
        ANS_LOGI("GetDeleteEntries count %{public}zu", entryList.size());
        for (auto entry : entryList) {
            callback_.OnDelete(changeNotification.GetDeviceId(), entry.key.ToString(), entry.value.ToString());
        }
    }
}
}  // namespace Notification
}  // namespace OHOS