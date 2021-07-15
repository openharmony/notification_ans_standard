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

#include "message_user.h"

namespace OHOS {
namespace Notification {

MessageUser::MessageUser()
    : uri_("")
{}

MessageUser::~MessageUser()
{}

void MessageUser::SetKey(const std::string &key)
{
    key_ = key;
}

std::string MessageUser::GetKey() const
{
    return key_;
}

void MessageUser::SetName(const std::string &name)
{
    name_ = name;
}

std::string MessageUser::GetName() const
{
    return name_;
}

void MessageUser::SetPixelMap(const std::shared_ptr<PixelMap> &pixelMap)
{
    pixelMap_ = pixelMap;
}

const std::shared_ptr<PixelMap> MessageUser::GetPixelMap() const
{
    return pixelMap_;
}

void MessageUser::SetUri(const Uri &uri)
{
    uri_ = uri;
}

Uri MessageUser::GetUri() const
{
    return uri_;
}

void MessageUser::SetMachine(bool machine)
{
    isMachine_ = machine;
}

bool MessageUser::IsMachine() const
{
    return isMachine_;
}

void MessageUser::SetUserAsImportant(bool userImportant)
{
    isUserImportant_ = userImportant;
}

bool MessageUser::IsUserImportant() const
{
    return isUserImportant_;
}

std::string MessageUser::Dump() const
{
    return "MessageUser[key = " + key_ +
                        ", name = " + name_ +
                        ", pixelMap = " + (pixelMap_ ? "not null" : "null") +
                        ", uri = " + uri_.ToString() +
                        ", isMachine = " + std::to_string(isMachine_) +
                        ", isUserImportant = " + std::to_string(isUserImportant_) + "]";
}

bool MessageUser::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(key_)) {
        return false;
    }

    if (!parcel.WriteString(name_)) {
        return false;
    }

    if (!parcel.WriteBool(isMachine_)) {
        return false;
    }

    if (!parcel.WriteBool(isUserImportant_)) {
        return false;
    }

    if (uri_.ToString().empty()) {
        if (!parcel.WriteInt32(VALUE_NULL)) {
            return false;
        }
    } else {
        if (!parcel.WriteInt32(VALUE_OBJECT)) {
            return false;
        }
        if (!parcel.WriteString((uri_.ToString()))) {
            return false;
        }
    }

    return true;
}

bool MessageUser::ReadFromParcel(Parcel &parcel)
{
    key_ = parcel.ReadString();
    name_ = parcel.ReadString();
    isMachine_ = parcel.ReadBool();
    isUserImportant_ = parcel.ReadBool();

    int empty = VALUE_NULL;
    if (!parcel.ReadInt32(empty)) {
        return false;
    }

    if (empty == VALUE_OBJECT) {
        uri_ = Uri((parcel.ReadString()));
    }

    return true;
}

MessageUser *MessageUser::Unmarshalling(Parcel &parcel)
{
    MessageUser *messageUser = new MessageUser();

    if (messageUser && !messageUser->ReadFromParcel(parcel)) {
        delete messageUser;
        messageUser = nullptr;
    }

    return messageUser;
}
}  // namespace Notification
}  // namespace OHOS