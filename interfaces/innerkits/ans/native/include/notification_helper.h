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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NATIVE_INCLUDE_NOTIFICATION_HELPER_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NATIVE_INCLUDE_NOTIFICATION_HELPER_H

#include "notification_bundle_option.h"
#include "notification_do_not_disturb_date.h"
#include "enabled_notification_callback_data.h"
#include "notification_request.h"
#include "notification_slot.h"
#include "notification_slot_group.h"
#include "notification_sorting_map.h"
#include "notification_subscriber.h"

namespace OHOS {
namespace Notification {
class NotificationHelper {
public:
    /**
     * @brief Creates a notification slot.
     * @note You can call the NotificationRequest::SetSlotType(NotificationConstant::SlotType) method to bind the slot
     * for publishing. A NotificationSlot instance cannot be used directly after being initialized. Instead, you have to
     * call this method to create a notification slot and bind the slot ID to a NotificationRequest object so that the
     * notification published can have all the characteristics set in the NotificationSlot. After a notification slot is
     * created by using this method, only the name and description of the notification slot can be changed. Changes to
     * the other attributes, such as the vibration status and notification tone, will no longer take effect.
     *
     * @param slot Indicates the notification slot to be created, which is set by NotificationSlot.
     *             This parameter must be specified.
     * @return Returns add notification slot result.
     */
    static ErrCode AddNotificationSlot(const NotificationSlot &slot);

    /**
     * @brief Adds a notification slot by type.
     *
     * @param slotType Indicates the notification slot type to be added.
     * @return Returns add notification slot result.
     */
    static ErrCode AddSlotByType(const NotificationConstant::SlotType &slotType);

    /**
     * @brief Creates multiple notification slots.
     *
     * @param slots Indicates the notification slots to create.
     * @return Returns add notification slots result.
     */
    static ErrCode AddNotificationSlots(const std::vector<NotificationSlot> &slots);

    /**
     * @brief Deletes a created notification slot based on the slot ID.
     *
     * @param slotType Indicates the type of the slot, which is created by AddNotificationSlot
     *                 This parameter must be specified.
     * @return Returns remove notification slot result.
     */
    static ErrCode RemoveNotificationSlot(const NotificationConstant::SlotType &slotType);

    /**
     * @brief Deletes all notification slots.
     *
     * @return Returns remove all slots result.
     */
    static ErrCode RemoveAllSlots();

    /**
     * @brief Queries a created notification slot.
     *
     * @param slotType Indicates the ID of the slot, which is created by AddNotificationSlot(NotificationSlot). This
     *        parameter must be specified.
     * @param slot Indicates the created NotificationSlot.
     * @return Returns the get notification slot result.
     */
    static ErrCode GetNotificationSlot(const NotificationConstant::SlotType &slotType, sptr<NotificationSlot> &slot);

    /**
     * @brief Obtains all notification slots of this application.
     * @param slots Indicates the created NotificationSlot.
     * @return Returns all notification slots of this application.
     */
    static ErrCode GetNotificationSlots(std::vector<sptr<NotificationSlot>> &slots);

    /**
     * @brief Creates a notification slot group to which a NotificationSlot object can be bound by
     * calling NotificationSlot::SetSlotGroup(string).
     * @note  A NotificationSlotGroup instance cannot be used directly after being initialized.
     *        Instead, you have to call this method to create a notification slot group so that you can bind
     *        NotificationSlot objects to it.
     *
     * @param slotGroup Indicates the notification slot group to be created, which is set by NotificationSlotGroup.
     *                  This parameter must be specified. the notification slot to be created, which is set by
     *                  NotificationSlot.
     * @return Returns add notification slot group result.
     */
    static ErrCode AddNotificationSlotGroup(const NotificationSlotGroup &slotGroup);

    /**
     * @brief Creates multiple notification slot groups.
     * @note The precautions for using this method are similar to those for
     *       AddNotificationSlotGroup(NotificationSlotGroup).
     *
     * @param slotGroups Indicates a list of NotificationSlotGroup objects to create. This parameter cannot be null.
     * @return Returns add notification slot groups result.
     */
    static ErrCode AddNotificationSlotGroups(const std::vector<NotificationSlotGroup> &slotGroups);

