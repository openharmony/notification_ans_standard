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
#include "ans_const_define.h"
#include "common_event_support.h"
#include "notification_slot.h"
#include "reminder_event_manager.h"
#include "time_service_client.h"
#include "singleton.h"

#include "reminder_data_manager.h"

namespace OHOS {
namespace Notification {
const int16_t ReminderDataManager::MAX_NUM_REMINDER_LIMIT_SYSTEM = 2000;
const int16_t ReminderDataManager::MAX_NUM_REMINDER_LIMIT_APP = 30;
const uint8_t ReminderDataManager::TIME_ZONE_CHANGE = 0;
const uint8_t ReminderDataManager::DATE_TIME_CHANGE = 1;
std::shared_ptr<ReminderDataManager> ReminderDataManager::REMINDER_DATA_MANAGER = nullptr;
std::mutex ReminderDataManager::MUTEX;
std::mutex ReminderDataManager::SHOW_MUTEX;
std::mutex ReminderDataManager::ALERT_MUTEX;

void ReminderDataManager::PublishReminder(sptr<ReminderRequest> &reminder,
    sptr<NotificationBundleOption> &bundleOption)
{
    if (CheckReminderLimitExceededLocked(bundleOption->GetBundleName())) {
        return;
    }
    UpdateAndSaveReminderLocked(reminder, bundleOption);
    StartRecentReminder();
}

void ReminderDataManager::CancelReminder(
    const int32_t &reminderId, const sptr<NotificationBundleOption> &bundleOption)
{
    sptr<ReminderRequest> reminder = FindReminderRequestLocked(reminderId, bundleOption->GetBundleName());
    if (reminder == nullptr) {
        ANSR_LOGW("Cancel reminder, not find the reminder");
        return;
    }
    if (activeReminderId_ == reminderId) {
        ANSR_LOGD("Cancel active reminder, id=%{public}d", reminderId);
        StopTimerLocked(TimerType::TRIGGER_TIMER);
    }
    if (alertingReminderId_ == reminderId) {
        StopTimerLocked(TimerType::ALERTING_TIMER);
        StopSoundAndVibrationLocked(reminder);
    }
    int32_t id = reminderId;
    RemoveReminderLocked(id);
    CancelNotification(reminder);
    StartRecentReminder();
}

void ReminderDataManager::CancelNotification(const sptr<ReminderRequest> &reminder) const
{
    if (!(reminder->IsShowing())) {
        ANSR_LOGD("No need to cancel notification");
        return;
    }
    sptr<NotificationRequest> notification = reminder->GetNotificationRequest();
    if (notification == nullptr) {
        ANSR_LOGW("Cancel notification fail");
        return;
    }
    ANSR_LOGD("Cancel notification");
    if (advancedNotificationService_ == nullptr) {
        ANSR_LOGE("Cancel notification fail");
        return;
    }
    sptr<NotificationBundleOption> bundleOption = FindNotificationBundleOption(reminder->GetReminderId());
    advancedNotificationService_->CancelPreparedNotification(
        notification->GetNotificationId(), ReminderRequest::NOTIFICATION_LABEL, bundleOption);
}

bool ReminderDataManager::CheckReminderLimitExceededLocked(const std::string &bundleName) const
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    if (totalCount_ >= ReminderDataManager::MAX_NUM_REMINDER_LIMIT_SYSTEM) {
        ANSR_LOGW("The number of validate reminders exceeds the system upper limit:%{public}d, \
            and new reminder can not be published", MAX_NUM_REMINDER_LIMIT_SYSTEM);
        return true;
    }
    int8_t count = 0;
    for (auto it = reminderVector_.begin(); it != reminderVector_.end(); ++it) {
        if ((*it)->IsExpired()) {
            continue;
        }
        auto mit = notificationBundleOptionMap_.find((*it)->GetReminderId());
        if (mit == notificationBundleOptionMap_.end()) {
            ANSR_LOGE("Error occur when get bundle option, reminderId=%{public}d", (*it)->GetReminderId());
        } else {
            if (mit->second->GetBundleName() == bundleName) {
                count++;
            }
        }
    }
    if (count >= ReminderDataManager::MAX_NUM_REMINDER_LIMIT_APP) {
        ANSR_LOGW("The number of validate reminders exceeds the application upper limit:%{public}d, and new \
            reminder can not be published", MAX_NUM_REMINDER_LIMIT_APP);
        return true;
    }
    return false;
}

void ReminderDataManager::CancelAllReminders(const sptr<NotificationBundleOption> &bundleOption)
{
    MUTEX.lock();
    auto it = notificationBundleOptionMap_.find(activeReminderId_);
    if (it == notificationBundleOptionMap_.end()) {
        ANSR_LOGW("Not get bundle option, reminderId=%{public}d", activeReminderId_);
    } else {
        if (it->second->GetBundleName() == bundleOption->GetBundleName()) {
            StopTimer(TimerType::TRIGGER_TIMER);
        }
    }
    for (auto vit = reminderVector_.begin(); vit != reminderVector_.end();) {
        int32_t reminderId = (*vit)->GetReminderId();
        auto mit = notificationBundleOptionMap_.find(reminderId);
        if (mit == notificationBundleOptionMap_.end()) {
            ANSR_LOGE("Get bundle option occur error, reminderId=%{public}d", reminderId);
            ++vit;
            continue;
        }
        if (mit->second->GetBundleName() == bundleOption->GetBundleName()) {
            if ((*vit)->IsAlerting()) {
                StopAlertingReminder(*vit);
            }
            CancelNotification(*vit);
            ANSR_LOGD("Containers(vector/map) remove. reminderId=%{public}d", reminderId);
            vit = reminderVector_.erase(vit);
            notificationBundleOptionMap_.erase(mit);
            totalCount_--;
            continue;
        }
        ++vit;
    }
    MUTEX.unlock();
    StartRecentReminder();
}

void ReminderDataManager::GetValidReminders(
    const sptr<NotificationBundleOption> bundleOption, std::vector<sptr<ReminderRequest>> &reminders)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    for (auto it = reminderVector_.begin(); it != reminderVector_.end(); ++it) {
        if ((*it)->IsExpired()) {
            continue;
        }
        int32_t reminderId = (*it)->GetReminderId();
        auto mit = notificationBundleOptionMap_.find(reminderId);
        if (mit == notificationBundleOptionMap_.end()) {
            ANSR_LOGE("Get bundle option occur error, reminderId=%{public}d", reminderId);
        } else {
            if (mit->second->GetBundleName() == bundleOption->GetBundleName()) {
                reminders.push_back(*it);
            }
        }
    }
}

