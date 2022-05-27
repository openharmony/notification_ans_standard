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

#include "notification_flags.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
void NotificationFlags::SetSoundEnabled(NotificationConstant::FlagStatus soundEnabled)
{
    soundEnabled_ = soundEnabled;
}

NotificationConstant::FlagStatus NotificationFlags::IsSoundEnabled() const
{
    return soundEnabled_;
}

void NotificationFlags::SetVibrationEnabled(NotificationConstant::FlagStatus vibrationEnabled)
{
    vibrationEnabled_ = vibrationEnabled;
}

NotificationConstant::FlagStatus NotificationFlags::IsVibrationEnabled() const
{
    return vibrationEnabled_;
}

std::string NotificationFlags::Dump()
{
    return "soundEnabled = " + std::to_string(static_cast<uint8_t>(soundEnabled_)) +
        ", vibrationEnabled = " + std::to_string(static_cast<uint8_t>(vibrationEnabled_));
}

bool NotificationFlags::ToJson(nlohmann::json &jsonObject) const
{
    jsonObject["soundEnabled"]     = soundEnabled_;
    jsonObject["vibrationEnabled"] = vibrationEnabled_;

    return true;
}

NotificationFlags *NotificationFlags::FromJson(const nlohmann::json &jsonObject)
{
    if (jsonObject.is_null() or !jsonObject.is_object()) {
        ANS_LOGE("Invalid JSON object");
        return nullptr;
    }

    auto pFlags = new (std::nothrow) NotificationFlags();
    if (pFlags == nullptr) {
        ANS_LOGE("Failed to create notificationFlags instance");
        return nullptr;
    }

    const auto &jsonEnd = jsonObject.cend();
    if (jsonObject.find("soundEnabled") != jsonEnd) {
        auto soundEnabled  = jsonObject.at("soundEnabled").get<uint8_t>();
        pFlags->soundEnabled_ = static_cast<NotificationConstant::FlagStatus>(soundEnabled);
    }

    if (jsonObject.find("vibrationEnabled") != jsonEnd) {
        auto vibrationEnabled = jsonObject.at("vibrationEnabled").get<uint8_t>();
        pFlags->vibrationEnabled_ = static_cast<NotificationConstant::FlagStatus>(vibrationEnabled);
    }

    return pFlags;
}

bool NotificationFlags::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint8(static_cast<uint8_t>(soundEnabled_))) {
        ANS_LOGE("Failed to write flag sound enable for the notification");
        return false;
    }

    if (!parcel.WriteUint8(static_cast<uint8_t>(vibrationEnabled_))) {
        ANS_LOGE("Failed to write flag vibration enable for the notification");
        return false;
    }

    return true;
}

NotificationFlags *NotificationFlags::Unmarshalling(Parcel &parcel)
{
    auto templ = new NotificationFlags();
    if ((templ != nullptr) && !templ->ReadFromParcel(parcel)) {
        delete templ;
        templ = nullptr;
    }

    return templ;
}

bool NotificationFlags::ReadFromParcel(Parcel &parcel)
{
    soundEnabled_ = static_cast<NotificationConstant::FlagStatus>(parcel.ReadUint8());
    vibrationEnabled_ = static_cast<NotificationConstant::FlagStatus>(parcel.ReadUint8());

    return true;
}
}  // namespace Notification
}  // namespace OHOS
