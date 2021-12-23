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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_REQUEST_H
#define BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_REQUEST_H

#include <map>
#include <string>

#include "notification_constant.h"
#include "notification_request.h"

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
         * @brief Indicates that this action button is used to close reminder's notfication.
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
        ACTION_BUTTON,
        REMOVAL_WANT_AGENT,
        CONTENT
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

    /**
     * @brief Copy construct from an exist reminder.
     *
     * @param Indicates the exist reminder.
     */
    explicit ReminderRequest(const ReminderRequest &other);
    ReminderRequest& operator = (const ReminderRequest &other);
    virtual ~ReminderRequest() {};

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
    bool CanRemove();

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

    /**
     * @brief Obtains reminder type.
     *
     * @return reminder type.
     */
    ReminderType GetReminderType() const;

    /**
     * @brief Obtains slot type.
     *
     * @return slot type.
     */
    NotificationConstant::SlotType GetSlotType() const;

    uint8_t GetState() const;

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
     * @brief Close the reminder by manual.
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
     * @brief Show the reminder on panel. TriggerTime will be updated to next.
     *
     * @param isSysTimeChanged true means it is called when the system time is changed by user, otherwise false.
     * @param allowToNotify true means that the notification will be shown as normal, otherwise false.
     */
    void OnShow(bool isSysTimeChanged, bool allowToNotify);

    /**
     * @brief When timezone change, reminder need to refresh next trigger time.
     *
     * @return true if need to show reminder immediately.
     */
    virtual bool OnTimeZoneChange();

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

    /**
     * @brief Sets slot type.
     *
     * @param slotType Indicates slot type.
     * @return Current reminder self.
     */
    ReminderRequest& SetSlotType(const NotificationConstant::SlotType &slotType);

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

    /**
     * @brief Updates {@link triggerTimeInMilli_} to next.
     * @note If next trigger time not exist, {@link isExpired_} flag will be set with true.
     *
     * @return true if next trigger time exist and set success.
     */
    virtual bool UpdateNextReminder();

    /**
     * @brief Update notification attibutes.
     *
     * Some attributes need to be updated after the reminder published or before the notification publish.
     * For example, action button should not init until the reminder is published successfully, as the reminder id is
     * assigned after that.
     *
     * @param type Indicates the update type.
     * @param extra Indicates the extra content.
     */
    void UpdateNotificationRequest(UpdateNotificationType type, std::string extra);

    static int32_t GLOBAL_ID;
    static const uint64_t INVALID_LONG_VALUE;
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

protected:
    ReminderRequest();
    explicit ReminderRequest(ReminderType reminderType);
    virtual uint64_t PreGetNextTriggerTimeIgnoreSnooze(bool forceToGetNext) const {
        return INVALID_LONG_VALUE;
    };

private:
    void AddActionButtons();
    void AddRemovalWantAgent();
    std::string GetState(const uint8_t state) const;
    bool HandleSysTimeChange(uint64_t oriTriggerTime, uint64_t optTriggerTime);
    bool HandleTimeZoneChange(uint64_t oldZoneTriggerTime, uint64_t newZoneTriggerTime, uint64_t optTriggerTime);
    void InitNotificationRequest();
    void SetState(bool deSet, const uint8_t newState, std::string function);
    void SetWantAgent();

    static const uint8_t REMINDER_STATUS_INACTIVE;
    static const uint8_t REMINDER_STATUS_SHOWING;

    std::string content_ {};
    std::string expiredContent_ {};
    std::string title_ {};
    bool isExpired_ {false};
    uint8_t state_ {0};
    int32_t notificationId_ {0};
    int32_t reminderId_ {-1};
    uint64_t triggerTimeInMilli_ {0};
    ReminderType reminderType_ {ReminderType::INVALID};
    NotificationConstant::SlotType slotType_ {NotificationConstant::SlotType::SOCIAL_COMMUNICATION};
    sptr<NotificationRequest> notificationRequest_ = nullptr;
    std::shared_ptr<WantAgentInfo> wantAgentInfo_ = nullptr;
    std::map<ActionButtonType, ActionButtonInfo> actionButtonMap_ {};
};
}  // namespace Reminder
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_REMINDER_REQUEST_H