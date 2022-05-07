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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_ANS_MANAGER_INTERFACE_H
#define BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_ANS_MANAGER_INTERFACE_H

#include <string>
#include <vector>

#include "ans_subscriber_interface.h"
#include "iremote_broker.h"
#include "notification_bundle_option.h"
#include "notification_constant.h"
#include "notification_do_not_disturb_date.h"
#include "notification_request.h"
#include "notification_slot.h"
#include "notification_slot_group.h"
#include "notification_subscribe_info.h"
#include "reminder_request.h"

namespace OHOS {
namespace Notification {
class IAnsManager : public IRemoteBroker {
public:
    IAnsManager() = default;
    virtual ~IAnsManager() = default;
    DISALLOW_COPY_AND_MOVE(IAnsManager);

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Notification.IAnsManager");

    /**
     * @brief Publishes a notification with a specified label.
     * @note If a notification with the same ID has been published by the current application and has not been deleted,
     *       this method will update the notification.
     *
     * @param label Indicates the label of the notification to publish.
     * @param notification Indicates the NotificationRequest object for setting the notification content.
     *                This parameter must be specified.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode Publish(const std::string &label, const sptr<NotificationRequest> &notification) = 0;

    /**
     * @brief Publishes a notification on a specified remote device.
     * @note If a notification with the same ID has been published by the current application and has not been deleted,
     *       this method will update the notification.
     *
     * @param notification Indicates the NotificationRequest object for setting the notification content.
     *                This parameter must be specified.
     * @param deviceId Indicates the ID of the remote device. If this parameter is null or an empty string,
     *                 the notification will be published on the local device.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode PublishToDevice(const sptr<NotificationRequest> &notification, const std::string &deviceId) = 0;

    /**
     * @brief Cancels a published notification matching the specified label and notificationId.
     *
     * @param notificationId Indicates the ID of the notification to cancel.
     * @param label Indicates the label of the notification to cancel.
     * @return Returns cancel notification result.
     */
    virtual ErrCode Cancel(int notificationId, const std::string &label) = 0;

