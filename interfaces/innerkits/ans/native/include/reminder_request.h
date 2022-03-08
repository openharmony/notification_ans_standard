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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_REQUEST_H
#define BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_REQUEST_H

#include <map>
#include <string>

#include "abs_shared_result_set.h"
#include "notification_bundle_option.h"
#include "notification_constant.h"
#include "notification_request.h"
#include "values_bucket.h"

namespace OHOS {
namespace Notification {
class ReminderRequest : public Parcelable {
public:
    /**
     * @brief Supported reminder type.
     */
    enum class ReminderType : uint8_t {
        /**
         * Indicates the classification of reminder for timer.
         */
        TIMER,

        /**
         * Indicates the classification of reminder for calendar.
         */
        CALENDAR,

        /**
         * Indicates the classification of reminder for alarm.
         */
        ALARM,
        INVALID
    };

    /**
     * @brief Supported action button type.
     */
    enum class ActionButtonType : uint8_t {
        /**
         * @brief Indicates that this action button is used to close reminder's notification.
         * It always works well, whether the application is running at the time.
         *
         */
        CLOSE,

        /**
         * @brief Indicates that this action button is used to snooze reminder.
         * It always work well, whether the application is running at the time.
         *
         */
        SNOOZE,
        INVALID
    };

    /**
     * @brief Supported notification update type.
     */
    enum class UpdateNotificationType : uint8_t {
        COMMON,
        REMOVAL_WANT_AGENT,
        WANT_AGENT,
        MAX_SCREEN_WANT_AGENT,
        BUNDLE_INFO,
        CONTENT
    };

    /**
     * @brief Enumerates the Time type for converting between c time and acture time.
     */
    enum class TimeTransferType : uint8_t {
        YEAR,
        MONTH,
        WEEK
    };

    /**
     * @brief Enumerates the Time format for print.
     */
    enum class TimeFormat : uint8_t {
        YMDHMS,
        HM
    };

    /**
     * @brief Attributes of action button.
     */
    struct ActionButtonInfo {
        /**
         * Type of the button.
         */
        ActionButtonType type;

        /**
         * Content show on the button.
         */
        std::string title = "";
    };

    /**
     * @brief Want agent information. Indicates the package and the ability to switch to.
     */
    struct WantAgentInfo {
        std::string pkgName = "";
        std::string abilityName = "";
    };

    struct MaxScreenAgentInfo {
        std::string pkgName = "";
        std::string abilityName = "";
    };

    /**
     * @brief Copy construct from an exist reminder.
     *
     * @param Indicates the exist reminder.
     */
    explicit ReminderRequest(const ReminderRequest &other);

    /**
     * @brief This constructor should only be used in background proxy service process
     * when reminder instance recovery from database.
     *
     * @param reminderId Indicates reminder id.
     */
    explicit ReminderRequest(int32_t reminderId);
    ReminderRequest& operator = (const ReminderRequest &other);
    virtual ~ReminderRequest() override {};

    /**
     * @brief Marshal a NotificationRequest object into a Parcel.
     *
     * @param parcel the object into the parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshal object from a Parcel.
     *
     * @return the NotificationRequest
     */
    static ReminderRequest *Unmarshalling(Parcel &parcel);
    virtual bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief If the reminder is showing on the notification panel, it should not be removed automatically.
     *
     * @return true if it can be removed automatically.
     */
    bool CanRemove() const;

    bool CanShow() const;

    /**
     * @brief Obtains all the information of the reminder.
     *
     * @return Information of the reminder.
     */
    std::string Dump() const;

    /**
     * @brief Obtains the configured action buttons.
     *
     * @return map of action buttons.
     */
    std::map<ActionButtonType, ActionButtonInfo> GetActionButtons() const;

    /**
     * @brief Obtains the configured content.
     *
     * @return content text.
     */
    std::string GetContent() const;

    /**
     * @brief Obtains the configured expired content.
     *
     * @return expired content text.
     */
    std::string GetExpiredContent() const;

    std::shared_ptr<MaxScreenAgentInfo> GetMaxScreenWantAgentInfo() const;

    /**
     * @brief Obtains notification id.
     *
     * @return notification id.
     */
    int32_t GetNotificationId() const;

    /**
     * @brief Obtains notification request.
     *
     * @return notification request instance.
     */
    sptr<NotificationRequest> GetNotificationRequest() const;

    /**
     * @brief Obtains reminder id.
     *
     * @return reminder id.
     */
    int32_t GetReminderId() const;

