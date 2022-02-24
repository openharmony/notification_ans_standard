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

#include "system_event_observer.h"

#include "advanced_notification_service.h"
#include "bundle_constants.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "notification_preferences.h"

namespace OHOS {
namespace Notification {
SystemEventObserver::SystemEventObserver(const ISystemEvent &callbacks) : callbacks_(callbacks)
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
#ifdef DISTRIBUTED_NOTIFICATION_SUPPORTED
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
#endif
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    EventFwk::CommonEventSubscribeInfo commonEventSubscribeInfo(matchingSkills);

    subscriber_ = std::make_shared<SystemEventSubscriber>(
        commonEventSubscribeInfo, std::bind(&SystemEventObserver::OnReceiveEvent, this, std::placeholders::_1));

    EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
}

SystemEventObserver::~SystemEventObserver()
{
    EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

void SystemEventObserver::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    auto want = data.GetWant();
    std::string action = want.GetAction();
    ANS_LOGD("OnReceiveEvent action is %{public}s.", action.c_str());
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
        if (callbacks_.onBundleRemoved != nullptr) {
            auto element = want.GetElement();
            std::string bundleName = element.GetBundleName();
            int uid = want.GetIntParam(AppExecFwk::Constants::UID, -1);
            sptr<NotificationBundleOption> bundleOption = new NotificationBundleOption(bundleName, uid);
            callbacks_.onBundleRemoved(bundleOption);
        }
#ifdef DISTRIBUTED_NOTIFICATION_SUPPORTED
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON) {
        if (callbacks_.onScreenOn != nullptr) {
            callbacks_.onScreenOn();
        }
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF) {
        if (callbacks_.onScreenOff != nullptr) {
            callbacks_.onScreenOff();
        }
#endif
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        NotificationPreferences::GetInstance().InitSettingFromDisturbDB();
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED) {
        int32_t userId = data.GetCode();
        callbacks_.onResourceRemove(userId);
    }
}
}  // namespace Notification
}  // namespace OHOS