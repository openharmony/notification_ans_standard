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

#include "notification_request.h"

#include "ans_image_util.h"
#include "ans_log_wrapper.h"
#include "ohos/aafwk/content/want_params_wrapper.h"
#include "want_agent_helper.h"

namespace OHOS {
namespace Notification {
const std::string NotificationRequest::CLASSIFICATION_ALARM {"alarm"};
const std::string NotificationRequest::CLASSIFICATION_CALL {"call"};
const std::string NotificationRequest::CLASSIFICATION_EMAIL {"email"};
const std::string NotificationRequest::CLASSIFICATION_ERROR {"err"};
const std::string NotificationRequest::CLASSIFICATION_EVENT {"event"};
const std::string NotificationRequest::CLASSIFICATION_MESSAGE {"msg"};
const std::string NotificationRequest::CLASSIFICATION_NAVIGATION {"navigation"};
const std::string NotificationRequest::CLASSIFICATION_PROGRESS {"progress"};
const std::string NotificationRequest::CLASSIFICATION_PROMO {"promo"};
const std::string NotificationRequest::CLASSIFICATION_RECOMMENDATION {"recommendation"};
const std::string NotificationRequest::CLASSIFICATION_REMINDER {"reminder"};
const std::string NotificationRequest::CLASSIFICATION_SERVICE {"service"};
const std::string NotificationRequest::CLASSIFICATION_SOCIAL {"social"};
const std::string NotificationRequest::CLASSIFICATION_STATUS {"status"};
const std::string NotificationRequest::CLASSIFICATION_SYSTEM {"sys"};
const std::string NotificationRequest::CLASSIFICATION_TRANSPORT {"transport"};

const uint32_t NotificationRequest::COLOR_DEFAULT {0};

const uint32_t NotificationRequest::COLOR_MASK {0xFF000000};
const std::size_t NotificationRequest::MAX_USER_INPUT_HISTORY {5};
const std::size_t NotificationRequest::MAX_ACTION_BUTTONS {3};

NotificationRequest::NotificationRequest(int32_t notificationId) : notificationId_(notificationId)
{
    createTime_ = GetNowSysTime();
    deliveryTime_ = GetNowSysTime();
}

NotificationRequest::NotificationRequest(const NotificationRequest &other)
{
    CopyBase(other);
    CopyOther(other);
}

NotificationRequest &NotificationRequest::operator=(const NotificationRequest &other)
{
    CopyBase(other);
    CopyOther(other);

    return *this;
}

NotificationRequest::~NotificationRequest()
{}

bool NotificationRequest::IsInProgress() const
{
    return inProgress_;
}

void NotificationRequest::SetInProgress(bool isOngoing)
{
    inProgress_ = isOngoing;
}

bool NotificationRequest::IsUnremovable() const
{
    return unremovable_;
}

void NotificationRequest::SetUnremovable(bool isUnremovable)
{
    unremovable_ = isUnremovable;
}

void NotificationRequest::SetBadgeNumber(int32_t number)
{
    badgeNumber_ = number;
}

int32_t NotificationRequest::GetBadgeNumber() const
{
    return badgeNumber_;
}

void NotificationRequest::SetNotificationId(int32_t notificationId)
{
    notificationId_ = notificationId;
}

int32_t NotificationRequest::GetNotificationId() const
{
    return notificationId_;
}

void NotificationRequest::SetWantAgent(const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> &wantAgent)
{
    wantAgent_ = wantAgent;
}

const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> NotificationRequest::GetWantAgent() const
{
    return wantAgent_;
}

void NotificationRequest::SetRemovalWantAgent(const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> &wantAgent)
{
    removalWantAgent_ = wantAgent;
}

const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> NotificationRequest::GetRemovalWantAgent() const
{
    return removalWantAgent_;
}

void NotificationRequest::SetMaxScreenWantAgent(const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> &wantAgent)
{
    maxScreenWantAgent_ = wantAgent;
}

const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> NotificationRequest::GetMaxScreenWantAgent() const
{
    return maxScreenWantAgent_;
}

void NotificationRequest::SetAdditionalData(const std::shared_ptr<AAFwk::WantParams> &extras)
{
    additionalParams_ = extras;
}

const std::shared_ptr<AAFwk::WantParams> NotificationRequest::GetAdditionalData() const
{
    return additionalParams_;
}

void NotificationRequest::SetDeliveryTime(int64_t deliveryTime)
{
    deliveryTime_ = deliveryTime;
}

int64_t NotificationRequest::GetDeliveryTime() const
{
    return deliveryTime_;
}

bool NotificationRequest::IsShowDeliveryTime() const
{
    return (deliveryTime_ != 0) && showDeliveryTime_;
}

void NotificationRequest::SetShowDeliveryTime(bool showDeliveryTime)
{
    showDeliveryTime_ = showDeliveryTime;
}

void NotificationRequest::AddActionButton(const std::shared_ptr<NotificationActionButton> &actionButton)
{
    if (!actionButton) {
        ANS_LOGW("actionButton can not be null");
        return;
    }

    if (actionButtons_.size() >= NotificationRequest::MAX_ACTION_BUTTONS) {
        ANS_LOGW("three action buttons have been already added");
        return;
    }

    actionButtons_.emplace_back(actionButton);
}

const std::vector<std::shared_ptr<NotificationActionButton>> NotificationRequest::GetActionButtons() const
{
    return actionButtons_;
}

void NotificationRequest::ClearActionButtons()
{
    actionButtons_.clear();
}

bool NotificationRequest::IsPermitSystemGeneratedContextualActionButtons() const
{
    return permitted_;
}

void NotificationRequest::SetPermitSystemGeneratedContextualActionButtons(bool permitted)
{
    permitted_ = permitted;
}

void NotificationRequest::AddMessageUser(const std::shared_ptr<MessageUser> &messageUser)
{
    if (!messageUser) {
        ANS_LOGI("messageUser can not be null");
        return;
    }

    messageUsers_.emplace_back(messageUser);
}

const std::vector<std::shared_ptr<MessageUser>> NotificationRequest::GetMessageUsers() const
{
    return messageUsers_;
}

bool NotificationRequest::IsAlertOneTime() const
{
    return alertOneTime_;
}

void NotificationRequest::SetAlertOneTime(bool isAlertOnce)
{
    alertOneTime_ = isAlertOnce;
}

void NotificationRequest::SetAutoDeletedTime(int64_t deletedTime)
{
    autoDeletedTime_ = deletedTime;
}

int64_t NotificationRequest::GetAutoDeletedTime() const
{
    return autoDeletedTime_;
}

void NotificationRequest::SetLittleIcon(const std::shared_ptr<Media::PixelMap> &littleIcon)
{
    littleIcon_ = littleIcon;
}

const std::shared_ptr<Media::PixelMap> NotificationRequest::GetLittleIcon() const
{
    return littleIcon_;
}

void NotificationRequest::SetBigIcon(const std::shared_ptr<Media::PixelMap> &bigIcon)
{
    bigIcon_ = bigIcon;
}

const std::shared_ptr<Media::PixelMap> NotificationRequest::GetBigIcon() const
{
    return bigIcon_;
}

void NotificationRequest::SetClassification(const std::string &classification)
{
    classification_ = classification;
}

std::string NotificationRequest::GetClassification() const
{
    return classification_;
}

void NotificationRequest::SetColor(uint32_t color)
{
    color_ = color;
    if (NotificationRequest::COLOR_DEFAULT != color_) {
        color_ = color_ | NotificationRequest::COLOR_MASK;
    }
}

uint32_t NotificationRequest::GetColor() const
{
    return color_;
}

bool NotificationRequest::IsColorEnabled() const
{
    if (!colorEnabled_) {
        return false;
    }

    // no valid content
    if (!notificationContent_) {
        ANS_LOGI("no valid notification content");
        return false;
    }

    // not a media content
    if (NotificationContent::Type::MEDIA != notificationContentType_) {
        ANS_LOGI("not a media notification content");
        return false;
    }

    auto basicContent = notificationContent_->GetNotificationContent();
    auto mediaContent = std::static_pointer_cast<NotificationMediaContent>(basicContent);
    if (!mediaContent->GetAVToken()) {
        ANS_LOGI("AVToken has not been attached");
        return false;
    }

    return true;
}

void NotificationRequest::SetColorEnabled(bool colorEnabled)
{
    colorEnabled_ = colorEnabled;
}

void NotificationRequest::SetContent(const std::shared_ptr<NotificationContent> &content)
{
    notificationContent_ = content;

    if (notificationContent_) {
        notificationContentType_ = notificationContent_->GetContentType();
        return;
    }

    notificationContentType_ = NotificationContent::Type::NONE;
}

const std::shared_ptr<NotificationContent> NotificationRequest::GetContent() const
{
    return notificationContent_;
}

NotificationContent::Type NotificationRequest::GetNotificationType() const
{
    return notificationContentType_;
}

bool NotificationRequest::IsCountdownTimer() const
{
    return isCountdown_;
}

void NotificationRequest::SetCountdownTimer(bool isCountDown)
{
    isCountdown_ = isCountDown;
}

void NotificationRequest::SetGroupAlertType(NotificationRequest::GroupAlertType type)
{
    groupAlertType_ = type;
}

NotificationRequest::GroupAlertType NotificationRequest::GetGroupAlertType() const
{
    return groupAlertType_;
}

bool NotificationRequest::IsGroupOverview() const
{
    return groupOverview_;
}

void NotificationRequest::SetGroupOverview(bool overView)
{
    groupOverview_ = overView;
}

void NotificationRequest::SetGroupName(const std::string &groupName)
{
    groupName_ = groupName;
}

std::string NotificationRequest::GetGroupName() const
{
    return groupName_;
}

bool NotificationRequest::IsOnlyLocal() const
{
    return onlyLocal_;
}

void NotificationRequest::SetOnlyLocal(bool flag)
{
    onlyLocal_ = flag;
}

void NotificationRequest::SetSettingsText(const std::string &text)
{
    if ((NotificationContent::Type::LONG_TEXT == notificationContentType_) ||
        (NotificationContent::Type::PICTURE == notificationContentType_)) {
        ANS_LOGW("This method is invalid if the notification content type has been set to LONG_TEXT or PICTURE.");
        return;
    }

    settingsText_ = text;
}

std::string NotificationRequest::GetSettingsText() const
{
    return settingsText_;
}

int64_t NotificationRequest::GetCreateTime() const
{
    return createTime_;
}

bool NotificationRequest::IsShowStopwatch() const
{
    return showStopwatch_;
}

void NotificationRequest::SetShowStopwatch(bool isShow)
{
    showStopwatch_ = isShow;
}

void NotificationRequest::SetSlotType(NotificationConstant::SlotType slotType)
{
    slotType_ = slotType;
}

NotificationConstant::SlotType NotificationRequest::GetSlotType() const
{
    return slotType_;
}

void NotificationRequest::SetSortingKey(const std::string &key)
{
    sortingKey_ = key;
}

std::string NotificationRequest::GetSortingKey() const
{
    return sortingKey_;
}

void NotificationRequest::SetStatusBarText(const std::string &text)
{
    statusBarText_ = text;
}

std::string NotificationRequest::GetStatusBarText() const
{
    return statusBarText_;
}

bool NotificationRequest::IsTapDismissed() const
{
    return tapDismissed_;
}

void NotificationRequest::SetTapDismissed(bool isDismissed)
{
    tapDismissed_ = isDismissed;
}

void NotificationRequest::SetVisibleness(NotificationConstant::VisiblenessType type)
{
    visiblenessType_ = type;
}

NotificationConstant::VisiblenessType NotificationRequest::GetVisibleness() const
{
    return visiblenessType_;
}

void NotificationRequest::SetBadgeIconStyle(NotificationRequest::BadgeStyle style)
{
    badgeStyle_ = style;
}

NotificationRequest::BadgeStyle NotificationRequest::GetBadgeIconStyle() const
{
    return badgeStyle_;
}

void NotificationRequest::SetShortcutId(const std::string &shortcutId)
{
    shortcutId_ = shortcutId;
}

std::string NotificationRequest::GetShortcutId() const
{
    return shortcutId_;
}

void NotificationRequest::SetFloatingIcon(bool floatingIcon)
{
    floatingIcon_ = floatingIcon;
}

bool NotificationRequest::IsFloatingIcon() const
{
    return floatingIcon_;
}

void NotificationRequest::SetProgressBar(int32_t progress, int32_t progressMax, bool indeterminate)
{
    progressValue_ = progress;
    progressMax_ = progressMax;
    progressIndeterminate_ = indeterminate;
}

int32_t NotificationRequest::GetProgressMax() const
{
    return progressMax_;
}

int32_t NotificationRequest::GetProgressValue() const
{
    return progressValue_;
}

bool NotificationRequest::IsProgressIndeterminate() const
{
    return progressIndeterminate_;
}

void NotificationRequest::SetNotificationUserInputHistory(const std::vector<std::string> &text)
{
    if (text.empty()) {
        userInputHistory_.clear();
        return;
    }

    auto vsize = std::min(NotificationRequest::MAX_USER_INPUT_HISTORY, text.size());
    userInputHistory_.assign(text.begin(), text.begin() + vsize);
}

std::vector<std::string> NotificationRequest::GetNotificationUserInputHistory() const
{
    return userInputHistory_;
}

void NotificationRequest::SetPublicNotification(const std::shared_ptr<NotificationRequest> &other)
{
    publicNotification_ = other;
}

const std::shared_ptr<NotificationRequest> NotificationRequest::GetPublicNotification() const
{
    return publicNotification_;
}

std::string NotificationRequest::GetNotificationHashCode() const
{
    if (creatorBundleName_.empty() || (creatorUid_ == 0) || ownerBundleName_.empty()) {
        return "";
    }

    return std::to_string(notificationId_) + "_" + creatorBundleName_ + "_" + std::to_string(creatorUid_) + "_" +
           ownerBundleName_;
}

void NotificationRequest::SetOwnerBundleName(const std::string &ownerName)
{
    ownerBundleName_ = ownerName;
}

std::string NotificationRequest::GetOwnerBundleName() const
{
    return ownerBundleName_;
}

void NotificationRequest::SetCreatorBundleName(const std::string &creatorName)
{
    creatorBundleName_ = creatorName;
}

std::string NotificationRequest::GetCreatorBundleName() const
{
    return creatorBundleName_;
}

void NotificationRequest::SetCreatorPid(pid_t pid)
{
    creatorPid_ = pid;
}

pid_t NotificationRequest::GetCreatorPid() const
{
    return creatorPid_;
}

void NotificationRequest::SetCreatorUid(pid_t uid)
{
    creatorUid_ = uid;
}

pid_t NotificationRequest::GetCreatorUid() const
{
    return creatorUid_;
}

void NotificationRequest::SetLabel(const std::string &label)
{
    label_ = label;
}

std::string NotificationRequest::GetLabel() const
{
    return label_;
}

void NotificationRequest::SetDistributed(bool distribute)
{
    distributedOptions_.SetDistributed(distribute);
}

void NotificationRequest::SetDevicesSupportDisplay(const std::vector<std::string> &devices)
{
    distributedOptions_.SetDevicesSupportDisplay(devices);
}

void NotificationRequest::SetDevicesSupportOperate(const std::vector<std::string> &devices)
{
    distributedOptions_.SetDevicesSupportOperate(devices);
}

NotificationDistributedOptions NotificationRequest::GetNotificationDistributedOptions() const
{
    return distributedOptions_;
}

void NotificationRequest::SetCreatorUserId(int32_t userId)
{
    creatorUserId_ = userId;
}

int32_t NotificationRequest::GetCreatorUserId() const
{
    return creatorUserId_;
}

std::string NotificationRequest::Dump()
{
    return "NotificationRequest{ "
            "notificationId = " + std::to_string(notificationId_) +
            ", slotType = " + std::to_string(static_cast<int32_t>(slotType_)) +
            ", createTime = " + std::to_string(createTime_) + ", deliveryTime = " + std::to_string(deliveryTime_) +
            ", autoDeletedTime = " + std::to_string(autoDeletedTime_) + ", settingsText = " + settingsText_ +
            ", creatorBundleName = " + creatorBundleName_ +
            ", creatorPid = " + std::to_string(static_cast<int32_t>(creatorPid_)) +
            ", creatorUid = " + std::to_string(static_cast<int32_t>(creatorUid_)) +
            ", ownerBundleName = " + ownerBundleName_ + ", groupName = " + groupName_ +
            ", statusBarText = " + statusBarText_ + ", label = " + label_ + ", shortcutId = " + shortcutId_ +
            ", sortingKey = " + sortingKey_ +
            ", groupAlertType = " + std::to_string(static_cast<int32_t>(groupAlertType_)) +
            ", color = " + std::to_string(color_) + ", badgeNumber = " + std::to_string(badgeNumber_) +
            ", visiblenessType = " + std::to_string(static_cast<int32_t>(visiblenessType_)) +
            ", progressValue = " + std::to_string(progressValue_) + ", progressMax = " + std::to_string(progressMax_) +
            ", badgeStyle = " + std::to_string(static_cast<int32_t>(badgeStyle_)) +
            ", classification = " + classification_ +
            ", notificationContentType = " + std::to_string(static_cast<int32_t>(notificationContentType_)) +
            ", showDeliveryTime = " + (showDeliveryTime_ ? "true" : "false") +
            ", tapDismissed = " + (tapDismissed_ ? "true" : "false") +
            ", colorEnabled = " + (colorEnabled_ ? "true" : "false") +
            ", alertOneTime = " + (alertOneTime_ ? "true" : "false") +
            ", showStopwatch = " + (showStopwatch_ ? "true" : "false") +
            ", isCountdown = " + (isCountdown_ ? "true" : "false") +
            ", inProgress = " + (inProgress_ ? "true" : "false") +
            ", groupOverview = " + (groupOverview_ ? "true" : "false") +
            ", progressIndeterminate = " + (progressIndeterminate_ ? "true" : "false") +
            ", unremovable = " + (unremovable_ ? "true" : "false") +
            ", floatingIcon = " + (floatingIcon_ ? "true" : "false") +
            ", onlyLocal = " + (onlyLocal_ ? "true" : "false") + ", permitted = " + (permitted_ ? "true" : "false") +
            ", removalWantAgent = " + (removalWantAgent_ ? "not null" : "null") +
            ", maxScreenWantAgent = " + (maxScreenWantAgent_ ? "not null" : "null") +
            ", additionalParams = " + (additionalParams_ ? "not null" : "null") +
            ", littleIcon = " + (littleIcon_ ? "not null" : "null") +
            ", bigIcon = " + (bigIcon_ ? "not null" : "null") +
            ", notificationContent = " + (notificationContent_ ? notificationContent_->Dump() : "null") +
            ", publicNotification = " + (publicNotification_ ? "not null" : "null") +
            ", notificationTemplate = " + (notificationTemplate_ ? "not null" : "null") +
            ", actionButtons = " + (!actionButtons_.empty() ? actionButtons_.at(0)->Dump() : "empty") +
            ", messageUsers = " + (!messageUsers_.empty() ? messageUsers_.at(0)->Dump() : "empty") +
            ", userInputHistory = " + (!userInputHistory_.empty() ? userInputHistory_.at(0) : "empty") +
            ", distributedOptions = " + distributedOptions_.Dump() +
            ", notificationFlags = " + (notificationFlags_ ? "not null" : "null") +
            ", creatorUserId = " + std::to_string(creatorUserId_) +
            ", receiverUserId = " + std::to_string(receiverUserId_) +
            " }";
}

bool NotificationRequest::ToJson(nlohmann::json &jsonObject) const
{
    jsonObject["version"]         = 1;

    jsonObject["id"]              = notificationId_;
    jsonObject["color"]           = color_;
    jsonObject["deliveryTime"]    = deliveryTime_;
    jsonObject["autoDeletedTime"] = autoDeletedTime_;

    jsonObject["creatorBundleName"] = creatorBundleName_;
    jsonObject["ownerBundleName"]   = ownerBundleName_;
    jsonObject["groupName"]         = groupName_;
    jsonObject["label"]             = label_;
    jsonObject["classification"]    = classification_;

    jsonObject["slotType"]       = static_cast<int32_t>(slotType_);
    jsonObject["badgeIconStyle"] = static_cast<int32_t>(badgeStyle_);

    jsonObject["showDeliveryTime"] = showDeliveryTime_;
    jsonObject["tapDismissed"]     = tapDismissed_;
    jsonObject["colorEnabled"]     = colorEnabled_;
    jsonObject["isOngoing"]        = inProgress_;
    jsonObject["isAlertOnce"]      = alertOneTime_;
    jsonObject["isStopwatch"]      = showStopwatch_;
    jsonObject["isCountdown"]      = isCountdown_;
    jsonObject["isUnremovable"]    = unremovable_;
    jsonObject["isFloatingIcon"]   = floatingIcon_;

    jsonObject["creatorBundleName"] = creatorBundleName_;
    jsonObject["creatorUid"]        = creatorUid_;
    jsonObject["creatorPid"]        = creatorPid_;
    jsonObject["creatorUserId"]     = creatorUserId_;
    jsonObject["receiverUserId"]    = receiverUserId_;

    if (!ConvertObjectsToJson(jsonObject)) {
        ANS_LOGE("Cannot convert objects to JSON");
        return false;
    }

    return true;
}

NotificationRequest *NotificationRequest::FromJson(const nlohmann::json &jsonObject)
{
    if (jsonObject.is_null() or !jsonObject.is_object()) {
        ANS_LOGE("Invalid JSON object");
        return nullptr;
    }

    auto pRequest = new (std::nothrow) NotificationRequest();
    if (pRequest == nullptr) {
        ANS_LOGE("Failed to create request instance");
        return nullptr;
    }

    const auto &jsonEnd = jsonObject.cend();
    if (jsonObject.find("version") != jsonEnd) {
        jsonObject.at("version").get<int32_t>();
    }

    ConvertJsonToNum(pRequest, jsonObject);

    ConvertJsonToString(pRequest, jsonObject);

    ConvertJsonToEnum(pRequest, jsonObject);

    ConvertJsonToBool(pRequest, jsonObject);

    if (jsonObject.find("wantAgent") != jsonEnd) {
        auto wantAgentValue  = jsonObject.at("wantAgent").get<std::string>();
        pRequest->wantAgent_ = AbilityRuntime::WantAgent::WantAgentHelper::FromString(wantAgentValue);
    }

    if (!ConvertJsonToNotificationContent(pRequest, jsonObject)) {
        delete pRequest;
        pRequest = nullptr;
        return nullptr;
    }

    if (!ConvertJsonToNotificationActionButton(pRequest, jsonObject)) {
        delete pRequest;
        pRequest = nullptr;
        return nullptr;
    }

    if (jsonObject.find("extraInfo") != jsonEnd) {
        auto extraInfoStr = jsonObject.at("extraInfo").get<std::string>();
        if (!extraInfoStr.empty()) {
            AAFwk::WantParams params    = AAFwk::WantParamWrapper::ParseWantParams(extraInfoStr);
            pRequest->additionalParams_ = std::make_shared<AAFwk::WantParams>(params);
        }
    }

    ConvertJsonToPixelMap(pRequest, jsonObject);

    if (!ConvertJsonToNotificationDistributedOptions(pRequest, jsonObject)) {
        delete pRequest;
        pRequest = nullptr;
        return nullptr;
    }

    if (!ConvertJsonToNotificationFlags(pRequest, jsonObject)) {
        delete pRequest;
        pRequest = nullptr;
        return nullptr;
    }

    return pRequest;
}

bool NotificationRequest::Marshalling(Parcel &parcel) const
{
    // write int
    if (!parcel.WriteInt32(notificationId_)) {
        ANS_LOGE("Failed to write notification Id");
        return false;
    }

    if (!parcel.WriteUint32(color_)) {
        ANS_LOGE("Failed to write color");
        return false;
    }

    if (!parcel.WriteInt32(badgeNumber_)) {
        ANS_LOGE("Failed to write badge number");
        return false;
    }

    if (!parcel.WriteInt32(progressValue_)) {
        ANS_LOGE("Failed to write progress value");
        return false;
    }

    if (!parcel.WriteInt32(progressMax_)) {
        ANS_LOGE("Failed to write progress max");
        return false;
    }

    if (!parcel.WriteInt64(createTime_)) {
        ANS_LOGE("Failed to write create time");
        return false;
    }

    if (!parcel.WriteInt64(deliveryTime_)) {
        ANS_LOGE("Failed to write delivery time");
        return false;
    }

    if (!parcel.WriteInt64(autoDeletedTime_)) {
        ANS_LOGE("Failed to write auto deleted time");
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(creatorPid_))) {
        ANS_LOGE("Failed to write creator pid");
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(creatorUid_))) {
        ANS_LOGE("Failed to write creator uid");
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(creatorUserId_))) {
        ANS_LOGE("Failed to write creator userId");
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(receiverUserId_))) {
        ANS_LOGE("Failed to write receiver userId");
        return false;
    }

    // write std::string
    if (!parcel.WriteString(settingsText_)) {
        ANS_LOGE("Failed to write settings text");
        return false;
    }

    if (!parcel.WriteString(creatorBundleName_)) {
        ANS_LOGE("Failed to write creator bundle name");
        return false;
    }

    if (!parcel.WriteString(ownerBundleName_)) {
        ANS_LOGE("Failed to write owner bundle name");
        return false;
    }

    if (!parcel.WriteString(groupName_)) {
        ANS_LOGE("Failed to write group name");
        return false;
    }

    if (!parcel.WriteString(statusBarText_)) {
        ANS_LOGE("Failed to write status bar text");
        return false;
    }

    if (!parcel.WriteString(label_)) {
        ANS_LOGE("Failed to write label");
        return false;
    }

    if (!parcel.WriteString(shortcutId_)) {
        ANS_LOGE("Failed to write shortcut Id");
        return false;
    }

    if (!parcel.WriteString(sortingKey_)) {
        ANS_LOGE("Failed to write sorting key");
        return false;
    }

    if (!parcel.WriteString(classification_)) {
        ANS_LOGE("Failed to write classification");
        return false;
    }

    // write enum
    if (!parcel.WriteInt32(static_cast<int32_t>(slotType_))) {
        ANS_LOGE("Failed to write slot type");
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(groupAlertType_))) {
        ANS_LOGE("Failed to write group alert type");
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(visiblenessType_))) {
        ANS_LOGE("Failed to write visibleness type");
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(badgeStyle_))) {
        ANS_LOGE("Failed to write badge type");
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(notificationContentType_))) {
        ANS_LOGE("Failed to write notification content type");
        return false;
    }

    // write bool
    if (!parcel.WriteBool(showDeliveryTime_)) {
        ANS_LOGE("Failed to write flag indicating whether to show delivery time");
        return false;
    }

    if (!parcel.WriteBool(tapDismissed_)) {
        ANS_LOGE("Failed to write flag tap dismissed");
        return false;
    }

    if (!parcel.WriteBool(colorEnabled_)) {
        ANS_LOGE("Failed to write flag indicating whether to enable background color");
        return false;
    }

    if (!parcel.WriteBool(alertOneTime_)) {
        ANS_LOGE("Failed to write flag indicating whether to have this notification alert only once");
        return false;
    }

    if (!parcel.WriteBool(showStopwatch_)) {
        ANS_LOGE("Failed to write flag show stop watch");
        return false;
    }

    if (!parcel.WriteBool(isCountdown_)) {
        ANS_LOGE("Failed to write flag indicating whether to show the notification creation time as a countdown timer");
        return false;
    }

    if (!parcel.WriteBool(inProgress_)) {
        ANS_LOGE("Failed to write flag indicating whether in progress");
        return false;
    }

    if (!parcel.WriteBool(groupOverview_)) {
        ANS_LOGE("Failed to write flag indicating whether to use this notification as the overview of its group");
        return false;
    }

    if (!parcel.WriteBool(progressIndeterminate_)) {
        ANS_LOGE("Failed to write progress indeterminate");
        return false;
    }

    if (!parcel.WriteBool(unremovable_)) {
        ANS_LOGE("Failed to write flag indicating whether unremovable");
        return false;
    }

    if (!parcel.WriteBool(floatingIcon_)) {
        ANS_LOGE("Failed to write flag floating icon");
        return false;
    }

    if (!parcel.WriteBool(onlyLocal_)) {
        ANS_LOGE("Failed to write flag only local");
        return false;
    }

    if (!parcel.WriteBool(permitted_)) {
        ANS_LOGE("Failed to write flag indicating whether to allow the platform to \
            generate contextual NotificationActionButton objects");
        return false;
    }

    // write objects which managed by std::shared_ptr
    bool valid {false};

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

    valid = removalWantAgent_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether removalWantAgent is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(removalWantAgent_.get())) {
            ANS_LOGE("Failed to write removalWantAgent");
            return false;
        }
    }

    valid = maxScreenWantAgent_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether maxScreenWantAgent is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(maxScreenWantAgent_.get())) {
            ANS_LOGE("Failed to write maxScreenWantAgent");
            return false;
        }
    }

    valid = additionalParams_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether additionalParams is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(additionalParams_.get())) {
            ANS_LOGE("Failed to write additionalParams");
            return false;
        }
    }

    valid = littleIcon_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether littleIcon is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(littleIcon_.get())) {
            ANS_LOGE("Failed to write littleIcon");
            return false;
        }
    }

    valid = bigIcon_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether bigIcon is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(bigIcon_.get())) {
            ANS_LOGE("Failed to write bigIcon");
            return false;
        }
    }

    valid = notificationContent_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether notificationContent is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(notificationContent_.get())) {
            ANS_LOGE("Failed to write notificationContent");
            return false;
        }
    }

    valid = publicNotification_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether publicNotification is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(publicNotification_.get())) {
            ANS_LOGE("Failed to write publicNotification");
            return false;
        }
    }

    // write std::vector
    if (!parcel.WriteInt32(static_cast<int32_t>(actionButtons_.size()))) {
        ANS_LOGE("Failed to write the size of actionButtons");
        return false;
    }

    for (auto it = actionButtons_.begin(); it != actionButtons_.end(); ++it) {
        if (!parcel.WriteParcelable(it->get())) {
            ANS_LOGE("Failed to write actionButton");
            return false;
        }
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(messageUsers_.size()))) {
        ANS_LOGE("Failed to write the size of messageUsers");
        return false;
    }

    for (auto it = messageUsers_.begin(); it != messageUsers_.end(); ++it) {
        if (!parcel.WriteParcelable(it->get())) {
            ANS_LOGE("Failed to write messageUser");
            return false;
        }
    }

    if (!parcel.WriteStringVector(userInputHistory_)) {
        ANS_LOGE("Failed to write userInputHistory");
        return false;
    }

    if (!parcel.WriteParcelable(&distributedOptions_)) {
        ANS_LOGE("Failed to write distributedOptions");
        return false;
    }

    valid = notificationTemplate_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write the flag which indicate whether publicNotification is null");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(notificationTemplate_.get())) {
            ANS_LOGE("Failed to write notificationTemplate");
            return false;
        }
    }

    valid = notificationFlags_ ? true : false;
    if (!parcel.WriteBool(valid)) {
        ANS_LOGE("Failed to write flags for the notification");
        return false;
    }

    if (valid) {
        if (!parcel.WriteParcelable(notificationFlags_.get())) {
            ANS_LOGE("Failed to write notification flags");
            return false;
        }
    }

    return true;
}