void ReminderDataManager::AddToShowedReminders(const sptr<ReminderRequest> &reminder)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::SHOW_MUTEX);
    for (auto it = shownReminderVector_.begin(); it != shownReminderVector_.end(); ++it) {
        if (reminder->GetReminderId() == (*it)->GetReminderId()) {
            ANSR_LOGD("Showed reminder is already exist");
            return;
        }
    }
    ANSR_LOGD("Containers(shownVector) add. reminderId=%{public}d", reminder->GetReminderId());
    shownReminderVector_.push_back(reminder);
}

void ReminderDataManager::OnProcessDiedLocked(const sptr<NotificationBundleOption> bundleOption)
{
    std::string bundleName = bundleOption->GetBundleName();
    int32_t uid = bundleOption->GetUid();
    ANSR_LOGI("OnProcessDiedLocked, bundleName=%{public}s, uid=%{public}d", bundleName.c_str(), uid);
    std::lock_guard<std::mutex> lock(ReminderDataManager::SHOW_MUTEX);
    for (auto it = shownReminderVector_.begin(); it != shownReminderVector_.end(); ++it) {
        int32_t reminderId = (*it)->GetReminderId();
        auto mit = notificationBundleOptionMap_.find(reminderId);
        if (mit == notificationBundleOptionMap_.end()) {
            ANSR_LOGE("Get bundle option occur error, reminderId=%{public}d", reminderId);
            continue;
        }
        if (mit->second->GetBundleName() != bundleName || mit->second->GetUid() != uid) {
            continue;
        }
        if ((*it)->IsAlerting()) {
            TerminateAlerting((*it), "onProcessDied");
        } else {
            CancelNotification(*it);
            (*it)->OnClose(false);
            ANSR_LOGD("Containers(shownVector) remove. reminderId=%{public}d", reminderId);
            shownReminderVector_.erase(it);
            --it;
        }
    }
}

std::shared_ptr<ReminderTimerInfo> ReminderDataManager::CreateTimerInfo(TimerType type) const
{
    auto sharedTimerInfo = std::make_shared<ReminderTimerInfo>();
    sharedTimerInfo->SetType(sharedTimerInfo->TIMER_TYPE_WAKEUP|sharedTimerInfo->TIMER_TYPE_EXACT);
    sharedTimerInfo->SetRepeat(false);
    sharedTimerInfo->SetInterval(0);

    int requestCode = 10;
    std::vector<WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG);
    auto want = std::make_shared<OHOS::AAFwk::Want>();

    switch (type) {
        case (TimerType::TRIGGER_TIMER): {
            want->SetAction(ReminderRequest::REMINDER_EVENT_ALARM_ALERT);
            break;
        }
        case (TimerType::ALERTING_TIMER): {
            if (alertingReminderId_ == -1) {
                ANSR_LOGE("Create alerting time out timer Illegal.");
                return sharedTimerInfo;
            }
            want->SetAction(ReminderRequest::REMINDER_EVENT_ALERT_TIMEOUT);
            want->SetParam(ReminderRequest::PARAM_REMINDER_ID, alertingReminderId_);
            break;
        }
        default:
            ANSR_LOGE("TimerType not support");
            break;
    }
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    wants.push_back(want);
    WantAgent::WantAgentInfo wantAgentInfo(
        requestCode,
        WantAgent::WantAgentConstant::OperationType::SEND_COMMON_EVENT,
        flags,
        wants,
        nullptr
    );
    std::shared_ptr<WantAgent::WantAgent> wantAgent = WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
    sharedTimerInfo->SetWantAgent(wantAgent);
    return sharedTimerInfo;
}

