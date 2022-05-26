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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_HELPER_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_HELPER_H

#include <vector>

#include "notification_slot.h"
#include "reminder_request.h"

namespace OHOS {
namespace Notification {
class ReminderHelper {
public:
    /**
     * Publishes a scheduled reminder.
     *
     * Third-party applications can call this method to publish a scheduled reminder. After this method is called,
     * the timing and pop-up notification functions of the calling application will be performed by the system service
     * agent in the background, even when the application is frozen or exits. You can call the
     * ReminderRequest::SetWantAgentInfo(WantAgentInfo wantAgentInfo) method to specify whether to start the
     * application after the pop-up notification is taped.
     *
     * The background agent maintains an ordered list of reminders for all third-party applications. The list is
     * updated based on the scheduled trigger time of each reminder. The system starts only one most recent scheduled
     * reminder at a time. If a reminder with a more recent trigger time is added, the new scheduled reminder will
     * be put on top of the list. The next reminder is triggered only after the current reminder is complete.
     *
     * @note One application can create a maximum of 30 valid reminders, and the total number of valid reminders
     * in the system cannot exceed 2000. The minimum snooze interval for a reminder is 5 minutes.
     *
     * @param reminder Indicates the reminder instance to publish. This parameter cannot be null. Otherwise,
     *                 an exception will be thrown due to invalid parameters, causing the application to crash.
     * @return Returns publish reminder result.
     *                 Reminder id will be set with a number >= 0 if publishing the reminder successfully, Otherwise
     *                 reminder id is -1. You can call reminder.GetReminderId() to get the reminder id.
     */
    static ErrCode PublishReminder(ReminderRequest &reminder);

    /**
     * Cancels a specified reminder.
     *
     * @param reminderId Indicates the ID of the reminder instance to cancel.
     * @return Returns cancel reminder result.
     */
    static ErrCode CancelReminder(const int32_t reminderId);

    /**
     * Cancels all reminders of current third part application.
     *
     * @return Returns cancel all reminders result.
     */
    static ErrCode CancelAllReminders();

    /**
     * Obtains all valid reminder notifications set by the current application, namely, the reminders that will
     * still be triggered later. If a reminder will never be triggered again, it is not considered a valid reminder.
     *
     * @param[out] validReminders Indicates an initial vector to receive the result.
     * @return Returns an array list containing all valid reminder notifications set by the current application.
     */
    static ErrCode GetValidReminders(std::vector<sptr<ReminderRequest>> &validReminders);

    /**
     * Creates a NotificationSlot.
     *
     * After a notification slot is created by using this method, only the name and description of the notification
     * slot can be changed. Changes to the other attributes, such as the vibration status and notification tone,
     * will no longer take effect.
     *
     * You can call the ReminderRequest#setSlotId(String) method to bind the slot for publishing a reminder.
     * When the application is uninstalled, all notification slots related to the application will be deleted.
     *
     * @param slot Indicates the NotificationSlot instance to add.
     * @return Returns add notification slot result.
     */
    static ErrCode AddNotificationSlot(const NotificationSlot &slot);

    /**
     * Removes a NotificationSlot instance used by the reminder.
     *
     * @param slotType Indicates the type of the slot, which is set when calling AddNotificationSlot to add a slot.
     * @return Returns remove notification slot result.
     */
    static ErrCode RemoveNotificationSlot(const NotificationConstant::SlotType &slotType);

private:
    static bool CheckPermission();
};
}  // namespace Reminder
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_HELPER_H