NotificationRequest *NotificationRequest::Unmarshalling(Parcel &parcel)
{
    auto objptr = new (std::nothrow) NotificationRequest();
    if ((objptr != nullptr) && !objptr->ReadFromParcel(parcel)) {
        delete objptr;
        objptr = nullptr;
    }

    return objptr;
}

bool NotificationRequest::ReadFromParcel(Parcel &parcel)
{
    notificationId_ = parcel.ReadInt32();
    color_ = parcel.ReadUint32();
    badgeNumber_ = parcel.ReadInt32();
    progressValue_ = parcel.ReadInt32();
    progressMax_ = parcel.ReadInt32();
    createTime_ = parcel.ReadInt64();
    deliveryTime_ = parcel.ReadInt64();
    autoDeletedTime_ = parcel.ReadInt64();

    creatorPid_ = static_cast<pid_t>(parcel.ReadInt32());
    creatorUid_ = static_cast<pid_t>(parcel.ReadInt32());
    creatorUserId_ = parcel.ReadInt32();
    receiverUserId_ = parcel.ReadInt32();

    if (!parcel.ReadString(settingsText_)) {
        ANS_LOGE("Failed to read settings text");
        return false;
    }

    if (!parcel.ReadString(creatorBundleName_)) {
        ANS_LOGE("Failed to read creator bundle name");
        return false;
    }

    if (!parcel.ReadString(ownerBundleName_)) {
        ANS_LOGE("Failed to read owner bundle name");
        return false;
    }

    if (!parcel.ReadString(groupName_)) {
        ANS_LOGE("Failed to read group name");
        return false;
    }

    if (!parcel.ReadString(statusBarText_)) {
        ANS_LOGE("Failed to read status bar text");
        return false;
    }

    if (!parcel.ReadString(label_)) {
        ANS_LOGE("Failed to read label");
        return false;
    }

    if (!parcel.ReadString(shortcutId_)) {
        ANS_LOGE("Failed to read shortcut Id");
        return false;
    }

    if (!parcel.ReadString(sortingKey_)) {
        ANS_LOGE("Failed to read sorting key");
        return false;
    }

    if (!parcel.ReadString(classification_)) {
        ANS_LOGE("Failed to read classification");
        return false;
    }

    slotType_ = static_cast<NotificationConstant::SlotType>(parcel.ReadInt32());
    groupAlertType_ = static_cast<NotificationRequest::GroupAlertType>(parcel.ReadInt32());
    visiblenessType_ = static_cast<NotificationConstant::VisiblenessType>(parcel.ReadInt32());
    badgeStyle_ = static_cast<NotificationRequest::BadgeStyle>(parcel.ReadInt32());
    notificationContentType_ = static_cast<NotificationContent::Type>(parcel.ReadInt32());

    showDeliveryTime_ = parcel.ReadBool();
    tapDismissed_ = parcel.ReadBool();
    colorEnabled_ = parcel.ReadBool();
    alertOneTime_ = parcel.ReadBool();
    showStopwatch_ = parcel.ReadBool();
    isCountdown_ = parcel.ReadBool();
    inProgress_ = parcel.ReadBool();
    groupOverview_ = parcel.ReadBool();
    progressIndeterminate_ = parcel.ReadBool();
    unremovable_ = parcel.ReadBool();
    floatingIcon_ = parcel.ReadBool();
    onlyLocal_ = parcel.ReadBool();
    permitted_ = parcel.ReadBool();

    bool valid {false};

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
        removalWantAgent_ = std::shared_ptr<AbilityRuntime::WantAgent::WantAgent>(
            parcel.ReadParcelable<AbilityRuntime::WantAgent::WantAgent>());
        if (!removalWantAgent_) {
            ANS_LOGE("Failed to read removalWantAgent");
            return false;
        }
    }

    valid = parcel.ReadBool();
    if (valid) {
        maxScreenWantAgent_ = std::shared_ptr<AbilityRuntime::WantAgent::WantAgent>(
            parcel.ReadParcelable<AbilityRuntime::WantAgent::WantAgent>());
        if (!maxScreenWantAgent_) {
            ANS_LOGE("Failed to read maxScreenWantAgent");
            return false;
        }
    }

    valid = parcel.ReadBool();
    if (valid) {
        additionalParams_ = std::shared_ptr<AAFwk::WantParams>(parcel.ReadParcelable<AAFwk::WantParams>());
        if (!additionalParams_) {
            ANS_LOGE("Failed to read additionalParams");
            return false;
        }
    }

    valid = parcel.ReadBool();
    if (valid) {
        littleIcon_ = std::shared_ptr<Media::PixelMap>(parcel.ReadParcelable<Media::PixelMap>());
        if (!littleIcon_) {
            ANS_LOGE("Failed to read littleIcon");
            return false;
        }
    }

    valid = parcel.ReadBool();
    if (valid) {
        bigIcon_ = std::shared_ptr<Media::PixelMap>(parcel.ReadParcelable<Media::PixelMap>());
        if (!bigIcon_) {
            ANS_LOGE("Failed to read bigIcon");
            return false;
        }
    }

    valid = parcel.ReadBool();
    if (valid) {
        notificationContent_ = std::shared_ptr<NotificationContent>(parcel.ReadParcelable<NotificationContent>());
        if (!notificationContent_) {
            ANS_LOGE("Failed to read notificationContent");
            return false;
        }
    }

    valid = parcel.ReadBool();
    if (valid) {
        publicNotification_ = std::shared_ptr<NotificationRequest>(parcel.ReadParcelable<NotificationRequest>());
        if (!publicNotification_) {
            ANS_LOGE("Failed to read publicNotification");
            return false;
        }
    }

    auto vsize = parcel.ReadInt32();
    for (auto it = 0; it < vsize; ++it) {
        auto member = parcel.ReadParcelable<NotificationActionButton>();
        if (member == nullptr) {
            ANS_LOGE("Failed to read actionButton");
            return false;
        }

        actionButtons_.emplace_back(member);
    }

    vsize = parcel.ReadInt32();
    for (uint64_t it = 0; it < vsize; ++it) {
        auto member = parcel.ReadParcelable<MessageUser>();
        if (member == nullptr) {
            ANS_LOGE("Failed to read messageUser");
            return false;
        }

        messageUsers_.emplace_back(member);
    }

    if (!parcel.ReadStringVector(&userInputHistory_)) {
        ANS_LOGE("Failed to read userInputHistory");
        return false;
    }

    auto pOpt = parcel.ReadParcelable<NotificationDistributedOptions>();
    if (pOpt == nullptr) {
        ANS_LOGE("Failed to read distributedOptions");
        return false;
    }
    distributedOptions_ = *pOpt;

    valid = parcel.ReadBool();
    if (valid) {
        notificationTemplate_ = std::shared_ptr<NotificationTemplate>(parcel.ReadParcelable<NotificationTemplate>());
        if (!notificationTemplate_) {
            ANS_LOGE("Failed to read notificationTemplate");
            return false;
        }
    }

    valid = parcel.ReadBool();
    if (valid) {
        notificationFlags_ = std::shared_ptr<NotificationFlags>(parcel.ReadParcelable<NotificationFlags>());
        if (!notificationFlags_) {
            ANS_LOGE("Failed to read notificationFlags");
            return false;
        }
    }

    return true;
}

