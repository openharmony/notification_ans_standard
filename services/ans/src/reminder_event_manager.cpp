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

#include "ans_log_wrapper.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "reminder_event_manager.h"
#include "bundle_constants.h"

using namespace OHOS::EventFwk;
namespace OHOS {
namespace Notification {
ReminderEventManager::ReminderEventManager(std::shared_ptr<ReminderDataManager> &reminderDataManager)
{
    init(reminderDataManager);
}

void ReminderEventManager::init(std::shared_ptr<ReminderDataManager> &reminderDataManager) const
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(ReminderRequest::REMINDER_EVENT_ALARM_ALERT);
    matchingSkills.AddEvent(ReminderRequest::REMINDER_EVENT_CLOSE_ALERT);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_DATA_CLEARED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_RESTARTED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_TIME_CHANGED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    auto subscriber = std::make_shared<ReminderEventSubscriber>(subscriberInfo, reminderDataManager);
    if (CommonEventManager::SubscribeCommonEvent(subscriber)) {
        REMINDER_LOGD("SubscribeCommonEvent ok");
    } else {
        REMINDER_LOGD("SubscribeCommonEvent fail");
    }
}

ReminderEventManager::ReminderEventSubscriber::ReminderEventSubscriber(
    const CommonEventSubscribeInfo &subscriberInfo,
    std::shared_ptr<ReminderDataManager> &reminderDataManager) : CommonEventSubscriber(subscriberInfo)
{
    reminderDataManager_ = reminderDataManager;
}

void ReminderEventManager::ReminderEventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    Want want = data.GetWant();
    std::string action = want.GetAction();
    REMINDER_LOGD("Recieved common event:%{public}s", action.c_str());
    if (action == ReminderRequest::REMINDER_EVENT_ALARM_ALERT) {
        reminderDataManager_->ShowReminder(false);
        return;
    }
    if (action == ReminderRequest::REMINDER_EVENT_CLOSE_ALERT) {
        reminderDataManager_->CloseReminder(want, true);
        return;
    }
    if (action == ReminderRequest::REMINDER_EVENT_REMOVE_NOTIFICATION) {
        reminderDataManager_->CloseReminder(want, false);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
        OHOS::AppExecFwk::ElementName ele = want.GetElement();
        std::string bundleName = ele.GetBundleName();
        int uid = want.GetIntParam(OHOS::AppExecFwk::Constants::UID, -1);
        REMINDER_LOGD("bundleName=%{public}s, uid=%{public}d", bundleName.c_str(), uid);
        sptr<NotificationBundleOption> bundleOption = new NotificationBundleOption(bundleName, uid);
        reminderDataManager_->CancelAllReminders(bundleOption);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_DATA_CLEARED) {
        // todo
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_RESTARTED) {
        // todo
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED) {
        reminderDataManager_->RefreshRemindersDueToSysTimeChange(ReminderDataManager::TIME_ZONE_CHANGE);
        return;
    }
    if (action == CommonEventSupport::COMMON_EVENT_TIME_CHANGED) {
        reminderDataManager_->RefreshRemindersDueToSysTimeChange(ReminderDataManager::DATE_TIME_CHANGE);
        return;
    }
}
}  // namespace OHOS
}  // namespace Notification