    /**
     * @brief Deletes a created notification slot group based on the slot group ID.
     *
     * @param slotGroupId Indicates the ID of the notification slot group, which is created by
     *                    AddNotificationSlotGroup(NotificationSlotGroup) This parameter must be specified.
     * @return Returns remove notification slot group result.
     */
    static ErrCode RemoveNotificationSlotGroup(const std::string &slotGroupId);

    /**
     * @brief Queries a created notification slot group.
     *
     * @param groupId Indicates the ID of the slot group.
     * @param group   Indicates the created NotificationSlotGroup.
     * @return Returns get notification slot group result.
     */
    static ErrCode GetNotificationSlotGroup(const std::string &groupId, sptr<NotificationSlotGroup> &group);

    /**
     * @brief Obtains a list of created notification slot groups.
     *
     * @param  groups Indicates a list of created notification slot groups.
     * @return Returns get notification slot groups result.
     */
    static ErrCode GetNotificationSlotGroups(std::vector<sptr<NotificationSlotGroup>> &groups);

    /**
     * @brief Obtains number of slot.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param num Indicates number of slot.
     * @return Returns get slot number by bundle result.
     */
    static ErrCode GetNotificationSlotNumAsBundle(const NotificationBundleOption &bundleOption, int &num);

    /**
     * @brief Publishes a notification.
     * @note If a notification with the same ID has been published by the current application and has not been deleted,
     * this method will update the notification.
     *
     * @param request Indicates the NotificationRequest object for setting the notification content.
     *                This parameter must be specified.
     * @return Returns publish notification result.
     */
    static ErrCode PublishNotification(const NotificationRequest &request);

    /**
     * @brief Publishes a notification with a specified label.
     * @note If a notification with the same ID has been published by the current application and has not been deleted,
     *       this method will update the notification.
     *
     * @param label Indicates the label of the notification to publish.
     * @param request Indicates the NotificationRequest object for setting the notification content.
     *                This parameter must be specified.
     * @return Returns publish notification result.
     */
    static ErrCode PublishNotification(const std::string &label, const NotificationRequest &request);

    /**
     * @brief Publishes a notification on a specified remote device.
     * @note If a notification with the same ID has been published by the current application and has not been deleted,
     *       this method will update the notification.
     *
     * @param request Indicates the NotificationRequest object for setting the notification content.
     *                This parameter must be specified.
     * @param deviceId Indicates the ID of the remote device. If this parameter is null or an empty string,
     *                 the notification will be published on the local device.
     * @return Returns publish notification result.
     */
    static ErrCode PublishNotification(const NotificationRequest &request, const std::string &deviceId);

    /**
     * @brief Cancels a published notification.
     *
     * @param notificationId Indicates the unique notification ID in the application.
     *                       The value must be the ID of a published notification.
     *                       Otherwise, this method does not take effect.
     * @return Returns cancel notification result.
     */
    static ErrCode CancelNotification(int32_t notificationId);

    /**
     * @brief Cancels a published notification matching the specified label and notificationId.
     *
     * @param label Indicates the label of the notification to cancel.
     * @param notificationId Indicates the ID of the notification to cancel.
     * @return Returns cancel notification result.
     */
    static ErrCode CancelNotification(const std::string &label, int32_t notificationId);

    /**
     * @brief Cancels all the published notifications.
     *
     * @note To cancel a specified notification, see CancelNotification(int_32).
     * @return Returns cancel all notifications result.
     */
    static ErrCode CancelAllNotifications();

    /**
     * @brief Obtains the number of active notifications of the current application in the system.
     *
     * @param nums Indicates the number of active notifications of the current application.
     * @return Returns get active notification nums result.
     */
    static ErrCode GetActiveNotificationNums(int32_t &num);

    /**
     * @brief Obtains active notifications of the current application in the system.
     *
     * @param  request Indicates active NotificationRequest objects of the current application.
     * @return Returns get active notifications result.
     */
    static ErrCode GetActiveNotifications(std::vector<sptr<NotificationRequest>> &request);

    /**
     * @brief Obtains the map for sorting notifications of the current application.
     *
     * @param sortingMap Indicates the NotificationSortingMap object for the current application.
     * @return Returns get current app sorting result.
     */
    static ErrCode GetCurrentAppSorting(sptr<NotificationSortingMap> &sortingMap);