int64_t NotificationRequest::GetNowSysTime()
{
    std::chrono::time_point<std::chrono::system_clock> nowSys = std::chrono::system_clock::now();
    auto epoch = nowSys.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    int64_t duration = value.count();
    return duration;
}

void NotificationRequest::SetTemplate(const std::shared_ptr<NotificationTemplate> &templ)
{
    notificationTemplate_ = templ;
}

std::shared_ptr<NotificationTemplate> NotificationRequest::GetTemplate() const
{
    return notificationTemplate_;
}

void NotificationRequest::SetFlags(const std::shared_ptr<NotificationFlags> &flags)
{
    notificationFlags_ = flags;
}

std::shared_ptr<NotificationFlags> NotificationRequest::GetFlags() const
{
    return notificationFlags_;
}

void NotificationRequest::SetReceiverUserId(int32_t userId)
{
    receiverUserId_ = userId;
}

int32_t NotificationRequest::GetReceiverUserId() const
{
    return receiverUserId_;
}

void NotificationRequest::CopyBase(const NotificationRequest &other)
{
    this->notificationId_ = other.notificationId_;
    this->color_ = other.color_;
    this->badgeNumber_ = other.badgeNumber_;
    this->progressValue_ = other.progressValue_;
    this->progressMax_ = other.progressMax_;
    this->createTime_ = other.createTime_;
    this->deliveryTime_ = other.deliveryTime_;
    this->autoDeletedTime_ = other.autoDeletedTime_;

    this->creatorPid_ = other.creatorPid_;
    this->creatorUid_ = other.creatorUid_;
    this->creatorUserId_ = other.creatorUserId_;
    this->receiverUserId_ = other.receiverUserId_;

    this->slotType_ = other.slotType_;
    this->settingsText_ = other.settingsText_;
    this->creatorBundleName_ = other.creatorBundleName_;
    this->ownerBundleName_ = other.ownerBundleName_;
    this->groupName_ = other.groupName_;
    this->statusBarText_ = other.statusBarText_;
    this->label_ = other.label_;
    this->shortcutId_ = other.shortcutId_;
    this->sortingKey_ = other.sortingKey_;
    this->classification_ = other.classification_;

    this->groupAlertType_ = other.groupAlertType_;
    this->visiblenessType_ = other.visiblenessType_;
    this->badgeStyle_ = other.badgeStyle_;
    this->notificationContentType_ = other.notificationContentType_;
}

