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

#include "ans_log_wrapper.h"
#include "want_agent_helper.h"

#include "reminder_request.h"

namespace OHOS {
namespace Notification {
int32_t ReminderRequest::GLOBAL_ID = 0;
const uint64_t ReminderRequest::INVALID_LONG_VALUE = 0;
const uint16_t ReminderRequest::MILLI_SECONDS = 1000;
const uint16_t ReminderRequest::SAME_TIME_DISTINGUISH_MILLISECONDS = 1000;
const uint8_t ReminderRequest::REMINDER_STATUS_INACTIVE = 0;
const uint8_t ReminderRequest::REMINDER_STATUS_SHOWING = 4;
const std::string ReminderRequest::NOTIFICATION_LABEL = "REMINDER_AGENT";
const std::string ReminderRequest::REMINDER_EVENT_ALARM_ALERT = "ohos.event.notification.reminder.ALARM_ALERT";
const std::string ReminderRequest::REMINDER_EVENT_CLOSE_ALERT = "ohos.event.notification.reminder.CLOSE_ALERT";
const std::string ReminderRequest::REMINDER_EVENT_SNOOZE_ALERT = "ohos.event.notification.reminder.SNOOZE_ALERT";
const std::string ReminderRequest::REMINDER_EVENT_ALERT_TIMEOUT = "ohos.event.notification.reminder.ALERT_TIMEOUT";
const std::string ReminderRequest::REMINDER_EVENT_REMOVE_NOTIFICATION =
    "ohos.event.notification.reminder.REMOVE_NOTIFICATION";
const std::string ReminderRequest::PARAM_REMINDER_ID = "REMINDER_ID";

ReminderRequest::ReminderRequest()
{
    wantAgentInfo_ = wantAgentInfo_ == nullptr ? std::make_shared<WantAgentInfo>() : wantAgentInfo_;
}

ReminderRequest::ReminderRequest(const ReminderRequest &other)
{
    this->actionButtonMap_ = other.actionButtonMap_;
    this->content_ = other.content_;
    this->expiredContent_ = other.expiredContent_;
    this->notificationId_ = other.notificationId_;
    this->reminderId_ = other.reminderId_;
    this->reminderType_ = other.reminderType_;
    this->slotType_ = other.slotType_;
    this->title_ = other.title_;
    this->triggerTimeInMilli_ = other.triggerTimeInMilli_;
    this->wantAgentInfo_ = other.wantAgentInfo_;
}

ReminderRequest::ReminderRequest(ReminderType reminderType)
{
    reminderType_ = reminderType;
    wantAgentInfo_ = wantAgentInfo_ == nullptr ? std::make_shared<WantAgentInfo>() : wantAgentInfo_;
}

bool ReminderRequest::CanRemove()
{
    if ((state_ & REMINDER_STATUS_SHOWING) == 0) {
        return true;
    }
    return false;
}

std::string ReminderRequest::Dump() const
{
    struct tm *timeInfo;
    const time_t nextTriggerTime = static_cast<time_t>(triggerTimeInMilli_ / 1000);
    timeInfo = localtime(&nextTriggerTime);
    uint8_t dateTimeLen = 80;
    char dateTimeBuffer[dateTimeLen];
    (void)strftime(dateTimeBuffer, dateTimeLen, "%Y-%m-%d %H:%M:%S", timeInfo);
    return "Reminder["
           "id=" + std::to_string(reminderId_) +
           ", type=" + std::to_string(static_cast<uint8_t>(reminderType_)) +
           ", state=" + GetState(state_) +
           ", nextTriggerTime=" + dateTimeBuffer +
           "]";
}

ReminderRequest& ReminderRequest::SetActionButton(const std::string &title, const ActionButtonType &type)
{
    if (type != ActionButtonType::CLOSE) {
        ANSR_LOGI("Button type only support: %{public}d", static_cast<uint8_t>(ActionButtonType::CLOSE));
        return *this;
    }
    ActionButtonInfo actionButtonInfo;
    actionButtonInfo.type = type;
    actionButtonInfo.title = title;
    actionButtonMap_.insert(std::pair<ActionButtonType, ActionButtonInfo>(type, actionButtonInfo));
    return *this;
}

ReminderRequest& ReminderRequest::SetContent(const std::string &content)
{
    content_ = content;
    return *this;
}

ReminderRequest& ReminderRequest::SetExpiredContent(const std::string &expiredContent)
{
    expiredContent_ = expiredContent;
    return *this;
}

void ReminderRequest::SetExpired(bool isExpired)
{
    isExpired_ = isExpired;
}

void ReminderRequest::InitReminderId()
{
    std::lock_guard<std::mutex> lock(std::mutex);
    if (GLOBAL_ID < 0) {
        ANSR_LOGW("GLOBAL_ID overdule");
        GLOBAL_ID = 0;
    }
    reminderId_ = ++GLOBAL_ID;
    ANSR_LOGI("reminderId_=%{public}d", reminderId_);
}

bool ReminderRequest::IsExpired() const
{
    return isExpired_;
}

bool ReminderRequest::IsShowing() const
{
    if ((state_ & REMINDER_STATUS_SHOWING) != 0) {
        return true;
    }
    return false;
}

void ReminderRequest::OnClose(bool updateNext)
{
    if ((state_ & REMINDER_STATUS_SHOWING) == 0) {
        ANSR_LOGE("onClose, the state of reminder is incorrect, state:%{public}s", GetState(state_).c_str());
        return;
    }
    SetState(false, REMINDER_STATUS_SHOWING, "onClose()");
    if (updateNext) {
        uint64_t nextTriggerTime = PreGetNextTriggerTimeIgnoreSnooze(false);
        if (nextTriggerTime == INVALID_LONG_VALUE) {
            isExpired_ = true;
        } else {
            SetTriggerTimeInMilli(nextTriggerTime);
        }
    }
}

bool ReminderRequest::OnDateTimeChange()
{
    uint64_t nextTriggerTime = PreGetNextTriggerTimeIgnoreSnooze(false);
    return HandleSysTimeChange(triggerTimeInMilli_, nextTriggerTime);
}

bool ReminderRequest::HandleSysTimeChange(uint64_t oriTriggerTime, uint64_t optTriggerTime)
{
    if (isExpired_) {
        return false;
    }
    bool showImmediately = false;
    if (optTriggerTime != INVALID_LONG_VALUE && optTriggerTime <= oriTriggerTime) {
        // case1. switch to a previous time
        SetTriggerTimeInMilli(optTriggerTime);
    } else {
        time_t now;
        time(&now);  // unit is seconds.
        if (static_cast<int64_t>(now) < 0) {
            ANSR_LOGE("Get now time error");
            return false;
        }
        if (oriTriggerTime <= (static_cast<uint64_t>(now) * MILLI_SECONDS)) {
            // case2. switch to a future time, trigger time is less than now time.
            // when the reminder show immediately, trigger time will update in onShow function.
            showImmediately = true;
        } else {
            // case3. switch to a future time, trigger time is larger than now time.
            showImmediately = false;
        }
    }
    return showImmediately;
}

bool ReminderRequest::HandleTimeZoneChange(
    uint64_t oldZoneTriggerTime, uint64_t newZoneTriggerTime, uint64_t optTriggerTime)
{
    if (isExpired_) {
        return false;
    }
    ANSR_LOGD("Handle timezone change, oldZoneTriggerTime:%{public}llu, newZoneTriggerTime:%{public}llu",
        (unsigned long long)oldZoneTriggerTime, (unsigned long long)newZoneTriggerTime);
    bool showImmediately = false;
    if (optTriggerTime != INVALID_LONG_VALUE && oldZoneTriggerTime < newZoneTriggerTime) {
        // case1. timezone change to smaller
        SetTriggerTimeInMilli(optTriggerTime);
    } else {
        time_t now;
        time(&now);  // unit is seconds.
        if (static_cast<int64_t>(now) < 0) {
            ANSR_LOGE("Get now time error");
            return false;
        }
        if (newZoneTriggerTime <= (static_cast<uint64_t>(now))) {
            // case2. timezone change to larger
            showImmediately = true;
        } else {
            SetTriggerTimeInMilli(newZoneTriggerTime);
            showImmediately = false;
        }
    }
    return showImmediately;
}

void ReminderRequest::OnSameNotificationIdCovered()
{
    SetState(false, REMINDER_STATUS_SHOWING, "OnSameNotificationIdCovered");
}

void ReminderRequest::OnShow(bool isSysTimeChanged, bool allowToNotify)
{
    if (allowToNotify) {
        SetState(true, REMINDER_STATUS_SHOWING, "OnShow");
    }
    UpdateNextReminder();
}

bool ReminderRequest::OnTimeZoneChange()
{
    time_t oldZoneTriggerTime = static_cast<time_t>(triggerTimeInMilli_ / MILLI_SECONDS);
    struct tm *oriTime;
    oriTime = gmtime(&oldZoneTriggerTime);
    time_t newZoneTriggerTime = mktime(oriTime);
    uint64_t nextTriggerTime = PreGetNextTriggerTimeIgnoreSnooze(false);
    return HandleTimeZoneChange(oldZoneTriggerTime, newZoneTriggerTime, nextTriggerTime);
}

ReminderRequest& ReminderRequest::SetNotificationId(int32_t notificationId)
{
    notificationId_ = notificationId;
    return *this;
}

ReminderRequest& ReminderRequest::SetSlotType(const NotificationConstant::SlotType &slotType)
{
    slotType_ = slotType;
    return *this;
}

ReminderRequest& ReminderRequest::SetTitle(const std::string &title)
{
    title_ = title;
    return *this;
}

void ReminderRequest::SetTriggerTimeInMilli(uint64_t triggerTimeInMilli)
{
    triggerTimeInMilli_ = triggerTimeInMilli;
}

ReminderRequest& ReminderRequest::SetWantAgentInfo(const std::shared_ptr<WantAgentInfo> &wantAgentInfo)
{
    wantAgentInfo_ = wantAgentInfo;
    return *this;
}

std::map<ReminderRequest::ActionButtonType, ReminderRequest::ActionButtonInfo> ReminderRequest::GetActionButtons()
    const
{
    return actionButtonMap_;
}

std::string ReminderRequest::GetContent() const
{
    return content_;
}

std::string ReminderRequest::GetExpiredContent() const
{
    return expiredContent_;
}

int32_t ReminderRequest::GetNotificationId() const
{
    return notificationId_;
}

sptr<NotificationRequest> ReminderRequest::GetNotificationRequest() const
{
    return notificationRequest_;
}

int32_t ReminderRequest::GetReminderId() const
{
    return reminderId_;
}

void ReminderRequest::SetReminderId(int32_t reminderId)
{
    reminderId_ = reminderId;
}

NotificationConstant::SlotType ReminderRequest::GetSlotType() const
{
    return slotType_;
}

uint8_t ReminderRequest::GetState() const
{
    return state_;
}

std::string ReminderRequest::GetTitle() const
{
    return title_;
}

uint64_t ReminderRequest::GetTriggerTimeInMilli() const
{
    return triggerTimeInMilli_;
}

std::shared_ptr<ReminderRequest::WantAgentInfo> ReminderRequest::GetWantAgentInfo() const
{
    return wantAgentInfo_;
}

ReminderRequest::ReminderType ReminderRequest::GetReminderType() const
{
    return reminderType_;
}

bool ReminderRequest::UpdateNextReminder()
{
    return false;
}

void ReminderRequest::UpdateNotificationRequest(UpdateNotificationType type, std::string extra)
{
    ANSR_LOGI("UpdateNotification type=%{public}d", static_cast<uint8_t>(type));
    switch (type) {
        case UpdateNotificationType::ACTION_BUTTON: {
            AddActionButtons();
            break;
        }
        case UpdateNotificationType::REMOVAL_WANT_AGENT: {
            AddRemovalWantAgent();
            break;
        }
        case UpdateNotificationType::CONTENT: {
            break;
        }
        default:
            break;
    }
}

bool ReminderRequest::Marshalling(Parcel &parcel) const
{
    // write string
    if (!parcel.WriteString(content_)) {
        ANSR_LOGE("Failed to write content");
        return false;
    }
    if (!parcel.WriteString(expiredContent_)) {
        ANSR_LOGE("Failed to write expiredContent");
        return false;
    }
    if (!parcel.WriteString(title_)) {
        ANSR_LOGE("Failed to write title");
        return false;
    }
    if (!parcel.WriteString(wantAgentInfo_->abilityName)) {
        ANSR_LOGE("Failed to write wantAgentInfo`s abilityName");
        return false;
    }
    if (!parcel.WriteString(wantAgentInfo_->pkgName)) {
        ANSR_LOGE("Failed to write wantAgentInfo`s pkgName");
        return false;
    }

    // write int
    if (!parcel.WriteInt32(reminderId_)) {
        ANSR_LOGE("Failed to write reminderId");
        return false;
    }
    if (!parcel.WriteInt32(notificationId_)) {
        ANSR_LOGE("Failed to write notificationId");
        return false;
    }
    if (!parcel.WriteUint64(triggerTimeInMilli_)) {
        ANSR_LOGE("Failed to write triggerTimeInMilli");
        return false;
    }

    // write enum
    if (!parcel.WriteUint8(static_cast<uint8_t>(reminderType_))) {
        ANSR_LOGE("Failed to write reminder type");
        return false;
    }
    if (!parcel.WriteInt32(static_cast<int32_t>(slotType_))) {
        ANSR_LOGE("Failed to write slot type");
        return false;
    }

    // write map
    int32_t buttonMapSize = static_cast<int32_t>(actionButtonMap_.size());
    if (!parcel.WriteInt32(buttonMapSize)) {
        ANSR_LOGE("Failed to write action button size");
        return false;
    }
    for (auto it = actionButtonMap_.begin(); it != actionButtonMap_.end(); ++it) {
        if (!parcel.WriteUint8(static_cast<uint8_t>(it->first))) {
            ANSR_LOGE("Failed to write action button type");
            return false;
        }
        if (!parcel.WriteString(static_cast<std::string>(it->second.title))) {
            ANSR_LOGE("Failed to write action button title");
            return false;
        }
    }
    return true;
}

ReminderRequest *ReminderRequest::Unmarshalling(Parcel &parcel)
{
    auto objptr = new ReminderRequest();
    if ((objptr != nullptr) && !objptr->ReadFromParcel(parcel)) {
        delete objptr;
        objptr = nullptr;
    }

    return objptr;
}

bool ReminderRequest::ReadFromParcel(Parcel &parcel)
{
    // read string
    if (!parcel.ReadString(content_)) {
        ANSR_LOGE("Failed to read content");
        return false;
    }
    if (!parcel.ReadString(expiredContent_)) {
        ANSR_LOGE("to read expiredContent");
        return false;
    }
    if (!parcel.ReadString(title_)) {
        ANSR_LOGE("Failed to read title");
        return false;
    }
    if (!parcel.ReadString(wantAgentInfo_->abilityName)) {
        ANSR_LOGE("Failed to read wantAgentInfo`s abilityName");
        return false;
    }
    if (!parcel.ReadString(wantAgentInfo_->pkgName)) {
        ANSR_LOGE("Failed to read wantAgentInfo`s pkgName");
        return false;
    }

    // read int
    int32_t tempReminderId = -1;
    if (!parcel.ReadInt32(tempReminderId)) {
        ANSR_LOGE("Failed to read tempReminderId");
        return false;
    }
    reminderId_ = tempReminderId == -1 ? reminderId_ : tempReminderId;

    if (!parcel.ReadInt32(notificationId_)) {
        ANSR_LOGE("Failed to read notificationId");
        return false;
    }
    if (!parcel.ReadUint64(triggerTimeInMilli_)) {
        ANSR_LOGE("Failed to read triggerTimeInMilli");
        return false;
    }

    // read enum
    uint8_t reminderType = static_cast<uint8_t>(ReminderType::INVALID);
    if (!parcel.ReadUint8(reminderType)) {
        ANSR_LOGE("Failed to read reminderType");
        return false;
    }
    reminderType_ = static_cast<ReminderType>(reminderType);

    int32_t slotType = static_cast<int32_t>(NotificationConstant::SlotType::OTHER);
    if (!parcel.ReadInt32(slotType)) {
        ANSR_LOGE("Failed to read slotType");
        return false;
    }
    slotType_ = static_cast<NotificationConstant::SlotType>(slotType);

    // read map
    int32_t buttonMapSize = 0;
    if (!parcel.ReadInt32(buttonMapSize)) {
        ANSR_LOGE("Failed to read buttonMapSize");
        return false;
    }
    for (int i = 0; i < buttonMapSize; i++) {
        uint8_t buttonType = static_cast<uint8_t>(ActionButtonType::INVALID);
        if (!parcel.ReadUint8(buttonType)) {
            ANSR_LOGE("Failed to read buttonType");
            return false;
        }
        ActionButtonType type = static_cast<ActionButtonType>(buttonType);
        std::string title = parcel.ReadString();
        ActionButtonInfo info;
        info.type = type;
        info.title = title;
        actionButtonMap_.insert(std::pair<ActionButtonType, ActionButtonInfo>(type, info));
    }
    InitNotificationRequest();
    return true;
}

void ReminderRequest::InitNotificationRequest()
{
    ANSR_LOGI("Init notification");
    auto notificationNormalContent = std::make_shared<NotificationNormalContent>();
    notificationNormalContent->SetText(content_);
    notificationNormalContent->SetTitle(title_);
    auto notificationContent = std::make_shared<NotificationContent>(notificationNormalContent);
    notificationRequest_ = new NotificationRequest(notificationId_);
    notificationRequest_->SetLabel(NOTIFICATION_LABEL);
    notificationRequest_->SetSlotType(slotType_);
    notificationRequest_->SetContent(notificationContent);
    SetWantAgent();  // todo move to updateNotification
}

std::string ReminderRequest::GetState(const uint8_t state) const
{
    std::string stateInfo = "Inactive";
    if (state == REMINDER_STATUS_INACTIVE) {
        return stateInfo;
    } else {
        if ((state & REMINDER_STATUS_SHOWING) != 0) {
            stateInfo = "Showing";
        }
    }
    return stateInfo;
}

void ReminderRequest::AddActionButtons()
{
    int requestCode = 10;
    std::vector<WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    for (auto it = actionButtonMap_.begin(); it != actionButtonMap_.end(); ++it) {
        auto want = std::make_shared<OHOS::AAFwk::Want>();
        auto type = it->first;
        if (type == ActionButtonType::CLOSE) {
            want->SetAction(REMINDER_EVENT_CLOSE_ALERT);
            ANSR_LOGD("Add action button, type is close");
        }
        want->SetParam("REMINDER_ID", reminderId_);
        std::vector<std::shared_ptr<AAFwk::Want>> wants;
        wants.push_back(want);
        auto title = static_cast<std::string>(it->second.title);
        WantAgent::WantAgentInfo buttonWantAgentInfo(
            requestCode,
            WantAgent::WantAgentConstant::OperationType::SEND_COMMON_EVENT,
            flags,
            wants,
            nullptr
        );
        std::shared_ptr<WantAgent::WantAgent> buttonWantAgent =
            WantAgent::WantAgentHelper::GetWantAgent(buttonWantAgentInfo);
        std::shared_ptr<NotificationActionButton> actionButton
            = NotificationActionButton::Create(nullptr, title, buttonWantAgent);
        notificationRequest_->AddActionButton(actionButton);
    }
}

void ReminderRequest::AddRemovalWantAgent()
{
    int requestCode = 10;
    std::vector<WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    auto want = std::make_shared<OHOS::AAFwk::Want>();
    want->SetAction(REMINDER_EVENT_REMOVE_NOTIFICATION);
    want->SetParam(PARAM_REMINDER_ID, reminderId_);
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    wants.push_back(want);
    WantAgent::WantAgentInfo wantAgentInfo(
        requestCode,
        WantAgent::WantAgentConstant::OperationType::SEND_COMMON_EVENT,
        flags,
        wants,
        nullptr
    );
    std::shared_ptr<WantAgent::WantAgent> wantAgent = WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
    notificationRequest_->SetRemovalWantAgent(wantAgent);
}

void ReminderRequest::SetWantAgent()
{
    int requestCode = 10;
    std::vector<WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    auto want = std::make_shared<OHOS::AAFwk::Want>();
    AppExecFwk::ElementName element("", wantAgentInfo_->pkgName, wantAgentInfo_->abilityName);
    want->SetElement(element);
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    wants.push_back(want);
    WantAgent::WantAgentInfo wantAgentInfo(
        requestCode,
        WantAgent::WantAgentConstant::OperationType::START_ABILITY,
        flags,
        wants,
        nullptr
    );
    std::shared_ptr<WantAgent::WantAgent> wantAgent = WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
    notificationRequest_->SetWantAgent(wantAgent);
}

void ReminderRequest::SetState(bool deSet, const uint8_t newState, std::string function)
{
    uint8_t oldState = state_;
    if (deSet) {
        state_ |= newState;
    } else {
        state_ &= ~newState;
    }
    ANSR_LOGI("Switch the reminder(id=%{public}d) state, from %{public}s to %{public}s, called by %{public}s",
        reminderId_, GetState(oldState).c_str(), GetState(state_).c_str(), function.c_str());
}
}
}