    uint64_t GetReminderTimeInMilli() const;

    /**
     * @brief Obtains reminder type.
     *
     * @return reminder type.
     */
    ReminderType GetReminderType() const;

    /**
     * @brief Obtains the ringing or vibration duration configured for this reminder.
     *
     * @return uint16_t The ringing or vibration duration in seconds.
     */
    uint16_t GetRingDuration() const;

    /**
     * @brief Obtains slot type.
     *
     * @return slot type.
     */
    NotificationConstant::SlotType GetSlotType() const;

    std::string GetSnoozeContent() const;
    uint8_t GetSnoozeTimes() const;
    uint8_t GetSnoozeTimesDynamic() const;
    uint8_t GetState() const;

    /**
     * @brief Obtains the Time Interval in seconds.
     *
     * @return uint64_t Time Interval in seconds.
     */
    uint64_t GetTimeInterval() const;

    /**
     * @brief Obtains title.
     *
     * @return title.
     */
    std::string GetTitle() const;

    /**
     * @brief Obtains trigger time in milli.
     *
     * @return trigger time.
     */
    uint64_t GetTriggerTimeInMilli() const;

    int GetUserId() const;
    int32_t GetUid() const;

    /**
     * @brief Obtains want agent information.
     *
     * @return want agent information.
     */
    std::shared_ptr<WantAgentInfo> GetWantAgentInfo() const;

    /**
     * @brief Inits reminder id when publish reminder success.
     * Assign a unique reminder id for each reminder.
     */
    void InitReminderId();

    /**
     * @brief Inits reminder userId when publish reminder success.
     *
     * When package remove, user id is sended by wantAgent, but we cannot get the uid according user id as the
     * package has been removed, and the bundleOption can not be create with correct uid. so we need to record
     * the user id, and use it to judge which user the reminder belong to.
     *
     * @param userId Indicates the userId which the reminder belong to.
     */
    void InitUserId(const int &userId);

    /**
     * @brief Inites reminder uid when publish reminder success.
     *
     * When system reboot and recovery from database, we cannot get the uid according user id as BMS has not be
     * ready. So we need to record the uid in order to create correct bundleOption.
     *
     * @param uid Indicates the uid which the reminder belong to.
     */
    void InitUid(const int32_t &uid);

    /**
     * @brief Check the reminder is alerting or not.
     *
     * @return true if the reminder is playing sound or vibrating.
     */
    bool IsAlerting() const;

    /**
     * @brief Check the reminder is expired or not.
     *
     * @return true is the reminder is expired.
     */
    bool IsExpired() const;

    /**
     * @brief Check the reminder is showing on the panel.
     *
     * @return true if the reminder is showing on the panel.
     */
    bool IsShowing() const;

    /**
     * @brief Closes the reminder by manual.
     *
     * 1) Resets the state of "Alering/Showing/Snooze"
     * 2) Resets snoozeTimesDynamic_ if update to next trigger time, otherwise set reminder to expired.
     *
     * @param updateNext Whether to update to next reminder.
     */
    void OnClose(bool updateNext);

    /**
     * @brief When date/time change, reminder need to refresh next trigger time.
     *
     * @return true if need to show reminder immediately.
     */
    virtual bool OnDateTimeChange();

    /**
     * When shown notification is covered by a new notification with the same id, we should remove
     * the state of showing, so that the reminder can be removed automatically when it is expired.
     */
    void OnSameNotificationIdCovered();

    /**
     * @brief Shows the reminder on panel. TriggerTime will be updated to next.
     *
     * @param isPlaySoundOrVibration true means it is play sound or vibration.
     * @param isSysTimeChanged true means it is called when the system time is changed by user, otherwise false.
     * @param allowToNotify true means that the notification will be shown as normal, otherwise false.
     */
    void OnShow(bool isPlaySoundOrVibration, bool isSysTimeChanged, bool allowToNotify);

    /**
     * @brief Reset the state of "Showing" when the reminder is shown failed.
     */
    void OnShowFail();

    /**
     * @brief Snooze the reminder by manual.
     *
     * 1) Updates the trigger time to the next one.
     * 2) Updates the notification content for "Snooze".
     * 3) Switches the state from "Showing[, Alerting]" to "Snooze".
     */
    bool OnSnooze();

    /**
     * @brief Starts the reminder
     *
     * Sets the state from "Inactive" to "Active".
     */
    void OnStart();

    /**
     * @brief Stops the reminder.
     *
     * Sets the state from "Active" to "Inactive".
     */
    void OnStop();