void NotificationRequest::CopyOther(const NotificationRequest &other)
{
    this->showDeliveryTime_ = other.showDeliveryTime_;
    this->tapDismissed_ = other.tapDismissed_;
    this->colorEnabled_ = other.colorEnabled_;
    this->alertOneTime_ = other.alertOneTime_;
    this->showStopwatch_ = other.showStopwatch_;
    this->isCountdown_ = other.isCountdown_;
    this->inProgress_ = other.inProgress_;
    this->groupOverview_ = other.groupOverview_;
    this->progressIndeterminate_ = other.progressIndeterminate_;
    this->unremovable_ = other.unremovable_;
    this->floatingIcon_ = other.floatingIcon_;
    this->onlyLocal_ = other.onlyLocal_;
    this->permitted_ = other.permitted_;

    this->wantAgent_ = other.wantAgent_;
    this->removalWantAgent_ = other.removalWantAgent_;
    this->maxScreenWantAgent_ = other.maxScreenWantAgent_;
    this->additionalParams_ = other.additionalParams_;
    this->littleIcon_ = other.littleIcon_;
    this->bigIcon_ = other.bigIcon_;
    this->notificationContent_ = other.notificationContent_;
    this->publicNotification_ = other.publicNotification_;

    this->actionButtons_ = other.actionButtons_;
    this->messageUsers_ = other.messageUsers_;
    this->userInputHistory_ = other.userInputHistory_;

    this->distributedOptions_ = other.distributedOptions_;

    this->notificationTemplate_ = other.notificationTemplate_;
    this->notificationFlags_ = other.notificationFlags_;
}

