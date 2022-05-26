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

#include "enabled_notification_callback_data.h"
#include "ans_log_wrapper.h"
#include "string_ex.h"

namespace OHOS {
namespace Notification {
EnabledNotificationCallbackData::EnabledNotificationCallbackData(std::string bundle, uid_t uid, bool enable)
    : bundle_(bundle), uid_(uid), enable_(enable)
{}

void EnabledNotificationCallbackData::SetBundle(const std::string bundle)
{
    bundle_ = bundle;
}

std::string EnabledNotificationCallbackData::GetBundle() const
{
    return bundle_;
}

void EnabledNotificationCallbackData::SetUid(const uid_t uid)
{
    uid_ = uid;
}

uid_t EnabledNotificationCallbackData::GetUid() const
{
    return uid_;
}

void EnabledNotificationCallbackData::SetEnable(const bool enable)
{
    enable_ = enable;
}

bool EnabledNotificationCallbackData::GetEnable() const
{
    return enable_;
}

std::string EnabledNotificationCallbackData::Dump()
{
    return "EnabledNotificationCallbackData{ "
            "bundle = " + bundle_ +
            ", uid = " + std::to_string(uid_) +
            ", enable = " + std::to_string(enable_) +
            " }";
}

bool EnabledNotificationCallbackData::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString16(Str8ToStr16(bundle_))) {
        ANS_LOGE("Failed to write bundle name");
        return false;
    }

    if (!parcel.WriteInt32(uid_)) {
        ANS_LOGE("Failed to write uid");
        return false;
    }

    if (!parcel.WriteBool(enable_)) {
        ANS_LOGE("Failed to write enable");
        return false;
    }

    return true;
}

EnabledNotificationCallbackData *EnabledNotificationCallbackData::Unmarshalling(Parcel &parcel)
{
    auto objptr = new (std::nothrow) EnabledNotificationCallbackData();
    if ((objptr != nullptr) && !objptr->ReadFromParcel(parcel)) {
        delete objptr;
        objptr = nullptr;
    }

    return objptr;
}

bool EnabledNotificationCallbackData::ReadFromParcel(Parcel &parcel)
{
    bundle_ = Str16ToStr8(parcel.ReadString16());
    uid_ = static_cast<uid_t>(parcel.ReadInt32());
    enable_ = parcel.ReadBool();

    return true;
}
}  // namespace Notification
}  // namespace OHOS