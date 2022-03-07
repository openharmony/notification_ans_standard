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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_DATA_MANAGER_H
#define BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_DATA_MANAGER_H

#include <map>
#include <vector>

#include "advanced_notification_service.h"
#include "player.h"
#include "reminder_request.h"
#include "reminder_timer_info.h"

namespace OHOS {
namespace Notification {
class ReminderDataManager final {
public:
    ReminderDataManager() {};
    ~ReminderDataManager() {};

    ReminderDataManager(ReminderDataManager &other) = delete;
    ReminderDataManager& operator = (const ReminderDataManager &other) = delete;

    /**
     * @brief Cancels all the reminders relative to the bundle option.
     *
     * @param bundleOption Indicates the bundle option.
     * @param userId Indicates the user id which the bundle belong to.
     */
    void CancelAllReminders(const sptr<NotificationBundleOption> &bundleOption, int userId);

    /**
     * @brief Cancels the target reminder relative to the reminder id and bundle option.
     *
     * @param reminderId Indicates the reminder id.
     * @param bundleOption Indicates the bundle option.
     */
    void CancelReminder(const int32_t &reminderId, const sptr<NotificationBundleOption> &bundleOption);

    /**
     * @brief Close the target reminder which is showing on panel.
     *        This is manul operation by user: 1.Click close button of the reminder, 2.remove reminder notification.
     *
     * @param want Want information that transferred when the event trigger by user.
     * @param cancelNotification Indicates whether need to cancel notification or not.
     */
    void CloseReminder(const OHOS::EventFwk::Want &want, bool cancelNotification);

    /**
     * Dump all the reminders information.
     *
     * @return reminders informations.
     */
    std::string Dump() const;

    /**
     * Obtains the single instance.
     *
     * @return Single instance of ReminderDataManager.
     */
    static std::shared_ptr<ReminderDataManager> GetInstance();

    /**
     * Obtains all the valid reminders (which are not expired) relative to the bundle option.
     *
     * @param bundleOption Indicates the bundle option.
     * @param[out] reminders return the valid reminders.
     */
    void GetValidReminders(
        const sptr<NotificationBundleOption> bundleOption, std::vector<sptr<ReminderRequest>> &reminders);

    /**
     * @brief Triggered when third party application died.
     *
     * @param bundleOption Indicates the bundleOption of third party application.
     */
    void OnProcessDiedLocked(const sptr<NotificationBundleOption> bundleOption);

    /**
     * Publishs a scheduled reminder.
     *
     * @param reminder Indicates the reminder.
     * @param bundleOption Indicates bundle option the reminder belongs to.
     */
    void PublishReminder(sptr<ReminderRequest> &reminder, sptr<NotificationBundleOption> &bundleOption);

    /**
     * @brief Refresh all reminders when date/time or timeZone of device changed by user.
     *
     * @param type Indicates it is triggered by dateTime change or timeZone change.
     */
    void RefreshRemindersDueToSysTimeChange(uint8_t type);

    /**
     * @brief Indicates the single instance of ans that used to execute operations in service.
     *
     * @param Indicates the single instance of ans notification service.
     */
    void SetService(AdvancedNotificationService *advancedNotificationService);

    /**
     * @brief Show the reminder.
     *
     * @param isSysTimeChanged Indicates it is triggered as dateTime changed by user or not.
     */
    void ShowActiveReminder();

    /**
     * @brief Snooze the reminder by manual.
     * 1) Snooze the trigger time to the next.
     * 2) Update the notification(Update notification lable/content...; Stop audio player and vibrator)
     * 3) Show the notification dialog in the SystemUI
     * 4) Start a new reminder, which is recent one now.
     *
     * @param want Which contains the given reminder.
     */
    void SnoozeReminder(const OHOS::EventFwk::Want &want);

    /**
     * Starts the recent reminder timing.
     */
    void StartRecentReminder();

    /**
     * @brief Terminate the alerting reminder.
     *
     * 1. Stop sound and vibrate.
     * 2. Stop the alerting timer.
     * 3. Update the reminder state.
     * 4. Update the display content of the notification.
     *
     * @param want Which contains the given reminder.
     */
    void TerminateAlerting(const OHOS::EventFwk::Want &want);

    static const uint8_t TIME_ZONE_CHANGE;
    static const uint8_t DATE_TIME_CHANGE;

private:
    enum class TimerType : uint8_t {
        TRIGGER_TIMER,
        ALERTING_TIMER
    };

    /**
     * Add default slot to the reminder if no slot set by user.
     *
     * @param reminder Indicates the reminder.
     */
    void AddDefaultSlotIfNeeded(sptr<ReminderRequest> &reminder);

    /**
     * Add reminder to showed reminder vector.
     *
     * @param reminder Indicates the showed reminder.
     */
    void AddToShowedReminders(const sptr<ReminderRequest> &reminder);

    /**
     * Cancels the notification relative to the reminder.
     *
     * @param reminder Indicates the reminder.
     */
    void CancelNotification(const sptr<ReminderRequest> &reminder) const;