sptr<ReminderRequest> ReminderDataManager::FindReminderRequestLocked(const int32_t &reminderId)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    for (auto it = reminderVector_.begin(); it != reminderVector_.end(); ++it) {
        if (reminderId == (*it)->GetReminderId()) {
            return *it;
        }
    }
    ANSR_LOGD("Not find the reminder");
    return nullptr;
}

sptr<ReminderRequest> ReminderDataManager::FindReminderRequestLocked(
    const int32_t &reminderId, const std::string &pkgName)
{
    sptr<ReminderRequest> reminder = FindReminderRequestLocked(reminderId);
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    if (reminder == nullptr) {
        return nullptr;
    }
    auto bundleOption = FindNotificationBundleOption(reminderId);
    if (bundleOption == nullptr) {
        ANSR_LOGW("Not find the reminder due to bundle info is null");
        return nullptr;
    }
    if (bundleOption -> GetBundleName() != pkgName) {
        ANSR_LOGW("Not find the reminder due to package name not match");
        return nullptr;
    }
    return reminder;
}

sptr<NotificationBundleOption> ReminderDataManager::FindNotificationBundleOption(const int32_t &reminderId) const
{
    auto it = notificationBundleOptionMap_.find(reminderId);
    if (it == notificationBundleOptionMap_.end()) {
        return nullptr;
    } else {
        return it->second;
    }
}

bool ReminderDataManager::cmp(sptr<ReminderRequest> &reminderRequest, sptr<ReminderRequest> &other)
{
    return reminderRequest->GetTriggerTimeInMilli() < other->GetTriggerTimeInMilli();
}

void ReminderDataManager::CloseReminder(const OHOS::EventFwk::Want &want, bool cancelNotification)
{
    int32_t reminderId = static_cast<int32_t>(want.GetIntParam(ReminderRequest::PARAM_REMINDER_ID, -1));
    sptr<ReminderRequest> reminder = FindReminderRequestLocked(reminderId);
    if (reminder == nullptr) {
        ANSR_LOGW("Invilate reminder id: %{public}d", reminderId);
        return;
    }
    CloseReminder(reminder, cancelNotification);
}

void ReminderDataManager::CloseReminder(const sptr<ReminderRequest> &reminder, bool cancelNotification)
{
    int32_t reminderId = reminder->GetReminderId();
    if (activeReminderId_ == reminderId) {
        ANSR_LOGD("Stop active reminder due to CloseReminder");
        StopTimerLocked(TimerType::TRIGGER_TIMER);
    }
    if (alertingReminderId_ == reminderId) {
        StopTimerLocked(TimerType::ALERTING_TIMER);
        StopSoundAndVibrationLocked(reminder);
    }
    reminder->OnClose(true);
    RemoveFromShowedReminders(reminder);
    if (cancelNotification) {
        CancelNotification(reminder);
    }
    StartRecentReminder();
}

std::shared_ptr<ReminderDataManager> ReminderDataManager::GetInstance()
{
    if (REMINDER_DATA_MANAGER == nullptr) {
        REMINDER_DATA_MANAGER = std::make_shared<ReminderDataManager>();
        ReminderEventManager reminderEventManager(REMINDER_DATA_MANAGER);
    }
    return REMINDER_DATA_MANAGER;
}

void ReminderDataManager::RefreshRemindersDueToSysTimeChange(uint8_t type)
{
    std::string typeInfo = type == TIME_ZONE_CHANGE ? "timeZone" : "dateTime";
    ANSR_LOGI("Refresh all reminders due to %{public}s changed by user", typeInfo.c_str());
    if (activeReminderId_ != -1) {
        ANSR_LOGD("Stop active reminder due to date/time or timeZone change");
        StopTimerLocked(TimerType::TRIGGER_TIMER);
    }
    std::vector<sptr<ReminderRequest>> showImmediately = RefreshRemindersLocked(type);
    if (!showImmediately.empty()) {
        ANSR_LOGD("Refresh all reminders, show expired reminders immediately");
        HandleImmediatelyShow(showImmediately, true);
    }
    StartRecentReminder();
}

void ReminderDataManager::TerminateAlerting(const OHOS::EventFwk::Want &want)
{
    int32_t reminderId = static_cast<int32_t>(want.GetIntParam(ReminderRequest::PARAM_REMINDER_ID, -1));
    sptr<ReminderRequest> reminder = FindReminderRequestLocked(reminderId);
    if (reminder == nullptr) {
        ANSR_LOGE("Invilate reminder id: %{public}d", reminderId);
        return;
    }
    TerminateAlerting(reminder, "timeOut");
}

void ReminderDataManager::TerminateAlerting(const uint16_t waitInMilli, const sptr<ReminderRequest> &reminder)
{
    sleep(waitInMilli);
    TerminateAlerting(reminder, "waitInMillis");
}

