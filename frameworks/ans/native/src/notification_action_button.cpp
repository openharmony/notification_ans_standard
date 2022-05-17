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

#include "notification_action_button.h"

#include "ans_image_util.h"
#include "ans_log_wrapper.h"
#include "want_agent_helper.h"
#include "want_params_wrapper.h"

namespace OHOS {
namespace Notification {
std::shared_ptr<NotificationActionButton> NotificationActionButton::Create(const std::shared_ptr<Media::PixelMap> &icon,
    const std::string &title, const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> &wantAgent,
    const std::shared_ptr<AAFwk::WantParams> &extras, NotificationConstant::SemanticActionButton semanticActionButton,
    bool autoCreatedReplies, const std::vector<std::shared_ptr<NotificationUserInput>> &mimeTypeOnlyInputs,
    const std::shared_ptr<NotificationUserInput> &userInput, bool isContextual)
{
    if (isContextual && (!icon || !wantAgent)) {
        ANS_LOGE("icon or wantAgent can not be null when isContextual is true");
        return {};
    }

    auto realExtras = extras;
    if (!realExtras) {
        realExtras = std::make_shared<AAFwk::WantParams>();
        if (!realExtras) {
            ANS_LOGE("create WantParams object failed");
            return {};
        }
    }

    std::shared_ptr<NotificationUserInput> textInput = userInput;
    std::vector<std::shared_ptr<NotificationUserInput>> onlyInputs = mimeTypeOnlyInputs;
    if (userInput && (userInput->IsMimeTypeOnly())) {
        onlyInputs.push_back(userInput);
    }

    auto pActionButton = new (std::nothrow) NotificationActionButton(icon,
        title,
        wantAgent,
        realExtras,
        semanticActionButton,
        autoCreatedReplies,
        onlyInputs,
        textInput,
        isContextual);
    if (pActionButton == nullptr) {
        ANS_LOGE("create NotificationActionButton object failed");
        return {};
    }

    return std::shared_ptr<NotificationActionButton>(pActionButton);
}

std::shared_ptr<NotificationActionButton> NotificationActionButton::Create(
    const std::shared_ptr<NotificationActionButton> &actionButton)
{
    if (!actionButton) {
        ANS_LOGW("invalid input NotificationActionButton object");
        return {};
    }

    return NotificationActionButton::Create(actionButton->GetIcon(),
        actionButton->GetTitle(),
        actionButton->GetWantAgent(),
        actionButton->GetAdditionalData(),
        actionButton->GetSemanticActionButton(),
        actionButton->IsAutoCreatedReplies(),
        actionButton->GetMimeTypeOnlyUserInputs(),
        actionButton->GetUserInput(),
        actionButton->IsContextDependent());
}

NotificationActionButton::NotificationActionButton(const std::shared_ptr<Media::PixelMap> &icon,
    const std::string &title, const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> &wantAgent,
    const std::shared_ptr<AAFwk::WantParams> &extras, NotificationConstant::SemanticActionButton semanticActionButton,
    bool autoCreatedReplies, const std::vector<std::shared_ptr<NotificationUserInput>> &mimeTypeOnlyInputs,
    const std::shared_ptr<NotificationUserInput> &userInput, bool isContextual)
    : icon_(icon),
      title_(title),
      wantAgent_(wantAgent),
      extras_(extras),
      semanticActionButton_(semanticActionButton),
      autoCreatedReplies_(autoCreatedReplies),
      mimeTypeOnlyUserInputs_(mimeTypeOnlyInputs),
      userInput_(userInput),
      isContextual_(isContextual)
{}

const std::shared_ptr<Media::PixelMap> NotificationActionButton::GetIcon() const
{
    return icon_;
}

std::string NotificationActionButton::GetTitle() const
{
    return title_;
}

const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> NotificationActionButton::GetWantAgent() const
{
    return wantAgent_;
}

void NotificationActionButton::AddAdditionalData(AAFwk::WantParams &extras)
{
    if (extras_) {
        *extras_ = extras;
    }
}

const std::shared_ptr<AAFwk::WantParams> NotificationActionButton::GetAdditionalData() const
{
    return extras_;
}

void NotificationActionButton::SetSemanticActionButton(NotificationConstant::SemanticActionButton semanticActionButton)
{
    semanticActionButton_ = semanticActionButton;
}

NotificationConstant::SemanticActionButton NotificationActionButton::GetSemanticActionButton() const
{
    return semanticActionButton_;
}

void NotificationActionButton::AddMimeTypeOnlyUserInput(const std::shared_ptr<NotificationUserInput> &userInput)
{
    if (!userInput) {
        ANS_LOGE("The userInput is invalid.");
        return;
    }

    if (!userInput->IsMimeTypeOnly()) {
        ANS_LOGE("The userInput is not a particular MIME types.");
        return;
    }

    mimeTypeOnlyUserInputs_.emplace_back(userInput);
}

std::vector<std::shared_ptr<NotificationUserInput>> NotificationActionButton::GetMimeTypeOnlyUserInputs() const
{
    return mimeTypeOnlyUserInputs_;
}

void NotificationActionButton::AddNotificationUserInput(const std::shared_ptr<NotificationUserInput> &userInput)
{
    userInput_ = userInput;
}

const std::shared_ptr<NotificationUserInput> NotificationActionButton::GetUserInput() const
{
    return userInput_;
}

void NotificationActionButton::SetAutoCreatedReplies(bool autoCreatedReplies)
{
    autoCreatedReplies_ = autoCreatedReplies;
}

bool NotificationActionButton::IsAutoCreatedReplies() const
{
    return autoCreatedReplies_;
}

void NotificationActionButton::SetContextDependent(bool isContextual)
{
    isContextual_ = isContextual;
}

bool NotificationActionButton::IsContextDependent() const
{
    return isContextual_;
}

std::string NotificationActionButton::Dump()
{
    std::string mimeTypeOnlyUserInputs = "";
    for (auto &item : mimeTypeOnlyUserInputs_) {
        if (!item) {
            mimeTypeOnlyUserInputs += "nullptr, ";
            continue;
        }
        mimeTypeOnlyUserInputs += item->Dump();
        mimeTypeOnlyUserInputs += ", ";
    }

    std::string userInput = "";
    if (userInput_ == nullptr) {
        userInput += "nullptr, ";
    } else {
        userInput += userInput_->Dump();
        userInput += ", ";
    }

    return "NotificationActionButton{ "
            "title = " + title_ +
            ", semanticActionButton = " + std::to_string(static_cast<int32_t>(semanticActionButton_)) +
            ", autoCreatedReplies = " + (autoCreatedReplies_ ? "true" : "false") +
            ", isContextual = " + (isContextual_ ? "true" : "false") +
            ", mimeTypeOnlyUserInputs = [" + mimeTypeOnlyUserInputs + "]" +
            ", userInputs = [" + userInput + "]" +
            " }";
}

bool NotificationActionButton::ToJson(nlohmann::json &jsonObject) const
{
    jsonObject["icon"]      = AnsImageUtil::PackImage(icon_);
    jsonObject["title"]     = title_;
    jsonObject["wantAgent"] = wantAgent_ ? AbilityRuntime::WantAgent::WantAgentHelper::ToString(wantAgent_) : "";

    std::string extrasStr;
    if (extras_) {
        AAFwk::WantParamWrapper wWrapper(*extras_);
        extrasStr = wWrapper.ToString();
    }
    jsonObject["extras"] = extrasStr;

    return true;
}

NotificationActionButton *NotificationActionButton::FromJson(const nlohmann::json &jsonObject)
{
    if (jsonObject.is_null() or !jsonObject.is_object()) {
        ANS_LOGE("Invalid JSON object");
        return nullptr;
    }

    auto pButton = new (std::nothrow) NotificationActionButton();
    if (pButton == nullptr) {
        ANS_LOGE("Failed to create actionButton instance");
        return nullptr;
    }

    const auto &jsonEnd = jsonObject.cend();
    if (jsonObject.find("icon") != jsonEnd) {
        auto iconStr   = jsonObject.at("icon").get<std::string>();
        pButton->icon_ = AnsImageUtil::UnPackImage(iconStr);
    }

    if (jsonObject.find("title") != jsonEnd) {
        pButton->title_ = jsonObject.at("title").get<std::string>();
    }

    if (jsonObject.find("wantAgent") != jsonEnd) {
        auto wantAgentValue = jsonObject.at("wantAgent").get<std::string>();
        pButton->wantAgent_ = AbilityRuntime::WantAgent::WantAgentHelper::FromString(wantAgentValue);
    }

    if (jsonObject.find("extras") != jsonEnd) {
        auto extrasString = jsonObject.at("extras").get<std::string>();
        if (!extrasString.empty()) {
            AAFwk::WantParams params = AAFwk::WantParamWrapper::ParseWantParams(extrasString);
            pButton->extras_ = std::make_shared<AAFwk::WantParams>(params);
        }
    }

    return pButton;
}

bool NotificationActionButton::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(title_)) {
        ANS_LOGE("Failed to write title");
        return false;
    }

