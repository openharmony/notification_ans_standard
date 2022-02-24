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
#include "ans_notification.h"
#include "application_env_impl.h"
#include "bundle_mgr_interface.h"
#include "iservice_registry.h"
#include "notification_helper.h"
#include "singleton.h"
#include "system_ability_definition.h"

#include "reminder_helper.h"

namespace OHOS {
namespace Notification {
ErrCode ReminderHelper::PublishReminder(ReminderRequest &reminder)
{
    ANSR_LOGI("PublishReminder start");
    NotificationSlot slot(reminder.GetSlotType());
    NotificationHelper::AddNotificationSlot(slot);
    return DelayedSingleton<AnsNotification>::GetInstance()->PublishReminder(reminder);
}

ErrCode ReminderHelper::CancelReminder(const int32_t reminderId)
{
    ANSR_LOGI("CancelReminder start");
    return DelayedSingleton<AnsNotification>::GetInstance()->CancelReminder(reminderId);
}

ErrCode ReminderHelper::CancelAllReminders()
{
    ANSR_LOGI("CancelAllReminders start");
    return DelayedSingleton<AnsNotification>::GetInstance()->CancelAllReminders();
}

bool ReminderHelper::CheckPermission()
{
    ANSR_LOGI("CheckPermission");
    AppExecFwk::ApplicationEnvImpl* application = AppExecFwk::ApplicationEnvImpl::GetInstance();
    std::string pkgName = application->GetBundleName();
    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemManager == nullptr) {
        ANSR_LOGE("Permission denied");
        return false;
    }
    auto bundleManager =
        iface_cast<AppExecFwk::IBundleMgr>(systemManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID));
    if (bundleManager->CheckPermission(pkgName, "ohos.permission.PUBLISH_AGENT_REMINDER") != 0) {
        ANSR_LOGE("Permission denied");
        return false;
    }
    return true;
}

ErrCode ReminderHelper::GetValidReminders(std::vector<sptr<ReminderRequest>> &validReminders)
{
    ANSR_LOGI("GetValidReminders start");
    return DelayedSingleton<AnsNotification>::GetInstance()->GetValidReminders(validReminders);
}

ErrCode AddNotificationSlot(const NotificationSlot &slot)
{
    ANSR_LOGI("AddNotificationSlot start");
    return DelayedSingleton<AnsNotification>::GetInstance()->AddNotificationSlot(slot);
}

ErrCode RemoveNotificationSlot(const NotificationConstant::SlotType &slotType)
{
    ANSR_LOGI("RemoveNotificationSlot start");
    return DelayedSingleton<AnsNotification>::GetInstance()->RemoveNotificationSlot(slotType);
}
}
}