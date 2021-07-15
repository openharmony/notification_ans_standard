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

#include "notification_slot_filter.h"

#include "notification_preferences.h"

namespace OHOS {
namespace Notification {

void NotificationSlotFilter::OnStart()
{}

void NotificationSlotFilter::OnStop()
{}

ErrCode NotificationSlotFilter::OnPublish(const sptr<Notification> &notification)
{
    sptr<NotificationSlot> slot;
    ErrCode result = NotificationPreferences::GetInstance().GetNotificationSlot(
        notification->GetBundleName(), notification->request_->GetSlotType(), slot);
    if (result == ERR_OK && slot != nullptr) {
        if (slot->CanEnableLight()) {
            notification->SetLedLightColor(slot->GetLedLightColor());
        }

        if (slot->CanVibrate()) {
            notification->SetVibrationStyle(slot->GetVibrationStyle());
        }

        notification->SetSound(slot->GetSound());

        notification->request_->SetVisibleness(slot->GetLockScreenVisibleness());
    }

    return result;
}

}  // namespace Notification
}  // namespace OHOS