    /**
     * Check whether the number limit of reminders if exceeded.
     *
     * @param bundleName Indicates the target bundle.
     * @return true if number limit is exceeded.
     */
    bool CheckReminderLimitExceededLocked(const std::string &bundleName) const;
    void CloseReminder(const sptr<ReminderRequest> &reminder, bool cancelNotification);

    /**
     * Create a information for timer, such as timer type, repeat policy, interval and want agent.
     *
     * @param type Indicates the timer type.
     * @return pointer of ReminderTimerInfo.
     */
    std::shared_ptr<ReminderTimerInfo> CreateTimerInfo(TimerType type) const;

    std::string GetSoundUri(const sptr<ReminderRequest> &reminder);

    /**
     * Find the reminder from reminderVector_ by reminder id.
     *
     * @param reminderId Indicates the reminder id.
     * @return pointer of reminder request or nullptr.
     */
    sptr<ReminderRequest> FindReminderRequestLocked(const int32_t &reminderId);

    /**
     * Find the reminder from {@link reminderVector_} and
     * {@link notificationBundleOptionMap_} by reminder id and pkgName.
     *
     * @param reminderId Indicates the reminder id.
     * @param pkgName Indicates the package name.
     * @return pointer of reminder request or nullptr.
     */
    sptr<ReminderRequest> FindReminderRequestLocked(const int32_t &reminderId, const std::string &pkgName);

    /**
     * Find bundle option from {@link notificationBundleOptionMap_} by reminder id.
     *
     * @param reminderId Indicates the reminder id.
     * @return pointer of NotificationBundleOption or nullptr.
     */
    sptr<NotificationBundleOption> FindNotificationBundleOption(const int32_t &reminderId) const;

    /**
     * Obtains the recent reminder which is not expired from reminder vector.
     *
     * The expired reminders will be removed from reminderVector_ and notificationBundleOptionMap_.
     *
     * @return pointer of reminder object.
     */
    sptr<ReminderRequest> GetRecentReminderLocked();

    /**
     * Obtains all the reminders of the target bundle name.
     *
     * @param bundleName Indicates the bundle name.
     * @return all the reminders of the target bundle name.
     */
    std::vector<sptr<ReminderRequest>> GetSameBundleRemindersLocked(std::string &bundleName);

    void HandleImmediatelyShow(std::vector<sptr<ReminderRequest>> &showImmediately, bool isSysTimeChanged);

    /**
     * @brief Refresh the reminder due to date/time or timeZone change by user.
     *
     * @param type Indicates it is date/time change or timeZone change.
     * @param reminder Indicates the target reminder.
     * @return sptr<ReminderRequest> Returns the target reminder if it is need to show immediately, otherwise nullptr.
     */
    sptr<ReminderRequest> HandleRefreshReminder(uint8_t &type, sptr<ReminderRequest> &reminder);

    /**
     * @brief Handles all the reminders that have the same notification id and belong to same application
     *        with the current reminder. Unset the state of "showing".
     *
     * @param reminder Indicates the current reminder.
     */
    void HandleSameNotificationIdShowing(const sptr<ReminderRequest> reminder);

    bool HandleSysTimeChange(const sptr<ReminderRequest> reminder) const;

    /**
     * Judge the two reminders is belong to the same application or not.
     *
     * @param reminder Indicates the first reminder.
     * @param otherPkgName Indicates the package name of second reminder.
     * @param otherUserId Indicates the user id of second reminder.
     * @return true if the two reminders belong to the same application.
     */
    bool IsBelongToSameApp(
        const sptr<ReminderRequest> reminder, const std::string otherPkgName, const int otherUserId);

    void PlaySoundAndVibrationLocked(const sptr<ReminderRequest> &reminder);
    void PlaySoundAndVibration(const sptr<ReminderRequest> &reminder);
    void StopSoundAndVibrationLocked(const sptr<ReminderRequest> &reminder);
    void StopSoundAndVibration(const sptr<ReminderRequest> &reminder);

    /**
     * Remove from showed reminder vector.
     *
     * @param reminder Indicates the reminder need to remove.
     */
    void RemoveFromShowedReminders(const sptr<ReminderRequest> &reminder);

    /**
     * @brief Refresh the all reminders due to date/time or timeZone change by user.
     *
     * @param type Indicates it is date/time change or timeZone change.
     * @return reminders that need to show immediately.
     */
    std::vector<sptr<ReminderRequest>> RefreshRemindersLocked(uint8_t type);

    /**
     * Removes the reminder.
     * 1. removes the reminder from reminderVector_ and notificationBundleOptionMap_.
     * 2. cancels the notification.
     *
     * @param reminderId Indicates the reminder id.
     */
    void RemoveReminderLocked(const int32_t &reminderId);

    /**
     * Resets timer status.
     * 1. Sets timerId_ or timerIdAlerting_ with 0.
     * 2. Sets activeReminderId_ or alertingReminderId with -1.
     *
     * @param type Indicates the timer type.
     */
    void ResetStates(TimerType type);

