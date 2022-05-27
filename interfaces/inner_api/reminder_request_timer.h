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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_REQUEST_TIMER_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_REQUEST_TIMER_H

#include "reminder_request.h"

namespace OHOS {
namespace Notification {
class ReminderRequestTimer : public ReminderRequest {
public:
    /**
     * A constructor used to create a ReminderRequestTimer instance. The countdown timer reminder
     * will be triggered after a specified duration.
     *
     * @note The input parameter must be larger than 0 and less than UINT64_MAX/1000,
     * otherwise, the application may crash due to an illegal parameter exception.
     *
     * @param countDownTimeInSeconds Indicates the duration after which this timer reminder will be triggered.
     */
    explicit ReminderRequestTimer(uint64_t countDownTimeInSeconds);

    /**
     * @brief This constructor should only be used in background proxy service process
     * when reminder instance recovery from database.
     *
     * @param reminderId Indicates reminder id.
     */
    explicit ReminderRequestTimer(int32_t reminderId) : ReminderRequest(reminderId) {};

    /**
     * @brief Copy construct from an exist reminder.
     *
     * @param Indicates the exist reminder.
     */
    explicit ReminderRequestTimer(const ReminderRequestTimer &other);
    ReminderRequestTimer& operator = (const ReminderRequestTimer &other);
    ~ReminderRequestTimer() override {};

    uint64_t GetInitInfo() const;
    virtual bool OnDateTimeChange() override;
    virtual bool OnTimeZoneChange() override;
    virtual bool UpdateNextReminder() override;

    /**
     * Marshal a NotificationRequest object into a Parcel.
     * @param parcel the object into the parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * Unmarshal object from a Parcel.
     * @return the NotificationRequest
     */
    static ReminderRequestTimer *Unmarshalling(Parcel &parcel);

    bool ReadFromParcel(Parcel &parcel) override;

protected:
    virtual uint64_t PreGetNextTriggerTimeIgnoreSnooze(bool ignoreRepeat, bool forceToGetNext) const override;

private:
    ReminderRequestTimer() {};
    void CheckParamsValid(const uint64_t countDownTimeInSeconds) const;
    void UpdateTimeInfo(const std::string &description);
    uint64_t countDownTimeInSeconds_ {0};
    uint64_t firstRealTimeInMilliSeconds_ {0};
    uint64_t whenToChangeSysTime_ {0};
};
}  // namespace Reminder
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_REMINDER_REQUEST_TIMER_H