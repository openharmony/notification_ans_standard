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
#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_REMINDER_COMMON_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_REMINDER_COMMON_H

#include "ans_log_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "reminder_helper.h"
#include "reminder_request.h"

namespace OHOS {
namespace ReminderAgentNapi {
using namespace OHOS::Notification;

namespace {
const char* REMINDER_TYPE = "reminderType";
const char* ALARM_HOUR = "hour";
const char* ALARM_MINUTE = "minute";
const char* ALARM_DAYS_OF_WEEK = "daysOfWeek";
const char* TIMER_COUNT_DOWN_TIME = "triggerTimeInSeconds";
const char* ACTION_BUTTON = "actionButton";
const char* ACTION_BUTTON_TITLE = "title";
const char* ACTION_BUTTON_TYPE = "type";
const char* WANT_AGENT = "wantAgent";
const char* WANT_AGENT_PKG = "pkgName";
const char* WANT_AGENT_ABILITY = "abilityName";
const char* TITLE = "title";
const char* CONTENT = "content";
const char* EXPIRED_CONTENT = "expiredContent";
const char* NOTIFICATION_ID = "notificationId";
const char* SLOT_TYPE = "slotType";
}

class ReminderCommon {
    ReminderCommon();
    ~ReminderCommon();
    ReminderCommon(ReminderCommon &other) = delete;
    ReminderCommon& operator = (const ReminderCommon &other) = delete;

public:
    static napi_value GetReminderRequest(
        const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder);

    static bool GetStringUtf8(const napi_env &env, const napi_value &value,
        const char* propertyName, char* propertyVal, const int32_t size);

    static bool GetInt32(const napi_env &env, const napi_value &value,
        const char* propertyName, int32_t& propertyVal);

    static bool GetInt64(const napi_env &env, const napi_value &value,
        const char* propertyName, int64_t& propertyVal);

    static bool GetObject(const napi_env &env, const napi_value &value,
        const char* propertyName, napi_value& propertyVal);

private:
    static bool CreateReminder(const napi_env &env, const napi_value &value,
        const int32_t &reminderType, std::shared_ptr<ReminderRequest>& reminder);

    static bool GetPropertyValIfExist(const napi_env &env, const napi_value &value,
        const char* propertyName, napi_value& propertyVal);

    static void GenWantAgent(
        const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder);

    static bool GenActionButtons(
        const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder);

    static napi_value GenReminder(
        const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder);

    static napi_value CreateReminderTimer(
        const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder);

    static napi_value CreateReminderAlarm(
        const napi_env &env, const napi_value &value, std::shared_ptr<ReminderRequest>& reminder);

    static napi_value ParseDaysOfWeek(
        const napi_env &env, const napi_value &value, std::vector<uint8_t> &daysOfWeek);
};
}  // namespace OHOS
}  // namespace ReminderAgentNapi

#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_REMINDER_COMMON_H