void ReminderDataManager::TerminateAlerting(const sptr<ReminderRequest> &reminder, const std::string &reason)
{
    ANSR_LOGI("Terminate the alerting reminder, %{public}s, called by %{public}s",
        reminder->Dump().c_str(), reason.c_str());
    StopAlertingReminder(reminder);

    if (reminder == nullptr) {
        ANSR_LOGE("TerminateAlerting illegal.");
        return;
    }
    if (!reminder->OnTerminate()) {
        return;
    }
    int32_t reminderId = reminder->GetReminderId();
    sptr<NotificationBundleOption> bundleOption = FindNotificationBundleOption(reminderId);
    sptr<NotificationRequest> notificationRequest = reminder->GetNotificationRequest();
    if (bundleOption == nullptr) {
        ANSR_LOGE("Get bundle option fail, reminderId=%{public}d", reminderId);
        return;
    }
    ANSR_LOGD("publish(update) notification.(reminderId=%{public}d)", reminder->GetReminderId());
    UpdateNotification(reminder);
    advancedNotificationService_->PublishPreparedNotification(notificationRequest, bundleOption);
}

void ReminderDataManager::UpdateAndSaveReminderLocked(
    const sptr<ReminderRequest> &reminder, const sptr<NotificationBundleOption> &bundleOption)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    reminder->InitReminderId();
    int32_t reminderId = reminder->GetReminderId();
    ANSR_LOGD("Containers(map) add. reminderId=%{public}d", reminderId);
    auto ret = notificationBundleOptionMap_.insert(
        std::pair<int32_t, sptr<NotificationBundleOption>>(reminderId, bundleOption));
    if (!ret.second) {
        ANSR_LOGE("Containers add to map error");
        return;
    }
    ANSR_LOGD("Containers(vector) add. reminderId=%{public}d", reminderId);
    reminderVector_.push_back(reminder);
    totalCount_++;
}

void ReminderDataManager::SetService(AdvancedNotificationService *advancedNotificationService)
{
    advancedNotificationService_ = advancedNotificationService;
}

void ReminderDataManager::ShowActiveReminder()
{
    ANSR_LOGI("Begin to show reminder.");
    if (activeReminderId_ == -1) {
        ANSR_LOGE("Active reminder not exist");
        return;
    }
    if (HandleSysTimeChange(activeReminder_)) {
        ResetStates(TimerType::TRIGGER_TIMER);
        return;
    }
    ShowActiveReminderExtendLocked(activeReminder_);
    ResetStates(TimerType::TRIGGER_TIMER);
    StartRecentReminder();
}

bool ReminderDataManager::HandleSysTimeChange(const sptr<ReminderRequest> reminder) const
{
    if (reminder->CanShow()) {
        return false;
    } else {
        ANSR_LOGI("handleSystimeChange, no need to show reminder again.");
        return true;
    }
}

void ReminderDataManager::SetActiveReminder(const sptr<ReminderRequest> &reminder)
{
    if (reminder == nullptr) {
        // activeReminder_ should not be set with null as it point to actual object.
        activeReminderId_ = -1;
    } else {
        activeReminderId_ = reminder->GetReminderId();
        activeReminder_ = reminder;
    }
}

void ReminderDataManager::SetAlertingReminder(const sptr<ReminderRequest> &reminder)
{
    if (reminder == nullptr) {
        // alertingReminder_ should not be set with null as it point to actual object.
        alertingReminderId_ = -1;
    } else {
        alertingReminderId_ = reminder->GetReminderId();
        alertingReminder_ = reminder;
    }
}

void ReminderDataManager::ShowActiveReminderExtendLocked(sptr<ReminderRequest> &reminder)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    uint64_t triggerTime = reminder->GetTriggerTimeInMilli();
    bool isAlerting = false;
    sptr<ReminderRequest> playSoundReminder = nullptr;
    for (auto it = reminderVector_.begin(); it != reminderVector_.end(); ++it) {
        if ((*it)->IsExpired()) {
            continue;
        }
        if ((*it)->GetTriggerTimeInMilli() - triggerTime > ReminderRequest::SAME_TIME_DISTINGUISH_MILLISECONDS) {
            continue;
        }
        if (!isAlerting) {
            playSoundReminder = (*it);
            isAlerting = true;
        } else {
            ShowReminder((*it), false, false, false, false);
        }
    }
    if (playSoundReminder != nullptr) {
        ShowReminder(playSoundReminder, true, false, false, true);
    }
}

void ReminderDataManager::ShowReminder(const sptr<ReminderRequest> &reminder, const bool &isNeedToPlaySound,
    const bool &isNeedToStartNext, const bool &isSysTimeChanged, const bool &needScheduleTimeout)
{
    ANSR_LOGD("Show the reminder(Play sound: %{public}d), %{public}s"
        , static_cast<int32_t>(isNeedToPlaySound), reminder->Dump().c_str());
    int32_t reminderId = reminder->GetReminderId();
    sptr<NotificationBundleOption> bundleOption = FindNotificationBundleOption(reminderId);
    sptr<NotificationRequest> notificationRequest = reminder->GetNotificationRequest();
    if (bundleOption == nullptr) {
        ANSR_LOGE("Get bundle option fail, reminderId=%{public}d", reminderId);
        return;
    }
    if (advancedNotificationService_ == nullptr) {
        ANSR_LOGE("ShowReminder fail");
        reminder->OnShow(false, isSysTimeChanged, false);
        return;
    }
    if (isNeedToPlaySound) {  // todo if shouldAlert
        PlaySoundAndVibration(reminder);  // play sound and vibration
        reminder->OnShow(true, isSysTimeChanged, true);
        if (needScheduleTimeout) {
            StartTimer(reminder, TimerType::ALERTING_TIMER);
        } else {
            TerminateAlerting(ReminderRequest::MILLI_SECONDS, reminder);
        }
    } else {
        reminder->OnShow(false, isSysTimeChanged, true);
    }
    AddToShowedReminders(reminder);
    ANSR_LOGD("publish notification.(reminderId=%{public}d)", reminder->GetReminderId());
    UpdateNotification(reminder);  // this should be called after OnShow
    ErrCode errCode
        = advancedNotificationService_->PublishPreparedNotification(notificationRequest, bundleOption);
    if (errCode != ERR_OK) {
        reminder->OnShowFail();
        RemoveFromShowedReminders(reminder);
    } else {
        HandleSameNotificationIdShowing(reminder);
    }
    if (isNeedToStartNext) {
        StartRecentReminder();
    }
}

