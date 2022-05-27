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

#include "notification_multiline_content.h"

#include <algorithm>

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
const std::vector<std::string>::size_type NotificationMultiLineContent::MAX_LINES {7};

void NotificationMultiLineContent::SetExpandedTitle(const std::string &exTitle)
{
    expandedTitle_ = exTitle;
}

std::string NotificationMultiLineContent::GetExpandedTitle() const
{
    return expandedTitle_;
}

void NotificationMultiLineContent::SetBriefText(const std::string &briefText)
{
    briefText_ = briefText;
}

std::string NotificationMultiLineContent::GetBriefText() const
{
    return briefText_;
}

void NotificationMultiLineContent::AddSingleLine(const std::string &oneLine)
{
    if (allLines_.size() >= NotificationMultiLineContent::MAX_LINES) {
        ANS_LOGW("already added seven lines");
        return;
    }

    allLines_.emplace_back(oneLine);
}

std::vector<std::string> NotificationMultiLineContent::GetAllLines() const
{
    return allLines_;
}

std::string NotificationMultiLineContent::Dump()
{
    std::string lines {};
    std::for_each(
        allLines_.begin(), allLines_.end(), [&lines](const std::string &line) { lines += " " + line + ","; });
    lines.pop_back();

    return "NotificationMultiLineContent{ " + NotificationBasicContent::Dump() +
            ", briefText = " + briefText_ +
            ", expandedTitle = " + expandedTitle_ +
            ", allLines = [" + lines + "]" +
            " }";
}

bool NotificationMultiLineContent::ToJson(nlohmann::json &jsonObject) const
{
    if (!NotificationBasicContent::ToJson(jsonObject)) {
        ANS_LOGE("Cannot convert basicContent to JSON");
        return false;
    }

    jsonObject["expandedTitle"] = expandedTitle_;
    jsonObject["briefText"]     = briefText_;
    jsonObject["allLines"]      = nlohmann::json(allLines_);

    return true;
}

NotificationMultiLineContent *NotificationMultiLineContent::FromJson(const nlohmann::json &jsonObject)
{
    if (jsonObject.is_null() or !jsonObject.is_object()) {
        ANS_LOGE("Invalid JSON object");
        return nullptr;
    }

    auto pContent = new (std::nothrow) NotificationMultiLineContent();
    if (pContent == nullptr) {
        ANS_LOGE("Failed to create multiLineContent instance");
        return nullptr;
    }

    pContent->ReadFromJson(jsonObject);

    const auto &jsonEnd = jsonObject.cend();
    if (jsonObject.find("expandedTitle") != jsonEnd) {
        pContent->expandedTitle_ = jsonObject.at("expandedTitle").get<std::string>();
    }

    if (jsonObject.find("briefText") != jsonEnd) {
        pContent->briefText_ = jsonObject.at("briefText").get<std::string>();
    }

    if (jsonObject.find("allLines") != jsonEnd) {
        pContent->allLines_ = jsonObject.at("allLines").get<std::vector<std::string>>();
    }

    return pContent;
}

bool NotificationMultiLineContent::Marshalling(Parcel &parcel) const
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

    if (!parcel.WriteStringVector(allLines_)) {
        ANS_LOGE("Failed to write all lines");
        return false;
    }

    return true;
}

NotificationMultiLineContent *NotificationMultiLineContent::Unmarshalling(Parcel &parcel)
{
    auto pContent = new (std::nothrow) NotificationMultiLineContent();
    if ((pContent != nullptr) && !pContent->ReadFromParcel(parcel)) {
        delete pContent;
        pContent = nullptr;
    }

    return pContent;
}

bool NotificationMultiLineContent::ReadFromParcel(Parcel &parcel)
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

    if (!parcel.ReadStringVector(&allLines_)) {
        ANS_LOGE("Failed to read all lines");
        return false;
    }

    return true;
}
}  // namespace Notification
}  // namespace OHOS
