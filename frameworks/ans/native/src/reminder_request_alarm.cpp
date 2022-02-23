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

#include "reminder_request_alarm.h"

namespace OHOS {
namespace Notification {
const uint8_t ReminderRequestAlarm::DAYS_PER_WEEK = 7;
const uint8_t ReminderRequestAlarm::MONDAY = 1;
const uint8_t ReminderRequestAlarm::SUNDAY = 7;
const uint8_t ReminderRequestAlarm::HOURS_PER_DAY = 24;
const uint16_t ReminderRequestAlarm::SECONDS_PER_HOUR = 3600;
const uint8_t ReminderRequestAlarm::MINUTES_PER_HOUR = 60;
const int8_t ReminderRequestAlarm::INVALID_INT_VALUE = -1;
const int8_t ReminderRequestAlarm::DEFAULT_SNOOZE_TIMES = 3;

ReminderRequestAlarm::ReminderRequestAlarm(uint8_t hour, uint8_t minute, const std::vector<uint8_t> daysOfWeek)
    : ReminderRequest(ReminderRequest::ReminderType::ALARM)
{
    SetSnoozeTimes(DEFAULT_SNOOZE_TIMES);
    hour_ = hour;
    minute_ = minute;
    CheckParamValid();
    SetDaysOfWeek(true, daysOfWeek);
    SetTriggerTimeInMilli(GetNextTriggerTime(true));
}

ReminderRequestAlarm::ReminderRequestAlarm(const ReminderRequestAlarm &other) : ReminderRequest(other)
{
    this->hour_ = other.hour_;
    this->minute_ = other.minute_;
    this->repeatDays_ = other.repeatDays_;
    ANSR_LOGD("hour_=%{public}d, minute_=%{public}d, repeatDays_=%{public}d", hour_, minute_, repeatDays_);
}

void ReminderRequestAlarm::CheckParamValid() const
{
    if (hour_ >= HOURS_PER_DAY || hour_ < 0) {
        ANSR_LOGE("setted hour is not between [0, 24)");
        throw std::invalid_argument("setted hour is not between [0, 24)");
    }
    if (minute_ < 0 || minute_ >= MINUTES_PER_HOUR) {
        ANSR_LOGE("setted minute is not between [0, 60)");
        throw std::invalid_argument("setted minute is not between [0, 60)");
    }
}

bool ReminderRequestAlarm::IsRepeatReminder() const
{
    if ((repeatDays_ != 0) || ((GetTimeInterval() > 0) && (GetSnoozeTimes() > 0))) {
        return true;
    } else {
        return false;
    }
}

void ReminderRequestAlarm::SetDaysOfWeek(bool set, std::vector<uint8_t> daysOfWeek)
{
    if (daysOfWeek.size() == 0) {
        return;
    }
    if (daysOfWeek.size() > DAYS_PER_WEEK) {
        ANSR_LOGE("The length of daysOfWeek should not larger than 7");
        throw std::invalid_argument("The length of daysOfWeek should not larger than 7");
    }
    for (std::vector<uint8_t>::iterator it = daysOfWeek.begin(); it != daysOfWeek.end(); ++it) {
        if (*it < MONDAY || *it > SUNDAY) {
            continue;
        }
        if (set) {
            repeatDays_ |= 1 << (*it - 1);
        } else {
            repeatDays_ &= ~(1 << (*it - 1));
        }
    }
}

uint64_t ReminderRequestAlarm::PreGetNextTriggerTimeIgnoreSnooze(bool ignoreRepeat, bool forceToGetNext) const
{
    if (ignoreRepeat || (repeatDays_)) {
        return GetNextTriggerTime(forceToGetNext);
    } else {
        return INVALID_LONG_LONG_VALUE;
    }
}

uint64_t ReminderRequestAlarm::GetNextTriggerTime(bool forceToGetNext) const
{
    time_t now;
    (void)time(&now);  // unit is seconds.
    struct tm *nowTime = localtime(&now);
    if (nowTime == nullptr) {
        ANSR_LOGW("Get local time fail.");
        return 0;
    }

    ANSR_LOGD("Now: year=%{public}d, mon=%{public}d, day=%{public}d, hour=%{public}d, "
        "min=%{public}d, sec=%{public}d, week=%{public}d, Target: tar_hour=%{public}d, tar_min=%{public}d",
        GetActualTime(TimeTransferType::YEAR, nowTime->tm_year),
        GetActualTime(TimeTransferType::MONTH, nowTime->tm_mon),
        nowTime->tm_mday, nowTime->tm_hour, nowTime->tm_min, nowTime->tm_sec,
        GetActualTime(TimeTransferType::WEEK, nowTime->tm_wday), hour_, minute_);

    struct tm tar;
    tar.tm_year = nowTime->tm_year;
    tar.tm_mon = nowTime->tm_mon;
    tar.tm_mday = nowTime->tm_mday;
    tar.tm_hour = hour_;
    tar.tm_min = minute_;
    tar.tm_sec = 0;

    const time_t target = mktime(&tar);
    int8_t nextDayInterval = GetNextAlarm(now, target);
    time_t nextTriggerTime = 0;
    if (nextDayInterval == INVALID_INT_VALUE) {
        if (now >= target) {
            if (forceToGetNext) {
                nextTriggerTime = target + 1 * HOURS_PER_DAY * SECONDS_PER_HOUR;
            }
        } else {
            nextTriggerTime = target;
        }
    } else {
        nextTriggerTime = target + nextDayInterval * HOURS_PER_DAY * SECONDS_PER_HOUR;
    }
    struct tm *test = localtime(&nextTriggerTime);
    if (test == nullptr) {
        return 0;
    }
    ANSR_LOGI("NextTriggerTime: year=%{public}d, mon=%{public}d, day=%{public}d, hour=%{public}d, "
        "min=%{public}d, sec=%{public}d, week=%{public}d, nextTriggerTime=%{public}lld",
        GetActualTime(TimeTransferType::YEAR, test->tm_year),
        GetActualTime(TimeTransferType::MONTH, test->tm_mon),
        test->tm_mday, test->tm_hour, test->tm_min, test->tm_sec,
        GetActualTime(TimeTransferType::WEEK, test->tm_wday), (long long)nextTriggerTime);

    if (static_cast<int64_t>(nextTriggerTime) <= 0) {
        return 0;
    }
    return ReminderRequest::GetDurationSinceEpochInMilli(nextTriggerTime);
}

int8_t ReminderRequestAlarm::GetNextAlarm(const time_t now, const time_t target) const
{
    if (repeatDays_ == 0) {
        return INVALID_INT_VALUE;
    }
    int today = GetActualTime(TimeTransferType::WEEK, gmtime(&now)->tm_wday);
    int dayCount = now >= target ? 1 : 0;
    for (; dayCount <= DAYS_PER_WEEK; dayCount++) {
        int day = (today + dayCount) % DAYS_PER_WEEK;
        day = (day == 0) ? SUNDAY : day;
        if (IsRepeatDay(day)) {
            break;
        }
    }
    ANSR_LOGI("NextDayInterval is %{public}d", dayCount);
    return dayCount;
}

bool ReminderRequestAlarm::IsRepeatDay(int day) const
{
    return (repeatDays_ & (1 << (day - 1))) > 0;
}

std::vector<int> ReminderRequestAlarm::GetDaysOfWeek() const
{
    std::vector<int> repeatDays;
    int days[] = {1, 2, 3, 4, 5, 6, 7};
    int len = sizeof(days) / sizeof(int);
    for (int i = 0; i < len; i++) {
        if (IsRepeatDay(days[i])) {
            repeatDays.push_back(days[i]);
        }
    }
    return repeatDays;
}

uint8_t ReminderRequestAlarm::GetHour() const
{
    return hour_;
}

uint8_t ReminderRequestAlarm::GetMinute() const
{
    return minute_;
}

uint8_t ReminderRequestAlarm::GetRepeatDay() const
{
    return repeatDays_;
}

bool ReminderRequestAlarm::OnDateTimeChange()
{
    return ReminderRequest::OnDateTimeChange();
}

bool ReminderRequestAlarm::OnTimeZoneChange()
{
    return ReminderRequest::OnTimeZoneChange();
}

bool ReminderRequestAlarm::UpdateNextReminder()
{
    ANSR_LOGD("UpdateNextReminder alarm time");
    if (IsRepeatReminder()) {
        uint8_t letfSnoozeTimes = GetSnoozeTimesDynamic();
        if ((letfSnoozeTimes > 0) && (GetTimeInterval() > 0)) {
            ANSR_LOGI("Left times: %{public}d, update next triggerTime", GetSnoozeTimesDynamic());
            SetTriggerTimeInMilli(GetTriggerTimeInMilli() + GetTimeInterval() * MILLI_SECONDS);
            SetSnoozeTimesDynamic(--letfSnoozeTimes);
        } else {
            SetSnoozeTimesDynamic(GetSnoozeTimes());
            if (repeatDays_ == 0) {
                ANSR_LOGI("No need to update next triggerTime");
                SetExpired(true);
                return false;
            }
            uint64_t nextTriggerTime = GetNextTriggerTime(true);
            if (nextTriggerTime != INVALID_LONG_LONG_VALUE) {
                ANSR_LOGI("Set next trigger time successful, reset dynamic snoozeTimes");
                SetTriggerTimeInMilli(nextTriggerTime);
            } else {
                ANSR_LOGW("Set reminder to expired");
                SetExpired(true);
                return false;
            }
        }
        return true;
    } else {
        ANSR_LOGD("Single time reminder, not need to update next trigger time");
        SetSnoozeTimesDynamic(GetSnoozeTimes());
        SetExpired(true);
        return false;
    }
}

bool ReminderRequestAlarm::Marshalling(Parcel &parcel) const
{
    ReminderRequest::Marshalling(parcel);

    // write int
    if (!parcel.WriteUint8(hour_)) {
        ANSR_LOGE("Failed to write hour");
        return false;
    }
    if (!parcel.WriteUint8(minute_)) {
        ANSR_LOGE("Failed to write minute");
        return false;
    }
    if (!parcel.WriteUint8(repeatDays_)) {
        ANSR_LOGE("Failed to write daysOfWeek");
        return false;
    }
    return true;
}

ReminderRequestAlarm *ReminderRequestAlarm::Unmarshalling(Parcel &parcel)
{
    ANSR_LOGD("New alarm");
    auto objptr = new (std::nothrow) ReminderRequestAlarm();
    if ((objptr != nullptr) && !objptr->ReadFromParcel(parcel)) {
        delete objptr;
        objptr = nullptr;
    }
    return objptr;
}

bool ReminderRequestAlarm::ReadFromParcel(Parcel &parcel)
{
    ReminderRequest::ReadFromParcel(parcel);

    // read int
    if (!parcel.ReadUint8(hour_)) {
        ANSR_LOGE("Failed to read hour");
        return false;
    }
    if (!parcel.ReadUint8(minute_)) {
        ANSR_LOGE("Failed to read minute");
        return false;
    }
    if (!parcel.ReadUint8(repeatDays_)) {
        ANSR_LOGE("Failed to read repeatDays");
        return false;
    }
    ANSR_LOGD("hour_=%{public}d, minute_=%{public}d, repeatDays_=%{public}d", hour_, minute_, repeatDays_);
    return true;
}
}
}