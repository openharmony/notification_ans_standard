/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "reminder_request.h"

#include "ans_log_wrapper.h"
#include "bundle_mgr_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "os_account_manager.h"
#include "reminder_store.h"
#include "system_ability_definition.h"
#include "want_agent_helper.h"

namespace OHOS {
namespace Notification {
namespace {
const int BASE_YEAR = 1900;
}

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
const std::string ReminderRequest::SEP_BUTTON_SINGLE = "<SEP,/>";
const std::string ReminderRequest::SEP_BUTTON_MULTI = "<SEP#/>";
const std::string ReminderRequest::SEP_WANT_AGENT = "<SEP#/>";

// For database recovery.
const std::string ReminderRequest::REMINDER_ID = "reminder_id";
const std::string ReminderRequest::PKG_NAME = "package_name";
const std::string ReminderRequest::USER_ID = "user_id";
const std::string ReminderRequest::UID = "uid";
const std::string ReminderRequest::APP_LABEL = "app_label";
const std::string ReminderRequest::REMINDER_TYPE = "reminder_type";
const std::string ReminderRequest::REMINDER_TIME = "reminder_time";
const std::string ReminderRequest::TRIGGER_TIME = "trigger_time";
const std::string ReminderRequest::RTC_TRIGGER_TIME = "rtc_trigger_time";
const std::string ReminderRequest::TIME_INTERVAL = "time_interval";
const std::string ReminderRequest::SNOOZE_TIMES = "snooze_times";
const std::string ReminderRequest::DYNAMIC_SNOOZE_TIMES = "dynamic_snooze_times";
const std::string ReminderRequest::RING_DURATION = "ring_duration";
const std::string ReminderRequest::IS_EXPIRED = "is_expired";
const std::string ReminderRequest::IS_ACTIVE = "is_active";
const std::string ReminderRequest::STATE = "state";
const std::string ReminderRequest::ZONE_ID = "zone_id";
const std::string ReminderRequest::HAS_SCHEDULED_TIMEOUT = "has_ScheduledTimeout";
const std::string ReminderRequest::ACTION_BUTTON_INFO = "button_info";
const std::string ReminderRequest::SLOT_ID = "slot_id";
const std::string ReminderRequest::NOTIFICATION_ID = "notification_id";
const std::string ReminderRequest::TITLE = "title";
const std::string ReminderRequest::CONTENT = "content";
const std::string ReminderRequest::SNOOZE_CONTENT = "snooze_content";
const std::string ReminderRequest::EXPIRED_CONTENT = "expired_content";
const std::string ReminderRequest::AGENT = "agent";
const std::string ReminderRequest::MAX_SCREEN_AGENT = "maxScreen_agent";

std::string ReminderRequest::sqlOfAddColumns = "";
std::vector<std::string> ReminderRequest::columns;

ReminderRequest::ReminderRequest()
{
    InitServerObj();
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

ReminderRequest::ReminderRequest(int32_t reminderId)
{
    reminderId_ = reminderId;
    InitServerObj();
}

ReminderRequest::ReminderRequest(ReminderType reminderType)
{
    reminderType_ = reminderType;
    InitServerObj();
}

bool ReminderRequest::CanRemove() const
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

void ReminderRequest::InitUserId(const int &userId)
{
    userId_ = userId;
}

void ReminderRequest::InitUid(const int32_t &uid)
{
    uid_ = uid;
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
        if (newZoneTriggerTime <= GetDurationSinceEpochInMilli(now)) {
            snoozeTimesDynamic_ = 0;
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
    struct tm oriTime;
    (void)gmtime_r(&oldZoneTriggerTime, &oriTime);
    time_t newZoneTriggerTime = mktime(&oriTime);
    uint64_t nextTriggerTime = PreGetNextTriggerTimeIgnoreSnooze(true, false);
    return HandleTimeZoneChange(
        triggerTimeInMilli_, GetDurationSinceEpochInMilli(newZoneTriggerTime), nextTriggerTime);
}

int64_t ReminderRequest::RecoverInt64FromDb(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet,
    const std::string &columnName, const DbRecoveryType &columnType)
{
    if (resultSet == nullptr) {
        ANSR_LOGE("ResultSet is null");
        return 0;
    }
    switch (columnType) {
        case (DbRecoveryType::INT): {
            int value;
            resultSet->GetInt(ReminderStore::GetColumnIndex(columnName), value);
            return static_cast<int64_t>(value);
        }
        case (DbRecoveryType::LONG): {
            int64_t value;
            resultSet->GetLong(ReminderStore::GetColumnIndex(columnName), value);
            return value;
        }
        default: {
            ANSR_LOGD("ColumnType not support.");
            break;
        }
    }
    ANSR_LOGE("Recover data error");
    return 0;
}

void ReminderRequest::RecoverFromDb(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{
    if (resultSet == nullptr) {
        ANSR_LOGE("ResultSet is null");
        return;
    }

    // reminderId
    resultSet->GetInt(ReminderStore::GetColumnIndex(REMINDER_ID), reminderId_);

    // userId
    resultSet->GetInt(ReminderStore::GetColumnIndex(USER_ID), userId_);

    // bundleName
    resultSet->GetString(ReminderStore::GetColumnIndex(PKG_NAME), bundleName_);

    // uid
    resultSet->GetInt(ReminderStore::GetColumnIndex(UID), uid_);

    // reminderType
    int reminderType;
    resultSet->GetInt(ReminderStore::GetColumnIndex(REMINDER_TYPE), reminderType);
    reminderType_ = ReminderType(reminderType);

    // reminderTime
    reminderTimeInMilli_ =
        static_cast<uint64_t>(RecoverInt64FromDb(resultSet, REMINDER_TIME, DbRecoveryType::LONG));

    // triggerTime
    triggerTimeInMilli_ =
        static_cast<uint64_t>(RecoverInt64FromDb(resultSet, TRIGGER_TIME, DbRecoveryType::LONG));

    // timeInterval
    uint64_t timeIntervalInSecond =
        static_cast<uint64_t>(RecoverInt64FromDb(resultSet, TIME_INTERVAL, DbRecoveryType::LONG));
    SetTimeInterval(timeIntervalInSecond);

    // snoozeTimes
    snoozeTimes_ = static_cast<uint8_t>(RecoverInt64FromDb(resultSet, SNOOZE_TIMES, DbRecoveryType::INT));

    // dynamicSnoozeTimes
    snoozeTimesDynamic_ =
        static_cast<uint8_t>(RecoverInt64FromDb(resultSet, DYNAMIC_SNOOZE_TIMES, DbRecoveryType::INT));

    // ringDuration
    uint64_t ringDurationInSecond =
        static_cast<uint64_t>(RecoverInt64FromDb(resultSet, RING_DURATION, DbRecoveryType::LONG));
    SetRingDuration(ringDurationInSecond);

    // isExpired
    std::string isExpired;
    resultSet->GetString(ReminderStore::GetColumnIndex(IS_EXPIRED), isExpired);
    isExpired_ = isExpired == "true" ? true : false;

    // state
    state_ = static_cast<uint8_t>(RecoverInt64FromDb(resultSet, STATE, DbRecoveryType::INT));

    // action buttons
    RecoverActionButton(resultSet);

    // slotType
    int slotType;
    resultSet->GetInt(ReminderStore::GetColumnIndex(SLOT_ID), slotType);
    slotType_ = NotificationConstant::SlotType(slotType);

    // notification id
    resultSet->GetInt(ReminderStore::GetColumnIndex(NOTIFICATION_ID), notificationId_);

    // title
    resultSet->GetString(ReminderStore::GetColumnIndex(TITLE), title_);

    // content
    resultSet->GetString(ReminderStore::GetColumnIndex(CONTENT), content_);

    // snoozeContent
    resultSet->GetString(ReminderStore::GetColumnIndex(SNOOZE_CONTENT), snoozeContent_);

    // expiredContent
    resultSet->GetString(ReminderStore::GetColumnIndex(EXPIRED_CONTENT), expiredContent_);

    InitNotificationRequest();  // must set before wantAgent & maxScreenWantAgent

    // wantAgent
    std::string wantAgent;
    resultSet->GetString(ReminderStore::GetColumnIndex(AGENT), wantAgent);
    RecoverWantAgent(wantAgent, 0);

    // maxScreenWantAgent
    std::string maxScreenWantAgent;
    resultSet->GetString(ReminderStore::GetColumnIndex(MAX_SCREEN_AGENT), maxScreenWantAgent);
    RecoverWantAgent(wantAgent, 1);
}

void ReminderRequest::RecoverActionButton(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{
    if (resultSet == nullptr) {
        ANSR_LOGE("ResultSet is null");
        return;
    }
    std::string actionButtonInfo;
    resultSet->GetString(ReminderStore::GetColumnIndex(ACTION_BUTTON_INFO), actionButtonInfo);
    std::vector<std::string> multiButton = StringSplit(actionButtonInfo, SEP_BUTTON_MULTI);
    for (auto button : multiButton) {
        std::vector<std::string> singleButton = StringSplit(button, SEP_BUTTON_SINGLE);
        SetActionButton(singleButton.at(1), ActionButtonType(std::stoi(singleButton.at(0), nullptr)));
    }
}

std::vector<std::string> ReminderRequest::StringSplit(std::string source, const std::string &split) const
{
    std::vector<std::string> result;
    if (source.empty()) {
        return result;
    }
    size_t pos = 0;
    while ((pos = source.find(split)) != std::string::npos) {
        std::string token = source.substr(0, pos);
        if (!token.empty()) {
            result.push_back(token);
        }
        source.erase(0, pos + split.length());
    }
    if (!source.empty()) {
        result.push_back(source);
    }
    return result;
}

void ReminderRequest::RecoverWantAgent(std::string wantAgentInfo, const uint8_t &type)
{
    std::vector<std::string> info = StringSplit(wantAgentInfo, ReminderRequest::SEP_WANT_AGENT);
    uint8_t minLen = 2;
    if (info.size() < minLen) {
        ANSR_LOGW("RecoverWantAgent fail");
        return;
    }
    ANSR_LOGD("pkg=%{public}s, ability=%{public}s", info.at(0).c_str(), info.at(1).c_str());
    switch (type) {
        case 0: {
            auto wantAgentInfo = std::make_shared<ReminderRequest::WantAgentInfo>();
            wantAgentInfo->pkgName = info.at(0);
            wantAgentInfo->abilityName = info.at(1);
            SetWantAgentInfo(wantAgentInfo);
            break;
        }
        case 1: {
            auto maxScreenWantAgentInfo = std::make_shared<ReminderRequest::MaxScreenAgentInfo>();
            maxScreenWantAgentInfo->pkgName = info.at(0);
            maxScreenWantAgentInfo->abilityName = info.at(1);
            SetMaxScreenWantAgentInfo(maxScreenWantAgentInfo);
            break;
        }
        default: {
            ANSR_LOGW("RecoverWantAgent type not support");
            break;
        }
    }
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

int ReminderRequest::GetUserId() const
{
    return userId_;
}

int32_t ReminderRequest::GetUid() const
{
    return uid_;
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
        case UpdateNotificationType::BUNDLE_INFO: {
            ANSR_LOGI("UpdateNotification hap information");
            UpdateNotificationBundleInfo();
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
    for (auto button : actionButtonMap_) {
        if (!parcel.WriteUint8(static_cast<uint8_t>(button.first))) {
            ANSR_LOGE("Failed to write action button type");
            return false;
        }
        if (!parcel.WriteString(static_cast<std::string>(button.second.title))) {
            ANSR_LOGE("Failed to write action button title");
            return false;
        }
    }
    return true;
}

ReminderRequest *ReminderRequest::Unmarshalling(Parcel &parcel)
{
    auto objptr = new (std::nothrow) ReminderRequest();
    if (objptr == nullptr) {
        ANSR_LOGE("Failed to create reminder due to no memory.");
        return objptr;
    }
    if (!objptr->ReadFromParcel(parcel)) {
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
    if (!InitNotificationRequest()) {
        return false;
    }
    return true;
}

bool ReminderRequest::InitNotificationRequest()
{
    ANSR_LOGI("Init notification");
    notificationRequest_ = new (std::nothrow) NotificationRequest(notificationId_);
    if (notificationRequest_ == nullptr) {
        ANSR_LOGE("Failed to create notification.");
        return false;
    }
    displayContent_ = content_;
    AddActionButtons(true);
    return true;
}

void ReminderRequest::InitServerObj()
{
    wantAgentInfo_ = wantAgentInfo_ == nullptr ? std::make_shared<WantAgentInfo>() : wantAgentInfo_;
    maxScreenWantAgentInfo_ =
        maxScreenWantAgentInfo_ == nullptr ? std::make_shared<MaxScreenAgentInfo>() : maxScreenWantAgentInfo_;
}

bool ReminderRequest::IsAlerting() const
{
    return (state_ & REMINDER_STATUS_ALERTING) != 0;
}

uint64_t ReminderRequest::GetDurationSinceEpochInMilli(const time_t target)
{
    auto tarEndTimePoint = std::chrono::system_clock::from_time_t(target);
    auto tarDuration = std::chrono::duration_cast<std::chrono::milliseconds>(tarEndTimePoint.time_since_epoch());
    int64_t tarDate = tarDuration.count();
    if (tarDate < 0) {
        ANSR_LOGW("tarDuration is less than 0.");
        return INVALID_LONG_LONG_VALUE;
    }
    return static_cast<uint64_t>(tarDate);
}

std::string ReminderRequest::GetDateTimeInfo(const time_t &timeInSecond) const
{
    return GetTimeInfoInner(timeInSecond, TimeFormat::YMDHMS);
}

std::string ReminderRequest::GetButtonInfo() const
{
    std::string info = "";
    bool isFirst = true;
    for (auto button : actionButtonMap_) {
        if (!isFirst) {
            info += SEP_BUTTON_MULTI;
        }
        ActionButtonInfo buttonInfo = button.second;
        info += std::to_string(static_cast<uint8_t>(button.first)) + SEP_BUTTON_SINGLE + buttonInfo.title;
        isFirst = false;
    }
    return info;
}

uint64_t ReminderRequest::GetNowInstantMilli() const
{
    time_t now;
    (void)time(&now);  // unit is seconds.
    if (static_cast<int64_t>(now) < 0) {
        ANSR_LOGE("Get now time error");
        return 0;
    }
    return GetDurationSinceEpochInMilli(now);
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
    struct tm timeInfo;
    (void)localtime_r(&timeInSecond, &timeInfo);
    switch (format) {
        case TimeFormat::YMDHMS: {
            (void)strftime(dateTimeBuffer, dateTimeLen, "%Y-%m-%d %H:%M:%S", &timeInfo);
            break;
        }
        case TimeFormat::HM: {
            (void)strftime(dateTimeBuffer, dateTimeLen, "%H:%M", &timeInfo);
            break;
        }
        default: {
            ANSR_LOGW("Time format not support.");
            break;
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
    std::vector<AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(AbilityRuntime::WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    for (auto button : actionButtonMap_) {
        auto want = std::make_shared<OHOS::AAFwk::Want>();
        auto type = button.first;
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
        want->SetParam(PARAM_REMINDER_ID, reminderId_);
        std::vector<std::shared_ptr<AAFwk::Want>> wants;
        wants.push_back(want);
        auto title = static_cast<std::string>(button.second.title);
        AbilityRuntime::WantAgent::WantAgentInfo buttonWantAgentInfo(
            requestCode,
            AbilityRuntime::WantAgent::WantAgentConstant::OperationType::SEND_COMMON_EVENT,
            flags,
            wants,
            nullptr
        );
        std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> buttonWantAgent =
            AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(buttonWantAgentInfo, userId_);
        std::shared_ptr<NotificationActionButton> actionButton
            = NotificationActionButton::Create(nullptr, title, buttonWantAgent);
        notificationRequest_->AddActionButton(actionButton);
    }
}

void ReminderRequest::AddRemovalWantAgent()
{
    int requestCode = 10;
    std::vector<AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(AbilityRuntime::WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    auto want = std::make_shared<OHOS::AAFwk::Want>();
    want->SetAction(REMINDER_EVENT_REMOVE_NOTIFICATION);
    want->SetParam(PARAM_REMINDER_ID, reminderId_);
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    wants.push_back(want);
    AbilityRuntime::WantAgent::WantAgentInfo wantAgentInfo(
        requestCode,
        AbilityRuntime::WantAgent::WantAgentConstant::OperationType::SEND_COMMON_EVENT,
        flags,
        wants,
        nullptr
    );
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent =
        AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo, userId_);
    notificationRequest_->SetRemovalWantAgent(wantAgent);
}

std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> ReminderRequest::CreateWantAgent(
    AppExecFwk::ElementName &element) const
{
    int requestCode = 10;
    std::vector<AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(AbilityRuntime::WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    auto want = std::make_shared<OHOS::AAFwk::Want>();
    want->SetElement(element);
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    wants.push_back(want);
    AbilityRuntime::WantAgent::WantAgentInfo wantAgentInfo(
        requestCode,
        AbilityRuntime::WantAgent::WantAgentConstant::OperationType::START_ABILITY,
        flags,
        wants,
        nullptr
    );
    return AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo, userId_);
}

void ReminderRequest::SetMaxScreenWantAgent(AppExecFwk::ElementName &element)
{
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent = CreateWantAgent(element);
    notificationRequest_->SetMaxScreenWantAgent(wantAgent);
}

void ReminderRequest::SetWantAgent(AppExecFwk::ElementName &element)
{
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent = CreateWantAgent(element);
    notificationRequest_->SetWantAgent(wantAgent);
}

void ReminderRequest::SetState(bool deSet, const uint8_t newState, std::string function)
{
    uint8_t oldState = state_;
    if (deSet) {
        state_ |= newState;
    } else {
        state_ &= static_cast<uint8_t>(~newState);
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
    time_t now;
    (void)time(&now);  // unit is seconds.
    notificationRequest_->SetDeliveryTime(GetDurationSinceEpochInMilli(now));
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
    auto flags = std::make_shared<NotificationFlags>();
    flags->SetSoundEnabled(NotificationConstant::FlagStatus::CLOSE);
    flags->SetVibrationEnabled(NotificationConstant::FlagStatus::CLOSE);
    notificationRequest_->SetFlags(flags);
}

void ReminderRequest::UpdateNotificationBundleInfo()
{
    std::string ownerBundleName = notificationRequest_->GetOwnerBundleName();
    if (!(ownerBundleName.empty())) {
        return;
    }
    ANSR_LOGD("ownerBundleName=%{public}s, bundleName_=%{public}s",
        ownerBundleName.c_str(), bundleName_.c_str());
    notificationRequest_->SetOwnerBundleName(bundleName_);
    notificationRequest_->SetCreatorBundleName(bundleName_);
    notificationRequest_->SetCreatorUid(uid_);
    ErrCode errCode = AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(uid_, userId_);
    if (errCode != ERR_OK) {
        ANSR_LOGE("GetOsAccountLocalIdFromUid fail.");
        return;
    }
    notificationRequest_->SetCreatorUserId(userId_);
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
            notificationRequest_->SetTapDismissed(false);
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
    ANSR_LOGD("UpdateNotification content and buttons");
    UpdateNotificationContent(false);
    UpdateActionButtons(false);
}

void ReminderRequest::UpdateNotificationStateForSnooze()
{
    ANSR_LOGD("UpdateNotification content and buttons");
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
        case (TimeTransferType::WEEK): {  // week
            int sunDay = 7;
            if (cTime == 0) {
                return sunDay;
            } else {
                return cTime;
            }
        }
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
        case (TimeTransferType::WEEK): {  // week
            int sunDay = 7;
            if (actualTime == sunDay) {
                return 0;
            } else {
                return actualTime;
            }
        }
        default:
            return -1;
    }
}

int32_t ReminderRequest::GetUid(const int &userId, const std::string &bundleName)
{
    AppExecFwk::ApplicationInfo info;
    sptr<ISystemAbilityManager> systemAbilityManager
        = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject  = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    sptr<AppExecFwk::IBundleMgr> bundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    bundleMgr->GetApplicationInfo(bundleName, AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO,
        static_cast<int32_t>(userId), info);
    return static_cast<int32_t>(info.uid);
}

int ReminderRequest::GetUserId(const int &uid)
{
    int userId = -1;
    AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(uid, userId);
    return userId;
}

void ReminderRequest::AppendValuesBucket(const sptr<ReminderRequest> &reminder,
    const sptr<NotificationBundleOption> &bundleOption, NativeRdb::ValuesBucket &values)
{
    values.PutInt(REMINDER_ID, reminder->GetReminderId());
    values.PutString(PKG_NAME, bundleOption->GetBundleName());
    values.PutInt(USER_ID, reminder->GetUserId());
    values.PutInt(UID, reminder->GetUid());
    values.PutString(APP_LABEL, "");  // no use, compatible with old version.
    values.PutInt(REMINDER_TYPE, static_cast<int>(reminder->GetReminderType()));
    values.PutLong(REMINDER_TIME, reminder->GetReminderTimeInMilli());
    values.PutLong(TRIGGER_TIME, reminder->GetTriggerTimeInMilli());
    values.PutLong(
        RTC_TRIGGER_TIME, reminder->GetTriggerTimeInMilli());  // no use, compatible with old version.
    values.PutLong(TIME_INTERVAL, reminder->GetTimeInterval());
    values.PutInt(SNOOZE_TIMES, reminder->GetSnoozeTimes());
    values.PutInt(DYNAMIC_SNOOZE_TIMES, reminder->GetSnoozeTimesDynamic());
    values.PutLong(RING_DURATION, reminder->GetRingDuration());
    values.PutString(IS_EXPIRED, reminder->IsExpired() ? "true" : "false");
    values.PutString(IS_ACTIVE, "");  // no use, compatible with old version.
    values.PutInt(STATE, reminder->GetState());
    values.PutString(ZONE_ID, "");  // no use, compatible with old version.
    values.PutString(HAS_SCHEDULED_TIMEOUT, "");  // no use, compatible with old version.
    values.PutString(ACTION_BUTTON_INFO, reminder->GetButtonInfo());
    values.PutInt(SLOT_ID, reminder->GetSlotType());
    values.PutInt(NOTIFICATION_ID, reminder->GetNotificationId());
    values.PutString(TITLE, reminder->GetTitle());
    values.PutString(CONTENT, reminder->GetContent());
    values.PutString(SNOOZE_CONTENT, reminder->GetSnoozeContent());
    values.PutString(EXPIRED_CONTENT, reminder->GetExpiredContent());
    auto wantAgentInfo = reminder->GetWantAgentInfo();
    if (wantAgentInfo == nullptr) {
        std::string info = "null" + ReminderRequest::SEP_WANT_AGENT + "null";
        values.PutString(AGENT, info);
    } else {
        values.PutString(AGENT, wantAgentInfo->pkgName
            + ReminderRequest::SEP_WANT_AGENT + wantAgentInfo->abilityName);
    }
    auto maxScreenWantAgentInfo = reminder->GetMaxScreenWantAgentInfo();
    if (maxScreenWantAgentInfo == nullptr) {
        std::string info = "null" + ReminderRequest::SEP_WANT_AGENT + "null";
        values.PutString(MAX_SCREEN_AGENT, info);
    } else {
        values.PutString(MAX_SCREEN_AGENT, maxScreenWantAgentInfo->pkgName
            + ReminderRequest::SEP_WANT_AGENT + maxScreenWantAgentInfo->abilityName);
    }
}

void ReminderRequest::InitDbColumns()
{
    AddColumn(REMINDER_ID, "INTEGER PRIMARY KEY", false);
    AddColumn(PKG_NAME, "TEXT NOT NULL", false);
    AddColumn(USER_ID, "INT NOT NULL", false);
    AddColumn(UID, "INT NOT NULL", false);
    AddColumn(APP_LABEL, "TEXT", false);
    AddColumn(REMINDER_TYPE, "INT NOT NULL", false);
    AddColumn(REMINDER_TIME, "BIGINT NOT NULL", false);
    AddColumn(TRIGGER_TIME, "BIGINT NOT NULL", false);
    AddColumn(RTC_TRIGGER_TIME, "BIGINT NOT NULL", false);
    AddColumn(TIME_INTERVAL, "BIGINT NOT NULL", false);
    AddColumn(SNOOZE_TIMES, "INT NOT NULL", false);
    AddColumn(DYNAMIC_SNOOZE_TIMES, "INT NOT NULL", false);
    AddColumn(RING_DURATION, "BIGINT NOT NULL", false);
    AddColumn(IS_EXPIRED, "TEXT NOT NULL", false);
    AddColumn(IS_ACTIVE, "TEXT NOT NULL", false);
    AddColumn(STATE, "INT NOT NULL", false);
    AddColumn(ZONE_ID, "TEXT", false);
    AddColumn(HAS_SCHEDULED_TIMEOUT, "TEXT", false);
    AddColumn(ACTION_BUTTON_INFO, "TEXT", false);
    AddColumn(SLOT_ID, "INT", false);
    AddColumn(NOTIFICATION_ID, "INT NOT NULL", false);
    AddColumn(TITLE, "TEXT", false);
    AddColumn(CONTENT, "TEXT", false);
    AddColumn(SNOOZE_CONTENT, "TEXT", false);
    AddColumn(EXPIRED_CONTENT, "TEXT", false);
    AddColumn(AGENT, "TEXT", false);
    AddColumn(MAX_SCREEN_AGENT, "TEXT", false);
}

void ReminderRequest::AddColumn(
    const std::string &name, const std::string &type, const bool &isEnd)
{
    columns.push_back(name);
    if (!isEnd) {
        sqlOfAddColumns += name + " " + type + ", ";
    } else {
        sqlOfAddColumns += name + " " + type;
    }
}
}
}