    /**
     * @brief Allows another application to act as an agent to publish notifications in the name of your application
     * bundle.
     *
     * @param agent Indicates the name of the application bundle that can publish notifications for your application.
     * @return Returns set notification agent result.
     */
    static ErrCode SetNotificationAgent(const std::string &agent);

    /**
     * @brief Obtains the name of the application bundle that can publish notifications in the name of your application.
     *
     * @param agent Indicates the name of the application bundle that can publish notifications for your application if
     * any; returns null otherwise.
     * @return Returns get notification agent result.
     */
    static ErrCode GetNotificationAgent(std::string &agent);

    /**
     * @brief Checks whether your application has permission to publish notifications by calling
     * PublishNotificationAsBundle(string, NotificationRequest) in the name of another application indicated by the
     * given representativeBundle.
     *
     * @param representativeBundle Indicates the name of application bundle your application is representing.
     * @param canPublish Indicates whether your application has permission to publish notifications.
     * @return Returns can publish notification as bundle result.
     */
    static ErrCode CanPublishNotificationAsBundle(const std::string &representativeBundle, bool &canPublish);

    /**
     * @brief Publishes a notification in the name of a specified application bundle.
     * @note If the notification to be published has the same ID as a published notification that has not been canceled,
     * the existing notification will be replaced by the new one.
     *
     * @param request Indicates the NotificationRequest object for setting the notification content.
     *                This parameter must be specified.
     * @param representativeBundle Indicates the name of the application bundle that allows your application to publish
     *                             notifications for it by calling setNotificationAgent.
     * @return Returns publish notification as bundle result.
     */
    static ErrCode PublishNotificationAsBundle(
        const std::string &representativeBundle, const NotificationRequest &request);

    /**
     * @brief Sets the number of active notifications of the current application as the number to be displayed on the
     * notification badge.
     *
     * @return Returns set notification badge num result.
     */
    static ErrCode SetNotificationBadgeNum();

    /**
     * @brief Sets the number to be displayed on the notification badge of the application.
     *
     * @param num Indicates the number to display. A negative number indicates that the badge setting remains unchanged.
     *            The value 0 indicates that no badge is displayed on the application icon.
     *            If the value is greater than 99, 99+ will be displayed.
     * @return Returns set notification badge num result.
     */
    static ErrCode SetNotificationBadgeNum(int32_t num);

    /**
     * @brief Checks whether this application has permission to publish notifications. The caller must have
     * system permissions to call this method.
     *
     * @param  allowed True if this application has the permission; returns false otherwise
     * @return Returns is allowed notify result.
     */
    static ErrCode IsAllowedNotify(bool &allowed);

    /**
     * @brief Checks whether this application has permission to publish notifications.
     *
     * @param  allowed True if this application has the permission; returns false otherwise
     * @return Returns is allowed notify result.
     */
    static ErrCode IsAllowedNotifySelf(bool &allowed);

    /**
     * @brief Allow the current application to publish notifications on a specified device.
     *
     * @param deviceId Indicates the ID of the device running the application. At present, this parameter can
     *                 only be null or an empty string, indicating the current device.
     * @return Returns set notifications enabled for default bundle result.
     */
    static ErrCode RequestEnableNotification(std::string &deviceId);

    /**
     * @brief Checks whether this application is in the suspended state.Applications in this state cannot publish
     * notifications.
     *
     * @param suspended True if this application is suspended; false otherwise.
     * @return Returns are notifications suspended.
     */
    static ErrCode AreNotificationsSuspended(bool &suspended);

    /**
     * @brief Checks whether this application has permission to modify the Do Not Disturb (DND) notification policy.
     *
     * @param hasPermission True if this application is suspended; false otherwise.
     * @return Returns has notification policy access permission.
     */
    static ErrCode HasNotificationPolicyAccessPermission(bool &hasPermission);

    /**
     * @brief Obtains the importance level of this application.
     *
     * @param  importance Indicates the importance level of this application, which can be LEVEL_NONE,
               LEVEL_MIN, LEVEL_LOW, LEVEL_DEFAULT, LEVEL_HIGH, or LEVEL_UNDEFINED.
     * @return Returns get bundle importance result
     */
    static ErrCode GetBundleImportance(NotificationSlot::NotificationLevel &importance);