    /**
     * @brief Terminate the alerting reminder, which is executed when the ring duration is over.
     *
     * 1) Disables the state of "Alerting".
     * 2) Updates the notification content for "Alert".
     *
     * @return false if alerting state has already been set false before calling the method.
     */
    bool OnTerminate();

    /**
     * @brief When timezone change, reminder need to refresh next trigger time.
     *
     * @return true if need to show reminder immediately.
     */
    virtual bool OnTimeZoneChange();

    /**
     * @brief Recovery reminder instance from database record.
     *
     * @param resultSet Indicates the resultSet with pointer to the row of record data.
     */
    virtual void RecoverFromDb(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet);
    void RecoverActionButton(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet);
    void RecoverWantAgent(std::string wantAgentInfo, const uint8_t &type);

    /**
     * @brief Sets action button.
     *
     * @param title Indicates the title of the button.
     * @param type Indicates the type of the button.
     * @return Current reminder self.
     */
    ReminderRequest& SetActionButton(const std::string &title, const ActionButtonType &type);

    /**
     * @brief Sets reminder content.
     *
     * @param content Indicates content text.
     * @return Current reminder self.
     */
    ReminderRequest& SetContent(const std::string &content);

    /**
     * @brief Sets reminder is expired or not.
     *
     * @param isExpired Indicates the reminder is expired or not.
     */
    void SetExpired(bool isExpired);

    /**
     * @brief Sets expired content.
     *
     * @param expiredContent Indicates expired content.
     * @return Current reminder self.
     */
    ReminderRequest& SetExpiredContent(const std::string &expiredContent);

    ReminderRequest& SetMaxScreenWantAgentInfo(const std::shared_ptr<MaxScreenAgentInfo> &maxScreenWantAgentInfo);

    /**
     * @brief Sets notification id.
     *
     * @param notificationId Indicates notification id.
     * @return Current reminder self.
     */
    ReminderRequest& SetNotificationId(int32_t notificationId);

    /**
     * @brief Sets reminder id.
     *
     * @param reminderId Indicates reminder id.
     */
    void SetReminderId(int32_t reminderId);

    void SetReminderTimeInMilli(const uint64_t reminderTimeInMilli);

    /**
     * @brief Sets the ringing or vibration duration for this reminder, in seconds.
     *
     * @param ringDurationInSeconds Indicates the duration. The default is 1 second.
     * @return Current reminder self.
     */
    ReminderRequest& SetRingDuration(const uint64_t ringDurationInSeconds);

    /**
     * @brief Sets slot type.
     *
     * @param slotType Indicates slot type.
     * @return Current reminder self.
     */
    ReminderRequest& SetSlotType(const NotificationConstant::SlotType &slotType);

    ReminderRequest& SetSnoozeContent(const std::string &snoozeContent);

    /**
     * @brief Set the number of snooze times for this reminder.
     *
     * @note If the value of snoozeTimes is less than or equals to 0, this reminder is a one-shot
     * reminder and will not be snoozed.
     *
     * It the value of snoozeTimes is greater than 0, for example, snoozeTimes=3, this reminder
     * will be snoozed three times after the first alarm, that is, this reminder will be triggered
     * for four times.
     *
     * This method does not take affect on the reminders for countdown timers.
     *
     * @param snoozeTimes Indicates the number of times that the reminder will be snoozed.
     * @return ReminderRequest& Current reminder self.
     */
    ReminderRequest& SetSnoozeTimes(const uint8_t snoozeTimes);

    ReminderRequest& SetSnoozeTimesDynamic(const uint8_t snooziTimes);

    /**
     * @brief Sets the Time Interval for this reminder, in seconds. The default value is 0.
     *
     * @note The minimum snooze interval is 5 minute. If the snooze interval is set to a value greater
     * than 0 and less than 5 minutes, the system converts it to 5 minutes by default.
     *
     * This method does not take effect on the reminders for countdown timers.
     *
     * @param timeIntervalInSeconds Indicates the snooze interval to set. If the value is less or equals to 0,
     * the reminder will not be snoozed.
     * @return ReminderRequest& Current reminder self.
     */
    ReminderRequest& SetTimeInterval(const uint64_t timeIntervalInSeconds);

    /**
     * @brief Sets title.
     *
     * @param title Indicates title.
     * @return Current reminder self.
     */
    ReminderRequest& SetTitle(const std::string &title);