bool NotificationRequest::ConvertObjectsToJson(nlohmann::json &jsonObject) const
{
    jsonObject["wantAgent"] = wantAgent_ ? AbilityRuntime::WantAgent::WantAgentHelper::ToString(wantAgent_) : "";

    nlohmann::json contentObj;
    if (notificationContent_) {
        if (!NotificationJsonConverter::ConvertToJosn(notificationContent_.get(), contentObj)) {
            ANS_LOGE("Cannot convert notificationContent to JSON");
            return false;
        }
    }
    jsonObject["content"] = contentObj;

    nlohmann::json buttonsArr = nlohmann::json::array();
    for (auto &btn : actionButtons_) {
        if (!btn) {
            continue;
        }

        nlohmann::json btnObj;
        if (!NotificationJsonConverter::ConvertToJosn(btn.get(), btnObj)) {
            ANS_LOGE("Cannot convert actionButton to JSON");
            return false;
        }

        buttonsArr.emplace_back(btnObj);
    }
    jsonObject["actionButtons"] = buttonsArr;

    std::string extraInfoStr;
    if (additionalParams_) {
        AAFwk::WantParamWrapper wWrapper(*additionalParams_);
        extraInfoStr = wWrapper.ToString();
    }
    jsonObject["extraInfo"] = extraInfoStr;

    jsonObject["smallIcon"] = AnsImageUtil::PackImage(littleIcon_);
    jsonObject["largeIcon"] = AnsImageUtil::PackImage(bigIcon_);

    nlohmann::json optObj;
    if (!NotificationJsonConverter::ConvertToJosn(&distributedOptions_, optObj)) {
        ANS_LOGE("Cannot convert distributedOptions to JSON");
        return false;
    }
    jsonObject["distributedOptions"] = optObj;

    if (notificationFlags_) {
        nlohmann::json flagsObj;
        if (!NotificationJsonConverter::ConvertToJosn(notificationFlags_.get(), flagsObj)) {
            ANS_LOGE("Cannot convert notificationFlags to JSON");
            return false;
        }
        jsonObject["notificationFlags"] = flagsObj;
    }

    return true;
}