    /**
     * @brief Subscribes to notifications from all applications. This method can be called only by applications
     * with required system permissions.
     * @note  To subscribe to a notification, inherit the {NotificationSubscriber} class, override its
     *        callback methods and create a subscriber. The subscriber will be used as a parameter of this method.
     *        After the notification is published, subscribers that meet the filter criteria can receive the
     *        notification. To subscribe to notifications published only by specified sources, for example,
     *        notifications from certain applications,
     *        call the {SubscribeNotification(NotificationSubscriber, NotificationSubscribeInfo)} method.
     *
     * @param subscriber Indicates the {NotificationSubscriber} to receive notifications.
     *                   This parameter must be specified.
     * @return Returns unsubscribe notification result.
     */
    static ErrCode SubscribeNotification(const NotificationSubscriber &subscriber);

    /**
     * @brief Subscribes to all notifications based on the filtering criteria. This method can be called only
     * by applications with required system permissions.
     * @note  After {subscribeInfo} is specified, a subscriber receives only the notifications that
     *        meet the filter criteria specified by {subscribeInfo}.
     *        To subscribe to a notification, inherit the {NotificationSubscriber} class, override its
     *        callback methods and create a subscriber. The subscriber will be used as a parameter of this method.
     *        After the notification is published, subscribers that meet the filter criteria can receive the
     *        notification. To subscribe to and receive all notifications, call the
     *        {SubscribeNotification(NotificationSubscriber)} method.
     *
     * @param subscriber Indicates the subscribers to receive notifications. This parameter must be specified.
     *                   For details, see {NotificationSubscriber}.
     * @param subscribeInfo Indicates the filters for specified notification sources, including application name,
     *                      user ID, or device name. This parameter is optional.
     * @return Returns subscribe notification result.
     */
    static ErrCode SubscribeNotification(
        const NotificationSubscriber &subscriber, const NotificationSubscribeInfo &subscribeInfo);

    /**
     * @brief Unsubscribes from all notifications. This method can be called only by applications with required
     * system permissions.
     * @note Generally, you subscribe to a notification by calling the
     *       {SubscribeNotification(NotificationSubscriber)} method. If you do not want your application
     *       to receive a notification any longer, unsubscribe from that notification using this method.
     *       You can unsubscribe from only those notifications that your application has subscribed to.
     *        To unsubscribe from notifications published only by specified sources, for example,
     *       notifications from certain applications, call the
     *       {UnSubscribeNotification(NotificationSubscriber, NotificationSubscribeInfo)} method.
     *
     * @param subscriber Indicates the {NotificationSubscriber} to receive notifications.
     *                   This parameter must be specified.
     * @return Returns unsubscribe notification result.
     */
    static ErrCode UnSubscribeNotification(NotificationSubscriber &subscriber);

    /**
     * @brief Unsubscribes from all notifications based on the filtering criteria. This method can be called
     * only by applications with required system permissions.
     * @note A subscriber will no longer receive the notifications from specified notification sources.
     *
     * @param subscriber Indicates the {NotificationSubscriber} to receive notifications.
     *                   This parameter must be specified.
     * @param subscribeInfo Indicates the filters for , including application name,
     *                      user ID, or device name. This parameter is optional.
     * @return Returns unsubscribe notification result.
     */
    static ErrCode UnSubscribeNotification(NotificationSubscriber &subscriber, NotificationSubscribeInfo subscribeInfo);

    /**
     * @brief Removes a specified removable notification of other applications.
     * @note Your application must have platform signature to use this method.
     *
     * @param key Indicates the key of the notification to remove.
     * @return Returns remove notification result.
     */
    static ErrCode RemoveNotification(const std::string &key);

    /**
     * @brief Removes a specified removable notification of other applications.
     * @note Your application must have platform signature to use this method.
     *
     * @param bundleOption Indicates the bundle name and uid of the application whose notifications are to be removed.
     * @param notificationId Indicates the id of the notification to remove.
     * @param label Indicates the label of the notification to remove.
     * @return Returns remove notification result.
     */
    static ErrCode RemoveNotification(
        const NotificationBundleOption &bundleOption, const int32_t notificationId, const std::string &label);

    /**
     * @brief Removes a specified removable notification of other applications.
     * @note Your application must have platform signature to use this method.
     *
     * @param bundleOption Indicates the bundle name and uid of the application whose notifications are to be removed.
     * @return Returns remove notification result.
     */
    static ErrCode RemoveAllNotifications(const NotificationBundleOption &bundleOption);

