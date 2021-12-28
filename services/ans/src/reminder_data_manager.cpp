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
#include "common_event_support.h"
#include "reminder_event_manager.h"
#include "time_service_client.h"
#include "singleton.h"

#include "reminder_data_manager.h"

namespace OHOS {
namespace Notification {
const int16_t ReminderDataManager::MAX_NUM_REMINDER_LIMIT_SYSTEM = 2000;
const int16_t ReminderDataManager::MAX_NUM_REMINDER_LIMIT_APP = 30;
const uint16_t ReminderDataManager::SAME_TIME_DISTINGUISH_MILLISECONDS = 1000;
const uint8_t ReminderDataManager::TIME_ZONE_CHANGE = 0;
const uint8_t ReminderDataManager::DATE_TIME_CHANGE = 1;
std::shared_ptr<ReminderDataManager> ReminderDataManager::REMINDER_DATA_MANAGER = nullptr;
std::mutex ReminderDataManager::MUTEX;

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
    if (activeReminderId_ != -1 && activeReminderId_ == reminderId) {
        ANSR_LOGD("Cancel active reminder, id=%{public}d", activeReminderId_);
        StopTimerLocked();
    }
    int32_t id = reminderId;
    RemoveReminderLocked(id);
    CancelNotification(reminder);
    StartRecentReminder();
}

void ReminderDataManager::CancelNotification(sptr<ReminderRequest> &reminder) const
{
    if (!(reminder->IsShowing())) {
        ANSR_LOGD("No need to cancel notification");
        return;
    }
    sptr<NotificationRequest> notification = reminder->GetNotificationRequest();
    if (notification == nullptr) {
        ANSR_LOGW("Cancel notification fail");
    } else {
        ANSR_LOGD("Cancel notification");
        if (advancedNotificationService_ == nullptr) {
            ANSR_LOGE("Cancel notification fail");
            return;
        }
        advancedNotificationService_->Cancel(notification->GetNotificationId(), ReminderRequest::NOTIFICATION_LABEL);
    }
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
            reminder can not be published", MAX_NUM_REMINDER_LIMIT_SYSTEM);
        return true;
    }
    return false;
}