    /**
     * @brief Sets trigger time.
     *
     * @param triggerTimeInMilli Indicates trigger time in milli.
     */
    void SetTriggerTimeInMilli(uint64_t triggerTimeInMilli);

    /**
     * @brief Sets want agent information.
     *
     * @param wantAgentInfo Indicates want agent information.
     * @return Current reminder self.
     */
    ReminderRequest& SetWantAgentInfo(const std::shared_ptr<WantAgentInfo> &wantAgentInfo);

    bool ShouldShowImmediately() const;

    /**
     * @brief Updates {@link triggerTimeInMilli_} to next.
     * @note If next trigger time not exist, {@link isExpired_} flag will be set with true.
     *
     * @return true if next trigger time exist and set success.
     */
    virtual bool UpdateNextReminder();

    /**
     * @brief Update notification attributes.
     *
     * Some attributes need to be updated after the reminder published or before the notification publish.
     * For example, action button should not init until the reminder is published successfully, as the reminder id is
     * assigned after that.
     *
     * @param type Indicates the update type.
     * @param extra Indicates the extra content.
     */
    void UpdateNotificationRequest(UpdateNotificationType type, std::string extra);

    static int GetActualTime(const TimeTransferType &type, int cTime);
    static int GetCTime(const TimeTransferType &type, int actualTime);
    static uint64_t GetDurationSinceEpochInMilli(const time_t target);
    static int32_t GetUid(const int &userId, const std::string &bundleName);
    static int GetUserId(const int &uid);
    static void AppendValuesBucket(const sptr<ReminderRequest> &reminder,
        const sptr<NotificationBundleOption> &bundleOption, NativeRdb::ValuesBucket &values);

    static int32_t GLOBAL_ID;
    static const uint64_t INVALID_LONG_LONG_VALUE;
    static const uint16_t INVALID_U16_VALUE;
    static const uint8_t INVALID_U8_VALUE;
    static const uint16_t MILLI_SECONDS;
    static const uint16_t SAME_TIME_DISTINGUISH_MILLISECONDS;
    static const std::string NOTIFICATION_LABEL;

    /**
     * @brief Show the reminder with a notification.
     */
    static const std::string REMINDER_EVENT_ALARM_ALERT;

    /**
     * @brief Close the reminder when click the close button of notification.
     */
    static const std::string REMINDER_EVENT_CLOSE_ALERT;

    /**
     * @brief Snooze the reminder when click the snooze button of notification.
     */
    static const std::string REMINDER_EVENT_SNOOZE_ALERT;

    /**
     * @brief Used to control ring duration.
     */
    static const std::string REMINDER_EVENT_ALERT_TIMEOUT;

    /**
     * @brief Update the reminder when remove notification from the systemUI.
     */
    static const std::string REMINDER_EVENT_REMOVE_NOTIFICATION;
    static const std::string PARAM_REMINDER_ID;
    static const uint8_t REMINDER_STATUS_INACTIVE;
    static const uint8_t REMINDER_STATUS_ACTIVE;
    static const uint8_t REMINDER_STATUS_ALERTING;
    static const uint8_t REMINDER_STATUS_SHOWING;
    static const uint8_t REMINDER_STATUS_SNOOZE;

    // For database recovery.
    static void Init();
    static const std::string REMINDER_ID;
    static const std::string PKG_NAME;
    static const std::string USER_ID;
    static const std::string UID;
    static const std::string APP_LABEL;
    static const std::string REMINDER_TYPE;
    static const std::string REMINDER_TIME;
    static const std::string TRIGGER_TIME;
    static const std::string RTC_TRIGGER_TIME;
    static const std::string TIME_INTERVAL;
    static const std::string SNOOZE_TIMES;
    static const std::string DYNAMIC_SNOOZE_TIMES;
    static const std::string RING_DURATION;
    static const std::string IS_EXPIRED;
    static const std::string IS_ACTIVE;
    static const std::string STATE;
    static const std::string ZONE_ID;
    static const std::string HAS_SCHEDULED_TIMEOUT;
    static const std::string ACTION_BUTTON_INFO;
    static const std::string SLOT_ID;
    static const std::string NOTIFICATION_ID;
    static const std::string TITLE;
    static const std::string CONTENT;
    static const std::string SNOOZE_CONTENT;
    static const std::string EXPIRED_CONTENT;
    static const std::string AGENT;
    static const std::string MAX_SCREEN_AGENT;
    static std::string sqlOfAddColumns;
    static std::vector<std::string> columns;

protected:
    enum class DbRecoveryType : uint8_t {
        INT,
        LONG
    };
    ReminderRequest();
    explicit ReminderRequest(ReminderType reminderType);
    std::string GetDateTimeInfo(const time_t &timeInSecond) const;
    virtual uint64_t PreGetNextTriggerTimeIgnoreSnooze(bool ignoreRepeat, bool forceToGetNext) const
    {
        return INVALID_LONG_LONG_VALUE;
    }
    int64_t RecoverInt64FromDb(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet,
        const std::string &columnName, const DbRecoveryType &columnType);