void ReminderDataManager::UpdateNotification(const sptr<ReminderRequest> &reminder)
{
    reminder->UpdateNotificationRequest(ReminderRequest::UpdateNotificationType::COMMON, "");
    reminder->UpdateNotificationRequest(ReminderRequest::UpdateNotificationType::REMOVAL_WANT_AGENT, "");
    reminder->UpdateNotificationRequest(ReminderRequest::UpdateNotificationType::WANT_AGENT, "");
    reminder->UpdateNotificationRequest(ReminderRequest::UpdateNotificationType::MAX_SCREEN_WANT_AGENT, "");
}

void ReminderDataManager::SnoozeReminder(const OHOS::EventFwk::Want &want)
{
    int32_t reminderId = static_cast<int32_t>(want.GetIntParam(ReminderRequest::PARAM_REMINDER_ID, -1));
    sptr<ReminderRequest> reminder = FindReminderRequestLocked(reminderId);
    if (reminder == nullptr) {
        ANSR_LOGW("Invilate reminder id: %{public}d", reminderId);
        return;
    }
    SnoozeReminderImpl(reminder);
}

void ReminderDataManager::SnoozeReminderImpl(sptr<ReminderRequest> &reminder)
{
    ANSR_LOGI("Snooze the reminder request, %{public}s", reminder->Dump().c_str());
    int32_t reminderId = reminder->GetReminderId();
    if (activeReminderId_ == reminderId) {
        ANSR_LOGD("Cancel active reminder, id=%{public}d", activeReminderId_);
        StopTimerLocked(TimerType::TRIGGER_TIMER);
    }

    // 1) Snooze the reminder by manual
    if (alertingReminderId_ == reminder->GetReminderId()) {
        StopTimerLocked(TimerType::ALERTING_TIMER);
        StopSoundAndVibrationLocked(reminder);
    }
    reminder->OnSnooze();

    // 2) Show the notification dialog in the systemUI
    sptr<NotificationBundleOption> bundleOption = FindNotificationBundleOption(reminderId);
    sptr<NotificationRequest> notificationRequest = reminder->GetNotificationRequest();
    if (bundleOption == nullptr) {
        ANSR_LOGW("snoozeReminder, invalid bundle option");
        return;
    }
    ANSR_LOGD("publish(update) notification.(reminderId=%{public}d)", reminder->GetReminderId());
    UpdateNotification(reminder);
    advancedNotificationService_->PublishPreparedNotification(notificationRequest, bundleOption);
    StartRecentReminder();
}

// snoozeReminder(bool snoozeAll)

void ReminderDataManager::StartRecentReminder()
{
    sptr<ReminderRequest> reminder = GetRecentReminderLocked();
    if (reminder == nullptr) {
        ANSR_LOGI("No reminder need to start");
        SetActiveReminder(reminder);
        return;
    }
    if (activeReminderId_ == reminder->GetReminderId()) {
        ANSR_LOGI("Recent reminder has already run, no need to start again.");
        return;
    }
    if (activeReminderId_ != -1) {
        StopTimerLocked(TimerType::TRIGGER_TIMER);
        activeReminder_->OnStop();
    }
    ANSR_LOGI("Start recent reminder");
    StartTimerLocked(reminder, TimerType::TRIGGER_TIMER);
    reminder->OnStart();
    SetActiveReminder(reminder);
}

void ReminderDataManager::StopAlertingReminder(const sptr<ReminderRequest> &reminder)
{
    if (reminder == nullptr) {
        ANSR_LOGE("StopAlertingReminder illegal.");
        return;
    }
    if (alertingReminderId_ == -1 || reminder->GetReminderId() != alertingReminderId_) {
        ANSR_LOGE("StopAlertingReminder is illegal.");
        return;
    }
    StopSoundAndVibration(alertingReminder_);
    StopTimer(TimerType::ALERTING_TIMER);
}