    /**
     * @brief Removes all removable notifications of a specified bundle.
     * @note Your application must have platform signature to use this method.
     *
     * @param bundleOption Indicates the bundle name and uid of the application whose notifications are to be removed.
     * @return Returns remove notifications result.
     */
    static ErrCode RemoveNotificationsByBundle(const NotificationBundleOption &bundleOption);

    /**
     * @brief Removes all removable notifications in the system.
     * @note Your application must have platform signature to use this method.
     * @return Returns remove notifications result.
     */
    static ErrCode RemoveNotifications();

    /**
     * @brief Obtains all notification slots belonging to the specified bundle.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param slots Indicates a list of notification slots.
     * @return Returns get notification slots for bundle result.
     */
    static ErrCode GetNotificationSlotsForBundle(
        const NotificationBundleOption &bundleOption, std::vector<sptr<NotificationSlot>> &slots);

    /**
     * @brief Update all notification slots for the specified bundle.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param slots Indicates a list of new notification slots.
     * @return Returns update notification slots for bundle result.
     */
    static ErrCode UpdateNotificationSlots(
        const NotificationBundleOption &bundleOption, const std::vector<sptr<NotificationSlot>> &slots);

    /**
     * @brief Update all notification slot groups for the specified bundle.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param groups Indicates a list of new notification slot groups.
     * @return Returns update notification slot groups for bundle result.
     */
    static ErrCode UpdateNotificationSlotGroups(
        const NotificationBundleOption &bundleOption, const std::vector<sptr<NotificationSlotGroup>> &groups);

    /**
     * @brief Obtains all active notifications in the current system. The caller must have system permissions to
     * call this method.
     *
     * @param notification Indicates all active notifications of this application.
     * @return Returns get all active notifications
     */
    static ErrCode GetAllActiveNotifications(std::vector<sptr<Notification>> &notification);

    /**
     * @brief Obtains the active notifications corresponding to the specified key in the system. To call this method
     * to obtain particular active notifications, you must have received the notifications and obtained the key
     * via {Notification::GetKey()}.
     *
     * @param key Indicates the key array for querying corresponding active notifications.
     *            If this parameter is null, this method returns all active notifications in the system.
     * @param notification Indicates the set of active notifications corresponding to the specified key.
     * @return Returns get all active notifications.
     */
    static ErrCode GetAllActiveNotifications(
        const std::vector<std::string> key, std::vector<sptr<Notification>> &notification);

    /**
     * @brief Checks whether a specified application has the permission to publish notifications. If bundle specifies
     * the current application, no permission is required for calling this method. If bundle specifies another
     * application, the caller must have system permissions.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param allowed True if the application has permissions; false otherwise.
     * @return Returns is allowed notify result.
     */
    static ErrCode IsAllowedNotify(const NotificationBundleOption &bundleOption, bool &allowed);

    /**
     * @brief Sets whether to allow all applications to publish notifications on a specified device. The caller must
     * have system permissions to call this method.
     *
     * @param deviceId Indicates the ID of the device running the application. At present, this parameter can only
     *                 be null or an empty string, indicating the current device.
     * @param enabled Specifies whether to allow all applications to publish notifications. The value true
     *                indicates that notifications are allowed, and the value false indicates that notifications
     *                are not allowed.
     * @return Returns set notifications enabled for all bundles result.
     */
    static ErrCode SetNotificationsEnabledForAllBundles(const std::string &deviceId, bool enabled);

    /**
     * @brief Sets whether to allow the current application to publish notifications on a specified device. The caller
     * must have system permissions to call this method.
     *
     * @param deviceId Indicates the ID of the device running the application. At present, this parameter can
     *                 only be null or an empty string, indicating the current device.
     * @param enabled Specifies whether to allow the current application to publish notifications. The value
     *                true indicates that notifications are allowed, and the value false indicates that
     *                notifications are not allowed.
     * @return Returns set notifications enabled for default bundle result.
     */
    static ErrCode SetNotificationsEnabledForDefaultBundle(const std::string &deviceId, bool enabled);