    /**
     * @brief For database recovery.
     *
     * Add column to create table of database.
     *
     * @param name Indicates the column name.
     * @param type Indicates the type of the column.
     * @param isEnd Indicates whether it is the last column.
     */
    static void AddColumn(const std::string &name, const std::string &type, const bool &isEnd);

    static const int BASE_YEAR;

private:
    void AddActionButtons(const bool includeSnooze);
    void AddRemovalWantAgent();
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> CreateWantAgent(AppExecFwk::ElementName &element) const;
    std::string GetButtonInfo() const;
    uint64_t GetNowInstantMilli() const;
    std::string GetShowTime(const uint64_t showTime) const;
    std::string GetTimeInfoInner(const time_t &timeInSecond, const TimeFormat &format) const;
    std::string GetState(const uint8_t state) const;
    bool HandleSysTimeChange(uint64_t oriTriggerTime, uint64_t optTriggerTime);
    bool HandleTimeZoneChange(uint64_t oldZoneTriggerTime, uint64_t newZoneTriggerTime, uint64_t optTriggerTime);
    bool InitNotificationRequest();
    void InitServerObj();
    void SetMaxScreenWantAgent(AppExecFwk::ElementName &element);
    void SetState(bool deSet, const uint8_t newState, std::string function);
    void SetWantAgent(AppExecFwk::ElementName &element);
    std::vector<std::string> StringSplit(std::string source, const std::string &split) const;
    void UpdateActionButtons(const bool &setSnooze);
    bool UpdateNextReminder(const bool &force);
    void UpdateNotificationContent(const bool &setSnooze);
    void UpdateNotificationCommon();

    /**
     * @brief Used for reminder recovery from database.
     *
     * @param bundleName Indicates the third part bundle name.
     */
    void UpdateNotificationBundleInfo();

    /**
     * @brief Update the notification, which will be shown for the "Alerting" reminder.
     * 1. Update the notification label/content.
     * 2. Restore the snooze action button.
     */
    void UpdateNotificationStateForAlert();

    /**
     * @brief Update the notification, which will be shown when user do a snooze.
     * 1. Update the notification label/content.
     * 2. Remove the snooze action button.
     */
    void UpdateNotificationStateForSnooze();

    static const uint32_t MIN_TIME_INTERVAL_IN_MILLI;
    static const std::string SEP_BUTTON_SINGLE;
    static const std::string SEP_BUTTON_MULTI;
    static const std::string SEP_WANT_AGENT;

    std::string content_ {};
    std::string expiredContent_ {};
    std::string snoozeContent_ {};
    std::string displayContent_ {};
    std::string title_ {};
    std::string bundleName_ {};
    bool isExpired_ {false};
    uint8_t snoozeTimes_ {0};
    uint8_t snoozeTimesDynamic_ {0};
    uint8_t state_ {0};
    int32_t notificationId_ {0};
    int32_t reminderId_ {-1};
    int userId_ {-1};
    int32_t uid_ {-1};

    // Indicates the reminder has been shown in the past time.
    // When the reminder has been created but not showed, it is equals to 0.
    uint64_t reminderTimeInMilli_ {0};
    uint64_t ringDurationInMilli_ {MILLI_SECONDS};
    uint64_t triggerTimeInMilli_ {0};
    uint64_t timeIntervalInMilli_ {0};
    ReminderType reminderType_ {ReminderType::INVALID};
    NotificationConstant::SlotType slotType_ {NotificationConstant::SlotType::SOCIAL_COMMUNICATION};
    sptr<NotificationRequest> notificationRequest_ = nullptr;
    std::shared_ptr<WantAgentInfo> wantAgentInfo_ = nullptr;
    std::shared_ptr<MaxScreenAgentInfo> maxScreenWantAgentInfo_ = nullptr;
    std::map<ActionButtonType, ActionButtonInfo> actionButtonMap_ {};
};
}  // namespace Reminder
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_REQUEST_H