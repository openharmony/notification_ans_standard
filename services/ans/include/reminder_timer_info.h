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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_TIMER_INFO_H
#define BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_TIMER_INFO_H

#include "advanced_notification_service.h"
#include "itimer_info.h"
#include "notification_request.h"
#include "reminder_request.h"

namespace OHOS {
namespace Notification {
class ReminderTimerInfo : public MiscServices::ITimerInfo {
public:
    ReminderTimerInfo() {};
    virtual ~ReminderTimerInfo() {};

    ReminderTimerInfo(ReminderTimerInfo &other) = delete;
    ReminderTimerInfo& operator = (const ReminderTimerInfo &other) = delete;

    inline void SetAction(const std::string &action)
    {
        action_ = action;
    }

    /**
     * When timing is up, this function will execute as call back.
     */
    void OnTrigger() override;

    /**
     * Indicates the timing type.
     */
    void SetType(const int &type) override;

    /**
     * Indicates the repeat policy.
     */
    void SetRepeat(bool repeat) override;

    /**
     * Indicates the interval time for repeat timing.
     */
    void SetInterval(const uint64_t &interval) override;

    /**
     * Indicates the want agent information.
     */
    void SetWantAgent(std::shared_ptr<OHOS::AbilityRuntime::WantAgent::WantAgent> wantAgent) override;

private:
    std::string action_;
};
} // namespace OHOS
} // namespace Notification

#endif  // BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_TIMER_INFO_H
