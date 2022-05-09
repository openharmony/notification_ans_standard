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

#include "getnotificationslotnumasbundle_fuzzer.h"

#include "notification_helper.h"

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        Notification::NotificationBundleOption bundleOption;
        std::string bundleName = reinterpret_cast<const char*>(data);
        bundleOption.SetBundleName(bundleName);
        int32_t uid = static_cast<int32_t>(U32_AT(data));
        bundleOption.SetUid(uid);
        uint64_t num;
        return Notification::NotificationHelper::GetNotificationSlotNumAsBundle(bundleOption, num) == ERR_OK;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