    if (!parcel.WriteBool(autoCreatedReplies_)) {
        ANS_LOGE("Failed to write flag autoCreatedReplies");
        return false;
    }

    if (!parcel.WriteBool(isContextual_)) {
        ANS_LOGE("Failed to write flag isContextual");
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(semanticActionButton_))) {
        ANS_LOGE("Failed to write semanticActionButton");
        return false;
    }

    bool valid {false};

    valid = icon_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether icon is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(icon_.get())) {
            ANS_LOGE("Failed to write icon");
            return false;
        }
    }

    valid = wantAgent_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether wantAgent is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(wantAgent_.get())) {
            ANS_LOGE("Failed to write wantAgent");
            return false;
        }
    }

    valid = extras_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether extras is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(extras_.get())) {
            ANS_LOGE("Failed to write extras");
            return false;
        }
    }

    valid = userInput_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether userInput is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(userInput_.get())) {
            ANS_LOGE("Failed to write userInput");
            return false;
        }
    }

    return true;
}

NotificationActionButton *NotificationActionButton::Unmarshalling(Parcel &parcel)
{
    auto pButton = new (std::nothrow) NotificationActionButton();
    if ((pButton != nullptr) && !pButton->ReadFromParcel(parcel)) {
        delete pButton;
        pButton = nullptr;
    }

    return pButton;
}

