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
#include "common.h"
#include "reminder_request_alarm.h"
#include "reminder_request_calendar.h"
#include "reminder_request_timer.h"

#include "reminder/reminder_common.h"

namespace OHOS {
namespace ReminderAgentNapi {
using namespace OHOS::Notification;

napi_value ReminderCommon::GetReminderRequest(
    const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder)
{
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    if (valuetype != napi_object) {
        ANSR_LOGW("Wrong argument type. Object expected.");
        return nullptr;
    }

    // gen reminder
    if (GenReminder(env, value, reminder) == nullptr) {
        return nullptr;
    }
    return NotificationNapi::Common::NapiGetNull(env);
}

bool ReminderCommon::GenActionButtons(
    const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder)
{
    char str[NotificationNapi::STR_MAX_SIZE] = {0};
    napi_valuetype valuetype = napi_undefined;
    napi_value actionButtons = nullptr;
    if (!GetObject(env, value, ReminderAgentNapi::ACTION_BUTTON, actionButtons)) {
        return true;
    }
    bool isArray = false;
    napi_is_array(env, actionButtons, &isArray);
    if (!isArray) {
        ANSR_LOGW("Wrong argument type:%{public}s. array expected.", ACTION_BUTTON);
        return false;
    }

    uint32_t length = 0;
    napi_get_array_length(env, actionButtons, &length);
    for (size_t i = 0; i < length; i++) {
        napi_value actionButton = nullptr;
        napi_get_element(env, actionButtons, i, &actionButton);
        NAPI_CALL(env, napi_typeof(env, actionButton, &valuetype));
        if (valuetype != napi_object) {
            ANSR_LOGW("Wrong element type:%{public}s. object expected.", ACTION_BUTTON);
            return false;
        }

        int32_t buttonType = static_cast<int32_t>(ReminderRequest::ActionButtonType::INVALID);
        if (GetStringUtf8(env, actionButton,
            ReminderAgentNapi::ACTION_BUTTON_TITLE, str, NotificationNapi::STR_MAX_SIZE) &&
            GetInt32(env, actionButton, ReminderAgentNapi::ACTION_BUTTON_TYPE, buttonType, false)) {
            if (ReminderRequest::ActionButtonType(buttonType) != ReminderRequest::ActionButtonType::CLOSE &&
                ReminderRequest::ActionButtonType(buttonType) != ReminderRequest::ActionButtonType::SNOOZE) {
                ANSR_LOGW("Wrong argument type:%{public}s. buttonType not support.", ACTION_BUTTON);
                return false;
            }
            std::string title(str);
            reminder->SetActionButton(title, static_cast<ReminderRequest::ActionButtonType>(buttonType));
            ANSR_LOGD("button title=%{public}s, type=%{public}d", title.c_str(), buttonType);
        } else {
            ANSR_LOGW("Parse action button error.");
            return false;
        }
    }
    return true;
}

void ReminderCommon::GenWantAgent(
    const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder)
{
    char str[NotificationNapi::STR_MAX_SIZE] = {0};
    napi_value wantAgent = nullptr;
    if (GetObject(env, value, ReminderAgentNapi::WANT_AGENT, wantAgent)) {
        auto wantAgentInfo = std::make_shared<ReminderRequest::WantAgentInfo>();
        if (GetStringUtf8(env, wantAgent, ReminderAgentNapi::WANT_AGENT_PKG, str, NotificationNapi::STR_MAX_SIZE)) {
            wantAgentInfo->pkgName = str;
        }
        if (GetStringUtf8(env, wantAgent,
            ReminderAgentNapi::WANT_AGENT_ABILITY, str, NotificationNapi::STR_MAX_SIZE)) {
            wantAgentInfo->abilityName = str;
        }
        reminder->SetWantAgentInfo(wantAgentInfo);
    }
}

void ReminderCommon::GenMaxScreenWantAgent(
    const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder)
{
    char str[NotificationNapi::STR_MAX_SIZE] = {0};
    napi_value maxScreenWantAgent = nullptr;
    if (GetObject(env, value, ReminderAgentNapi::MAX_SCREEN_WANT_AGENT, maxScreenWantAgent)) {
        std::shared_ptr<ReminderRequest::MaxScreenAgentInfo> maxScreenWantAgentInfo(
            new ReminderRequest::MaxScreenAgentInfo());
        if (GetStringUtf8(env, maxScreenWantAgent,
            ReminderAgentNapi::MAX_SCREEN_WANT_AGENT_PKG, str, NotificationNapi::STR_MAX_SIZE)) {
            maxScreenWantAgentInfo->pkgName = str;
        }
        if (GetStringUtf8(env, maxScreenWantAgent,
            ReminderAgentNapi::MAX_SCREEN_WANT_AGENT_ABILITY, str, NotificationNapi::STR_MAX_SIZE)) {
            maxScreenWantAgentInfo->abilityName = str;
        }
        reminder->SetMaxScreenWantAgentInfo(maxScreenWantAgentInfo);
    }
}
bool ReminderCommon::CreateReminder(
    const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder)
{
    napi_value result = nullptr;
    napi_get_named_property(env, value, ReminderAgentNapi::REMINDER_TYPE, &result);
    int32_t reminderType = -1;
    napi_get_value_int32(env, result, &reminderType);
    switch (ReminderRequest::ReminderType(reminderType)) {
        case ReminderRequest::ReminderType::TIMER:
            CreateReminderTimer(env, value, reminder);
            break;
        case ReminderRequest::ReminderType::ALARM:
            CreateReminderAlarm(env, value, reminder);
            break;
        case ReminderRequest::ReminderType::CALENDAR:
            CreateReminderCalendar(env, value, reminder);
            break;
        default:
            ANSR_LOGW("Reminder type is not support. (type:%{public}d)", reminderType);
            break;
    }
    if (reminder == nullptr) {
        ANSR_LOGW("Instance of reminder error.");
        return false;
    }
    return true;
}

napi_value ReminderCommon::GenReminder(
    const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder)
{
    // reminderType
    bool hasProperty = false;
    NAPI_CALL(env, napi_has_named_property(env, value, ReminderAgentNapi::REMINDER_TYPE, &hasProperty));
    if (!hasProperty) {
        ANSR_LOGW("Property %{public}s expected.", ReminderAgentNapi::REMINDER_TYPE);
        return nullptr;
    }

    // createReminder
    if (!CreateReminder(env, value, reminder)) {
        return nullptr;
    }
    char str[NotificationNapi::STR_MAX_SIZE] = {0};

    // title
    if (GetStringUtf8(env, value, ReminderAgentNapi::TITLE, str, NotificationNapi::STR_MAX_SIZE)) {
        reminder->SetTitle(str);
    }

    // content
    if (GetStringUtf8(env, value, ReminderAgentNapi::CONTENT, str, NotificationNapi::STR_MAX_SIZE)) {
        reminder->SetContent(str);
    }

    // expiredContent
    if (GetStringUtf8(env, value, ReminderAgentNapi::EXPIRED_CONTENT, str, NotificationNapi::STR_MAX_SIZE)) {
        reminder->SetExpiredContent(str);
    }

    // snoozeContent
    if (GetStringUtf8(env, value, ReminderAgentNapi::SNOOZE_CONTENT, str, NotificationNapi::STR_MAX_SIZE)) {
        reminder->SetSnoozeContent(str);
    }

    // ringDuration
    int64_t propVal = 0;
    if (GetInt64(env, value, ReminderAgentNapi::RING_DURATION, propVal)) {
        if (propVal < 0) {
            reminder->SetRingDuration(0);
        } else {
            uint64_t ringDuration = static_cast<uint64_t>(propVal);
            reminder->SetRingDuration(ringDuration);
        }
    }

    // timeInterval
    if (GetInt64(env, value, ReminderAgentNapi::TIME_INTERVAL, propVal)) {
        if (propVal < 0) {
            reminder->SetTimeInterval(0);
        } else {
            uint64_t timeInterval = static_cast<uint64_t>(propVal);
            reminder->SetTimeInterval(timeInterval);
        }
    }

    // notificationId
    int32_t propertyVal = 0;
    if (GetInt32(env, value, ReminderAgentNapi::NOTIFICATION_ID, propertyVal, false)) {
        reminder->SetNotificationId(propertyVal);
    }

    // snoozeTimes
    if (GetInt32(env, value, ReminderAgentNapi::SNOOZE_TIMES, propertyVal, false)) {
        if (propertyVal < 0) {
            reminder->SetSnoozeTimes(0);
        } else {
            uint8_t snoozeTimes = propertyVal > UINT8_MAX ? UINT8_MAX : propertyVal;
            reminder->SetSnoozeTimes(static_cast<uint8_t>(snoozeTimes));
        }
    }

    // slotType
    int32_t slotType = 0;
    if (GetInt32(env, value, ReminderAgentNapi::SLOT_TYPE, slotType, false)) {
        enum NotificationConstant::SlotType actureType = NotificationConstant::SlotType::OTHER;
        if (!NotificationNapi::Common::SlotTypeJSToC(NotificationNapi::SlotType(slotType), actureType)) {
            ANSR_LOGW("slot type not support.");
            return nullptr;
        }
        reminder->SetSlotType(actureType);
    }

    // wantAgent
    GenWantAgent(env, value, reminder);

    // maxScreenWantAgent
    GenMaxScreenWantAgent(env, value, reminder);

    // actionButtons
    if (!GenActionButtons(env, value, reminder)) {
        return nullptr;
    }
    return NotificationNapi::Common::NapiGetNull(env);
}

bool ReminderCommon::GetStringUtf8(const napi_env &env, const napi_value &value,
    const char* propertyName, char* propertyVal, const int32_t size)
{
    bool hasProperty = false;
    napi_value result = nullptr;
    napi_valuetype valuetype = napi_undefined;
    size_t strLen = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, propertyName, &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, propertyName, &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        if (valuetype != napi_string) {
            ANSR_LOGW("Wrong argument type:%{public}s. string expected.", propertyName);
            return false;
        }
        NAPI_CALL(env, napi_get_value_string_utf8(env, result, propertyVal, size - 1, &strLen));
    }
    return hasProperty;
}

bool ReminderCommon::GetInt32(const napi_env &env, const napi_value &value,
    const char* propertyName, int32_t& propertyVal, bool isNecessary)
{
    napi_value result = nullptr;
    if (!GetPropertyValIfExist(env, value, propertyName, result)) {
        if (isNecessary) {
            ANSR_LOGW("Correct property %{public}s expected.", propertyName);
        }
        return false;
    }
    napi_get_value_int32(env, result, &propertyVal);
    return true;
}

bool ReminderCommon::GetInt64(const napi_env &env, const napi_value &value,
    const char* propertyName, int64_t& propertyVal)
{
    napi_value result = nullptr;
    if (!GetPropertyValIfExist(env, value, propertyName, result)) {
        return false;
    }
    napi_get_value_int64(env, result, &propertyVal);
    return true;
}

bool ReminderCommon::GetPropertyValIfExist(const napi_env &env, const napi_value &value,
    const char* propertyName, napi_value& propertyVal)
{
    napi_valuetype valuetype = napi_undefined;
    if (propertyName == nullptr) {
        propertyVal = value;
    } else {
        bool hasProperty = false;
        NAPI_CALL(env, napi_has_named_property(env, value, propertyName, &hasProperty));
        if (!hasProperty) {
            return false;
        }
        napi_get_named_property(env, value, propertyName, &propertyVal);
    }
    NAPI_CALL(env, napi_typeof(env, propertyVal, &valuetype));
    if (valuetype != napi_number) {
        if (propertyName == nullptr) {
            ANSR_LOGW("Wrong argument type. number expected.");
        } else {
            ANSR_LOGW("Wrong argument type:%{public}s, number expected.", propertyName);
        }
        return false;
    }
    return true;
}

bool ReminderCommon::GetObject(const napi_env &env, const napi_value &value,
    const char* propertyName, napi_value& propertyVal)
{
    bool hasProperty = false;
    napi_valuetype valuetype = napi_undefined;

    NAPI_CALL(env, napi_has_named_property(env, value, propertyName, &hasProperty));
    if (!hasProperty) {
        return false;
    }
    napi_get_named_property(env, value, propertyName, &propertyVal);
    NAPI_CALL(env, napi_typeof(env, propertyVal, &valuetype));
    if (valuetype != napi_object) {
        ANSR_LOGW("Wrong argument type:%{public}s. object expected.", propertyName);
        return false;
    }
    return true;
}

napi_value ReminderCommon::CreateReminderTimer(
    const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder)
{
    int64_t propertyCountDownTime = 0;
    if (!GetInt64(env, value, ReminderAgentNapi::TIMER_COUNT_DOWN_TIME, propertyCountDownTime)) {
        ANSR_LOGW("Correct property %{public}s expected.", ReminderAgentNapi::TIMER_COUNT_DOWN_TIME);
        return nullptr;
    }

    if (propertyCountDownTime <= 0) {
        ANSR_LOGW("Create countDown reminder fail: designated %{public}s should be set larger than 0.",
            ReminderAgentNapi::TIMER_COUNT_DOWN_TIME);
        return nullptr;
    }

    reminder = std::make_shared<ReminderRequestTimer>(static_cast<uint64_t>(propertyCountDownTime));
    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value ReminderCommon::CreateReminderAlarm(
    const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder)
{
    // hour
    int32_t propertyHourVal = 0;
    if (!GetInt32(env, value, ReminderAgentNapi::ALARM_HOUR, propertyHourVal, true)) {
        return nullptr;
    }

    // minute
    int32_t propertyMinuteVal = 0;
    if (!GetInt32(env, value, ReminderAgentNapi::ALARM_MINUTE, propertyMinuteVal, true)) {
        return nullptr;
    }
    if (propertyHourVal < 0 || propertyHourVal > 23) {
        ANSR_LOGW("Create alarm reminder fail: designated %{public}s must between [0, 23].",
            ReminderAgentNapi::ALARM_HOUR);
        return nullptr;
    }
    if (propertyMinuteVal < 0 || propertyMinuteVal > 59) {
        ANSR_LOGW("Create alarm reminder fail: designated %{public}s must between [0, 59].",
            ReminderAgentNapi::ALARM_MINUTE);
        return nullptr;
    }

    // daysOfWeek
    std::vector<uint8_t> daysOfWeek;
    uint8_t maxDaysOfWeek = 7;
    if (ParseInt32Array(env, value, ReminderAgentNapi::ALARM_DAYS_OF_WEEK, daysOfWeek, maxDaysOfWeek) == nullptr) {
        return nullptr;
    }
    reminder = std::make_shared<ReminderRequestAlarm>(
        static_cast<uint8_t>(propertyHourVal), static_cast<uint8_t>(propertyMinuteVal), daysOfWeek);
    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value ReminderCommon::CreateReminderCalendar(
    const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder)
{
    napi_value dateTimeObj = nullptr;
    if (!GetObject(env, value, ReminderAgentNapi::CALENDAR_DATE_TIME, dateTimeObj)) {
        ANSR_LOGW("Create calender reminder fail: dateTime must be setted.");
        return nullptr;
    }

    // year month day hour minute second
    int32_t propertyYearVal = 0;
    int32_t propertyMonthVal = 0;
    int32_t propertyDayVal = 0;
    int32_t propertyHourVal = 0;
    int32_t propertyMinteVal = 0;
    if (!GetInt32(env, dateTimeObj, ReminderAgentNapi::CALENDAR_YEAR, propertyYearVal, true)
        || !GetInt32(env, dateTimeObj, ReminderAgentNapi::CALENDAR_MONTH, propertyMonthVal, true)
        || !GetInt32(env, dateTimeObj, ReminderAgentNapi::CALENDAR_DAY, propertyDayVal, true)
        || !GetInt32(env, dateTimeObj, ReminderAgentNapi::CALENDAR_HOUR, propertyHourVal, true)
        || !GetInt32(env, dateTimeObj, ReminderAgentNapi::CALENDAR_MINUTE, propertyMinteVal, true)) {
        return nullptr;
    }
    if (!CheckCalendarParams(propertyYearVal, propertyMonthVal, propertyDayVal,
        propertyHourVal, propertyMinteVal)) {
        return nullptr;
    }

    // repeatMonth
    std::vector<uint8_t> repeatMonths;
    if (ParseInt32Array(env, value, ReminderAgentNapi::CALENDAR_REPEAT_MONTHS, repeatMonths,
        ReminderRequestCalendar::MAX_MONTHS_OF_YEAR) == nullptr) {
        return nullptr;
    }

    // repeatDay
    std::vector<uint8_t> repeatDays;
    if (ParseInt32Array(env, value, ReminderAgentNapi::CALENDAR_REPEAT_DAYS, repeatDays,
        ReminderRequestCalendar::MAX_DAYS_OF_MONTH) == nullptr) {
        return nullptr;
    }

    tm dateTime;
    dateTime.tm_year = ReminderRequest::GetCTime(ReminderRequest::TimeTransferType::YEAR, propertyYearVal);
    dateTime.tm_mon = ReminderRequest::GetCTime(ReminderRequest::TimeTransferType::MONTH, propertyMonthVal);
    dateTime.tm_mday = propertyDayVal;
    dateTime.tm_hour = propertyHourVal;
    dateTime.tm_min = propertyMinteVal;
    dateTime.tm_sec = 0;
    reminder = std::make_shared<ReminderRequestCalendar>(dateTime, repeatMonths, repeatDays);
    return NotificationNapi::Common::NapiGetNull(env);
}

bool ReminderCommon::CheckCalendarParams(const int32_t &year, const int32_t &month, const int32_t &day,
    const int32_t &hour, const int32_t &min)
{
    if (year < 0 || year > UINT16_MAX) {
        ANSR_LOGW("Create calender reminder fail: designated %{public}s must between [0, %{public}u]",
            ReminderAgentNapi::CALENDAR_YEAR, UINT16_MAX);
        return false;
    }
    if (month < 1 || month > ReminderRequestCalendar::MAX_MONTHS_OF_YEAR) {
        ANSR_LOGW("Create calender reminder fail: designated %{public}s must between [1, %{public}u]",
            ReminderAgentNapi::CALENDAR_MONTH, ReminderRequestCalendar::MAX_MONTHS_OF_YEAR);
        return false;
    }
    uint8_t maxDaysOfMonth = ReminderRequestCalendar::GetDaysOfMonth(static_cast<uint16_t>(year), month);
    if (day < 1 || day > maxDaysOfMonth) {
        ANSR_LOGW("Create calender reminder fail: designated %{public}s must between [1, %{public}u]",
            ReminderAgentNapi::CALENDAR_DAY, maxDaysOfMonth);
        return false;
    }
    return true;
}

napi_value ReminderCommon::ParseInt32Array(const napi_env &env, const napi_value &value,
    const char* propertyName, std::vector<uint8_t> &propertyVal, uint8_t maxLen)
{
    napi_value result = nullptr;
    if (!GetObject(env, value, propertyName, result)) {
        return NotificationNapi::Common::NapiGetNull(env);
    }
    if (result != nullptr) {
        bool isArray = false;
        napi_is_array(env, result, &isArray);
        if (!isArray) {
            ANSR_LOGW("Property %{public}s is expected to be an array.", propertyName);
            return nullptr;
        }
        uint32_t length = 0;
        napi_get_array_length(env, result, &length);
        if (length > maxLen) {
            ANSR_LOGW("The max length of array of %{public}s is %{public}d.", propertyName, maxLen);
            return nullptr;
        }
        napi_valuetype valuetype = napi_undefined;
        for (size_t i = 0; i < length; i++) {
            int32_t propertyDayVal = 10;
            napi_value repeatDayVal = nullptr;
            napi_get_element(env, result, i, &repeatDayVal);
            NAPI_CALL(env, napi_typeof(env, repeatDayVal, &valuetype));
            if (valuetype != napi_number) {
                ANSR_LOGW("%{public}s's element is expected to be number.", propertyName);
                return nullptr;
            }
            napi_get_value_int32(env, repeatDayVal, &propertyDayVal);
            if (propertyDayVal < 1 || propertyDayVal > maxLen) {
                ANSR_LOGW("%{public}s's element must between [1, %{public}d].", propertyName, maxLen);
                return nullptr;
            }
            propertyVal.push_back(static_cast<uint8_t>(propertyDayVal));
        }
    }
    return NotificationNapi::Common::NapiGetNull(env);
}
}
}
