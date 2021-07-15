/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"){}
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

#include "notification.h"

#include <sstream>

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
Notification::Notification(){};

Notification::Notification(const sptr<NotificationRequest> &request)
{
    request_ = request;
    key_ = GenerateNotificationKey(GetUid(), GetLabel(), GetId());
}

Notification::Notification(const Notification &other)
{
    key_ = other.key_;
    ledLightColor_ = other.ledLightColor_;
    lockscreenVisibleness_ = other.lockscreenVisibleness_;
    request_ = other.request_;
    postTime_ = other.postTime_;
    sound_ = other.sound_;
    vibrationStyle_ = other.vibrationStyle_;
}

bool Notification::EnableLight() const
{
    if (ledLightColor_ == -1) {
        return false;
    }
    return true;
}

bool Notification::EnableSound() const
{
    if (sound_ == nullptr) {
        return false;
    }
    return true;
}

bool Notification::EnableVibrate() const
{
    if (!vibrationStyle_.size()) {
        return false;
    }
    return true;
}

std::string Notification::GetBundleName() const
{
    if (request_ == nullptr) {
        return "";
    }
    return request_->GetOwnerBundleName();
}

std::string Notification::GetCreateBundle() const
{
    if (request_ == nullptr) {
        return "";
    }
    return request_->GetCreatorBundleName();
}

std::string Notification::GetLabel() const
{
    if (request_ == nullptr) {
        return "";
    }
    return request_->GetLabel();
}

int32_t Notification::GetLedLightColor() const
{
    return ledLightColor_;
}

int32_t Notification::GetLockscreenVisibleness() const
{
    return lockscreenVisibleness_;
}

std::string Notification::GetGroup() const
{
    if (request_ == nullptr) {
        return "";
    }
    return request_->GetGroupValue();
}

int32_t Notification::GetId() const
{
    if (request_ == nullptr) {
        return -1;
    }
    return request_->GetNotificationId();
}

std::string Notification::GetKey() const
{
    return key_;
}

NotificationRequest Notification::GetNotificationRequest() const
{
    return *request_;
}

int64_t Notification::GetPostTime() const
{
    return postTime_;
}

Uri Notification::GetSound() const
{
    return *sound_;
}

int32_t Notification::GetUid() const
{
    if (request_ == nullptr) {
        return -1;
    }
    return request_->GetCreatorUid();
}

std::vector<int64_t> Notification::GetVibrationStyle() const
{
    return vibrationStyle_;
}

bool Notification::IsGroup() const
{
    if (request_ == nullptr) {
        return false;
    }
    return !(request_->GetGroupValue() == "");
}

bool Notification::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(key_)) {
        ANS_LOGE("Can't wirte key");
        return false;
    }
    if (!parcel.WriteInt32(ledLightColor_)) {
        ANS_LOGE("Can't write ledLigthColor");
        return false;
    }

    if (!parcel.WriteInt32(lockscreenVisibleness_)) {
        ANS_LOGE("Can't write visbleness");
        return false;
    }

    if (!parcel.WriteStrongParcelable(request_)) {
        ANS_LOGE("Can't write request");
        return false;
    }

    if (!parcel.WriteInt64(postTime_)) {
        ANS_LOGE("Can't write postTime");
        return false;
    }

    if (!parcel.WriteString(sound_->ToString())) {
        ANS_LOGE("Can't write sound");
        return false;
    }

    if (!parcel.WriteInt64Vector(vibrationStyle_)) {
        ANS_LOGE("Can't write vibrationStyle");
        return false;
    }

    return true;
}

bool Notification::ReadFromParcel(Parcel &parcel)
{
    // Read key_
    key_ = parcel.ReadString();

    // Read ledLightColor_
    ledLightColor_ = parcel.ReadInt32();

    // Read lockscreenVisibleness_
    lockscreenVisibleness_ = parcel.ReadInt32();

    // Read request_
    request_ = parcel.ReadStrongParcelable<NotificationRequest>();

    // Read postTime_
    postTime_ = parcel.ReadInt64();

    // Read sound_
    sound_ = std::make_shared<Uri>(parcel.ReadString());

    // Read vibrationStyle_
    parcel.ReadInt64Vector(&vibrationStyle_);

    return true;
}

Notification *Notification::Unmarshalling(Parcel &parcel)
{
    Notification *n = new Notification();
    if (n && !n->ReadFromParcel(parcel)) {
        ANS_LOGE("Read from parcel error");
        delete n;
        n = nullptr;
    }
    return n;
}

void Notification::SetLedLightColor(const int32_t &color)
{
    ledLightColor_ = color;
}

void Notification::SetLockScreenVisbleness(const int32_t &visbleness)
{
    lockscreenVisibleness_ = visbleness;
}

void Notification::SetPostTime(const int64_t &time)
{
    postTime_ = time;
}

void Notification::SetSound(const Uri &sound)
{
    sound_ = std::make_shared<Uri>(sound.ToString());
}

void Notification::SetVibrationStyle(const std::vector<int64_t> &style)
{
    vibrationStyle_ = style;
}

std::string Notification::GenerateNotificationKey(int32_t uid, const std::string &label, int32_t id)
{
    const char *KEY_SPLITER = "_";

    std::stringstream stream;
    stream << uid << KEY_SPLITER << label << KEY_SPLITER << id;

    return stream.str();
}

std::string Notification::Dump() const
{
    std::string dump = "Notification{ key = " + key_ + ", ledLightColor = " + std::to_string(ledLightColor_) +
                       ", lockscreenVisbleness = " + std::to_string(lockscreenVisibleness_) + ",request = ";
    if (request_ == nullptr) {
        dump += "nullptr";
    } else {
        dump += request_->Dump();
    }
    dump = dump + ",postTime = " + std::to_string(postTime_) + ",sound = " + sound_->ToString() + "vibrationStyle = ";
    for (auto &style : vibrationStyle_) {
        dump += std::to_string(style);
        dump += ",";
    }
    return dump;
}

}  // namespace Notification
}  // namespace OHOS