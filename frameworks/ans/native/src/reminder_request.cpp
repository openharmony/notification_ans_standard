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
const uint64_t ReminderRequest::INVALID_LONG_LONG_VALUE = 0;
const uint16_t ReminderRequest::INVALID_U16_VALUE = 0;
const uint16_t ReminderRequest::MILLI_SECONDS = 1000;
const uint16_t ReminderRequest::SAME_TIME_DISTINGUISH_MILLISECONDS = 1000;
const uint32_t ReminderRequest::MIN_TIME_INTERVAL_IN_MILLI = 5 * 60 * 1000;
const uint8_t ReminderRequest::INVALID_U8_VALUE = 0;
const uint8_t ReminderRequest::REMINDER_STATUS_INACTIVE = 0;
const uint8_t ReminderRequest::REMINDER_STATUS_ACTIVE = 1;
const uint8_t ReminderRequest::REMINDER_STATUS_ALERTING = 2;
const uint8_t ReminderRequest::REMINDER_STATUS_SHOWING = 4;
const uint8_t ReminderRequest::REMINDER_STATUS_SNOOZE = 8;
const std::string ReminderRequest::NOTIFICATION_LABEL = "REMINDER_AGENT";
const std::string ReminderRequest::REMINDER_EVENT_ALARM_ALERT = "ohos.event.notification.reminder.ALARM_ALERT";
const std::string ReminderRequest::REMINDER_EVENT_CLOSE_ALERT = "ohos.event.notification.reminder.CLOSE_ALERT";
const std::string ReminderRequest::REMINDER_EVENT_SNOOZE_ALERT = "ohos.event.notification.reminder.SNOOZE_ALERT";
const std::string ReminderRequest::REMINDER_EVENT_ALERT_TIMEOUT = "ohos.event.notification.reminder.ALERT_TIMEOUT";
const std::string ReminderRequest::REMINDER_EVENT_REMOVE_NOTIFICATION =
    "ohos.event.notification.reminder.REMOVE_NOTIFICATION";
const std::string ReminderRequest::PARAM_REMINDER_ID = "REMINDER_ID";
const int ReminderRequest::BASE_YEAR = 1900;

ReminderRequest::ReminderRequest()
{
    wantAgentInfo_ = wantAgentInfo_ == nullptr ? std::make_shared<WantAgentInfo>() : wantAgentInfo_;
    maxScreenWantAgentInfo_ =
        maxScreenWantAgentInfo_ == nullptr ? std::make_shared<MaxScreenAgentInfo>() : maxScreenWantAgentInfo_;
}

ReminderRequest::ReminderRequest(const ReminderRequest &other)
{
    this->content_ = other.content_;
    this->expiredContent_ = other.expiredContent_;
    this->snoozeContent_ = other.snoozeContent_;
    this->displayContent_ = other.displayContent_;
    this->title_ = other.title_;
    this->isExpired_ = other.isExpired_;
    this->snoozeTimes_ = other.snoozeTimes_;
    this->snoozeTimesDynamic_ = other.snoozeTimesDynamic_;
    this->state_ = other.state_;
    this->notificationId_ = other.notificationId_;
    this->reminderId_ = other.reminderId_;
    this->reminderTimeInMilli_ = other.reminderTimeInMilli_;
    this->ringDurationInMilli_ = other.ringDurationInMilli_;
    this->triggerTimeInMilli_ = other.triggerTimeInMilli_;
    this->timeIntervalInMilli_ = other.timeIntervalInMilli_;
    this->reminderType_ = other.reminderType_;
    this->slotType_ = other.slotType_;
    this->notificationRequest_ = other.notificationRequest_;
    this->wantAgentInfo_ = other.wantAgentInfo_;
    this->maxScreenWantAgentInfo_ = other.maxScreenWantAgentInfo_;
    this->actionButtonMap_ = other.actionButtonMap_;
}

