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

#include "notification_slot_group.h"

#include "string_ex.h"

namespace OHOS {
namespace Notification {
const int MAX_TEXT_LENGTH = 1000;
const std::string LINE_SEPARATOR = "\n";

NotificationSlotGroup::NotificationSlotGroup()
{}

NotificationSlotGroup::NotificationSlotGroup(const std::string &id, const std::string &name) : id_(id), name_(name)
{
    id_ = TruncateString(id);
    name_ = TruncateString(name);
}

NotificationSlotGroup::~NotificationSlotGroup()
{}

std::string NotificationSlotGroup::GetDescription() const
{
    return description_;
}

std::string NotificationSlotGroup::GetId() const
{
    return id_;
}

std::string NotificationSlotGroup::GetName() const
{
    return name_;
}

std::vector<NotificationSlot> NotificationSlotGroup::GetSlots() const
{
    return slots_;
}

bool NotificationSlotGroup::IsDisabled() const
{
    return isDisabled_;
}

void NotificationSlotGroup::SetDescription(const std::string &description)
{
    description_ = TruncateString(description);
}

std::string NotificationSlotGroup::Dump() const
{
    std::string contents;
    for (auto it = slots_.begin(); it != slots_.end(); ++it) {
        contents += it->Dump();
        if (it != slots_.end() - 1) {
            contents += ",";
        }
    }
    return "NotificationSlotGroup[id = " + id_ +
                                  ", name = " + name_ +
                                  ", description = " + description_ +
                                  ", slots = " + contents +
                                  ", isDisabled = " + std::to_string(isDisabled_) + "]";
}

bool NotificationSlotGroup::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(id_)) {
        return false;
    }

    if (!parcel.WriteString(name_)) {
        return false;
    }

    if (!parcel.WriteString(description_)) {
        return false;
    }

    if (slots_.size() == 0) {
        if (!parcel.WriteInt32(0)) {
            return false;
        }
    } else {
        if (!parcel.WriteInt32(slots_.size())) {
            return false;
        }
        for (auto it = slots_.begin(); it != slots_.end(); ++it) {
            if (!it->Marshalling(parcel)) {
                return false;
            }
        }
    }

    if (!parcel.WriteBool(isDisabled_)) {
        return false;
    }

    return true;
}

bool NotificationSlotGroup::ReadFromParcel(Parcel &parcel)
{
    id_ = parcel.ReadString();
    name_ = parcel.ReadString();
    description_ = parcel.ReadString();

    if (slots_.size() == 0) {
        if (!parcel.ReadInt32()) {
            return false;
        }
    } else {
        if (!parcel.ReadInt32()) {
            return false;
        }
        for (auto it = slots_.begin(); it != slots_.end(); ++it) {
            slots_.emplace_back(*it->Unmarshalling(parcel));
        }
    }

    isDisabled_ = parcel.ReadBool();

    return true;
}

NotificationSlotGroup *NotificationSlotGroup::Unmarshalling(Parcel &parcel)
{
    NotificationSlotGroup *notificationSlotGroup = new NotificationSlotGroup();

    if (!notificationSlotGroup && !notificationSlotGroup->ReadFromParcel(parcel)) {
        delete notificationSlotGroup;
        notificationSlotGroup = nullptr;
    }

    return notificationSlotGroup;
}

std::string NotificationSlotGroup::TruncateString(const std::string &inPutString)
{
    std::string temp = inPutString;
    if (inPutString.length() > MAX_TEXT_LENGTH) {
        temp = inPutString.substr(0, MAX_TEXT_LENGTH);
    }
    return temp;
}
}  // namespace Notification
}  // namespace OHOS