std::string ReminderDataManager::Dump() const
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    std::map<std::string, std::vector<sptr<ReminderRequest>>> bundleNameMap;
    for (auto it = reminderVector_.begin(); it != reminderVector_.end(); ++it) {
        if ((*it)->IsExpired()) {
            continue;
        }
        int32_t reminderId = (*it)->GetReminderId();
        auto mit = notificationBundleOptionMap_.find(reminderId);
        if (mit == notificationBundleOptionMap_.end()) {
            ANSR_LOGE("Dump get notificationBundleOption(reminderId=%{public}d) fail", reminderId);
            continue;
        }
        std::string bundleName = mit->second->GetBundleName();
        auto val = bundleNameMap.find(bundleName);
        if (val == bundleNameMap.end()) {
            std::vector<sptr<ReminderRequest>> reminders;
            reminders.push_back(*it);
            bundleNameMap.insert(std::pair<std::string, std::vector<sptr<ReminderRequest>>>(bundleName, reminders));
        } else {
            val->second.push_back(*it);
        }
    }

    std::string allReminders = "";
    for (auto it = bundleNameMap.begin(); it != bundleNameMap.end(); ++it) {
        std::string bundleName = it->first;
        std::vector<sptr<ReminderRequest>> reminders = it->second;
        sort(reminders.begin(), reminders.end(), cmp);
        std::string oneBundleReminders = bundleName + ":{\n";
        oneBundleReminders += "    totalCount:" + std::to_string(reminders.size()) + ",\n";
        oneBundleReminders += "    reminders:{\n";
        for (auto vit = reminders.begin(); vit != reminders.end(); ++vit) {
            oneBundleReminders += "        [\n";
            std::string reminderInfo = (*vit)->Dump();
            oneBundleReminders += "            " + reminderInfo + "\n";
            oneBundleReminders += "        ],\n";
        }
        oneBundleReminders += "    },\n";
        oneBundleReminders += "},\n";
        allReminders += oneBundleReminders;
    }

    return "ReminderDataManager{ totalCount:" + std::to_string(totalCount_) + ",\n" +
           "timerId:" + std::to_string(timerId_) + ",\n" +
           "activeReminderId:" + std::to_string(activeReminderId_) + ",\n" +
           allReminders + "}";
}

sptr<ReminderRequest> ReminderDataManager::GetRecentReminderLocked()
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    sort(reminderVector_.begin(), reminderVector_.end(), cmp);
    for (auto it = reminderVector_.begin(); it != reminderVector_.end();) {
        if (!(*it)->IsExpired()) {
            ANSR_LOGI("GetRecentReminderLocked: %{public}s", (*it)->Dump().c_str());
            return *it;
        }
        if (!(*it)->CanRemove()) {
            ANSR_LOGD("Reminder has been expired: %{public}s", (*it)->Dump().c_str());
            it++;
            continue;
        }
        int32_t reminderId = (*it)->GetReminderId();
        ANSR_LOGD("Containers(vector) remove. reminderId=%{public}d", reminderId);
        auto mit = notificationBundleOptionMap_.find((*it)->GetReminderId());
        if (mit == notificationBundleOptionMap_.end()) {
            ANSR_LOGE("Remove notificationBundleOption(reminderId=%{public}d) fail",
                (*it)->GetReminderId());
        } else {
            ANSR_LOGD("Containers(vector/map) remove. reminderId=%{public}d", reminderId);
            notificationBundleOptionMap_.erase(mit);
        }
        it = reminderVector_.erase(it);
        totalCount_--;
    }
    return nullptr;
}

std::vector<sptr<ReminderRequest>> ReminderDataManager::GetSameBundleRemindersLocked(std::string &bundleName)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    std::vector<sptr<ReminderRequest>> reminders;
    for (auto it = reminderVector_.begin(); it != reminderVector_.end(); ++it) {
        sptr<NotificationBundleOption>  bundleOption = FindNotificationBundleOption((*it)->GetReminderId());
        if (bundleOption == nullptr) {
            ANSR_LOGW("GetSameBundleRemindersLocked get notificationBundleOption(reminderId=%{public}d) fail",
                (*it)->GetReminderId());
            continue;
        }
        if (bundleName == bundleOption->GetBundleName()) {
            reminders.push_back((*it));
        }
    }
    return reminders;
}

void ReminderDataManager::HandleImmediatelyShow(
    std::vector<sptr<ReminderRequest>> &showImmediately, bool isSysTimeChanged)
{
    bool isAlerting = false;
    for (auto it = showImmediately.begin(); it != showImmediately.end(); ++it) {
        if (!isAlerting) {
            ShowReminder((*it), true, false, isSysTimeChanged, false);
            isAlerting = true;
        } else {
            ShowReminder((*it), false, false, isSysTimeChanged, false);
        }
    }
}

sptr<ReminderRequest> ReminderDataManager::HandleRefreshReminder(uint8_t &type, sptr<ReminderRequest> &reminder)
{
    reminder->SetReminderTimeInMilli(ReminderRequest::INVALID_LONG_LONG_VALUE);
    uint64_t triggerTimeBefore = reminder->GetTriggerTimeInMilli();
    bool needShowImmediately = false;
    if (type == TIME_ZONE_CHANGE) {
        needShowImmediately = reminder->OnTimeZoneChange();
    }
    if (type == DATE_TIME_CHANGE) {
        needShowImmediately = reminder->OnDateTimeChange();
    }
    if (!needShowImmediately) {
        uint64_t triggerTimeAfter = reminder->GetTriggerTimeInMilli();
        if (triggerTimeBefore != triggerTimeAfter
            || reminder->GetReminderId() == alertingReminderId_) {
            CloseReminder(reminder, true);
        }
        return nullptr;
    } else {
        return reminder;
    }
}

