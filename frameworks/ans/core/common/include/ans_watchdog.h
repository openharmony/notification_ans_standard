/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_INNERKITS_BASE_INCLUDE_ANS_WATCHDOG_H
#define BASE_NOTIFICATION_ANS_STANDARD_INNERKITS_BASE_INCLUDE_ANS_WATCHDOG_H

#include "event_handler.h"
#include "event_runner.h"

namespace OHOS {
namespace Notification {
class AnsWatchdog {
public:
    AnsWatchdog() = delete;
    ~AnsWatchdog() = delete;
  
    static void AddHandlerThread(std::shared_ptr<AppExecFwk::EventHandler> handler,
    std::shared_ptr<AppExecFwk::EventRunner> runner);
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_INNERKITS_BASE_INCLUDE_ANS_WATCHDOG_H