bool NotificationActionButton::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(title_)) {
        ANS_LOGE("Failed to read title");
        return false;
    }

    autoCreatedReplies_ = parcel.ReadBool();

    isContextual_ = parcel.ReadBool();

    semanticActionButton_ = static_cast<NotificationConstant::SemanticActionButton>(parcel.ReadInt32());

    bool valid {false};

    valid = parcel.ReadBool();
    if (valid) {
        icon_ = std::shared_ptr<Media::PixelMap>(parcel.ReadParcelable<Media::PixelMap>());
        if (!icon_) {
            ANS_LOGE("Failed to read icon");
            return false;
        }
    }

    valid = parcel.ReadBool();
    if (valid) {
        wantAgent_ = std::shared_ptr<AbilityRuntime::WantAgent::WantAgent>(
            parcel.ReadParcelable<AbilityRuntime::WantAgent::WantAgent>());
        if (!wantAgent_) {
            ANS_LOGE("Failed to read wantAgent");
            return false;
        }
    }

    valid = parcel.ReadBool();
    if (valid) {
        extras_ = std::shared_ptr<AAFwk::WantParams>(parcel.ReadParcelable<AAFwk::WantParams>());
        if (!extras_) {
            ANS_LOGE("Failed to read extras");
            return false;
        }
    }

    valid = parcel.ReadBool();
    if (valid) {
        userInput_ = std::shared_ptr<NotificationUserInput>(parcel.ReadParcelable<NotificationUserInput>());
        if (!userInput_) {
            ANS_LOGE("Failed to read userInput");
            return false;
        }
    }

    return true;
}
}  // namespace Notification
}  // namespace OHOS