void ReminderDataManager::CancelAllReminders(const sptr<NotificationBundleOption> &bundleOption)
{
    MUTEX.lock();
    auto it = notificationBundleOptionMap_.find(activeReminderId_);
    if (it == notificationBundleOptionMap_.end()) {
        ANSR_LOGD("Get bundle option error, reminderId=%{public}d", activeReminderId_);
    } else {
        if (it->second->GetBundleName() == bundleOption->GetBundleName()) {
            StopTimer();
        }
    }
    for (auto vit = reminderVector_.begin(); vit != reminderVector_.end();) {
        int32_t reminderId = (*vit)->GetReminderId();
        auto mit = notificationBundleOptionMap_.find(reminderId);
        if (mit == notificationBundleOptionMap_.end()) {
            ANSR_LOGE("Get bundle option occur error, reminderId=%{public}d", reminderId);
        } else {
            if (mit->second->GetBundleName() == bundleOption->GetBundleName()) {
                CancelNotification(*vit);
                ANSR_LOGD("Containers(vector/map) remove. reminderId=%{public}d", reminderId);
                vit = reminderVector_.erase(vit);
                notificationBundleOptionMap_.erase(mit);
                totalCount_--;
                continue;
            }
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

std::shared_ptr<ReminderTimerInfo> ReminderDataManager::CreateTimerInfo() const
{
    auto sharedTimerInfo = std::make_shared<ReminderTimerInfo>();
    sharedTimerInfo->SetType(sharedTimerInfo->TIMER_TYPE_WAKEUP|sharedTimerInfo->TIMER_TYPE_EXACT);
    sharedTimerInfo->SetRepeat(false);
    sharedTimerInfo->SetInterval(0);
    sharedTimerInfo->SetWantAgent(nullptr);
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

sptr<NotificationBundleOption> ReminderDataManager::FindNotificationBundleOption(const int32_t &reminderId)
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
    CloseReminder(reminderId, cancelNotification);
}

void ReminderDataManager::CloseReminder(const int32_t &reminderId, bool cancelNotification)
{
    sptr<ReminderRequest> reminder = FindReminderRequestLocked(reminderId);
    if (reminder == nullptr) {
        ANSR_LOGW("Invilate reminder id: %{public}d", reminderId);
        return;
    }
    if (activeReminderId_ != -1 && activeReminderId_ == reminderId) {
        ANSR_LOGD("Stop active reminder due to CloseReminder");
        StopTimerLocked();
    }
    reminder->OnClose(true);
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
        StopTimerLocked();
    }
    std::vector<sptr<ReminderRequest>> showImmediately = RefreshRemindersLocked(type);
    if (!showImmediately.empty()) {
        ANSR_LOGD("Refresh all reminders, show expired reminders immediately");
        HandleImmediatelyShow(showImmediately, true);
    }
    StartRecentReminder();
}

void ReminderDataManager::UpdateAndSaveReminderLocked(
    const sptr<ReminderRequest> &reminder, const sptr<NotificationBundleOption> &bundleOption)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    reminder->InitReminderId();
    int32_t reminderId = reminder->GetReminderId();
    ANSR_LOGD("Containers(vector/map) add. reminderId=%{public}d", reminderId);
    auto ret = notificationBundleOptionMap_.insert(
        std::pair<int32_t, sptr<NotificationBundleOption>>(reminderId, bundleOption));
    if (!ret.second) {
        ANSR_LOGE("Containers add to map error");
        return;
    }
    reminderVector_.push_back(reminder);
    totalCount_++;
}

void ReminderDataManager::SetService(AdvancedNotificationService *advancedNotificationService)
{
    advancedNotificationService_ = advancedNotificationService;
}

void ReminderDataManager::ShowReminder(bool isSysTimeChanged)
{
    ANSR_LOGD("ShowReminder");
    if (activeReminderId_ == -1) {
        ANSR_LOGE("Active reminder not exist");
        return;
    }
    sptr<ReminderRequest> reminder = FindReminderRequestLocked(activeReminderId_);
    ShowDesignatedReminderLocked(reminder, isSysTimeChanged);
    ResetStates();
    StartRecentReminder();
}

void ReminderDataManager::ShowDesignatedReminderLocked(sptr<ReminderRequest> &reminder, bool isSysTimeChanged)
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    uint64_t triggerTime = reminder->GetTriggerTimeInMilli();
    for (auto it = reminderVector_.begin(); it != reminderVector_.end(); ++it) {
        if ((*it)->IsExpired()) {
            continue;
        }
        if ((*it)->GetTriggerTimeInMilli() - triggerTime > ReminderDataManager::SAME_TIME_DISTINGUISH_MILLISECONDS) {
            continue;
        }
        if (isSysTimeChanged) {
            if ((*it)->GetReminderId() != reminder->GetReminderId()) {
                continue;
            }
        }
        int32_t reminderId = (*it)->GetReminderId();
        sptr<NotificationBundleOption> bundleOption = FindNotificationBundleOption(reminderId);
        sptr<NotificationRequest> notificationRequest = (*it)->GetNotificationRequest();
        if (bundleOption == nullptr) {
            ANSR_LOGE("Get bundle option fail, reminderId=%{public}d", reminderId);
            continue;
        }
        if (advancedNotificationService_ == nullptr) {
            ANSR_LOGE("ShowReminder fail");
            (*it)->OnShow(isSysTimeChanged, false);
        } else {
            ANSR_LOGD("publish notification.(reminderId=%{public}d)", reminderId);
            (*it)->UpdateNotificationRequest(ReminderRequest::UpdateNotificationType::REMOVAL_WANT_AGENT, "");
            (*it)->UpdateNotificationRequest(ReminderRequest::UpdateNotificationType::ACTION_BUTTON, "");
            advancedNotificationService_->PublishSavedNotification(notificationRequest, bundleOption);
            (*it)->OnShow(isSysTimeChanged, true);
            HandleSameNotificationIdShowing((*it));
        }
    }
}