void NotificationRequest::ConvertJsonToNum(NotificationRequest *target, const nlohmann::json &jsonObject)
{
    if (target == nullptr) {
        ANS_LOGE("Invalid input parameter");
        return;
    }

    const auto &jsonEnd = jsonObject.cend();

    if (jsonObject.find("id") != jsonEnd) {
        target->notificationId_ = jsonObject.at("id").get<int32_t>();
    }

    if (jsonObject.find("color") != jsonEnd) {
        target->color_ = jsonObject.at("color").get<uint32_t>();
    }

    if (jsonObject.find("deliveryTime") != jsonEnd) {
        target->deliveryTime_ = jsonObject.at("deliveryTime").get<int64_t>();
    }

    if (jsonObject.find("autoDeletedTime") != jsonEnd) {
        target->autoDeletedTime_ = jsonObject.at("autoDeletedTime").get<int64_t>();
    }

    if (jsonObject.find("creatorUid") != jsonEnd) {
        target->creatorUid_ = jsonObject.at("creatorUid").get<int32_t>();
    }

    if (jsonObject.find("creatorPid") != jsonEnd) {
        target->creatorPid_ = jsonObject.at("creatorPid").get<int32_t>();
    }

    if (jsonObject.find("creatorUserId") != jsonEnd) {
        target->creatorUserId_ = jsonObject.at("creatorUserId").get<int32_t>();
    }

    if (jsonObject.find("receiverUserId") != jsonEnd) {
        target->receiverUserId_ = jsonObject.at("receiverUserId").get<int32_t>();
    }
}

