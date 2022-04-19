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

#include "ans_watchdog.h"
#include "ans_log_wrapper.h"
#include "xcollie/watchdog.h"

namespace OHOS {
namespace Notification {
void AnsWatchdog::AddHandlerThread(std::shared_ptr<AppExecFwk::EventHandler> handler,
    std::shared_ptr<AppExecFwk::EventRunner> runner)
{
    if (handler != nullptr && runner != nullptr) {
        std::string threadName = handler->GetEventRunner()->GetRunnerThreadName();
        unsigned int timeval = 5000;
        if (Singleton<HiviewDFX::Watchdog>::GetInstance().AddThread(threadName, handler, timeval) != 0) {
            ANS_LOGE("Failed to Add handler Thread");
        }
    }
}
}  // namespace Notification
}  // namespace OHOS