    /**
     * @brief Cancels all the published notifications.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode CancelAll() = 0;

    /**
     * @brief Adds a notification slot by type.
     *
     * @param slotType Indicates the notification slot type to be added.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode AddSlotByType(NotificationConstant::SlotType slotType) = 0;

    /**
     * @brief Creates multiple notification slots.
     *
     * @param slots Indicates the notification slots to create.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode AddSlots(const std::vector<sptr<NotificationSlot>> &slots) = 0;

    /**
     * @brief Deletes a created notification slot based on the slot ID.
     *
     * @param slotType Indicates the type of the slot, which is created by AddNotificationSlot
     *                 This parameter must be specified.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode RemoveSlotByType(const NotificationConstant::SlotType &slotType) = 0;

    /**
     * @brief Deletes all notification slots.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode RemoveAllSlots() = 0;

    /**
     * @brief Creates multiple notification slot groups.
     * @note The precautions for using this method are similar to those for
     *       AddNotificationSlotGroup(NotificationSlotGroup).
     *
     * @param groups Indicates a list of NotificationSlotGroup objects to create. This parameter cannot be null.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode AddSlotGroups(std::vector<sptr<NotificationSlotGroup>> groups) = 0;

    /**
     * @brief Queries a created notification slot.
     *
     * @param slotType Indicates the ID of the slot, which is created by AddNotificationSlot(NotificationSlot). This
     *        parameter must be specified.
     * @param slot Indicates the created NotificationSlot.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetSlotByType(const NotificationConstant::SlotType &slotType, sptr<NotificationSlot> &slot) = 0;

    /**
     * @brief Obtains all notification slots of this application.
     *
     * @param slots Indicates the created NotificationSlot.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetSlots(std::vector<sptr<NotificationSlot>> &slots) = 0;

    /**
     * @brief Queries a created notification slot group.
     *
     * @param groupId Indicates the ID of the slot group.
     * @param group   Indicates the created NotificationSlotGroup.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetSlotGroup(const std::string &groupId, sptr<NotificationSlotGroup> &group) = 0;

    /**
     * @brief Obtains a list of created notification slot groups.
     *
     * @param  groups Indicates a list of created notification slot groups.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetSlotGroups(std::vector<sptr<NotificationSlotGroup>> &groups) = 0;

    /**
     * @brief Obtains the number of slot.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param num Indicates the number of slot.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetSlotNumAsBundle(const sptr<NotificationBundleOption> &bundleOption, uint64_t &num) = 0;

    /**
     * @brief Deletes multiple notification slot groups.
     *
     * @param groupIds Indicates the IDs of the notification slot groups, which is created by
     *                    AddNotificationSlotGroup(NotificationSlotGroup) This parameter must be specified.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode RemoveSlotGroups(const std::vector<std::string> &groupIds) = 0;

    /**
     * @brief Obtains active notifications of the current application in the system.
     *
     * @param notifications Indicates active NotificationRequest objects of the current application.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetActiveNotifications(std::vector<sptr<NotificationRequest>> &notifications) = 0;

    /**
     * @brief Obtains the number of active notifications of the current application in the system.
     *
     * @param num Indicates the number of active notifications of the current application.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetActiveNotificationNums(uint64_t &num) = 0;

    /**
     * @brief Obtains all active notifications in the current system. The caller must have system permissions to
     * call this method.
     *
     * @param notifications Indicates all active notifications of this application.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetAllActiveNotifications(std::vector<sptr<Notification>> &notifications) = 0;

    /**
     * @brief Obtains the active notifications corresponding to the specified key in the system. To call this method
     * to obtain particular active notifications, you must have received the notifications and obtained the key
     * via {Notification::GetKey()}.
     *
     * @param key Indicates the key array for querying corresponding active notifications.
     *            If this parameter is null, this method returns all active notifications in the system.
     * @param notification Indicates the set of active notifications corresponding to the specified key.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetSpecialActiveNotifications(
        const std::vector<std::string> &key, std::vector<sptr<Notification>> &notifications) = 0;

    /**
     * @brief Allows another application to act as an agent to publish notifications in the name of your application
     * bundle.
     *
     * @param agent Indicates the name of the application bundle that can publish notifications for your application.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode SetNotificationAgent(const std::string &agent) = 0;

    /**
     * @brief Obtains the name of the application bundle that can publish notifications in the name of your application.
     *
     * @param agent Indicates the name of the application bundle that can publish notifications for your application if
     * any; returns null otherwise.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetNotificationAgent(std::string &agent) = 0;

    /**
     * @brief Checks whether your application has permission to publish notifications by calling
     * PublishNotificationAsBundle(string, NotificationRequest) in the name of another application indicated by the
     * given representativeBundle.
     *
     * @param representativeBundle Indicates the name of application bundle your application is representing.
     * @param canPublish Indicates whether your application has permission to publish notifications.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode CanPublishAsBundle(const std::string &representativeBundle, bool &canPublish) = 0;

    /**
     * @brief Publishes a notification in the name of a specified application bundle.
     * @note If the notification to be published has the same ID as a published notification that has not been canceled,
     * the existing notification will be replaced by the new one.
     *
     * @param notification Indicates the NotificationRequest object for setting the notification content.
     *                This parameter must be specified.
     * @param representativeBundle Indicates the name of the application bundle that allows your application to publish
     *                             notifications for it by calling setNotificationAgent.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode PublishAsBundle(
        const sptr<NotificationRequest> notification, const std::string &representativeBundle) = 0;

    /**
     * @brief Sets the number of active notifications of the current application as the number to be displayed on the
     * notification badge.
     *
     * @param num Indicates the badge number.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode SetNotificationBadgeNum(int num) = 0;

    /**
     * @brief Obtains the importance level of this application.
     *
     * @param importance Indicates the importance level of this application, which can be LEVEL_NONE,
               LEVEL_MIN, LEVEL_LOW, LEVEL_DEFAULT, LEVEL_HIGH, or LEVEL_UNDEFINED.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetBundleImportance(int &importance) = 0;

    /**
     * @brief Checks whether this application has permission to modify the Do Not Disturb (DND) notification policy.
     *
     * @param granted True if the application has permission; false for otherwise.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode HasNotificationPolicyAccessPermission(bool &granted) = 0;

    /**
     * @brief Set whether to allow private notifications.
     *
     * @param allow Indicates the flag that allows private notification.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode SetPrivateNotificationsAllowed(bool allow) = 0;

    /**
     * @brief Get whether to allow private notifications.
     *
     * @param allow Indicates the flag that allows private notification.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetPrivateNotificationsAllowed(bool &allow) = 0;

    /**
     * @brief Delete notification based on key.
     *
     * @param key Indicates the key to delete notification.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode Delete(const std::string &key) = 0;

    /**
     * @brief Delete notification.
     *
     * @param bundleOption Indicates the NotificationBundleOption of the notification.
     * @param notificationId Indicates the id of the notification.
     * @param label Indicates the label of the notification.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode RemoveNotification(
        const sptr<NotificationBundleOption> &bundleOption, int notificationId, const std::string &label) = 0;

    /**
     * @brief Delete all notifications.
     *
     * @param bundleOption Indicates the NotificationBundleOption of notifications.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode RemoveAllNotifications(const sptr<NotificationBundleOption> &bundleOption) = 0;

    /**
     * @brief Remove notifications based on bundle.
     *
     * @param bundleOption Indicates the NotificationBundleOption of notifications.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode DeleteByBundle(const sptr<NotificationBundleOption> &bundleOption) = 0;

    /**
     * @brief Remove all notifications.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode DeleteAll() = 0;

    /**
     * @brief Get all the slots corresponding to the bundle.
     *
     * @param bundleOption Indicates the NotificationBundleOption object.
     * @param slots Indicates the notification slots.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetSlotsByBundle(
        const sptr<NotificationBundleOption> &bundleOption, std::vector<sptr<NotificationSlot>> &slots) = 0;

    /**
     * @brief Update slots according to bundle.
     *
     * @param bundleOption Indicates the NotificationBundleOption object.
     * @param slots Indicates the notification slots to be updated.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode UpdateSlots(
        const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlot>> &slots) = 0;

    /**
     * @brief Update slotgroup according to bundle.
     *
     * @param bundleOption Indicates the NotificationBundleOption object.
     * @param groups Indicates the notification slots to be updated.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode UpdateSlotGroups(
        const sptr<NotificationBundleOption> &bundleOption, const std::vector<sptr<NotificationSlotGroup>> &groups) = 0;

    /**
     * @brief Allow notifications to be sent based on the deviceId.
     *
     * @param deviceId Indicates the device Id.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode RequestEnableNotification(const std::string &deviceId) = 0;

    /**
     * @brief Set whether to allow the specified deviceId to send notifications for current bundle.
     *
     * @param deviceId Indicates the device Id.
     * @param enabled Indicates the flag that allows notification to be pulished.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode SetNotificationsEnabledForBundle(const std::string &deviceId, bool enabled) = 0;

    /**
     * @brief Set whether to allow the specified deviceId to send notifications for all bundles.
     *
     * @param deviceId Indicates the device Id.
     * @param enabled Indicates the flag that allows notification to be pulished.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode SetNotificationsEnabledForAllBundles(const std::string &deviceId, bool enabled) = 0;

    /**
     * @brief Set whether to allow the specified bundle to send notifications.
     *
     * @param bundleOption Indicates the NotificationBundleOption object.
     * @param enabled Indicates the flag that allows notification to be pulished.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode SetNotificationsEnabledForSpecialBundle(
        const std::string &deviceId, const sptr<NotificationBundleOption> &bundleOption, bool enabled) = 0;

    /**
     * @brief Sets whether the bundle allows the banner to display notification.
     *
     * @param bundleOption Indicates the NotificationBundleOption object.
     * @param enabled Indicates the flag that allows badge to be shown.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode SetShowBadgeEnabledForBundle(const sptr<NotificationBundleOption> &bundleOption, bool enabled) = 0;

    /**
     * @brief Gets whether the bundle allows the badge to display the status of notifications.
     *
     * @param bundleOption Indicates the NotificationBundleOption object.
     * @param enabled Indicates the flag that allows badge to be shown.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetShowBadgeEnabledForBundle(const sptr<NotificationBundleOption> &bundleOption, bool &enabled) = 0;

    /**
     * @brief Gets whether allows the badge to display the status of notifications.
     *
     * @param enabled Indicates the flag that allows badge to be shown.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetShowBadgeEnabled(bool &enabled) = 0;

    /**
     * @brief Subscribes notifications.
     *
     * @param subscriber Indicates the subscriber.
     * @param info Indicates the NotificationSubscribeInfo object.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode Subscribe(const sptr<IAnsSubscriber> &subscriber, const sptr<NotificationSubscribeInfo> &info) = 0;

    /**
     * @brief Unsubscribes notifications.
     *
     * @param subscriber Indicates the subscriber.
     * @param info Indicates the NotificationSubscribeInfo object.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode Unsubscribe(
        const sptr<IAnsSubscriber> &subscriber, const sptr<NotificationSubscribeInfo> &info) = 0;

    /**
     * @brief Obtains whether notifications are suspended.
     *
     * @param suspended Indicates the suspended status.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode AreNotificationsSuspended(bool &suspended) = 0;

    /**
     * @brief Get the notification sorting status of the current app.
     *
     * @param sortingMap Indicates the NotificationSortingMap object.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetCurrentAppSorting(sptr<NotificationSortingMap> &sortingMap) = 0;

    /**
     * @brief Checks whether this device is allowed to publish notifications.
     *
     * @param allowed Indicates the flag that allows notification.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode IsAllowedNotify(bool &allowed) = 0;

    /**
     * @brief Checks whether this application is allowed to publish notifications.
     *
     * @param allowed Indicates the flag that allows notification.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode IsAllowedNotifySelf(bool &allowed) = 0;

    /**
     * @brief Checks whether notifications are allowed for a specific bundle.
     *
     * @param bundleOption Indicates the NotificationBundleOption object.
     * @param allowed Indicates the flag that allows notification.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode IsSpecialBundleAllowedNotify(const sptr<NotificationBundleOption> &bundleOption, bool &allowed) = 0;

    /**
     * @brief Set do not disturb date.
     *
     * @param date Indicates the NotificationDoNotDisturbDate object.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode SetDoNotDisturbDate(const sptr<NotificationDoNotDisturbDate> &date) = 0;

    /**
     * @brief Get do not disturb date.
     *
     * @param date Indicates the NotificationDoNotDisturbDate object.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetDoNotDisturbDate(sptr<NotificationDoNotDisturbDate> &date) = 0;

    /**
     * @brief Get whether Do Not Disturb mode is supported.
     *
     * @param doesSupport Indicates the flag that supports DND mode.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode DoesSupportDoNotDisturbMode(bool &doesSupport) = 0;

    /**
     * @brief Cancel notifications according to group.
     *
     * @param groupName Indicates the group name.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode CancelGroup(const std::string &groupName) = 0;

    /**
     * @brief Delete notifications according to bundle and group.
     *
     * @param bundleOption Indicates the NotificationBundleOption object.
     * @param groupName Indicates the group name.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode RemoveGroupByBundle(
        const sptr<NotificationBundleOption> &bundleOption, const std::string &groupName) = 0;

    /**
     * @brief Gets whether distributed notification is enabled.
     *
     * @param enabled Indicates the enabled flag.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode IsDistributedEnabled(bool &enabled) = 0;

    /**
     * @brief Sets distributed notification enabled or disabled.
     *
     * @param enabled Indicates the enabled flag.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode EnableDistributed(bool enabled) = 0;

    /**
     * @brief Sets distributed notification enabled or disabled for specific bundle.
     *
     * @param bundleOption Indicates the NotificationBundleOption object.
     * @param enabled Indicates the enabled flag.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode EnableDistributedByBundle(const sptr<NotificationBundleOption> &bundleOption, bool enabled) = 0;

    /**
     * @brief Sets distributed notification enabled or disabled for current bundle.
     *
     * @param enabled Indicates the enabled flag.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode EnableDistributedSelf(bool enabled) = 0;

    /**
     * @brief Gets whether distributed notification is enabled for specific bundle.
     *
     * @param bundleOption Indicates the NotificationBundleOption object.
     * @param enabled Indicates the enabled flag.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode IsDistributedEnableByBundle(const sptr<NotificationBundleOption> &bundleOption, bool &enabled) = 0;

    /**
     * @brief Get the reminder type of the current device.
     *
     * @param remindType Reminder type for the device.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetDeviceRemindType(NotificationConstant::RemindType &remindType) = 0;

    /**
     * @brief Dump current running status for debuging.
     *
     * @param dumpOption Indicates the dump action that needs to be performed.
     * @param dumpInfo Indicates the dump information.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode ShellDump(const std::string &dumpOption, std::vector<std::string> &dumpInfo) = 0;

    /**
     * @brief Publishes a continuous notification.
     *
     * @param request Notification requests that need to be posted.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode PublishContinuousTaskNotification(const sptr<NotificationRequest> &request) = 0;

    /**
     * @brief Cancels a continuous notification.
     *
     * @param label Identifies the label of the specified notification.
     * @param notificationId Identifies the id of the specified notification.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode CancelContinuousTaskNotification(const std::string &label, int32_t notificationId) = 0;

    /**
     * @brief Publishes a reminder notification.
     *
     * @param reminder Identifies the reminder notification request that needs to be published.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode PublishReminder(sptr<ReminderRequest> &reminder) = 0;

    /**
     * @brief Cancel a reminder notifications.
     *
     * @param reminderId Identifies the reminders id that needs to be canceled.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode CancelReminder(const int32_t reminderId) = 0;

    /**
     * @brief Get all valid reminder notifications.
     *
     * @param reminders Identifies the list of all valid notifications.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetValidReminders(std::vector<sptr<ReminderRequest>> &reminders) = 0;

    /**
     * @brief Cancel all reminder notifications.
     *
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode CancelAllReminders() = 0;

    /**
     * @brief Checks whether this device is support template.
     *
     * @param templateName Identifies the template name for searching as a condition.
     * @param support Identifies the support flag.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode IsSupportTemplate(const std::string &templateName, bool &support) = 0;

    /**
     * @brief Checks Whether the specified users is allowed to publish notifications.
     *
     * @param userId Identifies the user's id.
     * @param allowed Identifies the allowed flag.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode IsSpecialUserAllowedNotify(const int32_t &userId, bool &allowed) = 0;

    /**
     * @brief Sets whether to allow all applications to publish notifications on a specified device. The caller must
     * have system permissions to call this method.
     *
     * @param deviceId Indicates the ID of the device running the application. At present, this parameter can only
     *                 be null or an empty string, indicating the current device.
     * @param enabled Specifies whether to allow all applications to publish notifications. The value true
     *                indicates that notifications are allowed, and the value false indicates that notifications
     *                are not allowed.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode SetNotificationsEnabledByUser(const int32_t &deviceId, bool enabled) = 0;

    /**
     * @brief Delete all notifications by user.
     *
     * @param userId Indicates the user id.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode DeleteAllByUser(const int32_t &userId) = 0;

    /**
     * @brief Set do not disturb date by user.
     *
     * @param userId Indicates the user id.
     * @param date Indicates NotificationDoNotDisturbDate object.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode SetDoNotDisturbDate(const int32_t &userId, const sptr<NotificationDoNotDisturbDate> &date) = 0;

    /**
     * @brief Get the do not disturb date by user.
     *
     * @param userId Indicates the user id.
     * @param date Indicates the NotificationDoNotDisturbDate object.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual ErrCode GetDoNotDisturbDate(const int32_t &userId, sptr<NotificationDoNotDisturbDate> &date) = 0;
    virtual ErrCode SetEnabledForBundleSlot(const sptr<NotificationBundleOption> &bundleOption,
        const NotificationConstant::SlotType &slotType, bool enabled) = 0;
    virtual ErrCode GetEnabledForBundleSlot(const sptr<NotificationBundleOption> &bundleOption,
        const NotificationConstant::SlotType &slotType, bool &enabled) = 0;

protected:
    enum TransactId : uint32_t {
        PUBLISH_NOTIFICATION = FIRST_CALL_TRANSACTION,
        PUBLISH_NOTIFICATION_TO_DEVICE,
        CANCEL_NOTIFICATION,
        CANCEL_ALL_NOTIFICATIONS,
        ADD_SLOT_BY_TYPE,
        ADD_SLOTS,
        REMOVE_SLOT_BY_TYPE,
        REMOVE_ALL_SLOTS,
        ADD_SLOT_GROUPS,
        GET_SLOT_BY_TYPE,
        GET_SLOTS,
        GET_SLOT_GROUP,
        GET_SLOT_GROUPS,
        GET_SLOT_NUM_AS_BUNDLE,
        REMOVE_SLOT_GROUPS,
        GET_ACTIVE_NOTIFICATIONS,
        GET_ACTIVE_NOTIFICATION_NUMS,
        GET_ALL_ACTIVE_NOTIFICATIONS,
        GET_SPECIAL_ACTIVE_NOTIFICATIONS,
        SET_NOTIFICATION_AGENT,
        GET_NOTIFICATION_AGENT,
        CAN_PUBLISH_AS_BUNDLE,
        PUBLISH_AS_BUNDLE,
        SET_NOTIFICATION_BADGE_NUM,
        GET_BUNDLE_IMPORTANCE,
        IS_NOTIFICATION_POLICY_ACCESS_GRANTED,
        SET_PRIVATIVE_NOTIFICATIONS_ALLOWED,
        GET_PRIVATIVE_NOTIFICATIONS_ALLOWED,
        REMOVE_NOTIFICATION,
        REMOVE_ALL_NOTIFICATIONS,
        DELETE_NOTIFICATION,
        DELETE_NOTIFICATION_BY_BUNDLE,
        DELETE_ALL_NOTIFICATIONS,
        GET_SLOTS_BY_BUNDLE,
        UPDATE_SLOTS,
        UPDATE_SLOT_GROUPS,
        REQUEST_ENABLE_NOTIFICATION,
        SET_NOTIFICATION_ENABLED_FOR_BUNDLE,
        SET_NOTIFICATION_ENABLED_FOR_ALL_BUNDLE,
        SET_NOTIFICATION_ENABLED_FOR_SPECIAL_BUNDLE,
        SET_SHOW_BADGE_ENABLED_FOR_BUNDLE,
        GET_SHOW_BADGE_ENABLED_FOR_BUNDLE,
        GET_SHOW_BADGE_ENABLED,
        SUBSCRIBE_NOTIFICATION,
        UNSUBSCRIBE_NOTIFICATION,
        ARE_NOTIFICATION_SUSPENDED,
        GET_CURRENT_APP_SORTING,
        IS_ALLOWED_NOTIFY,
        IS_ALLOWED_NOTIFY_SELF,
        IS_SPECIAL_BUNDLE_ALLOWED_NOTIFY,
        SET_DO_NOT_DISTURB_DATE,
        GET_DO_NOT_DISTURB_DATE,
        DOES_SUPPORT_DO_NOT_DISTURB_MODE,
        CANCEL_GROUP,
        REMOVE_GROUP_BY_BUNDLE,
        IS_DISTRIBUTED_ENABLED,
        ENABLE_DISTRIBUTED,
        ENABLE_DISTRIBUTED_BY_BUNDLE,
        ENABLE_DISTRIBUTED_SELF,
        IS_DISTRIBUTED_ENABLED_BY_BUNDLE,
        GET_DEVICE_REMIND_TYPE,
        SHELL_DUMP,
        PUBLISH_CONTINUOUS_TASK_NOTIFICATION,
        CANCEL_CONTINUOUS_TASK_NOTIFICATION,
        PUBLISH_REMINDER,
        CANCEL_REMINDER,
        CANCEL_ALL_REMINDERS,
        GET_ALL_VALID_REMINDERS,
        IS_SUPPORT_TEMPLATE,
        IS_SPECIAL_USER_ALLOWED_NOTIFY,
        SET_NOTIFICATION_ENABLED_BY_USER,
        DELETE_ALL_NOTIFICATIONS_BY_USER,
        SET_DO_NOT_DISTURB_DATE_BY_USER,
        GET_DO_NOT_DISTURB_DATE_BY_USER,
        SET_ENABLED_FOR_BUNDLE_SLOT,
        GET_ENABLED_FOR_BUNDLE_SLOT
    };
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_ANS_MANAGER_INTERFACE_H