void ReminderDataManager::HandleSameNotificationIdShowing(const sptr<ReminderRequest> reminder)
{
    // not add ReminderDataManager::MUTEX, as ShowActiveReminderExtendLocked has locked
    int32_t notificationId = reminder->GetNotificationId();
    ANSR_LOGD("HandleSameNotificationIdShowing notificationId=%{public}d", notificationId);
    int32_t curReminderId = reminder->GetReminderId();
    for (auto it = reminderVector_.begin(); it != reminderVector_.end(); ++it) {
        int32_t tmpId = (*it)->GetReminderId();
        if (tmpId == curReminderId) {
            continue;
        }
        if (!(*it)->IsShowing()) {
            continue;
        }
        sptr<NotificationBundleOption>  bundleOption = FindNotificationBundleOption(tmpId);
        if (bundleOption == nullptr) {
            ANSR_LOGW("Get notificationBundleOption(reminderId=%{public}d) fail", tmpId);
            continue;
        }
        if (notificationId == (*it)->GetNotificationId() &&
            IsBelongToSameApp(reminder, bundleOption->GetBundleName(), 0)) {
            if ((*it)->IsAlerting()) {
                StopAlertingReminder(*it);
            }
            (*it)->OnSameNotificationIdCovered();
            RemoveFromShowedReminders(*it);
        }
    }
}

bool ReminderDataManager::IsBelongToSameApp(
    const sptr<ReminderRequest> reminder, const std::string otherPkgName, const int otherUserId)
{
    ANSR_LOGD("otherUserId=%{public}d, (currently, userId not support)", otherUserId);
    int32_t reminderId = reminder->GetReminderId();
    sptr<NotificationBundleOption>  bundleOption = FindNotificationBundleOption(reminderId);
    if (bundleOption == nullptr) {
        ANSR_LOGW("IsBelongToSameApp get notificationBundleOption(reminderId=%{public}d) fail", reminderId);
        return false;
    }
    if (bundleOption->GetBundleName() == otherPkgName) {
        return true;
    }
    return false;
}

void ReminderDataManager::PlaySoundAndVibrationLocked(const sptr<ReminderRequest> &reminder)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::ALERT_MUTEX);  // todo check died lock
    PlaySoundAndVibration(reminder);
}

void ReminderDataManager::PlaySoundAndVibration(const sptr<ReminderRequest> &reminder)
{
    if (reminder == nullptr) {
        ANSR_LOGE("Play sound and vibration failed as reminder is null.");
        return;
    }
    if (alertingReminderId_ != -1) {
        TerminateAlerting(alertingReminder_, "PlaySoundAndVibration");
    }
    ANSR_LOGD("Play sound and vibration, reminderId=%{public}d", reminder->GetReminderId());
    if (soundPlayer_ == nullptr) {
        soundPlayer_ = Media::PlayerFactory::CreatePlayer();
    }
    std::string uri = GetSoundUri(reminder);
    ANSR_LOGD("uri:%{public}s", uri.c_str());
    soundPlayer_->SetSource(uri);
    soundPlayer_->SetLooping(true);
    soundPlayer_->Prepare();
    soundPlayer_->Play();
    SetAlertingReminder(reminder);
}

std::string ReminderDataManager::GetSoundUri(const sptr<ReminderRequest> &reminder)
{
    sptr<NotificationBundleOption> bundle = FindNotificationBundleOption(reminder->GetReminderId());
    std::vector<sptr<NotificationSlot>> slots;
    ErrCode errCode = advancedNotificationService_->GetSlotsByBundle(bundle, slots);
    Uri uri = DEFAULT_NOTIFICATION_SOUND;
    if (errCode != ERR_OK) {
        ANSR_LOGW("Get sound uri fail, use default sound instead.");
        return uri.GetSchemeSpecificPart();
    }
    for (auto it = slots.begin(); it != slots.end(); ++it) {
        if ((*it)->GetType() == reminder->GetSlotType()) {
            uri = (*it)->GetSound();
            break;
        }
    }
    return uri.GetSchemeSpecificPart();
}

void ReminderDataManager::StopSoundAndVibrationLocked(const sptr<ReminderRequest> &reminder)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::ALERT_MUTEX);
    StopSoundAndVibration(reminder);
}

void ReminderDataManager::StopSoundAndVibration(const sptr<ReminderRequest> &reminder)
{
    if (reminder == nullptr) {
        ANSR_LOGE("Stop sound and vibration failed as reminder is null.");
        return;
    }
    if (alertingReminderId_ == -1 || (reminder->GetReminderId() != alertingReminderId_)) {
        ANSR_LOGE("Stop sound and vibration failed as alertingReminder is illegal.");
        return;
    }
    ANSR_LOGD("Stop sound and vibration, reminderId=%{public}d", reminder->GetReminderId());
    soundPlayer_->Stop();
    sptr<ReminderRequest> nullReminder = nullptr;
    SetAlertingReminder(nullReminder);
}

