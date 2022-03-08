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

#include "reminder_request_calendar.h"

#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
const uint8_t ReminderRequestCalendar::MAX_MONTHS_OF_YEAR = 12;
const uint8_t ReminderRequestCalendar::MAX_DAYS_OF_MONTH = 31;
const uint8_t ReminderRequestCalendar::JANUARY = 1;
const uint8_t ReminderRequestCalendar::DECEMBER = 12;
const uint8_t ReminderRequestCalendar::DEFAULT_SNOOZE_TIMES = 3;

// For database recovery.
const std::string ReminderRequestCalendar::REPEAT_DAYS = "repeat_days";
const std::string ReminderRequestCalendar::REPEAT_MONTHS = "repeat_months";
const std::string ReminderRequestCalendar::FIRST_DESIGNATE_YEAR = "first_designate_year";
const std::string ReminderRequestCalendar::FIRST_DESIGNATE_MONTH = "first_designate_month";
const std::string ReminderRequestCalendar::FIRST_DESIGNATE_DAY = "first_designate_day";
const std::string ReminderRequestCalendar::CALENDAR_YEAR = "calendar_year";
const std::string ReminderRequestCalendar::CALENDAR_MONTH = "calendar_month";
const std::string ReminderRequestCalendar::CALENDAR_DAY = "calendar_day";
const std::string ReminderRequestCalendar::CALENDAR_HOUR = "calendar_hour";
const std::string ReminderRequestCalendar::CALENDAR_MINUTE = "calendar_minute";

ReminderRequestCalendar::ReminderRequestCalendar(const tm &dateTime,
    const std::vector<uint8_t> &repeatMonths, const std::vector<uint8_t> &repeatDays)
    : ReminderRequest(ReminderRequest::ReminderType::CALENDAR)
{
    // 1. record the information which designated by user at first time.
    firstDesignateYear_ = static_cast<uint16_t>(GetActualTime(TimeTransferType::YEAR, dateTime.tm_year));
    firstDesignateMonth_ = static_cast<uint8_t>(GetActualTime(TimeTransferType::MONTH, dateTime.tm_mon));
    firstDesignateDay_ = dateTime.tm_mday;
    SetRepeatMonths(repeatMonths);
    SetRepeatDaysOfMonth(repeatDays);
    SetSnoozeTimes(DEFAULT_SNOOZE_TIMES);

    // 2. get the latest valid trigger time.
    InitDateTime(dateTime);
    hour_ = static_cast<uint8_t>(dateTime_.tm_hour);
    minute_ = static_cast<uint8_t>(dateTime_.tm_min);
    uint64_t nextTriggerTime = INVALID_LONG_LONG_VALUE;
    if ((nextTriggerTime = GetNextTriggerTime()) != INVALID_LONG_LONG_VALUE) {
        time_t target = static_cast<time_t>(nextTriggerTime / MILLI_SECONDS);
        (void)localtime_r(&target, &dateTime_);
    } else {
        ANSR_LOGW("Not exist next trigger time, please check the param of ReminderRequestCalendar constructor.");
        throw std::invalid_argument(
            "Not exist next trigger time, please check the param of ReminderRequestCalendar constructor.");
    }

    // 2. set the time information (used to transfer to proxy service) which is decided to trigger firstly.
    year_ = static_cast<uint16_t>(GetActualTime(TimeTransferType::YEAR, dateTime_.tm_year));
    month_ = static_cast<uint8_t>(GetActualTime(TimeTransferType::MONTH, dateTime_.tm_mon));
    day_ = dateTime_.tm_mday;
    second_ = 0;
    SetTriggerTimeInMilli(nextTriggerTime);
}

ReminderRequestCalendar::ReminderRequestCalendar(const ReminderRequestCalendar &other) : ReminderRequest(other)
{
    dateTime_ = other.dateTime_;
    firstDesignateYear_ = other.firstDesignateYear_;
    firstDesignateMonth_ = other.firstDesignateMonth_;
    firstDesignateDay_ = other.firstDesignateDay_;
    year_ = other.year_;
    month_ = other.month_;
    day_ = other.day_;
    hour_ = other.hour_;
    minute_ = other.minute_;
    second_ = other.second_;
    repeatMonth_ = other.repeatMonth_;
    repeatDay_ = other.repeatDay_;
}

