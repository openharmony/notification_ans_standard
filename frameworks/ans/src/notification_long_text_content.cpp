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

#include "notification_long_text_content.h"

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
const std::size_t NotificationLongTextContent::MAX_LONGTEXT_LENGTH {1024};

NotificationLongTextContent::NotificationLongTextContent(const std::string &longText)
{
    SetLongText(longText);
}

void NotificationLongTextContent::SetExpandedTitle(const std::string &exTitle)
{
    expandedTitle_ = exTitle;
}

std::string NotificationLongTextContent::GetExpandedTitle() const
{
    return expandedTitle_;
}

void NotificationLongTextContent::SetBriefText(const std::string &briefText)
{
    briefText_ = briefText;
}

std::string NotificationLongTextContent::GetBriefText() const
{
    return briefText_;
}

void NotificationLongTextContent::SetLongText(const std::string &longText)
{
    if (longText.empty()) {
        longText_.clear();
        return;
    }

    auto length = std::min(NotificationLongTextContent::MAX_LONGTEXT_LENGTH, longText.length());
    longText_.assign(longText.begin(), longText.begin() + length);
}

std::string NotificationLongTextContent::GetLongText() const
{
    return longText_;
}

std::string NotificationLongTextContent::Dump()
{
    return "NotificationLongTextContent{ " + NotificationBasicContent::Dump() +
            ", longText = " + longText_ +
            ", briefText = " + briefText_ +
            ", expandedTitle = " + expandedTitle_ +
            " }";
}

bool NotificationLongTextContent::ToJson(nlohmann::json &jsonObject) const
{
    if (!NotificationBasicContent::ToJson(jsonObject)) {
        ANS_LOGE("Cannot convert basicContent to JSON");
        return false;
    }

    jsonObject["longText"]      = longText_;
    jsonObject["expandedTitle"] = expandedTitle_;
    jsonObject["briefText"]     = briefText_;

    return true;
}

NotificationLongTextContent *NotificationLongTextContent::FromJson(const nlohmann::json &jsonObject)
{
    if (jsonObject.is_null() or !jsonObject.is_object()) {
        ANS_LOGE("Invalid JSON object");
        return nullptr;
    }

    auto pContent = new (std::nothrow) NotificationLongTextContent();
    if (pContent == nullptr) {
        ANS_LOGE("Failed to create longTextContent instance");
        return nullptr;
    }

    pContent->ReadFromJson(jsonObject);

    const auto &jsonEnd = jsonObject.cend();
    if (jsonObject.find("longText") != jsonEnd) {
        pContent->longText_ = jsonObject.at("longText").get<std::string>();
    }

    if (jsonObject.find("expandedTitle") != jsonEnd) {
        pContent->expandedTitle_ = jsonObject.at("expandedTitle").get<std::string>();
    }

    if (jsonObject.find("briefText") != jsonEnd) {
        pContent->briefText_ = jsonObject.at("briefText").get<std::string>();
    }

    return pContent;
}

bool NotificationLongTextContent::Marshalling(Parcel &parcel) const
{
    if (!NotificationBasicContent::Marshalling(parcel)) {
        ANS_LOGE("Failed to write basic");
        return false;
    }

    if (!parcel.WriteString(expandedTitle_)) {
        ANS_LOGE("Failed to write expanded title");
        return false;
    }

    if (!parcel.WriteString(briefText_)) {
        ANS_LOGE("Failed to write brief text");
        return false;
    }

    if (!parcel.WriteString(longText_)) {
        ANS_LOGE("Failed to write longText");
        return false;
    }

    return true;
}

NotificationLongTextContent *NotificationLongTextContent::Unmarshalling(Parcel &parcel)
{
    auto pContent = new (std::nothrow) NotificationLongTextContent();
    if ((pContent != nullptr) && !pContent->ReadFromParcel(parcel)) {
        delete pContent;
        pContent = nullptr;
    }

    return pContent;
}

bool NotificationLongTextContent::ReadFromParcel(Parcel &parcel)
{
    if (!NotificationBasicContent::ReadFromParcel(parcel)) {
        ANS_LOGE("Failed to read basic");
        return false;
    }

    if (!parcel.ReadString(expandedTitle_)) {
        ANS_LOGE("Failed to read expanded title");
        return false;
    }

    if (!parcel.ReadString(briefText_)) {
        ANS_LOGE("Failed to read brief text");
        return false;
    }

    if (!parcel.ReadString(longText_)) {
        ANS_LOGE("Failed to read longtext");
        return false;
    }

    return true;
}
}  // namespace Notification
}  // namespace OHOS