ReminderRequest::ReminderRequest(ReminderType reminderType)
{
    reminderType_ = reminderType;
    wantAgentInfo_ = wantAgentInfo_ == nullptr ? std::make_shared<WantAgentInfo>() : wantAgentInfo_;
    maxScreenWantAgentInfo_ =
        maxScreenWantAgentInfo_ == nullptr ? std::make_shared<MaxScreenAgentInfo>() : maxScreenWantAgentInfo_;
}

bool ReminderRequest::CanRemove()
{
    if ((state_ & (REMINDER_STATUS_SHOWING | REMINDER_STATUS_ALERTING | REMINDER_STATUS_ACTIVE)) == 0) {
        return true;
    }
    return false;
}

bool ReminderRequest::CanShow() const
{
    // when system time change by user manually, and the reminde is to show immediately,
    // the show reminder just need to be triggered by ReminderDataManager#RefreshRemindersLocked(uint8_t).
    // we need to make the REMINDER_EVENT_ALARM_ALERT do nothing.
    uint64_t nowInstantMilli = GetNowInstantMilli();
    if (nowInstantMilli == 0) {
        return false;
    }
    if ((nowInstantMilli - GetReminderTimeInMilli()) < MIN_TIME_INTERVAL_IN_MILLI) {
        return false;
    }
    return true;
}

std::string ReminderRequest::Dump() const
{
    const time_t nextTriggerTime = static_cast<time_t>(triggerTimeInMilli_ / MILLI_SECONDS);
    std::string dateTimeInfo = GetTimeInfoInner(nextTriggerTime, TimeFormat::YMDHMS);
    return "Reminder["
           "reminderId=" + std::to_string(reminderId_) +
           ", type=" + std::to_string(static_cast<uint8_t>(reminderType_)) +
           ", state=" + GetState(state_) +
           ", nextTriggerTime=" + dateTimeInfo.c_str() +
           "]";
}