uint8_t ReminderRequestCalendar::GetDaysOfMonth(const uint16_t &year, const uint8_t &month)
{
    uint8_t days;
    uint8_t daysArray[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t february = 2;
    uint8_t leapMonth = 29;
    uint8_t nonLeapMonth = 28;
    uint16_t solarYear = 400;
    uint8_t leapParam1 = 4;
    uint8_t leapParam2 = 100;
    if (month == february) {
        days = ((((year % leapParam1 == 0) && (year % leapParam2 != 0)) || (year % solarYear == 0))
            ? leapMonth : nonLeapMonth);
    } else {
        days = daysArray[month - 1];
    }
    return days;
}

uint8_t ReminderRequestCalendar::GetNextDay(
    const uint16_t &settedYear, const uint8_t &settedMonth, const tm &now, const tm &target) const
{
    uint32_t repeatDayTmp = repeatDay_;
    uint8_t daysOfSpecialMonth = GetDaysOfMonth(settedYear, settedMonth);
    uint8_t setDayTmp = INVALID_U8_VALUE;
    for (uint8_t i = 1; i <= daysOfSpecialMonth; i++) {
        if ((repeatDayTmp & (1 << (i - 1))) > 0) {
            struct tm setTime;
            setTime.tm_year = GetCTime(TimeTransferType::YEAR, settedYear);
            setTime.tm_mon = GetCTime(TimeTransferType::MONTH, settedMonth);
            setTime.tm_mday = i;
            setTime.tm_hour = target.tm_hour;
            setTime.tm_min = target.tm_min;
            setTime.tm_sec = target.tm_sec;
            setTime.tm_isdst = -1;

            struct tm nowTime;
            nowTime.tm_year = now.tm_year;
            nowTime.tm_mon = now.tm_mon;
            nowTime.tm_mday = now.tm_mday;
            nowTime.tm_hour = now.tm_hour;
            nowTime.tm_min = now.tm_min;
            nowTime.tm_sec = now.tm_sec;
            nowTime.tm_isdst = -1;

            if (mktime(&nowTime) >= mktime(&setTime)) {
                continue;
            } else {
                setDayTmp = i;
                return setDayTmp;
            }
        }
    }
    return setDayTmp;
}

uint64_t ReminderRequestCalendar::GetNextTriggerTime() const
{
    uint64_t triggerTimeInMilli = INVALID_LONG_LONG_VALUE;
    time_t now;
    (void)time(&now);  // unit is seconds.
    struct tm nowTime;
    (void)localtime_r(&now, &nowTime);
    nowTime.tm_sec = 0;
    struct tm tarTime;
    tarTime.tm_year = GetCTime(TimeTransferType::YEAR, firstDesignateYear_);
    tarTime.tm_mon = GetCTime(TimeTransferType::MONTH, firstDesignateMonth_);
    tarTime.tm_mday = firstDesignateDay_;
    tarTime.tm_hour = hour_;
    tarTime.tm_min = minute_;
    tarTime.tm_sec = 0;
    tarTime.tm_isdst = -1;

    ANSR_LOGD("Now time is: %{public}s", GetDateTimeInfo(now).c_str());
    if (!(repeatMonth_ > 0 && repeatDay_ > 0)) {
        ANSR_LOGD("tarTime: %{public}d-%{public}d-%{public}d %{public}d:%{public}d:%{public}d",
            tarTime.tm_year, tarTime.tm_mon, tarTime.tm_mday, tarTime.tm_hour, tarTime.tm_min, tarTime.tm_sec);
        const time_t target = mktime(&tarTime);
        if (target == -1) {
            ANSR_LOGW("mktime return error.");
        }
        if (now < target) {
            triggerTimeInMilli = ReminderRequest::GetDurationSinceEpochInMilli(target);
            ANSR_LOGD("Next calendar time:%{public}s", GetDateTimeInfo(target).c_str());
        }
        return triggerTimeInMilli;
    }
    triggerTimeInMilli = GetNextTriggerTimeAsRepeatReminder(nowTime, tarTime);
    return triggerTimeInMilli;
}

uint64_t ReminderRequestCalendar::GetNextTriggerTimeAsRepeatReminder(const tm &nowTime, const tm &tarTime) const
{
    uint64_t triggerTimeInMilli = INVALID_LONG_LONG_VALUE;
    uint16_t setYear = static_cast<uint16_t>(GetActualTime(TimeTransferType::YEAR, nowTime.tm_year));
    uint8_t setMonth = INVALID_U8_VALUE;
    uint8_t setDay = INVALID_U8_VALUE;
    uint8_t beginMonth = static_cast<uint8_t>(GetActualTime(TimeTransferType::MONTH, nowTime.tm_mon));
    uint8_t count = 1;
    uint16_t repeatMonthTmp = repeatMonth_;
    for (uint8_t i = beginMonth; i < (MAX_MONTHS_OF_YEAR + beginMonth + 1); i++) {
        if ((repeatMonthTmp & (1 << ((i - 1) % MAX_MONTHS_OF_YEAR))) > 0) {
            setMonth = (i % MAX_MONTHS_OF_YEAR);
            setMonth = setMonth == 0 ? DECEMBER : setMonth;
            if (count != 1) {
                setYear = setMonth <= beginMonth ? setYear + 1 : setYear;
            }
            setDay = GetNextDay(setYear, setMonth, nowTime, tarTime);
        }
        if (setDay != INVALID_U8_VALUE) {
            break;
        }
        count++;
    }
    if ((triggerTimeInMilli = GetTimeInstantMilli(setYear, setMonth, setDay, hour_, minute_, second_))
        != INVALID_LONG_LONG_VALUE) {
        ANSR_LOGD("Next calendar time:%{public}hu/%{public}hhu/%{public}hhu %{public}hhu:%{public}hhu:%{public}hhu",
            setYear, setMonth, setDay, hour_, minute_, second_);
    }
    return triggerTimeInMilli;
}

uint64_t ReminderRequestCalendar::GetTimeInstantMilli(
    uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) const
{
    struct tm tar;
    tar.tm_year = GetCTime(TimeTransferType::YEAR, year);
    tar.tm_mon =  GetCTime(TimeTransferType::MONTH, month);
    tar.tm_mday = day;
    tar.tm_hour = hour;
    tar.tm_min = minute;
    tar.tm_sec = second;
    tar.tm_isdst = -1;

    ANSR_LOGD("tar: %{public}d-%{public}d-%{public}d %{public}d:%{public}d:%{public}d",
        tar.tm_year, tar.tm_mon, tar.tm_mday, tar.tm_hour, tar.tm_min, tar.tm_sec);
    const time_t target = mktime(&tar);
    if (target == -1) {
        ANSR_LOGW("mktime return error.");
        return INVALID_LONG_LONG_VALUE;
    }
    return ReminderRequest::GetDurationSinceEpochInMilli(target);
}

void ReminderRequestCalendar::InitDateTime()
{
    dateTime_.tm_year = GetCTime(TimeTransferType::YEAR, year_);
    dateTime_.tm_mon = GetCTime(TimeTransferType::MONTH, month_);
    dateTime_.tm_mday = day_;
    dateTime_.tm_hour = hour_;
    dateTime_.tm_min = minute_;
    dateTime_.tm_sec = second_;
    dateTime_.tm_isdst = -1;
}

void ReminderRequestCalendar::InitDateTime(const tm &dateTime)
{
    dateTime_.tm_year = dateTime.tm_year;
    dateTime_.tm_mon = dateTime.tm_mon;
    dateTime_.tm_mday = dateTime.tm_mday;
    dateTime_.tm_hour = dateTime.tm_hour;
    dateTime_.tm_min = dateTime.tm_min;
    dateTime_.tm_sec = dateTime.tm_sec;
    dateTime_.tm_isdst = -1;
}

bool ReminderRequestCalendar::IsRepeatReminder() const
{
    return (repeatMonth_ > 0 && repeatDay_ > 0) || (GetTimeInterval() > 0 && GetSnoozeTimes() > 0);
}


bool ReminderRequestCalendar::IsRepeatMonth(uint8_t month) const
{
    return (repeatMonth_ & (1 << (month - 1))) > 0;
}

bool ReminderRequestCalendar::IsRepeatDay(uint8_t day) const
{
    return (repeatDay_ & (1 << (day - 1))) > 0;
}

void ReminderRequestCalendar::SetDay(const uint8_t &day, const bool &isSet)
{
    if (day < 1 || day > MAX_DAYS_OF_MONTH) {
        return;
    }
    if (isSet) {
        repeatDay_ |= 1 << (day - 1);
    } else {
        repeatDay_ &= ~(1 << (day - 1));
    }
}

void ReminderRequestCalendar::SetMonth(const uint8_t &month, const bool &isSet)
{
    if (month < JANUARY || month > DECEMBER) {
        return;
    }
    if (isSet) {
        repeatMonth_ |= 1 << (month - 1);
    } else {
        repeatMonth_ &= ~ (1 << (month - 1));
    }
}

void ReminderRequestCalendar::SetRepeatMonths(const std::vector<uint8_t> &repeatMonths)
{
    if (repeatMonths.size() > MAX_MONTHS_OF_YEAR) {
        ANSR_LOGW("The length of repeat months array should not larger than %{public}hhu", MAX_MONTHS_OF_YEAR);
        throw std::invalid_argument(
            "The length of repeat months array should not larger than " + std::to_string(MAX_MONTHS_OF_YEAR));
    }
    repeatMonth_ = 0;
    for (auto it = repeatMonths.begin(); it != repeatMonths.end(); ++it) {
        SetMonth((*it), true);
    }
}

void ReminderRequestCalendar::SetRepeatDaysOfMonth(const std::vector<uint8_t> &repeateDays)
{
    if (repeateDays.size() > MAX_DAYS_OF_MONTH) {
        ANSR_LOGW("The length of repeat days array should not larger than %{public}hhu", MAX_DAYS_OF_MONTH);
        throw std::invalid_argument(
            "The length of repeat days array should not larger than " + std::to_string(MAX_DAYS_OF_MONTH));
    }
    repeatDay_ = 0;
    for (auto it = repeateDays.begin(); it != repeateDays.end(); ++it) {
        SetDay((*it), true);
    }
}

std::vector<uint8_t> ReminderRequestCalendar::GetRepeatMonths() const
{
    std::vector<uint8_t> repeatMonths;
    for (int i = 0; i < MAX_MONTHS_OF_YEAR; i++) {
        if (IsRepeatMonth(i + 1)) {
            repeatMonths.push_back(i + 1);
        }
    }
    return repeatMonths;
}

std::vector<uint8_t> ReminderRequestCalendar::GetRepeatDays() const
{
    std::vector<uint8_t> repeatDays;
    for (int i = 0; i < MAX_DAYS_OF_MONTH; i++) {
        if (IsRepeatDay(i + 1)) {
            repeatDays.push_back(i + 1);
        }
    }
    return repeatDays;
}

bool ReminderRequestCalendar::OnDateTimeChange()
{
    return ReminderRequest::OnDateTimeChange();
}

bool ReminderRequestCalendar::OnTimeZoneChange()
{
    return ReminderRequest::OnTimeZoneChange();
}

bool ReminderRequestCalendar::UpdateNextReminder()
{
    ANSR_LOGD("UpdateNextReminder calendar time");
    if (!IsRepeatReminder()) {
        ANSR_LOGI("No need to update next trigger time as it is an one-time reminder.");
        SetSnoozeTimesDynamic(GetSnoozeTimes());
        SetExpired(true);
        return false;
    }
    uint8_t leftSnoozeTimes = GetSnoozeTimesDynamic();
    if (leftSnoozeTimes > 0 && (GetTimeInterval() > 0)) {
        ANSR_LOGI("Left snooze times: %{public}d, update next triggerTime", leftSnoozeTimes);
        SetTriggerTimeInMilli(GetTriggerTimeInMilli() + GetTimeInterval() * MILLI_SECONDS);
        SetSnoozeTimesDynamic(--leftSnoozeTimes);
    } else {
        SetSnoozeTimesDynamic(GetSnoozeTimes());
        if (repeatMonth_ == 0 || repeatDay_ == 0) {
            ANSR_LOGI("Not a day repeat reminder, no need to update to next trigger time.");
            SetExpired(true);
            return false;
        } else {
            uint64_t nextTriggerTime = GetNextTriggerTime();
            if (nextTriggerTime != INVALID_LONG_LONG_VALUE) {
                ANSR_LOGI("Set next trigger time successful, reset dynamic snoozeTimes");
                SetTriggerTimeInMilli(nextTriggerTime);
            } else {
                ANSR_LOGW("Set next trigger time invalidate");
                SetExpired(true);
                return false;
            }
        }
    }
    return true;
}

uint64_t ReminderRequestCalendar::PreGetNextTriggerTimeIgnoreSnooze(bool ignoreRepeat, bool forceToGetNext) const
{
    if (ignoreRepeat || (repeatMonth_ > 0 && repeatDay_ > 0)) {
        return GetNextTriggerTime();
    } else {
        return INVALID_LONG_LONG_VALUE;
    }
}

bool ReminderRequestCalendar::Marshalling(Parcel &parcel) const
{
    ReminderRequest::Marshalling(parcel);

    // write int
    if (!parcel.WriteUint16(year_)) {
        ANSR_LOGE("Failed to write year");
        return false;
    }
    if (!parcel.WriteUint8(month_)) {
        ANSR_LOGE("Failed to write month");
        return false;
    }
    if (!parcel.WriteUint8(day_)) {
        ANSR_LOGE("Failed to write day");
        return false;
    }
    if (!parcel.WriteUint8(hour_)) {
        ANSR_LOGE("Failed to write hour");
        return false;
    }
    if (!parcel.WriteUint8(minute_)) {
        ANSR_LOGE("Failed to write minute");
        return false;
    }
    if (!parcel.WriteUint8(second_)) {
        ANSR_LOGE("Failed to write second");
        return false;
    }
    if (!parcel.WriteUint16(repeatMonth_)) {
        ANSR_LOGE("Failed to write repeatMonth");
        return false;
    }
    if (!parcel.WriteUint32(repeatDay_)) {
        ANSR_LOGE("Failed to write repeateDay");
        return false;
    }
    if (!parcel.WriteUint16(firstDesignateYear_)) {
        ANSR_LOGE("Failed to write firstDesignateYear");
        return false;
    }
    if (!parcel.WriteUint8(firstDesignateMonth_)) {
        ANSR_LOGE("Failed to write firstDesignateMonth");
        return false;
    }
    if (!parcel.WriteUint8(firstDesignateDay_)) {
        ANSR_LOGE("Failed to write firstDesignateDay");
        return false;
    }
    return true;
}

ReminderRequestCalendar *ReminderRequestCalendar::Unmarshalling(Parcel &parcel)
{
    ANSR_LOGD("New calendar");
    auto objptr = new ReminderRequestCalendar();
    if (objptr == nullptr) {
        ANS_LOGE("Failed to create reminder calendar due to no memory.");
        return objptr;
    }
    if (!objptr->ReadFromParcel(parcel)) {
        delete objptr;
        objptr = nullptr;
    }
    return objptr;
}

bool ReminderRequestCalendar::ReadFromParcel(Parcel &parcel)
{
    ReminderRequest::ReadFromParcel(parcel);

    // read int
    if (!parcel.ReadUint16(year_)) {
        ANSR_LOGE("Failed to read year");
        return false;
    }
    if (!parcel.ReadUint8(month_)) {
        ANSR_LOGE("Failed to read month");
        return false;
    }
    if (!parcel.ReadUint8(day_)) {
        ANSR_LOGE("Failed to read day");
        return false;
    }
    if (!parcel.ReadUint8(hour_)) {
        ANSR_LOGE("Failed to read hour");
        return false;
    }
    if (!parcel.ReadUint8(minute_)) {
        ANSR_LOGE("Failed to read minute");
        return false;
    }
    if (!parcel.ReadUint8(second_)) {
        ANSR_LOGE("Failed to read second");
        return false;
    }
    if (!parcel.ReadUint16(repeatMonth_)) {
        ANSR_LOGE("Failed to read repeatMonth");
        return false;
    }
    if (!parcel.ReadUint32(repeatDay_)) {
        ANSR_LOGE("Failed to read repeateDay");
        return false;
    }
    InitDateTime();
    if (!parcel.ReadUint16(firstDesignateYear_)) {
        ANSR_LOGE("Failed to read firstDesignateYear");
        return false;
    }
    if (!parcel.ReadUint8(firstDesignateMonth_)) {
        ANSR_LOGE("Failed to read firstDesignateMonth");
        return false;
    }
    if (!parcel.ReadUint8(firstDesignateDay_)) {
        ANSR_LOGE("Failed to read firstDesignateDay");
        return false;
    }
    return true;
}

void ReminderRequestCalendar::RecoverFromDb(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{
    ReminderRequest::RecoverFromDb(resultSet);

    // repeatDay
    repeatDay_ = static_cast<uint32_t>(RecoverInt64FromDb(resultSet, REPEAT_DAYS, DbRecoveryType::INT));

    // repeatMonth
    repeatMonth_ =
        static_cast<uint16_t>(RecoverInt64FromDb(resultSet, REPEAT_MONTHS, DbRecoveryType::INT));

    // firstDesignateYear
    firstDesignateYear_ =
        static_cast<uint16_t>(RecoverInt64FromDb(resultSet, FIRST_DESIGNATE_YEAR, DbRecoveryType::INT));

    // firstDesignateMonth
    firstDesignateMonth_ =
        static_cast<uint8_t>(RecoverInt64FromDb(resultSet, FIRST_DESIGNATE_MONTH, DbRecoveryType::INT));

    // firstDesignateDay
    firstDesignateDay_ =
        static_cast<uint8_t>(RecoverInt64FromDb(resultSet, FIRST_DESIGNATE_DAY, DbRecoveryType::INT));

    // year
    year_ = static_cast<uint16_t>(RecoverInt64FromDb(resultSet, CALENDAR_YEAR, DbRecoveryType::INT));

    // month
    month_ = static_cast<uint8_t>(RecoverInt64FromDb(resultSet, CALENDAR_MONTH, DbRecoveryType::INT));

    // day
    day_ = static_cast<uint8_t>(RecoverInt64FromDb(resultSet, CALENDAR_DAY, DbRecoveryType::INT));

    // hour
    hour_ = static_cast<uint8_t>(RecoverInt64FromDb(resultSet, CALENDAR_HOUR, DbRecoveryType::INT));

    // minute
    minute_ = static_cast<uint8_t>(RecoverInt64FromDb(resultSet, CALENDAR_MINUTE, DbRecoveryType::INT));
}

void ReminderRequestCalendar::AppendValuesBucket(const sptr<ReminderRequest> &reminder,
    const sptr<NotificationBundleOption> &bundleOption, NativeRdb::ValuesBucket &values)
{
    uint32_t repeatDay = 0;
    uint16_t repeatMonth = 0;
    uint16_t firstDesignateYear = 0;
    uint8_t firstDesignateMonth = 0;
    uint8_t firstDesignateDay = 0;
    uint16_t year = 0;
    uint8_t month = 0;
    uint8_t day = 0;
    uint8_t hour = 0;
    uint8_t minute = 0;
    if (reminder->GetReminderType() == ReminderRequest::ReminderType::CALENDAR) {
        ReminderRequestCalendar* calendar = static_cast<ReminderRequestCalendar*>(reminder.GetRefPtr());
        repeatDay = calendar->GetRepeatDay();
        repeatMonth = calendar->GetRepeatMonth();
        firstDesignateYear = calendar->GetFirstDesignateYear();
        firstDesignateMonth = calendar->GetFirstDesignageMonth();
        firstDesignateDay = calendar->GetFirstDesignateDay();
        year = calendar->GetYear();
        month = calendar->GetMonth();
        day = calendar->GetDay();
        hour = calendar->GetHour();
        minute = calendar->GetMinute();
    }
    values.PutInt(REPEAT_DAYS, repeatDay);
    values.PutInt(REPEAT_MONTHS, repeatMonth);
    values.PutInt(FIRST_DESIGNATE_YEAR, firstDesignateYear);
    values.PutInt(FIRST_DESIGNATE_MONTH, firstDesignateMonth);
    values.PutInt(FIRST_DESIGNATE_DAY, firstDesignateDay);
    values.PutInt(CALENDAR_YEAR, year);
    values.PutInt(CALENDAR_MONTH, month);
    values.PutInt(CALENDAR_DAY, day);
    values.PutInt(CALENDAR_HOUR, hour);
    values.PutInt(CALENDAR_MINUTE, minute);
}

void ReminderRequestCalendar::InitDbColumns()
{
    ReminderRequest::AddColumn(REPEAT_DAYS, "INT", false);
    ReminderRequest::AddColumn(REPEAT_MONTHS, "INT", false);
    ReminderRequest::AddColumn(FIRST_DESIGNATE_YEAR, "INT", false);
    ReminderRequest::AddColumn(FIRST_DESIGNATE_MONTH, "INT", false);
    ReminderRequest::AddColumn(FIRST_DESIGNATE_DAY, "INT", false);
    ReminderRequest::AddColumn(CALENDAR_YEAR, "INT", false);
    ReminderRequest::AddColumn(CALENDAR_MONTH, "INT", false);
    ReminderRequest::AddColumn(CALENDAR_DAY, "INT", false);
    ReminderRequest::AddColumn(CALENDAR_HOUR, "INT", false);
    ReminderRequest::AddColumn(CALENDAR_MINUTE, "INT", false);
}
}
}