    void SetActiveReminder(const sptr<ReminderRequest> &reminder);
    void SetAlertingReminder(const sptr<ReminderRequest> &reminder);
    void ShowActiveReminderExtendLocked(sptr<ReminderRequest> &reminder);

    /**
     * @brief Show the reminder on SystemUI.
     *
     * @param reminder Indicates the reminder to show.
     * @param isNeedToPlaySound Indicates whether need to play sound.
     * @param isNeedToStartNext Indicates whether need to start next reminder.
     * @param isSysTimeChanged Indicates whether it is triggerred as system time changed by user.
     * @param needScheduleTimeout Indicates whether need to control the ring duration.
     */
    void ShowReminder(const sptr<ReminderRequest> &reminder, const bool &isNeedToPlaySound,
        const bool &isNeedToStartNext, const bool &isSysTimeChanged, const bool &needScheduleTimeout);

    void SnoozeReminderImpl(sptr<ReminderRequest> &reminder);

    /**
     * Starts timing actually.
     *
     * @param reminderRequest Indicates the reminder.
     * @param type Indicates the timer type.
     */
    void StartTimerLocked(const sptr<ReminderRequest> &reminderRequest, TimerType type);
    void StartTimer(const sptr<ReminderRequest> &reminderRequest, TimerType type);

    /**
     * @brief Stop the alerting timer and update reminder information.
     *
     * 1. Stop sound and vibrate.
     * 2. Stop the alerting timer.
     *
     * @param reminder Indicates the target reminder.
     */
    void StopAlertingReminder(const sptr<ReminderRequest> &reminder);

    /**
     * Stops timing.
     *
     * @param type Indicates the timer type.
     */
    void StopTimer(TimerType type);
    void StopTimerLocked(TimerType type);

    /**
     * @brief Terminate the alerting reminder.
     *
     * 1. Stop sound and vibrate.
     * 2. Stop the alerting timer.
     * 3. Update the reminder state.
     * 4. Update the display content of the notification.
     *
     * @param reminder Indicates the reminder.
     * @param reason Indicates the description information.
     */
    void TerminateAlerting(const sptr<ReminderRequest> &reminder, const std::string &reason);
    void TerminateAlerting(const uint16_t waitInSecond, const sptr<ReminderRequest> &reminder);

    /**
     * @brief Assign unique reminder id and save reminder in memory.
     *
     * @param reminder Indicates a reminder.
     * @param bundleOption Indicates bundle option relative to the reminder.
     */
    void UpdateAndSaveReminderLocked(
        const sptr<ReminderRequest> &reminder, const sptr<NotificationBundleOption> &bundleOption);

    void UpdateNotification(const sptr<ReminderRequest> &reminder);

    static bool cmp(sptr<ReminderRequest> &reminderRequest, sptr<ReminderRequest> &other);

   /**
    * Single instance.
    */
    static std::shared_ptr<ReminderDataManager> REMINDER_DATA_MANAGER;

    /**
     * Used for multi-thread synchronise.
     */
    static std::mutex MUTEX;
    static std::mutex SHOW_MUTEX;
    static std::mutex ALERT_MUTEX;
    static std::mutex TIMER_MUTEX;

    /**
     * Max number of reminders limit for the whole system.
     */
    static const int16_t MAX_NUM_REMINDER_LIMIT_SYSTEM;

    /**
     * Max number of reminders limit for one application.
     */
    static const int16_t MAX_NUM_REMINDER_LIMIT_APP;

    /**
     * Vector used to record all the reminders in system.
     */
    std::vector<sptr<ReminderRequest>> reminderVector_;

    /**
     * Vector used to record all the reminders which has been shown on panel.
     */
    std::vector<sptr<ReminderRequest>> showedReminderVector_;

    /**
     * Map used to record all the bundle information of the reminders in system.
     */
    std::map<int32_t, sptr<NotificationBundleOption>> notificationBundleOptionMap_;

    /**
     * This timer is used to control the triggerTime of next reminder.
     */
    uint64_t timerId_ {0};

    /**
     * This timer is used to control the ringDuration of the alerting reminder.
     */
    uint64_t timerIdAlerting_ {0};

    /**
     * Indicates the active reminder that timing is taking effect.
     */
    int32_t activeReminderId_ = -1;
    sptr<ReminderRequest> activeReminder_ = nullptr;

    /**
     * Indicates the reminder which is playing sound or vibration.
     */
    int32_t alertingReminderId_ = -1;
    sptr<ReminderRequest> alertingReminder_ = nullptr;

    std::shared_ptr<Media::Player> soundPlayer_ = nullptr;

    /**
     * Indicates the total count of reminders in system.
     */
    int16_t totalCount_ {0};
    sptr<AdvancedNotificationService> advancedNotificationService_ = nullptr;
};
}  // namespace OHOS
}  // namespace Nofitifcation
#endif  // BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_DATA_MANAGER_H