ReminderRequest& ReminderRequest::SetActionButton(const std::string &title, const ActionButtonType &type)
{
    if (type != ActionButtonType::CLOSE && type != ActionButtonType::SNOOZE) {
        ANSR_LOGI("Button type only support: %{public}d or %{public}d",
            static_cast<uint8_t>(ActionButtonType::CLOSE), static_cast<uint8_t>(ActionButtonType::SNOOZE));
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
    SetState(false, REMINDER_STATUS_SHOWING | REMINDER_STATUS_SNOOZE, "onClose()");
    if ((state_ & REMINDER_STATUS_ALERTING) != 0) {
        SetState(false, REMINDER_STATUS_ALERTING, "onClose");
    }
    if (updateNext) {
        uint64_t nextTriggerTime = PreGetNextTriggerTimeIgnoreSnooze(true, false);
        if (nextTriggerTime == INVALID_LONG_LONG_VALUE) {
            isExpired_ = true;
        } else {
            SetTriggerTimeInMilli(nextTriggerTime);
            snoozeTimesDynamic_ = snoozeTimes_;
        }
    }
}

bool ReminderRequest::OnDateTimeChange()
{
    uint64_t nextTriggerTime = PreGetNextTriggerTimeIgnoreSnooze(true, false);
    return HandleSysTimeChange(triggerTimeInMilli_, nextTriggerTime);
}

bool ReminderRequest::HandleSysTimeChange(uint64_t oriTriggerTime, uint64_t optTriggerTime)
{
    if (isExpired_) {
        return false;
    }
    bool showImmediately = false;
    if (optTriggerTime != INVALID_LONG_LONG_VALUE && optTriggerTime <= oriTriggerTime) {
        // case1. switch to a previous time
        SetTriggerTimeInMilli(optTriggerTime);
        snoozeTimesDynamic_ = snoozeTimes_;
    } else {
        uint64_t now = GetNowInstantMilli();
        if (now == 0) {
            return false;
        }
        if (oriTriggerTime <= now) {
            // case2. switch to a future time, trigger time is less than now time.
            // when the reminder show immediately, trigger time will update in onShow function.
            snoozeTimesDynamic_ = 0;
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
    if (oldZoneTriggerTime == newZoneTriggerTime) {
        return false;
    }
    bool showImmediately = false;
    if (optTriggerTime != INVALID_LONG_LONG_VALUE && oldZoneTriggerTime < newZoneTriggerTime) {
        // case1. timezone change to smaller
        SetTriggerTimeInMilli(optTriggerTime);
        snoozeTimesDynamic_ = snoozeTimes_;
    } else {
        // case2. timezone change to larger
        time_t now;
        (void)time(&now);  // unit is seconds.
        if (static_cast<int64_t>(now) < 0) {
            ANSR_LOGE("Get now time error");
            return false;
        }
        if (newZoneTriggerTime <= (static_cast<uint64_t>(now))) {
            snoozeTimesDynamic_ = 0;
            showImmediately = true;
        } else {
            SetTriggerTimeInMilli(newZoneTriggerTime * MILLI_SECONDS);
            showImmediately = false;
        }
    }
    return showImmediately;
}

void ReminderRequest::OnSameNotificationIdCovered()
{
    SetState(false, REMINDER_STATUS_ALERTING | REMINDER_STATUS_SHOWING | REMINDER_STATUS_SNOOZE,
        "OnSameNotificationIdCovered");
}

void ReminderRequest::OnShow(bool isPlaySoundOrVibration, bool isSysTimeChanged, bool allowToNotify)
{
    if ((state_ & (REMINDER_STATUS_ACTIVE | REMINDER_STATUS_SNOOZE)) != 0) {
        SetState(false, REMINDER_STATUS_ACTIVE | REMINDER_STATUS_SNOOZE, "onShow()");
    }
    if (isSysTimeChanged) {
        uint64_t nowInstantMilli = GetNowInstantMilli();
        if (nowInstantMilli == 0) {
            ANSR_LOGW("Onshow, get now time error");
        }
        reminderTimeInMilli_ = nowInstantMilli;
    } else {
        reminderTimeInMilli_ = triggerTimeInMilli_;
    }
    UpdateNextReminder(false);
    if (allowToNotify) {
        SetState(true, REMINDER_STATUS_SHOWING, "OnShow");
        if (isPlaySoundOrVibration) {
            SetState(true, REMINDER_STATUS_ALERTING, "OnShow");
        }
        UpdateNotificationStateForAlert();
    }
}

void ReminderRequest::OnShowFail()
{
    SetState(false, REMINDER_STATUS_SHOWING, "OnShowFailed()");
}

bool ReminderRequest::OnSnooze()
{
    if ((state_ & REMINDER_STATUS_SNOOZE) != 0) {
        ANSR_LOGW("onSnooze, the state of reminder is incorrect, state: %{public}s", (GetState(state_)).c_str());
        return false;
    }
    if ((state_ & REMINDER_STATUS_ALERTING) != 0) {
        SetState(false, REMINDER_STATUS_ALERTING, "onSnooze()");
    }
    if (!UpdateNextReminder(true)) {
        return false;
    }
    UpdateNotificationStateForSnooze();
    if (timeIntervalInMilli_ > 0) {
        SetState(true, REMINDER_STATUS_SNOOZE, "onSnooze()");
    }
    return true;
}

void ReminderRequest::OnStart()
{
    if ((state_ & REMINDER_STATUS_ACTIVE) != 0) {
        ANSR_LOGE(
            "start failed, the state of reminder is incorrect, state: %{public}s", (GetState(state_)).c_str());
        return;
    }
    if (isExpired_) {
        ANSR_LOGE("start failed, the reminder is expired");
        return;
    }
    SetState(true, REMINDER_STATUS_ACTIVE, "OnStart()");
}

void ReminderRequest::OnStop()
{
    ANSR_LOGI("Stop the previous active reminder, %{public}s", this->Dump().c_str());
    if ((state_ & REMINDER_STATUS_ACTIVE) == 0) {
        ANSR_LOGW("onStop, the state of reminder is incorrect, state: %{public}s", (GetState(state_)).c_str());
        return;
    }
    SetState(false, REMINDER_STATUS_ACTIVE, "OnStop");
}

bool ReminderRequest::OnTerminate()
{
    if ((state_ & REMINDER_STATUS_ALERTING) == 0) {
        ANSR_LOGW("onTerminate, the state of reminder is %{public}s", (GetState(state_)).c_str());
        return false;
    }
    SetState(false, REMINDER_STATUS_ALERTING, "onTerminate");
    UpdateNotificationStateForAlert();
    return true;
}

bool ReminderRequest::OnTimeZoneChange()
{
    time_t oldZoneTriggerTime = static_cast<time_t>(triggerTimeInMilli_ / MILLI_SECONDS);
    struct tm *oriTime = gmtime(&oldZoneTriggerTime);
    time_t newZoneTriggerTime = mktime(oriTime);
    uint64_t nextTriggerTime = PreGetNextTriggerTimeIgnoreSnooze(true, false);
    return HandleTimeZoneChange(oldZoneTriggerTime, newZoneTriggerTime, nextTriggerTime);
}

ReminderRequest& ReminderRequest::SetMaxScreenWantAgentInfo(
    const std::shared_ptr<MaxScreenAgentInfo> &maxScreenWantAgentInfo)
{
    maxScreenWantAgentInfo_ = maxScreenWantAgentInfo;
    return *this;
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

ReminderRequest& ReminderRequest::SetSnoozeContent(const std::string &snoozeContent)
{
    snoozeContent_ = snoozeContent;
    return *this;
}

ReminderRequest& ReminderRequest::SetSnoozeTimes(const uint8_t snoozeTimes)
{
    snoozeTimes_ = snoozeTimes;
    SetSnoozeTimesDynamic(snoozeTimes);
    return *this;
}

ReminderRequest& ReminderRequest::SetSnoozeTimesDynamic(const uint8_t snooziTimes)
{
    snoozeTimesDynamic_ = snooziTimes;
    return *this;
}

ReminderRequest& ReminderRequest::SetTimeInterval(const uint64_t timeIntervalInSeconds)
{
    if (timeIntervalInSeconds < 0 || timeIntervalInSeconds > (UINT64_MAX / MILLI_SECONDS)) {
        ANSR_LOGW("SetTimeInterval, replace to set (0s), for the given is out of legal range");
        timeIntervalInMilli_ = 0;
    } else {
        uint64_t timeIntervalInMilli = timeIntervalInSeconds * MILLI_SECONDS;
        if (timeIntervalInMilli > 0 && timeIntervalInMilli < MIN_TIME_INTERVAL_IN_MILLI) {
            ANSR_LOGW("SetTimeInterval, replace to set %{public}u, for the given is 0<%{public}llu<%{public}u",
                MIN_TIME_INTERVAL_IN_MILLI / MILLI_SECONDS, (unsigned long long)timeIntervalInSeconds,
                MIN_TIME_INTERVAL_IN_MILLI / MILLI_SECONDS);
            timeIntervalInMilli_ = MIN_TIME_INTERVAL_IN_MILLI;
        } else {
            timeIntervalInMilli_ = timeIntervalInMilli;
        }
    }
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

bool ReminderRequest::ShouldShowImmediately() const
{
    uint64_t nowInstantMilli = GetNowInstantMilli();
    if (nowInstantMilli == 0) {
        return false;
    }
    if (triggerTimeInMilli_ > nowInstantMilli) {
        return false;
    }
    return true;
}

std::map<ReminderRequest::ActionButtonType, ReminderRequest::ActionButtonInfo> ReminderRequest::GetActionButtons(
    ) const
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

std::shared_ptr<ReminderRequest::MaxScreenAgentInfo> ReminderRequest::GetMaxScreenWantAgentInfo() const
{
    return maxScreenWantAgentInfo_;
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

uint64_t ReminderRequest::GetReminderTimeInMilli() const
{
    return reminderTimeInMilli_;
}

void ReminderRequest::SetReminderId(int32_t reminderId)
{
    reminderId_ = reminderId;
}

void ReminderRequest::SetReminderTimeInMilli(const uint64_t reminderTimeInMilli)
{
    reminderTimeInMilli_ = reminderTimeInMilli;
}

ReminderRequest& ReminderRequest::SetRingDuration(const uint64_t ringDurationInSeconds)
{
    if (ringDurationInSeconds <= 0 || ringDurationInSeconds > (UINT64_MAX / MILLI_SECONDS)) {
        ANSR_LOGW("setRingDuration, replace to set (1s), for the given is out of legal range");
        ringDurationInMilli_ = MILLI_SECONDS;
    } else {
        ringDurationInMilli_ = ringDurationInSeconds * MILLI_SECONDS;
    }
    return *this;
}

NotificationConstant::SlotType ReminderRequest::GetSlotType() const
{
    return slotType_;
}

std::string ReminderRequest::GetSnoozeContent() const
{
    return snoozeContent_;
}

uint8_t ReminderRequest::GetSnoozeTimes() const
{
    return snoozeTimes_;
}

uint8_t ReminderRequest::GetSnoozeTimesDynamic() const
{
    return snoozeTimesDynamic_;
}

uint8_t ReminderRequest::GetState() const
{
    return state_;
}

uint64_t ReminderRequest::GetTimeInterval() const
{
    return timeIntervalInMilli_ / MILLI_SECONDS;
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

uint16_t ReminderRequest::GetRingDuration() const
{
    return ringDurationInMilli_ / MILLI_SECONDS;
}

bool ReminderRequest::UpdateNextReminder()
{
    return false;
}

void ReminderRequest::UpdateNotificationRequest(UpdateNotificationType type, std::string extra)
{
    switch (type) {
        case UpdateNotificationType::COMMON: {
            ANSR_LOGI("UpdateNotification common information");
            UpdateNotificationCommon();
            break;
        }
        case UpdateNotificationType::REMOVAL_WANT_AGENT: {
            ANSR_LOGI("UpdateNotification removal_want_agent");
            AddRemovalWantAgent();
            break;
        }
        case UpdateNotificationType::WANT_AGENT: {
            ANSR_LOGI("UpdateNotification want_agent");
            AppExecFwk::ElementName wantAgent("", wantAgentInfo_->pkgName, wantAgentInfo_->abilityName);
            SetWantAgent(wantAgent);
            break;
        }
        case UpdateNotificationType::MAX_SCREEN_WANT_AGENT: {
            ANSR_LOGI("UpdateNotification max_screen_want_agent");
            AppExecFwk::ElementName maxScreenWantAgent(
                "", maxScreenWantAgentInfo_->pkgName, maxScreenWantAgentInfo_->abilityName);
            SetMaxScreenWantAgent(maxScreenWantAgent);
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
    if (!parcel.WriteString(snoozeContent_)) {
        ANSR_LOGE("Failed to write snoozeContent");
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
    if (!parcel.WriteString(maxScreenWantAgentInfo_->abilityName)) {
        ANSR_LOGE("Failed to write maxScreenWantAgentInfo`s abilityName");
        return false;
    }
    if (!parcel.WriteString(maxScreenWantAgentInfo_->pkgName)) {
        ANSR_LOGE("Failed to write maxScreenWantAgentInfo`s pkgName");
        return false;
    }

    // write bool
    if (!parcel.WriteBool(isExpired_)) {
        ANSR_LOGE("Failed to write isExpired");
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
    if (!parcel.WriteUint64(timeIntervalInMilli_)) {
        ANSR_LOGE("Failed to write timeIntervalInMilli");
        return false;
    }
    if (!parcel.WriteUint64(ringDurationInMilli_)) {
        ANSR_LOGE("Failed to write ringDurationInMilli");
        return false;
    }
    if (!parcel.WriteUint64(reminderTimeInMilli_)) {
        ANSR_LOGE("Failed to write reminderTimeInMilli");
        return false;
    }
    if (!parcel.WriteUint8(snoozeTimes_)) {
        ANSR_LOGE("Failed to write snoozeTimes");
        return false;
    }
    if (!parcel.WriteUint8(snoozeTimesDynamic_)) {
        ANSR_LOGE("Failed to write snoozeTimesDynamic");
        return false;
    }
    if (!parcel.WriteUint8(state_)) {
        ANSR_LOGE("Failed to write state");
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
    if (!parcel.ReadString(snoozeContent_)) {
        ANSR_LOGE("to read snoozeContent");
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
    if (!parcel.ReadString(maxScreenWantAgentInfo_->abilityName)) {
        ANSR_LOGE("Failed to read maxScreenWantAgentInfo`s abilityName");
        return false;
    }
    if (!parcel.ReadString(maxScreenWantAgentInfo_->pkgName)) {
        ANSR_LOGE("Failed to read maxScreenWantAgentInfo`s pkgName");
        return false;
    }

    // read bool
    if (!parcel.ReadBool(isExpired_)) {
        ANSR_LOGE("Failed to read isExpired");
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
    if (!parcel.ReadUint64(timeIntervalInMilli_)) {
        ANSR_LOGE("Failed to read timeIntervalInMilli");
        return false;
    }
    if (!parcel.ReadUint64(ringDurationInMilli_)) {
        ANSR_LOGE("Failed to read ringDurationInMilli");
        return false;
    }
    if (!parcel.ReadUint64(reminderTimeInMilli_)) {
        ANSR_LOGE("Failed to read reminderTimeInMilli");
        return false;
    }
    if (!parcel.ReadUint8(snoozeTimes_)) {
        ANSR_LOGE("Failed to read snoozeTimes");
        return false;
    }
    if (!parcel.ReadUint8(snoozeTimesDynamic_)) {
        ANSR_LOGE("Failed to read snoozeTimesDynamic");
        return false;
    }
    if (!parcel.ReadUint8(state_)) {
        ANSR_LOGE("Failed to read state");
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
    notificationRequest_ = new NotificationRequest(notificationId_);
    displayContent_ = content_;
    AddActionButtons(true);
}

bool ReminderRequest::IsAlerting() const
{
    return (state_ & REMINDER_STATUS_ALERTING) != 0;
}

std::string ReminderRequest::GetDateTimeInfo(const time_t &timeInSecond) const
{
    return GetTimeInfoInner(timeInSecond, TimeFormat::YMDHMS);
}

uint64_t ReminderRequest::GetNowInstantMilli() const
{
    time_t now;
    (void)time(&now);  // unit is seconds.
    if (static_cast<int64_t>(now) < 0) {
        ANSR_LOGE("Get now time error");
        return 0;
    }
    return static_cast<uint64_t>(now * MILLI_SECONDS);
}

std::string ReminderRequest::GetShowTime(const uint64_t showTime) const
{
    if (reminderType_ == ReminderType::TIMER) {
        return "";
    } else {
        const time_t showTimeInSecond = static_cast<time_t>(showTime / MILLI_SECONDS);
        return GetTimeInfoInner(showTimeInSecond, TimeFormat::HM);
    }
}

std::string ReminderRequest::GetTimeInfoInner(const time_t &timeInSecond, const TimeFormat &format) const
{
    uint8_t dateTimeLen = 80;
    char dateTimeBuffer[dateTimeLen];
    struct tm *timeInfo = localtime(&timeInSecond);
    if (timeInfo == nullptr) {
        ANSR_LOGW("GetTimeInfoInner fail.");
    } else {
        switch (format) {
            case TimeFormat::YMDHMS: {
                (void)strftime(dateTimeBuffer, dateTimeLen, "%Y-%m-%d %H:%M:%S", timeInfo);
                break;
            }
            case TimeFormat::HM: {
                (void)strftime(dateTimeBuffer, dateTimeLen, "%H:%M", timeInfo);
                break;
            }
            default: {
                ANSR_LOGW("Time format not support.");
                break;
            }
        }
    }
    std::string dateTimeInfo(dateTimeBuffer);
    return dateTimeInfo;
}

std::string ReminderRequest::GetState(const uint8_t state) const
{
    std::string stateInfo = "'";
    if (state == REMINDER_STATUS_INACTIVE) {
        stateInfo += "Inactive";
    } else {
        bool hasSeparator = false;
        if ((state & REMINDER_STATUS_ACTIVE) != 0) {
            stateInfo += "Active";
            hasSeparator = true;
        }
        if ((state & REMINDER_STATUS_ALERTING) != 0) {
            if (hasSeparator) {
                stateInfo += ",";
            }
            stateInfo += "Alerting";
            hasSeparator = true;
        }
        if ((state & REMINDER_STATUS_SHOWING) != 0) {
            if (hasSeparator) {
                stateInfo += ",";
            }
            stateInfo += "Showing";
            hasSeparator = true;
        }
        if ((state & REMINDER_STATUS_SNOOZE) != 0) {
            if (hasSeparator) {
                stateInfo += ",";
            }
            stateInfo += "Snooze";
        }
        stateInfo += "'";
    }
    return stateInfo;
}

void ReminderRequest::AddActionButtons(const bool includeSnooze)
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
        if (type == ActionButtonType::SNOOZE)  {
            if (includeSnooze) {
                want->SetAction(REMINDER_EVENT_SNOOZE_ALERT);
                ANSR_LOGD("Add action button, type is snooze");
            } else {
                ANSR_LOGD("Not add action button, type is snooze, as includeSnooze is false");
                continue;
            }
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

std::shared_ptr<WantAgent::WantAgent> ReminderRequest::CreateWantAgent(AppExecFwk::ElementName &element) const
{
    int requestCode = 10;
    std::vector<WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    auto want = std::make_shared<OHOS::AAFwk::Want>();
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
    return WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
}

void ReminderRequest::SetMaxScreenWantAgent(AppExecFwk::ElementName &element)
{
    std::shared_ptr<WantAgent::WantAgent> wantAgent = CreateWantAgent(element);
    notificationRequest_->SetMaxScreenWantAgent(wantAgent);
}

void ReminderRequest::SetWantAgent(AppExecFwk::ElementName &element)
{
    std::shared_ptr<WantAgent::WantAgent> wantAgent = CreateWantAgent(element);
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
    ANSR_LOGI("Switch the reminder(reminderId=%{public}d) state, from %{public}s to %{public}s, called by %{public}s",
        reminderId_, GetState(oldState).c_str(), GetState(state_).c_str(), function.c_str());
}

void ReminderRequest::UpdateActionButtons(const bool &setSnooze)
{
    if (notificationRequest_ == nullptr) {
        ANSR_LOGE("updateActionButtons failed, the notificationRequest is null");
        return;
    }
    notificationRequest_->ClearActionButtons();
    if (setSnooze || snoozeTimesDynamic_ == snoozeTimes_) {
        AddActionButtons(false);
    } else {
        AddActionButtons(true);
    }
}

bool ReminderRequest::UpdateNextReminder(const bool &force)
{
    bool result = true;
    if (force) {
        uint64_t nowInstantMilli = GetNowInstantMilli();
        if (nowInstantMilli == 0) {
            result = false;
        } else {
            triggerTimeInMilli_ = nowInstantMilli + timeIntervalInMilli_;
            snoozeTimesDynamic_ = snoozeTimes_;
            if (timeIntervalInMilli_ != 0) {
                isExpired_ = false;
            }
        }
    } else {
        result = UpdateNextReminder();
    }
    std::string info = result == true ? "success" : "no next";
    ANSR_LOGI("updateNextReminder(id=%{public}d, %{public}s): force=%{public}d, trigger time is: %{public}s",
        reminderId_, info.c_str(), force,
        GetDateTimeInfo(static_cast<time_t>(triggerTimeInMilli_ / MILLI_SECONDS)).c_str());
    return result;
}

void ReminderRequest::UpdateNotificationCommon()
{
    notificationRequest_->SetLabel(NOTIFICATION_LABEL);
    notificationRequest_->SetShowDeliveryTime(true);
    notificationRequest_->SetTapDismissed(true);
    notificationRequest_->SetSlotType(slotType_);
    auto notificationNormalContent = std::make_shared<NotificationNormalContent>();
    notificationNormalContent->SetText(displayContent_);
    notificationNormalContent->SetTitle(title_);
    auto notificationContent = std::make_shared<NotificationContent>(notificationNormalContent);
    notificationRequest_->SetContent(notificationContent);
    if (reminderType_ == ReminderRequest::ReminderType::TIMER
        || reminderType_ == ReminderRequest::ReminderType::ALARM) {
        notificationRequest_->SetUnremovable(true);
    }
    time_t now;
    (void)time(&now);  // unit is seconds.
    notificationRequest_->SetDeliveryTime(static_cast<int64_t>(now * MILLI_SECONDS));
}

void ReminderRequest::UpdateNotificationContent(const bool &setSnooze)
{
    if (notificationRequest_ == nullptr) {
        ANSR_LOGE("updateNotificationContent failed, the notificationRequest is null");
        return;
    }
    std::string extendContent = "";
    if (setSnooze) {
        if (timeIntervalInMilli_ != 0) {
            // snooze the reminder by manual
            extendContent = GetShowTime(triggerTimeInMilli_) +
                snoozeContent_ == "" ? "" : (" (" + snoozeContent_ + ")");
        } else {
            // the reminder is expired now, when timeInterval is 0
            extendContent = GetShowTime(reminderTimeInMilli_) +
                expiredContent_ == "" ? "" : (" (" + expiredContent_ + ")");
        }
    } else if (IsAlerting()) {
        // the reminder is alerting, or ring duration is 0
        extendContent = GetShowTime(reminderTimeInMilli_);
    } else if (snoozeTimesDynamic_ != snoozeTimes_) {
        // the reminder is snoozing by period artithmetic, when the ring duration is over.
        extendContent = GetShowTime(triggerTimeInMilli_) +
            snoozeContent_ == "" ? "" : (" (" + snoozeContent_ + ")");
        notificationRequest_->SetTapDismissed(false);
    } else {
        // the reminder has already snoozed by period arithmetic, when the ring duration is over.
        extendContent = GetShowTime(reminderTimeInMilli_) +
            expiredContent_ == "" ? "" : (" (" + expiredContent_ + ")");
    }
    displayContent_ = content_ + " " + extendContent;
    ANSR_LOGD("Display content=%{public}s", displayContent_.c_str());
}

void ReminderRequest::UpdateNotificationStateForAlert()
{
    UpdateNotificationContent(false);
    UpdateActionButtons(false);
}

void ReminderRequest::UpdateNotificationStateForSnooze()
{
    UpdateNotificationContent(true);
    UpdateActionButtons(true);
}

int ReminderRequest::GetActualTime(const TimeTransferType &type, int cTime)
{
    switch (type) {
        case (TimeTransferType::YEAR):  // year
            return BASE_YEAR + cTime;
        case (TimeTransferType::MONTH):  // month
            return 1 + cTime;
        case (TimeTransferType::WEEK):  // week
            return 1 + cTime;
        default:
            return -1;
    }
}

int ReminderRequest::GetCTime(const TimeTransferType &type, int actualTime)
{
    switch (type) {
        case (TimeTransferType::YEAR):  // year
            return actualTime - BASE_YEAR;
        case (TimeTransferType::MONTH):  // month
            return actualTime - 1;
        case (TimeTransferType::WEEK):  // week
            return actualTime - 1;
        default:
            return -1;
    }
}
}
}
