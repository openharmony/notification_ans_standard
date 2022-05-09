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

#include "addnotificationslots_fuzzer.h"

#include "notification_helper.h"

constexpr uint8_t ENABLE = 2;
constexpr uint8_t SLOT_LEVEL_NUM = 6;
constexpr uint8_t SLOT_VISIBLENESS_TYPE_NUM = 4;
constexpr uint8_t SLOT_TYPE_NUM = 5;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        std::string stringData = reinterpret_cast<const char*>(data);

        Notification::NotificationSlot slot;
        slot.SetDescription(stringData);
        slot.SetEnableLight(*data % ENABLE);
        slot.SetEnableVibration(*data % ENABLE);
        slot.SetLedLightColor(U32_AT(data));

        uint8_t level = *data % SLOT_LEVEL_NUM;
        Notification::NotificationSlot::NotificationLevel notificatoinLevel =
            Notification::NotificationSlot::NotificationLevel(level);
        slot.SetLevel(notificatoinLevel);

        uint8_t visibleness = *data % SLOT_VISIBLENESS_TYPE_NUM;
        Notification::NotificationConstant::VisiblenessType visiblenessType =
            Notification::NotificationConstant::VisiblenessType(visibleness);
        slot.SetLockscreenVisibleness(visiblenessType);

        slot.SetSlotGroup(stringData);

        uint8_t type = *data % SLOT_TYPE_NUM;
        Notification::NotificationConstant::SlotType slotType = Notification::NotificationConstant::SlotType(type);
        slot.SetType(slotType);

        std::vector<Notification::NotificationSlot> slots;
        slots.emplace_back(slot);
        return Notification::NotificationHelper::AddNotificationSlots(slots) == ERR_OK;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