    /**
     * @brief Sets whether to allow a specified application to publish notifications on a specified device. The caller
     * must have system permissions to call this method.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param deviceId Indicates the ID of the device running the application. At present, this parameter can only
     *                 be null or an empty string, indicating the current device.
     * @param enabled Specifies whether to allow the given application to publish notifications. The value
     *                true indicates that notifications are allowed, and the value false indicates that notifications
     *                are not allowed.
     * @return Returns set notifications enabled for specified bundle result.
     */
    static ErrCode SetNotificationsEnabledForSpecifiedBundle(
        const NotificationBundleOption &bundleOption, std::string &deviceId, bool enabled);

    /**
     * @brief Sets whether to allow a specified application to show badge.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param enabled Specifies whether to allow the given application to show badge.
     * @return Returns set result.
     */
    static ErrCode SetShowBadgeEnabledForBundle(const NotificationBundleOption &bundleOption, bool enabled);

    /**
     * @brief Obtains the flag that whether to allow a specified application to show badge.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param enabled Specifies whether to allow the given application to show badge.
     * @return Returns get result.
     */
    static ErrCode GetShowBadgeEnabledForBundle(const NotificationBundleOption &bundleOption, bool &enabled);

    /**
     * @brief Obtains the flag that whether to allow the current application to show badge.
     *
     * @param enabled Specifies whether to allow the given application to show badge.
     * @return Returns get result.
     */
    static ErrCode GetShowBadgeEnabled(bool &enabled);

    /**
     * @brief Cancel the notification of the specified group of this application.
     *
     * @param groupName Indicates the specified group name.
     * @return Returns cancel group result.
     */
    static ErrCode CancelGroup(const std::string &groupName);

    /**
     * @brief Remove the notification of the specified group of the specified application.
     *
     * @param bundleOption Indicates the bundle name and uid of the specified application.
     * @param groupName Indicates the specified group name.
     * @return Returns remove group by bundle result.
     */
    static ErrCode RemoveGroupByBundle(const NotificationBundleOption &bundleOption, const std::string &groupName);

    /**
     * @brief Sets the do not disturb time.
     * @note Your application must have system signature to call this method.
     *
     * @param doNotDisturbDate Indicates the do not disturb time to set.
     * @return Returns set do not disturb time result.
     */
    static ErrCode SetDoNotDisturbDate(const NotificationDoNotDisturbDate &doNotDisturbDate);

    /**
     * @brief Obtains the do not disturb time.
     * @note Your application must have system signature to call this method.
     *
     * @param doNotDisturbDate Indicates the do not disturb time to get.
     * @return Returns set do not disturb time result.
     */
    static ErrCode GetDoNotDisturbDate(NotificationDoNotDisturbDate &doNotDisturbDate);

    /**
     * @brief Obtains the flag that whether to support do not disturb mode.
     *
     * @param doesSupport Specifies whether to support do not disturb mode.
     * @return Returns check result.
     */
    static ErrCode DoesSupportDoNotDisturbMode(bool &doesSupport);

    /**
     * @brief Check if the device supports distributed notification.
     *
     * @param enabled True if the device supports distributed notification; false otherwise.
     * @return Returns is distributed enabled result.
     */
    static ErrCode IsDistributedEnabled(bool &enabled);

    /**
     * @brief Set whether the device supports distributed notifications.
     *
     * @param enable Specifies whether to enable the device to support distributed notification.
     *               The value true indicates that the device is enabled to support distributed notifications, and
     *               the value false indicates that the device is forbidden to support distributed notifications.
     * @return Returns enable distributed result.
     */
    static ErrCode EnableDistributed(const bool enabled);

    /**
     * @brief Set whether an application supports distributed notifications.
     *
     * @param bundleOption Indicates the bundle name and uid of an application.
     * @param enabled Specifies whether to enable an application to support distributed notification.
     *                The value true indicates that the application is enabled to support distributed notifications,
     *                and the value false indicates that the application is forbidden to support distributed
     *                notifications.
     * @return Returns enable distributed by bundle result.
     */
    static ErrCode EnableDistributedByBundle(const NotificationBundleOption &bundleOption, const bool enabled);

    /**
     * @brief Set whether this application supports distributed notifications.
     *
     * @param enabled Specifies whether to enable this application to support distributed notification.
     *                The value true indicates that this application is enabled to support distributed notifications,
     *                and the value false indicates that this application is forbidden to support distributed
     *                notifications.
     * @return Returns enable distributed self result.
     */
    static ErrCode EnableDistributedSelf(const bool enabled);

