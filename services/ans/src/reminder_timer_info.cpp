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

#include "reminder_timer_info.h"

#include "ans_log_wrapper.h"
#include "common_event_manager.h"
#include "reminder_request.h"

using namespace OHOS::EventFwk;
namespace OHOS {
namespace Notification {
void ReminderTimerInfo::SetType(const int &_type)
{
    type = _type;
}

void ReminderTimerInfo::SetRepeat(bool _repeat)
{
    repeat = _repeat;
}

void ReminderTimerInfo::SetInterval(const uint64_t &_interval)
{
    interval = _interval;
}

void ReminderTimerInfo::SetWantAgent(std::shared_ptr<OHOS::AbilityRuntime::WantAgent::WantAgent> _wantAgent)
{
    wantAgent = _wantAgent;
}

void ReminderTimerInfo::OnTrigger()
{
    ANSR_LOGI("Timing is arrivelled.");
}
}
}
