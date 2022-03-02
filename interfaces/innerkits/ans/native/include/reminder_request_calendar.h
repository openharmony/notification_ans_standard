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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_REQUEST_CALENDAR_H
#define BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_REQUEST_CALENDAR_H

#include "reminder_request.h"

namespace OHOS {
namespace Notification {
class ReminderRequestCalendar : public ReminderRequest {
public:
    /**
     * @brief A {@link ReminderRequest} child class used for creating reminders of calendar clocks.
     *
     * @note The params must meet the following conditions.
     * otherwise the application may crash due to an illegal parameter exception.
     * <ul>
     * <li> The length of repeatMonths vectory cannot exceed 12. </li>
     * <li> The length of repeateDays vectory cannot exceed 31. </li>
     * <li> There must be at least one valid reminder time. Ensure that the time specified by dateTime
     * does not expired, or repeatMonths and repeateDays are valid. </li>
     * </ul>
     *
     * The repeateMonths and repeatDays must both be set to implements repeated reminders.
     * By default, this reminder is not snoozed. You can call {@link SetTimeInterval} to
     * set the snooze interval.
     *
     * @param dateTime Indicates the date and time when this calendar event reminder will be triggered.
     *                 The time is accurate to minute. For example, the value
     *                 {@link LocalDateTime(2021, 3, 3, 16, 15)} indicates that the reminder will be
     *                 triggered at 16:15 on March 3, 2021.
     * @param repeatMonths Indicates the months in which this reminder will be repeated. For example,
     *                     the value {2, 4} indicates that the reminder will be triggered on particular
     *                     days in February and April.
     * @param repeatDays Indicates the days in a month when this reminder will be repeated. For example,
     *                   the value {2, 4} indicates that the reminder will be triggered on the second
     *                   and fourth day of the specific months.
     */
    ReminderRequestCalendar(const tm &dateTime,
        const std::vector<uint8_t> &repeatMonths, const std::vector<uint8_t> &repeatDays);

    /**
     * @brief This constructor should only be used in background proxy service process
     * when reminder instance recovery from database.
     *
     * @param reminderId Indicates reminder id.
     */
    ReminderRequestCalendar(int32_t reminderId) : ReminderRequest(reminderId) {};

    explicit ReminderRequestCalendar(const ReminderRequestCalendar &other);
    ReminderRequestCalendar& operator = (const ReminderRequestCalendar &other);
    ~ReminderRequestCalendar() override {}

    inline uint16_t GetYear() const
    {
        return year_;
    }

    inline uint8_t GetMonth() const
    {
        return month_;
    }

    inline uint8_t GetDay() const
    {
        return day_;
    }

    inline uint8_t GetHour() const
    {
        return hour_;
    }

    inline uint8_t GetMinute() const
    {
        return minute_;
    }

    inline uint8_t GetSecond() const
    {
        return second_;
    }

    inline uint16_t GetFirstDesignateYear() const
    {
        return firstDesignateYear_;
    }

    inline uint16_t GetFirstDesignageMonth() const
    {
        return firstDesignateMonth_;
    }

    inline uint16_t GetFirstDesignateDay() const
    {
        return firstDesignateDay_;
    }

    std::vector<uint8_t> GetRepeatMonths() const;
    std::vector<uint8_t> GetRepeatDays() const;

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
    static ReminderRequestCalendar *Unmarshalling(Parcel &parcel);

    /**
     * Unmarshal unique properties of alarm from a Parcel.
     *
     * @param parcel Indicates the Parcel.
     * @return true if read parcel success.
     */
    bool ReadFromParcel(Parcel &parcel) override;

    static const uint8_t MAX_MONTHS_OF_YEAR;
    static const uint8_t MAX_DAYS_OF_MONTH;
    virtual void RecoveryFromDb(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet) override;
    static void AppendValuesBucket(const sptr<ReminderRequest> &reminder,
        const sptr<NotificationBundleOption> &bundleOption, NativeRdb::ValuesBucket &values);
    static uint8_t GetDaysOfMonth(const uint16_t &year, const uint8_t &month);

class Instance {
public:
    const static std::string REPEAT_DAYS;
    const static std::string REPEAT_MONTHS;
    const static std::string FIRST_DESIGNATE_YEAR;
    const static std::string FIRST_DESIGNATE_MONTH;
    const static std::string FIRST_DESIGNATE_DAY;
    const static std::string CALENDAR_YEAR;
    const static std::string CALENDAR_MONTH;
    const static std::string CALENDAR_DAY;
    const static std::string CALENDAR_HOUR;
    const static std::string CALENDAR_MINUTE;

    static std::string SQL_ADD_COLUMNS;
    static std::vector<std::string> COLUMNS;
    static void Init();

private:
    static void AddColumn(const std::string &name, const std::string &type, const bool &isEnd);
};

protected:
    virtual uint64_t PreGetNextTriggerTimeIgnoreSnooze(bool ignoreRepeat, bool forceToGetNext) const override;

private:
    ReminderRequestCalendar() : ReminderRequest() {}

    uint8_t GetNextDay(const uint16_t &settedYear, const uint8_t &settedMonth, const tm &now, const tm &target) const;
    uint64_t GetNextTriggerTime() const;
    uint64_t GetNextTriggerTimeAsRepeatReminder(const tm &nowTime, const tm &tarTime) const;
    uint32_t GetRepeatDay() const
    {
        return repeatDay_;
    }
    uint16_t GetRepeatMonth() const
    {
        return repeatMonth_;
    }
    uint64_t GetTimeInstantMilli(
        uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) const;

    /**
     * @brief Init dateTime_ when read from parcel.
     */
    void InitDateTime();
    void InitDateTime(const tm &dateTime);
    bool IsRepeatReminder() const;
    bool IsRepeatMonth(uint8_t month) const;
    bool IsRepeatDay(uint8_t day) const;
    void SetDay(const uint8_t &day, const bool &isSet);
    void SetMonth(const uint8_t &month, const bool &isSet);
    void SetRepeatMonths(const std::vector<uint8_t> &repeatMonths);
    void SetRepeatDaysOfMonth(const std::vector<uint8_t> &repeateDays);

    static const uint8_t JANUARY;
    static const uint8_t DECEMBER;
    static const uint8_t DEFAULT_SNOOZE_TIMES;

    tm dateTime_;
    uint16_t firstDesignateYear_ {1};
    uint8_t firstDesignateMonth_ {1};
    uint8_t firstDesignateDay_ {1};
    uint16_t year_ {1};
    uint8_t month_ {1};
    uint8_t day_ {1};
    uint8_t hour_ {1};
    uint8_t minute_ {1};
    uint8_t second_ {0};
    uint16_t repeatMonth_ {0};
    uint32_t repeatDay_ {0};
};
}
}
#endif  // BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_REQUEST_CALENDAR_H