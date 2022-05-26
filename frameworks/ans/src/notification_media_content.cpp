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

#include "notification_media_content.h"

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
void NotificationMediaContent::SetAVToken(const std::shared_ptr<AVToken> &avToken)
{
    avToken_ = avToken;
}

const std::shared_ptr<AVToken> NotificationMediaContent::GetAVToken() const
{
    return avToken_;
}

void NotificationMediaContent::SetShownActions(const std::vector<uint32_t> &actions)
{
    sequenceNumbers_ = actions;
}

std::vector<uint32_t> NotificationMediaContent::GetShownActions() const
{
    return sequenceNumbers_;
}

std::string NotificationMediaContent::Dump()
{
    std::string numbers {};
    std::for_each(sequenceNumbers_.begin(), sequenceNumbers_.end(), [&numbers](int32_t num) {
        numbers += std::to_string(num) + ", ";
    });

    return "NotificationMediaContent{ " + NotificationBasicContent::Dump() +
            ", avToken = " + (avToken_ ? "not null" : "null") +
            ", sequenceNumbers = " + numbers +
            " }";
}

bool NotificationMediaContent::ToJson(nlohmann::json &jsonObject) const
{
    if (!NotificationBasicContent::ToJson(jsonObject)) {
        ANS_LOGE("Cannot convert basicContent to JSON");
        return false;
    }

    jsonObject["sequenceNumbers"] = nlohmann::json(sequenceNumbers_);

    return true;
}

NotificationMediaContent *NotificationMediaContent::FromJson(const nlohmann::json &jsonObject)
{
    if (jsonObject.is_null() or !jsonObject.is_object()) {
        ANS_LOGE("Invalid JSON object");
        return nullptr;
    }

    auto pContent = new (std::nothrow) NotificationMediaContent();
    if (pContent == nullptr) {
        ANS_LOGE("Failed to create mediaContent instance");
        return nullptr;
    }

    pContent->ReadFromJson(jsonObject);

    const auto& jsonEnd = jsonObject.cend();
    if (jsonObject.find("sequenceNumbers") != jsonEnd) {
        pContent->sequenceNumbers_ = jsonObject.at("sequenceNumbers").get<std::vector<uint32_t>>();
    }

    return pContent;
}

bool NotificationMediaContent::Marshalling(Parcel &parcel) const
{
    if (!NotificationBasicContent::Marshalling(parcel)) {
        ANS_LOGE("Failed to write basic");
        return false;
    }

    if (!parcel.WriteUInt32Vector(sequenceNumbers_)) {
        ANS_LOGE("Failed to write sequence numbers");
        return false;
    }

    return true;
}

NotificationMediaContent *NotificationMediaContent::Unmarshalling(Parcel &parcel)
{
    auto pContent = new (std::nothrow) NotificationMediaContent();
    if ((pContent != nullptr) && !pContent->ReadFromParcel(parcel)) {
        delete pContent;
        pContent = nullptr;
    }

    return pContent;
}

bool NotificationMediaContent::ReadFromParcel(Parcel &parcel)
{
    if (!NotificationBasicContent::ReadFromParcel(parcel)) {
        ANS_LOGE("Failed to read basic");
        return false;
    }

    if (!parcel.ReadUInt32Vector(&sequenceNumbers_)) {
        ANS_LOGE("Failed to read sequence numbers");
        return false;
    }

    return true;
}
}  // namespace Notification
}  // namespace OHOS