    /**
     * @brief Check whether an application supports distributed notifications.
     *
     * @param bundleOption Indicates the bundle name and uid of an application.
     * @param enabled True if the application supports distributed notification; false otherwise.
     * @return Returns is distributed enabled by bundle result.
     */
    static ErrCode IsDistributedEnableByBundle(const NotificationBundleOption &bundleOption, bool &enabled);

    /**
     * @brief Obtains the device remind type.
     * @note Your application must have system signature to call this method.
     *
     * @param remindType Indicates the device remind type to get.
     * @return Returns get device reminder type result.
     */
    static ErrCode GetDeviceRemindType(NotificationConstant::RemindType &remindType);

    /**
     * @brief Publishes a continuous task notification.
     * @param request Indicates the NotificationRequest object for setting the notification content.
     *                This parameter must be specified.
     * @return Returns publish continuous task notification result.
     */
    static ErrCode PublishContinuousTaskNotification(const NotificationRequest &request);

    /**
     * @brief Cancels a published continuous task notification matching the specified label and notificationId.
     *
     * @param label Indicates the label of the continuous task notification to cancel.
     * @param notificationId Indicates the ID of the continuous task notification to cancel.
     * @return Returns cancel continuous task notification result.
     */
    static ErrCode CancelContinuousTaskNotification(const std::string &label, int32_t notificationId);

    /**
     * @brief Obtains whether the template is supported by the system.
     *
     * @param support Indicates whether is it a system supported template.
     * @return Returns check result.
     */
    static ErrCode IsSupportTemplate(const std::string &templateName, bool &support);

    /**
     * @brief Checks whether this application has permission to publish notifications under the user.
     *
     * @param userId Indicates the userId of the application.
     * @param allowed True if the application has permissions; false otherwise.
     * @return Returns get allowed result.
     */
    static ErrCode IsAllowedNotify(const int32_t &userId, bool &allowed);

    /**
     * @brief Sets whether to allow all applications to publish notifications on a specified user.
     * The caller must have system permissions to call this method.
     *
     * @param userId Indicates the ID of the user running the application.
     * @param enabled Specifies whether to allow all applications to publish notifications. The value true
     *                indicates that notifications are allowed, and the value false indicates that notifications
     *                are not allowed.
     * @return Returns set notifications enabled for all bundles result.
     */
    static ErrCode SetNotificationsEnabledForAllBundles(const int32_t &userId, bool enabled);

    /**
     * @brief Removes notifications under specified user.
     * @note Your application must have platform signature to use this method.
     *
     * @param userId Indicates the ID of user whose notifications are to be removed.
     * @return Returns remove notification result.
     */
    static ErrCode RemoveNotifications(const int32_t &userId);

    /**
     * @brief Sets the do not disturb time on a specified user.
     * @note Your application must have system signature to call this method.
     *
     * @param userId Indicates the specific user.
     * @param doNotDisturbDate Indicates the do not disturb time to set.
     * @return Returns set do not disturb time result.
     */
    static ErrCode SetDoNotDisturbDate(const int32_t &userId, const NotificationDoNotDisturbDate &doNotDisturbDate);

    /**
     * @brief Obtains the do not disturb time on a specified user.
     * @note Your application must have system signature to call this method.
     *
     * @param userId Indicates the specific user.
     * @param doNotDisturbDate Indicates the do not disturb time to get.
     * @return Returns set do not disturb time result.
     */
    static ErrCode GetDoNotDisturbDate(const int32_t &userId, NotificationDoNotDisturbDate &doNotDisturbDate);

    /**
     * Set whether the application slot is enabled.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param slotType Indicates type of slot.
     * @param enabled the type of slot enabled.
     * @return Returns get slot number by bundle result.
     */
    static ErrCode SetEnabledForBundleSlot(
        const NotificationBundleOption &bundleOption, const NotificationConstant::SlotType &slotType, bool enabled);

    /**
     * Obtains whether the application slot is enabled.
     *
     * @param bundleOption Indicates the bundle name and uid of the application.
     * @param slotType Indicates type of slot.
     * @param enabled the type of slot enabled to get.
     * @return Returns get slot number by bundle result.
     */
    static ErrCode GetEnabledForBundleSlot(
        const NotificationBundleOption &bundleOption, const NotificationConstant::SlotType &slotType, bool &enabled);
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NATIVE_INCLUDE_NOTIFICATION_HELPER_H