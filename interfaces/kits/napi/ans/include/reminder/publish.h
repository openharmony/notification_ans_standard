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
#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_REMINDER_PUBLISH_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_REMINDER_PUBLISH_H

#include "reminder/reminder_common.h"

namespace OHOS {
namespace ReminderAgentNapi {
/**
 * @brief Cancels a reminder.
 *
 * @param env Indicates the context.
 * @param info Indicates the opaque datatype about the context.
 * @return opaque pointer that is used to represent a JavaScript value
 */
napi_value CancelReminder(napi_env env, napi_callback_info info);

/**
 * @brief Cancels all reminders of current bundle.
 *
 * @param env Indicates the context.
 * @param info Indicates the opaque datatype about the context.
 * @return opaque pointer that is used to represent a JavaScript value
 */
napi_value CancelAllReminders(napi_env env, napi_callback_info info);

/**
 * @brief Obtains all reminders of current bundle.
 *
 * @param env Indicates the context.
 * @param info Indicates the opaque datatype about the context.
 * @return opaque pointer that is used to represent a JavaScript value
 */
napi_value GetValidReminders(napi_env env, napi_callback_info info);

/**
 * @brief Publishes a reminder.
 *
 * @param env Indicates the context.
 * @param info Indicates the opaque datatype about the context.
 * @return opaque pointer that is used to represent a JavaScript value
 */
napi_value PublishReminder(napi_env env, napi_callback_info info);

/**
 * @brief Adds a slot type.
 *
 * @param env Indicates the context.
 * @param info Indicates the opaque datatype about the context.
 * @return opaque pointer that is used to represent a JavaScript value.
 */
napi_value AddSlot(napi_env env, napi_callback_info info);
}  // namespace ReminderAgentNapi
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_REMINDER_PUBLISH_H
