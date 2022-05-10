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

#include "publishnotification_fuzzer.h"

#include "notification_helper.h"

namespace OHOS {
    namespace {
        constexpr uint8_t ENABLE = 2;
        constexpr uint8_t SLOT_TYPE_NUM = 5;
        constexpr uint8_t FLAG_STATUS = 3;
    }
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        std::string stringData = reinterpret_cast<const char*>(data);
        Notification::NotificationRequest request;
        request.SetAlertOneTime(*data % ENABLE);

        int32_t style = static_cast<int32_t>(U32_AT(data));
        Notification::NotificationRequest::BadgeStyle badgeStyle =
            Notification::NotificationRequest::BadgeStyle(style);
        request.SetBadgeIconStyle(badgeStyle);
        request.SetBadgeNumber(style);
        request.SetClassification(stringData);

        uint32_t color = U32_AT(data);
        request.SetColor(color);
        request.SetColorEnabled(*data % ENABLE);

        std::shared_ptr<Notification::NotificationNormalContent> contentType =
            std::make_shared<Notification::NotificationNormalContent>();
        contentType->SetText(stringData);
        contentType->SetTitle(stringData);
        contentType->SetAdditionalText(stringData);
        std::shared_ptr<Notification::NotificationContent> content =
            std::make_shared<Notification::NotificationContent>(contentType);
        request.SetContent(content);
        request.SetCountdownTimer(*data % ENABLE);
        request.SetCreatorBundleName(stringData);
        request.SetDeliveryTime(style);

        std::shared_ptr<Notification::NotificationFlags> notificationFlages =
            std::make_shared<Notification::NotificationFlags>();
        int32_t soundEnabled = static_cast<int32_t>(*data % FLAG_STATUS);
        Notification::NotificationConstant::FlagStatus sound =
            Notification::NotificationConstant::FlagStatus(soundEnabled);
        notificationFlages->SetSoundEnabled(sound);
        notificationFlages->SetVibrationEnabled(sound);
        request.SetFlags(notificationFlages);

        Notification::NotificationRequest::GroupAlertType groupAlertType =
            Notification::NotificationRequest::GroupAlertType(color);
        request.SetGroupAlertType(groupAlertType);

        request.SetGroupName(stringData);
        request.SetGroupOverview(*data % ENABLE);
        request.SetLabel(stringData);
        request.SetNotificationId(style);
        request.SetOwnerBundleName(stringData);

        uint8_t types = *data % SLOT_TYPE_NUM;
        Notification::NotificationConstant::SlotType slotType = Notification::NotificationConstant::SlotType(types);
        request.SetSlotType(slotType);

        return Notification::NotificationHelper::PublishNotification(stringData, request) == ERR_OK;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
