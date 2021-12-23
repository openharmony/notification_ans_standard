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
#include "reminder/publish.h"
#include "reminder_request_alarm.h"
#include "reminder_request_timer.h"

namespace OHOS {
namespace ReminderAgentNapi {
static const int32_t PUBLISH_PARAM_LEN = 2;
static const int32_t CANCEL_PARAM_LEN = 2;
static const int32_t CANCEL_ALL_PARAM_LEN = 1;
static const int32_t GET_VALID_PARAM_LEN = 1;

struct AsyncCallbackInfo {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    int32_t reminderId = -1;
    std::shared_ptr<ReminderRequest> reminder = nullptr;
    NotificationNapi::CallbackPromiseInfo info;
    napi_value result = nullptr;
};

struct Parameters {
    int32_t reminderId = -1;
    std::shared_ptr<ReminderRequest> reminder = nullptr;
    napi_ref callback = nullptr;
};

napi_value GetCallback(const napi_env &env, const napi_value &value, Parameters &params)
{
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    if (valuetype != napi_function) {
        REMINDER_LOGE("Wrong argument type. Function expected.");
        return nullptr;
    }
    napi_create_reference(env, value, 1, &params.callback);
    return NotificationNapi::Common::NapiGetNull(env);
}

AsyncCallbackInfo* SetAsynccallbackinfo(const napi_env &env, Parameters& params, napi_value& promise)
{
    AsyncCallbackInfo *asynccallbackinfo = new (std::nothrow) AsyncCallbackInfo {
            .env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        NotificationNapi::Common::JSParaError(env, params.callback);
        return nullptr;
    }
    NotificationNapi::Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);
    return asynccallbackinfo;
}

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, Parameters &params)
{
    size_t argc = PUBLISH_PARAM_LEN;
    napi_value argv[PUBLISH_PARAM_LEN] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if (argc < 1 || argc > PUBLISH_PARAM_LEN) {
        REMINDER_LOGE("Wrong number of arguments");
        return nullptr;
    }

    // argv[0] : reminderRequest
    if (ReminderCommon::GetReminderRequest(env, argv[0], params.reminder) == nullptr) {
        REMINDER_LOGE("[reminderAgent]CreateReminder returns nullptr");
        return nullptr;
    }

    // argv[1]: callback
    if (argc == PUBLISH_PARAM_LEN) {
        if (GetCallback(env, argv[1], params) == nullptr) {
            REMINDER_LOGE("[reminderAgent]GetCallbak returns nullptr");
            return nullptr;
        }
    }
    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value ParseCanCelParameter(const napi_env &env, const napi_callback_info &info, Parameters &params)
{
    REMINDER_LOGI("ParseCanCelParameter");
    size_t argc = CANCEL_PARAM_LEN;
    napi_value argv[CANCEL_PARAM_LEN] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if (argc < 1 || argc > CANCEL_PARAM_LEN) {
        REMINDER_LOGE("Wrong number of arguments");
        return nullptr;
    }

    // argv[0]: reminder id
    int32_t reminderId = -1;
    if (!ReminderCommon::GetInt32(env, argv[0], nullptr, reminderId)) {
        REMINDER_LOGE("Param id of cancels Reminder should be a number.");
        return nullptr;
    }
    if (reminderId < 0) {
        REMINDER_LOGE("Param id of cancels Reminder is illegal.");
        return nullptr;
    }
    params.reminderId = reminderId;

    // argv[1]: callback
    if (argc >= CANCEL_PARAM_LEN) {
        if (GetCallback(env, argv[1], params) == nullptr) {
            REMINDER_LOGE("GetCallbak is nullptr");
            return nullptr;
        }
    }
    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value ParseCanCelAllParameter(const napi_env &env, const napi_callback_info &info, Parameters &params)
{
    REMINDER_LOGI("ParseCanCelAllParameter");
    size_t argc = CANCEL_ALL_PARAM_LEN;
    napi_value argv[CANCEL_ALL_PARAM_LEN] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if (argc > CANCEL_ALL_PARAM_LEN) {
        REMINDER_LOGE("Wrong number of arguments");
        return nullptr;
    }

    // argv[0]: callback
    if (argc == CANCEL_ALL_PARAM_LEN) {
        if (GetCallback(env, argv[0], params) == nullptr) {
            REMINDER_LOGE("getCallbak is nullptr");
            return nullptr;
        }
    }
    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value ParseGetValidParameter(const napi_env &env, const napi_callback_info &info, Parameters &params)
{
    size_t argc = GET_VALID_PARAM_LEN;
    napi_value argv[GET_VALID_PARAM_LEN] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if (argc < 0 || argc > GET_VALID_PARAM_LEN) {
        REMINDER_LOGE("Wrong number of arguments");
        return nullptr;
    }

    // argv[0]: callback
    if (argc == GET_VALID_PARAM_LEN) {
        if (GetCallback(env, argv[0], params) == nullptr) {
            REMINDER_LOGI("getCallbak is nullptr");
            return nullptr;
        }
    }
    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value CancelReminder(napi_env env, napi_callback_info info) {
    REMINDER_LOGI("Cancel reminder");

    // param
    Parameters params;
    if (ParseCanCelParameter(env, info, params) == nullptr) {
        return NotificationNapi::Common::JSParaError(env, params.callback);
    }

    // promise
    napi_value promise = nullptr;
    AsyncCallbackInfo *asynccallbackinfo = SetAsynccallbackinfo(env, params, promise);
    if (!asynccallbackinfo) {
        return NotificationNapi::Common::JSParaError(env, params.callback);
    }
    asynccallbackinfo->reminderId = params.reminderId;

    // resource name
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "cancelReminder", NAPI_AUTO_LENGTH, &resourceName);

    // create and queue async work
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            REMINDER_LOGI("Cancel napi_create_async_work start");
            AsyncCallbackInfo *asynccallbackinfo = (AsyncCallbackInfo *)data;
            asynccallbackinfo->info.errorCode = ReminderHelper::CancelReminder(asynccallbackinfo->reminderId);
        },
        [](napi_env env, napi_status status, void *data) {
            REMINDER_LOGI("Cancel napi_create_async_work complete start");
            AsyncCallbackInfo *asynccallbackinfo = (AsyncCallbackInfo *)data;
            NotificationNapi::Common::ReturnCallbackPromise(
                env, asynccallbackinfo->info, NotificationNapi::Common::NapiGetNull(env));
            if (asynccallbackinfo->info.callback != nullptr) {
                napi_delete_reference(env, asynccallbackinfo->info.callback);
            }
            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
            REMINDER_LOGI("Cancel napi_create_async_work complete end");
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return NotificationNapi::Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value CancelAllReminders(napi_env env, napi_callback_info info)
{
    REMINDER_LOGI("Cancel all reminder");

    // param
    Parameters params;
    if (ParseCanCelAllParameter(env, info, params) == nullptr) {
        return NotificationNapi::Common::JSParaError(env, params.callback);
    }

    // promise
    napi_value promise = nullptr;
    AsyncCallbackInfo *asynccallbackinfo = SetAsynccallbackinfo(env, params, promise);
    if (!asynccallbackinfo) {
        return NotificationNapi::Common::JSParaError(env, params.callback);
    }

    // resource name
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "cancelAllReminders", NAPI_AUTO_LENGTH, &resourceName);

    // create and queue async work
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            REMINDER_LOGI("CancelAll napi_create_async_work start");
            AsyncCallbackInfo *asynccallbackinfo = (AsyncCallbackInfo *)data;
            asynccallbackinfo->info.errorCode = ReminderHelper::CancelAllReminders();
        },
        [](napi_env env, napi_status status, void *data) {
            REMINDER_LOGD("CancelAll napi_create_async_work complete start");
            AsyncCallbackInfo *asynccallbackinfo = (AsyncCallbackInfo *)data;
            NotificationNapi::Common::ReturnCallbackPromise(
                env, asynccallbackinfo->info, NotificationNapi::Common::NapiGetNull(env));
            if (asynccallbackinfo->info.callback != nullptr) {
                napi_delete_reference(env, asynccallbackinfo->info.callback);
            }
            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
            REMINDER_LOGD("CancelAll napi_create_async_work complete end");
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return NotificationNapi::Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

void ParseReminder(
    const napi_env &env, ReminderRequest::ReminderType &type, ReminderRequest &reminder, napi_value &result)
{
    napi_value value = nullptr;
    switch (type) {
        case ReminderRequest::ReminderType::TIMER: {
            ReminderRequestTimer& timer = (ReminderRequestTimer&)reminder;
            napi_create_uint32(env, timer.GetInitInfo(), &value);
            napi_set_named_property(env, result, TIMER_COUNT_DOWN_TIME, value);
            break;
        }
        case ReminderRequest::ReminderType::ALARM: {
            REMINDER_LOGD("Parse alarm info");

            // hour
            ReminderRequestAlarm& alarm = (ReminderRequestAlarm&)reminder;
            napi_create_uint32(env, static_cast<uint32_t>(alarm.GetHour()), &value);

            // minute
            napi_set_named_property(env, result, ALARM_HOUR, value);
            napi_create_uint32(env, static_cast<uint32_t>(alarm.GetMinute()), &value);
            napi_set_named_property(env, result, ALARM_MINUTE, value);

            // daysOfWeek
            napi_create_array(env, &value);
            napi_set_named_property(env, result, ALARM_DAYS_OF_WEEK, value);
            int count = 0;
            for (auto day : alarm.GetDaysOfWeek()) {
                if (day) {
                    napi_value napiDay = nullptr;
                    napi_create_int32(env, day, &napiDay);
                    napi_set_element(env, value, count, napiDay);
                    count++;
                }
            }
            REMINDER_LOGD("Parse alarm info end");
            break;
        }
        default: {
            break;
        }
    }
}

void ParseActionButtons(const napi_env &env, ReminderRequest &reminder, napi_value &result)
{
    auto actionButtonsMap = reminder.GetActionButtons();

    // create array
    napi_value array = nullptr;
    napi_create_array(env, &array);
    napi_set_named_property(env, result, ACTION_BUTTON, array);
    int index = 0;
    for (std::map<ReminderRequest::ActionButtonType, ReminderRequest::ActionButtonInfo>::iterator it
        = actionButtonsMap.begin(); it != actionButtonsMap.end(); ++it) {

        // create obj
        napi_value actionButton = nullptr;
        napi_create_object(env, &actionButton);

        napi_value buttonInfo = nullptr;
        napi_create_uint32(env, static_cast<int32_t>(it->second.type), &buttonInfo);
        napi_set_named_property(env, actionButton, ACTION_BUTTON_TYPE, buttonInfo);
        napi_create_string_utf8(env, (it->second.title).c_str(), NAPI_AUTO_LENGTH, &buttonInfo);
        napi_set_named_property(env, actionButton, ACTION_BUTTON_TITLE, buttonInfo);

        // add obj to array
        napi_set_element(env, array, index, actionButton);
        index++;
    }
}

void ParseWantAgent(const napi_env &env, ReminderRequest &reminder, napi_value &result)
{
    // create obj
    napi_value wantAgentInfo = nullptr;
    napi_create_object(env, &wantAgentInfo);
    napi_set_named_property(env, result, WANT_AGENT, wantAgentInfo);

    napi_value info = nullptr;
    napi_create_string_utf8(env, (reminder.GetWantAgentInfo()->pkgName).c_str(), NAPI_AUTO_LENGTH, &info);
    napi_set_named_property(env, wantAgentInfo, WANT_AGENT_PKG, info);
    napi_create_string_utf8(env, (reminder.GetWantAgentInfo()->abilityName).c_str(), NAPI_AUTO_LENGTH, &info);
    napi_set_named_property(env, wantAgentInfo, WANT_AGENT_ABILITY, info);
}

napi_value SetValidReminder(const napi_env &env, ReminderRequest &reminder, napi_value &result)
{
    REMINDER_LOGI("enter");
    napi_value value = nullptr;

    napi_create_string_utf8(env, reminder.Dump().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "reminder", value);

    // type
    ReminderRequest::ReminderType type = reminder.GetReminderType();
    napi_create_int32(env, static_cast<int32_t>(type), &value);
    napi_set_named_property(env, result, REMINDER_TYPE, value);

    // reminder
    ParseReminder(env, type, reminder, result);

    // title
    napi_create_string_utf8(env, reminder.GetTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, TITLE, value);

    // content
    napi_create_string_utf8(env, reminder.GetContent().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, CONTENT, value);

    // expiredContent
    napi_create_string_utf8(env, reminder.GetExpiredContent().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, EXPIRED_CONTENT, value);

    // notificationId
    napi_create_int32(env, reminder.GetNotificationId(), &value);
    napi_set_named_property(env, result, NOTIFICATION_ID, value);

    // slotType
    napi_create_int32(env, static_cast<int32_t>(reminder.GetSlotType()), &value);
    napi_set_named_property(env, result, SLOT_TYPE, value);

    // wantAgent
    ParseWantAgent(env, reminder, result);

    // actionButtons
    ParseActionButtons(env, reminder, result);

    return NotificationNapi::Common::NapiGetBoolean(env, true);
}

void GetValidRemindersInner(napi_env env, std::vector<sptr<ReminderRequest>>& validReminders, napi_value& arr)
{
    int count = 0;
    napi_create_array(env, &arr);
    for (auto reminder : validReminders) {
        if (reminder == nullptr) {
            REMINDER_LOGE("reminder is null");
            continue;
        }
        napi_value result = nullptr;
        napi_create_object(env, &result);
        if (!SetValidReminder(env, *reminder, result)) {
            REMINDER_LOGW("Set reminder object failed");
            continue;
        }
        napi_set_element(env, arr, count, result);
        count++;
    }
    REMINDER_LOGI("GetValid reminders count = %{public}d", count);
}

napi_value GetValidReminders(napi_env env, napi_callback_info info)
{
    REMINDER_LOGI("Get valid reminders");

    // param
    Parameters params;
    if (ParseGetValidParameter(env, info, params) == nullptr) {
        return NotificationNapi::Common::JSParaError(env, params.callback);
    }

    // promise
    napi_value promise = nullptr;
    AsyncCallbackInfo *asynccallbackinfo = SetAsynccallbackinfo(env, params, promise);
    if (!asynccallbackinfo) {
        return NotificationNapi::Common::JSParaError(env, params.callback);
    }

    // resource name
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getValidReminders", NAPI_AUTO_LENGTH, &resourceName);

    // create and start async work
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            REMINDER_LOGI("GetValid reminders napi_create_async_work start");
            AsyncCallbackInfo *asynccallbackinfo = (AsyncCallbackInfo *)data;
            std::vector<sptr<ReminderRequest>> validReminders;
            asynccallbackinfo->info.errorCode = ReminderHelper::GetValidReminders(validReminders);
            if (asynccallbackinfo->info.errorCode != ERR_OK) {
                asynccallbackinfo->result = NotificationNapi::Common::NapiGetNull(env);
                return;
            }
            napi_value arr = nullptr;
            GetValidRemindersInner(env, validReminders, arr);
            asynccallbackinfo->result = arr;
        },
        [](napi_env env, napi_status status, void *data) {
            REMINDER_LOGI("GetValid reminders napi_create_async_work complete start");
            AsyncCallbackInfo *asynccallbackinfo = (AsyncCallbackInfo *)data;
            NotificationNapi::Common::ReturnCallbackPromise(
                env, asynccallbackinfo->info, asynccallbackinfo->result);
            if (asynccallbackinfo->info.callback != nullptr) {
                napi_delete_reference(env, asynccallbackinfo->info.callback);
            }
            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
            REMINDER_LOGI("GetValid reminders napi_create_async_work complete end");
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return NotificationNapi::Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value PublishReminder(napi_env env, napi_callback_info info)
{
    REMINDER_LOGI("PublishReminder");

    // param
    Parameters params;
    if (ParseParameters(env, info, params) == nullptr) {
        REMINDER_LOGE("Parse params error");
        return NotificationNapi::Common::JSParaError(env, params.callback);
    }

    // promise
    napi_value promise = nullptr;
    AsyncCallbackInfo *asynccallbackinfo = SetAsynccallbackinfo(env, params, promise);
    if (!asynccallbackinfo) {
        return NotificationNapi::Common::JSParaError(env, params.callback);
    }
    asynccallbackinfo->reminder = params.reminder;

    // resource name
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "publishReminder", NAPI_AUTO_LENGTH, &resourceName);

    // create and start async work
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            REMINDER_LOGI("Publish napi_create_async_work start");
            AsyncCallbackInfo *asynccallbackinfo = (AsyncCallbackInfo *)data;
            asynccallbackinfo->info.errorCode = ReminderHelper::PublishReminder(*(asynccallbackinfo->reminder));
            REMINDER_LOGD("Return reminderId=%{public}d", asynccallbackinfo->reminder->GetReminderId());
        },
        [](napi_env env, napi_status status, void *data) {
            REMINDER_LOGI("Publish napi_create_async_work complete start");
            AsyncCallbackInfo *asynccallbackinfo = (AsyncCallbackInfo *)data;
            NotificationNapi::Common::ReturnCallbackPromise(
                env, asynccallbackinfo->info, NotificationNapi::Common::NapiGetNull(env));
            if (asynccallbackinfo->info.callback != nullptr) {
                napi_delete_reference(env, asynccallbackinfo->info.callback);
            }
            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
            REMINDER_LOGI("Publish napi_create_async_work complete end");
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return NotificationNapi::Common::NapiGetNull(env);
    } else {
        return promise;
    }
}
}
}