void NotificationRequest::ConvertJsonToString(NotificationRequest *target, const nlohmann::json &jsonObject)
{
    if (target == nullptr) {
        ANS_LOGE("Invalid input parameter");
        return;
    }

    const auto &jsonEnd = jsonObject.cend();

    if (jsonObject.find("creatorBundleName") != jsonEnd) {
        target->creatorBundleName_ = jsonObject.at("creatorBundleName").get<std::string>();
    }

    if (jsonObject.find("ownerBundleName") != jsonEnd) {
        target->ownerBundleName_ = jsonObject.at("ownerBundleName").get<std::string>();
    }

    if (jsonObject.find("groupName") != jsonEnd) {
        target->groupName_ = jsonObject.at("groupName").get<std::string>();
    }

    if (jsonObject.find("label") != jsonEnd) {
        target->label_ = jsonObject.at("label").get<std::string>();
    }

    if (jsonObject.find("classification") != jsonEnd) {
        target->classification_ = jsonObject.at("classification").get<std::string>();
    }

    if (jsonObject.find("creatorBundleName") != jsonEnd) {
        target->creatorBundleName_ = jsonObject.at("creatorBundleName").get<std::string>();
    }
}

void NotificationRequest::ConvertJsonToEnum(NotificationRequest *target, const nlohmann::json &jsonObject)
{
    if (target == nullptr) {
        ANS_LOGE("Invalid input parameter");
        return;
    }

    const auto &jsonEnd = jsonObject.cend();

    if (jsonObject.find("slotType") != jsonEnd) {
        auto slotTypeValue  = jsonObject.at("slotType").get<int32_t>();
        target->slotType_ = static_cast<NotificationConstant::SlotType>(slotTypeValue);
    }

    if (jsonObject.find("badgeIconStyle") != jsonEnd) {
        auto badgeStyleValue  = jsonObject.at("badgeIconStyle").get<int32_t>();
        target->badgeStyle_ = static_cast<NotificationRequest::BadgeStyle>(badgeStyleValue);
    }
}