void ReminderDataManager::StartRecentReminder()
{
    sptr<ReminderRequest> reminder = GetRecentReminderLocked();
    if (reminder == nullptr) {
        ANSR_LOGI("No reminder need to start");
        return;
    }

    bool toStart = true;
    if (activeReminderId_ != -1) {
        if (activeReminderId_ != reminder->GetReminderId()) {
            ANSR_LOGI("Stop active reminder");
            StopTimerLocked();
        } else {
            ANSR_LOGI("Recent reminder has already run, no need to start again.");
            toStart = false;
        }
    }
    if (toStart) {
        ANSR_LOGI("Start recent reminder");
        StartTimerLocked(reminder);
    }
}

void ReminderDataManager::StopTimer()
{
    if (timerId_ == 0) {
        ANSR_LOGD("Timer is not running");
        return;
    }
    ANSR_LOGD("Stop timer id=%{public}llu", (unsigned long long)timerId_);
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    timer->StopTimer(timerId_);
    ResetStates();
}

void ReminderDataManager::StopTimerLocked()
{
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    StopTimer();
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
    for (auto reminder = reminderVector_.begin(); reminder != reminderVector_.end();) {
        if (!(*reminder)->IsExpired()) {
            ANSR_LOGD("GetRecentReminderLocked: %{public}s", (*reminder)->Dump().c_str());
            return *reminder;
        }
        if (!(*reminder)->CanRemove()) {
            ANSR_LOGD("Reminder has been expired: %{public}s", (*reminder)->Dump().c_str());
            reminder++;
            continue;
        }
        int32_t reminderId = (*reminder)->GetReminderId();
        ANSR_LOGD("Containers(vector) remove. reminderId=%{public}d", reminderId);
        auto it = notificationBundleOptionMap_.find((*reminder)->GetReminderId());
        if (it == notificationBundleOptionMap_.end()) {
            ANSR_LOGE("Remove notificationBundleOption(reminderId=%{public}d) fail",
                (*reminder)->GetReminderId());
        } else {
            ANSR_LOGD("Containers(map) remove. reminderId=%{public}d", reminderId);
            notificationBundleOptionMap_.erase(it);
        }
        reminder = reminderVector_.erase(reminder);
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
    for (auto it = showImmediately.begin(); it != showImmediately.end(); ++it) {
        ShowDesignatedReminderLocked((*it), isSysTimeChanged);
    }
}

sptr<ReminderRequest> ReminderDataManager::HandleRefreshReminder(uint8_t &type, sptr<ReminderRequest> &reminder)
{
    bool needShowImmediately = false;
    if (type == TIME_ZONE_CHANGE) {
        needShowImmediately = reminder->OnTimeZoneChange();
    }
    if (type == DATE_TIME_CHANGE) {
        needShowImmediately = reminder->OnDateTimeChange();
    }
    if (!needShowImmediately) {
        return nullptr;
    } else {
        return reminder;
    }
}

void ReminderDataManager::HandleSameNotificationIdShowing(const sptr<ReminderRequest> reminder)
{
    // not add ReminderDataManager::MUTEX, as ShowDesignatedReminderLocked has locked
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
            (*it)->OnSameNotificationIdCovered();
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
            ANSR_LOGD("Remove reminder(id=%{public}d) success", reminderId);
            break;
        } else {
            ++it;
        }
    }
    auto it = notificationBundleOptionMap_.find(reminderId);
    if (it == notificationBundleOptionMap_.end()) {
        ANSR_LOGE("Remove notificationBundleOption(reminderId=%{public}d) fail", reminderId);
    } else {
        ANSR_LOGD("Containers(map) remove: reminderId=%{public}d", reminderId);
        notificationBundleOptionMap_.erase(it);
    }
}

void ReminderDataManager::ResetStates()
{
    ANSR_LOGD("ResetStates");
    timerId_ = 0;
    activeReminderId_ = -1;
}

void ReminderDataManager::StartTimerLocked(sptr<ReminderRequest> &reminderRequest)
{
    ANSR_LOGD("Start timer: millSeconds=%{public}llu", (unsigned long long)(reminderRequest->GetTriggerTimeInMilli()));
    std::lock_guard<std::mutex> lock(ReminderDataManager::MUTEX);
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    timerId_ = timer->CreateTimer(REMINDER_DATA_MANAGER->CreateTimerInfo());
    timer->StartTimer(timerId_, reminderRequest->GetTriggerTimeInMilli());
    activeReminderId_ = reminderRequest->GetReminderId();
}
}
}