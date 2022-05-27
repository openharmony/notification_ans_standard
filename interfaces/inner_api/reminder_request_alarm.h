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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_REQUEST_ALARM_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_REQUEST_ALARM_H

#include "reminder_request.h"

#include <vector>

namespace OHOS {
namespace Notification {
class ReminderRequestAlarm : public ReminderRequest {
public:
    /**
     * @brief A {@link ReminderRequest} child class used for creating reminders of alarm clocks.
     * You can use this class to publish alarm reminders at a specified time (accurate to minute) on a
     * particular day or on particular days every week.
     *
     * @note The params must meet the following conditions,
     * otherwise the application may crash due to an illegal parameter exception.
     *
     * @param hour The value must between [0, 23].
     * @param minute The value must between [0, 59].
     * @param daysOfWeek The value must between [1, 7], and the length of array can not be greater than 7.
     *
     * @see ReminderRequestTimer
     */
    ReminderRequestAlarm(uint8_t hour, uint8_t minute, std::vector<uint8_t> daysOfWeek);

    /**
     * @brief This constructor should only be used in background proxy service process
     * when reminder instance recovery from database.
     *
     * @param reminderId Indicates reminder id.
     */
    explicit ReminderRequestAlarm(int32_t reminderId) : ReminderRequest(reminderId) {};

    /**
     * @brief Copy construct from an exist reminder.
     *
     * @param Indicates the exist alarm reminder.
     */
    explicit ReminderRequestAlarm(const ReminderRequestAlarm &other);
    ReminderRequestAlarm& operator = (const ReminderRequestAlarm &other);
    ~ReminderRequestAlarm() override {};

    /**
     * Obtains the repeat days vector.
     *
     * @return vector of repeat days.
     */
    std::vector<int32_t> GetDaysOfWeek() const;

    /**
     * @brief Obtains the setted {@link hour_}.
     *
     * @return setted hour.
     */
    uint8_t GetHour() const;

    /**
     * @brief Obtains the setted {@link minute_}.
     *
     * @return setted minute.
     */
    uint8_t GetMinute() const;
    uint8_t GetRepeatDay() const;
    virtual bool OnDateTimeChange() override;
    virtual bool OnTimeZoneChange() override;
    virtual bool UpdateNextReminder() override;

    /**
     * Marshal a reminder object into a Parcel.
     *
     * @param parcel Indicates the Parcel.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * Unmarshal object from a Parcel.
     *
     * @param parcel Indicates the Parcel.
     * @return reminder object.
     */
    static ReminderRequestAlarm *Unmarshalling(Parcel &parcel);

    /**
     * Unmarshal unique properties of alarm from a Parcel.
     *
     * @param parcel Indicates the Parcel.
     * @return true if read parcel success.
     */
    bool ReadFromParcel(Parcel &parcel) override;
    virtual void RecoverFromDb(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet) override;
    static void AppendValuesBucket(const sptr<ReminderRequest> &reminder,
        const sptr<NotificationBundleOption> &bundleOption, NativeRdb::ValuesBucket &values);

    // For database recovery.
    static void InitDbColumns();

protected:
    virtual uint64_t PreGetNextTriggerTimeIgnoreSnooze(bool ignoreRepeat, bool forceToGetNext) const override;

private:
    ReminderRequestAlarm() : ReminderRequest() {};
    void CheckParamValid() const;

    /**
     * Obtains the next trigger time.
     *
     * @param forceToGetNext Indicates whether force to get next reminder.
     *                       When set the alarm firstly, you should set force with true, so if repeat information
     *                       is not set, and the target time is overdue, the reminder will be set to next day.
     *                       When change the time manually by user, you should set force with false, so if repeat
     *                       information is not set, and target time is overdue, the reminder will not be set to
     *                       next day.
     * @return next trigger time in milli.
     */
    uint64_t GetNextTriggerTime(bool forceToGetNext) const;

    /**
     * Judge is it the repeat day setted by user or not.
     *
     * @param day Indicates the day of week.
     * @return true if it is a repeat day.
     */
    bool IsRepeatDay(int32_t day) const;
    bool IsRepeatReminder() const;
    void SetDaysOfWeek(bool set, std::vector<uint8_t> daysOfWeek);

    /**
     * Obtains the next day interval if it is a week repeat alarm.
     *
     * @param now Indicates current time.
     * @param now Indicatet target time.
     * @return next day interval. Returns {@link INVALID_INT_VALUE} if it is not a week repeat alarm.
     */
    int8_t GetNextAlarm(const time_t now, const time_t target) const;

    static const uint8_t DAYS_PER_WEEK;
    static const uint8_t MONDAY;
    static const uint8_t SUNDAY;
    static const uint8_t HOURS_PER_DAY;
    static const uint16_t SECONDS_PER_HOUR;
    static const uint8_t MINUTES_PER_HOUR;
    static const int8_t INVALID_INT_VALUE;
    static const int8_t DEFAULT_SNOOZE_TIMES;

    uint8_t hour_ = {0};
    uint8_t minute_ = {0};
    uint8_t repeatDays_ = {0};

    // For database recovery.
    static const std::string REPEAT_DAYS_OF_WEEK;
    static const std::string ALARM_HOUR;
    static const std::string ALARM_MINUTE;
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_REQUEST_ALARM_H