void NotificationRequest::ConvertJsonToBool(NotificationRequest *target, const nlohmann::json &jsonObject)
{
    if (target == nullptr) {
        ANS_LOGE("Invalid input parameter");
        return;
    }

    const auto &jsonEnd = jsonObject.cend();

    if (jsonObject.find("showDeliveryTime") != jsonEnd) {
        target->showDeliveryTime_ = jsonObject.at("showDeliveryTime").get<bool>();
    }

    if (jsonObject.find("tapDismissed") != jsonEnd) {
        target->tapDismissed_ = jsonObject.at("tapDismissed").get<bool>();
    }

    if (jsonObject.find("colorEnabled") != jsonEnd) {
        target->colorEnabled_ = jsonObject.at("colorEnabled").get<bool>();
    }

    if (jsonObject.find("isOngoing") != jsonEnd) {
        target->inProgress_ = jsonObject.at("isOngoing").get<bool>();
    }

    if (jsonObject.find("isAlertOnce") != jsonEnd) {
        target->alertOneTime_ = jsonObject.at("isAlertOnce").get<bool>();
    }

    if (jsonObject.find("isStopwatch") != jsonEnd) {
        target->showStopwatch_ = jsonObject.at("isStopwatch").get<bool>();
    }

    if (jsonObject.find("isCountdown") != jsonEnd) {
        target->isCountdown_ = jsonObject.at("isCountdown").get<bool>();
    }

    if (jsonObject.find("isUnremovable") != jsonEnd) {
        target->unremovable_ = jsonObject.at("isUnremovable").get<bool>();
    }

    if (jsonObject.find("isFloatingIcon") != jsonEnd) {
        target->floatingIcon_ = jsonObject.at("isFloatingIcon").get<bool>();
    }
}

void NotificationRequest::ConvertJsonToPixelMap(NotificationRequest *target, const nlohmann::json &jsonObject)
{
    if (target == nullptr) {
        ANS_LOGE("Invalid input parameter");
        return;
    }

    const auto &jsonEnd = jsonObject.cend();

    if (jsonObject.find("smallIcon") != jsonEnd) {
        auto littleIconStr    = jsonObject.at("smallIcon").get<std::string>();
        target->littleIcon_ = AnsImageUtil::UnPackImage(littleIconStr);
    }

    if (jsonObject.find("largeIcon") != jsonEnd) {
        auto bigIconStr    = jsonObject.at("largeIcon").get<std::string>();
        target->bigIcon_ = AnsImageUtil::UnPackImage(bigIconStr);
    }
}

bool NotificationRequest::ConvertJsonToNotificationContent(
    NotificationRequest *target, const nlohmann::json &jsonObject)
{
    if (target == nullptr) {
        ANS_LOGE("Invalid input parameter");
        return false;
    }

    const auto &jsonEnd = jsonObject.cend();

    if (jsonObject.find("content") != jsonEnd) {
        auto contentObj = jsonObject.at("content");
        if (!contentObj.is_null()) {
            auto pContent = NotificationJsonConverter::ConvertFromJosn<NotificationContent>(contentObj);
            if (pContent == nullptr) {
                ANS_LOGE("Failed to parse notification content!");
                return false;
            }

            target->notificationContent_ = std::shared_ptr<NotificationContent>(pContent);
        }
    }

    return true;
}

bool NotificationRequest::ConvertJsonToNotificationActionButton(
    NotificationRequest *target, const nlohmann::json &jsonObject)
{
    if (target == nullptr) {
        ANS_LOGE("Invalid input parameter");
        return false;
    }

    const auto &jsonEnd = jsonObject.cend();

    if (jsonObject.find("actionButtons") != jsonEnd) {
        auto buttonArr = jsonObject.at("actionButtons");
        for (auto &btnObj : buttonArr) {
            auto pBtn = NotificationJsonConverter::ConvertFromJosn<NotificationActionButton>(btnObj);
            if (pBtn == nullptr) {
                ANS_LOGE("Failed to parse actionButton!");
                return false;
            }

            target->actionButtons_.emplace_back(pBtn);
        }
    }

    return true;
}

bool NotificationRequest::ConvertJsonToNotificationDistributedOptions(
    NotificationRequest *target, const nlohmann::json &jsonObject)
{
    if (target == nullptr) {
        ANS_LOGE("Invalid input parameter");
        return false;
    }

    const auto &jsonEnd = jsonObject.cend();

    if (jsonObject.find("distributedOptions") != jsonEnd) {
        auto optObj = jsonObject.at("distributedOptions");
        if (!optObj.is_null()) {
            auto pOpt = NotificationJsonConverter::ConvertFromJosn<NotificationDistributedOptions>(optObj);
            if (pOpt == nullptr) {
                ANS_LOGE("Failed to parse distributedOptions!");
                return false;
            }

            target->distributedOptions_ = *pOpt;
        }
    }

    return true;
}

bool NotificationRequest::ConvertJsonToNotificationFlags(
    NotificationRequest *target, const nlohmann::json &jsonObject)
{
    if (target == nullptr) {
        ANS_LOGE("Invalid input parameter");
        return false;
    }

    const auto &jsonEnd = jsonObject.cend();

    if (jsonObject.find("notificationFlags") != jsonEnd) {
        auto flagsObj = jsonObject.at("notificationFlags");
        if (!flagsObj.is_null()) {
            auto pFlags = NotificationJsonConverter::ConvertFromJosn<NotificationFlags>(flagsObj);
            if (pFlags == nullptr) {
                ANS_LOGE("Failed to parse notificationFlags!");
                return false;
            }

            target->notificationFlags_ = std::shared_ptr<NotificationFlags>(pFlags);
        }
    }

    return true;
}
}  // namespace Notification
}  // namespace OHOS