void ReminderDataManager::RemoveFromShowedReminders(const sptr<ReminderRequest> &reminder)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::SHOW_MUTEX);
    for (auto it = shownReminderVector_.begin(); it != shownReminderVector_.end(); ++it) {
        if ((*it)->GetReminderId() == reminder->GetReminderId()) {
            ANSR_LOGD("Containers(shownVector) remove. reminderId=%{public}d", reminder->GetReminderId());
            shownReminderVector_.erase(it);
            break;
        }
    }
}

std::vector<sptr<ReminderRequest>> ReminderDataManager::RefreshRemindersLocked(uint8_t type)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    std::vector<sptr<ReminderRequest>> showImmediately;
    for (auto it = reminderVector_.begin(); it != reminderVector_.end(); ++it) {
        sptr<ReminderRequest> reminder = HandleRefreshReminder(type, (*it));
        if (reminder != nullptr) {
            showImmediately.push_back(reminder);
        }
    }
    return showImmediately;
}

void ReminderDataManager::RemoveReminderLocked(const int32_t &reminderId)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    for (auto it = reminderVector_.begin(); it != reminderVector_.end();) {
        if (reminderId == (*it)->GetReminderId()) {
            ANSR_LOGD("Containers(vector) remove. reminderId=%{public}d", reminderId);
            it = reminderVector_.erase(it);
            totalCount_--;
            break;
        } else {
            ++it;
        }
    }
    auto it = notificationBundleOptionMap_.find(reminderId);
    if (it == notificationBundleOptionMap_.end()) {
        ANSR_LOGE("Remove notificationBundleOption(reminderId=%{public}d) fail", reminderId);
    } else {
        ANSR_LOGD("Containers(map) remove. reminderId=%{public}d", reminderId);
        notificationBundleOptionMap_.erase(it);
    }
}

void ReminderDataManager::StartTimerLocked(const sptr<ReminderRequest> &reminderRequest, TimerType type)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    StartTimer(reminderRequest, type);
}

void ReminderDataManager::StartTimer(const sptr<ReminderRequest> &reminderRequest, TimerType type)
{
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    time_t now;
    (void)time(&now);  // unit is seconds.
    uint64_t triggerTime = 0;
    switch (type) {
        case TimerType::TRIGGER_TIMER: {
            if (timerId_ != 0) {
                ANSR_LOGE("Trigger timer has already started.");
                break;
            }
            triggerTime = reminderRequest->GetTriggerTimeInMilli();
            timerId_ = timer->CreateTimer(REMINDER_DATA_MANAGER->CreateTimerInfo(type));
            timer->StartTimer(timerId_, triggerTime);
            SetActiveReminder(reminderRequest);
            ANSR_LOGD("Start timing (next triggerTime), timerId=%{public}llu", (unsigned long long)timerId_);
            break;
        }
        case TimerType::ALERTING_TIMER: {
            if (timerIdAlerting_ != 0) {
                ANSR_LOGE("Alerting time out timer has already started.");
                break;
            }
            triggerTime = now * ReminderRequest::MILLI_SECONDS
                + static_cast<uint64_t>(reminderRequest->GetRingDuration() * ReminderRequest::MILLI_SECONDS);
            timerIdAlerting_ = timer->CreateTimer(REMINDER_DATA_MANAGER->CreateTimerInfo(type));
            timer->StartTimer(timerIdAlerting_, triggerTime);
            ANSR_LOGD(
                "Start timing (alerting time out), timerId=%{public}llu", (unsigned long long)timerIdAlerting_);
            break;
        }
        default: {
            ANSR_LOGE("TimerType not support");
            break;
        }
    }
    if (triggerTime == 0) {
        ANSR_LOGW("Start timer fail");
    } else {
        ANSR_LOGD("Timing info: now:(%{public}lld), tar:(%{public}llu)",
            (long long)(now * ReminderRequest::MILLI_SECONDS), (unsigned long long)(triggerTime));
    }
}

void ReminderDataManager::StopTimerLocked(TimerType type)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    StopTimer(type);
}

void ReminderDataManager::StopTimer(TimerType type)
{
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    uint64_t timerId = 0;
    switch (type) {
        case TimerType::TRIGGER_TIMER: {
            timerId = timerId_;
            ANSR_LOGD("Stop timing (next triggerTime)");
            break;
        }
        case TimerType::ALERTING_TIMER: {
            timerId = timerIdAlerting_;
            ANSR_LOGD("Stop timing (alerting time out)");
            break;
        }
        default: {
            ANSR_LOGE("TimerType not support");
            break;
        }
    }
    if (timerId == 0) {
        ANSR_LOGD("Timer is not running");
        return;
    }
    ANSR_LOGD("Stop timer id=%{public}llu", (unsigned long long)timerId);
    timer->StopTimer(timerId);
    ResetStates(type);
}

void ReminderDataManager::ResetStates(TimerType type)
{
    switch (type) {
        case TimerType::TRIGGER_TIMER: {
            ANSR_LOGD("ResetStates(activeReminder)");
            timerId_ = 0;
            activeReminderId_ = -1;
            break;
        }
        case TimerType::ALERTING_TIMER: {
            ANSR_LOGD("ResetStates(alertingReminder)");
            timerIdAlerting_ = 0;
            alertingReminderId_ = -1;
            break;
        }
        default: {
            ANSR_LOGE("TimerType not support");
            break;
        }
    }
}
}
}