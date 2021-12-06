/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { AsyncCallback } from './basic';
import { NotificationSlot } from './notification/notificationSlot';
import { NotificationSubscriber } from './notification/notificationSubscriber';
import { NotificationSubscribeInfo } from './notification/notificationSubscribeInfo';
import { NotificationRequest } from './notification/notificationRequest';

/**
 * Manages notifications.
 *
 * <p>Generally, only system applications have permissions on notification subscription and unsubscription.
 * You can specify the content of a notification to be published and the content is carried by
 * {@link NotificationRequest}. A notification ID is unique in an application and must be specified
 * when using {@link NotificationRequest} to carry the notification content. If a notification
 * with this ID has been published and you need to use this ID to publish another notification,
 * the original notification will be updated. In addition, the notification ID can be used to cancel
 * a notification by calling the {@link #cancel(int)} method.
 *
 * @name notification
 * @since 7
 * @sysCap SystemCapability.Notification.ANS
 * @devices phone, tablet, tv, wearable, car
 * @import import notification from '@ohos.notification';
 * @permission N/A
 */
declare namespace notification {
  /**
   * Publishes a notification.
   *
   * <p>If a notification with the same ID has been published by the current application and has not been deleted,
   * this method will update the notification.
   *
   * @param Publishes a notification.
   */
  function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
  function publish(request: NotificationRequest): Promise<void>;

  /**
   * Cancels a notification with the specified ID.
   *
   * @param ID of the notification to cancel, which must be unique in the application.
   */
  function cancel(id: number, callback: AsyncCallback<void>): void;

  /**
   * Cancels a notification with the specified label and ID.
   *
   * @param ID of the notification to cancel, which must be unique in the application.
   * @param Label of the notification to cancel.
   */
  function cancel(id: number, label: string, callback: AsyncCallback<void>): void;
  function cancel(id: number, label?: string): Promise<void>;

  /**
   * Cancels all notifications of the current application.
   */
  function cancelAll(callback: AsyncCallback<void>): void;
  function cancelAll(): Promise<void>;

  /**
   * Creates a notification slot.
   *
   * @param slot Indicates the notification slot to be created, which is set by {@link NotificationSlot}.
   * This parameter must be specified.
   *
   * @systemapi Hide this for inner system use.
   */
  function addSlot(slot: NotificationSlot, callback: AsyncCallback<void>): void;

  /**
   * Creates a notification slot.
   *
   * @param slot Indicates the notification slot to be created, which is set by {@link NotificationSlot}.
   * This parameter must be specified.
   *
   * @systemapi Hide this for inner system use.
   */
  function addSlot(slot: NotificationSlot): Promise<void>;

  /**
   * Adds a slot type.
   *
   * @param Slot type to add.
   */
  function addSlot(type: SlotType, callback: AsyncCallback<void>): void;
  function addSlot(type: SlotType): Promise<void>;

  /**
   * Creates a notification slot.
   *
   * @param slots Indicates the notification slots to be created, which is set by {@link NotificationSlot}.
   * This parameter must be specified.
   *
   * @systemapi Hide this for inner system use.
   */
  function addSlots(slots: Array<NotificationSlot>, callback: AsyncCallback<void>): void;

  /**
   * Creates a notification slot.
   *
   * @param slots Indicates the notification slots to be created, which is set by {@link NotificationSlot}.
   * This parameter must be specified.
   *
   * @systemapi Hide this for inner system use.
   */
  function addSlots(slots: Array<NotificationSlot>): Promise<void>;

  /**
   * Obtains a notification slot of the specified slot type.
   *
   * @param Type of the notification slot to obtain.
   *
   * @return Returns the created {@link NotificationSlot}.
   */
  function getSlot(slotType: SlotType, callback: AsyncCallback<NotificationSlot>): void;
  function getSlot(slotType: SlotType): Promise<NotificationSlot>;

  /**
   * Obtains all NotificationSlot objects created by the current application.
   *
   * @return Returns all notification slots of this application.
   */
  function getSlots(callback: AsyncCallback<Array<NotificationSlot>>): void;
  function getSlots(): Promise<Array<NotificationSlot>>;

  /**
   * Removes a NotificationSlot of the specified SlotType created by the current application.
   *
   * @param Type of the NotificationSlot to remove.
   */
  function removeSlot(slotType: SlotType, callback: AsyncCallback<void>): void;
  function removeSlot(slotType: SlotType): Promise<void>;

  /**
   * Removes all NotificationSlot objects created by the current application.
   */
  function removeAllSlots(callback: AsyncCallback<void>): void;
  function removeAllSlots(): Promise<void>;

  /**
   * Describes NotificationSlot types.
   */
  export enum SlotType {
    /**
    * NotificationSlot of an unknown type.
    */
    UNKNOWN_TYPE = 0,

    /**
    * NotificationSlot for social communication.
    */
    SOCIAL_COMMUNICATION = 1,

    /**
    * NotificationSlot for service information.
    */
    SERVICE_INFORMATION = 2,

    /**
    * NotificationSlot for service information.
    */
    CONTENT_INFORMATION = 3,

    /**
    * NotificationSlot for other purposes.
    */
    OTHER_TYPES = 0xFFFF,
  }

  /**
   * Describes notification content types.
   *
   * @name ContentType
   * @since 7
   * @sysCap SystemCapability.Notification.ANS
   * @devices phone, tablet, tv, wearable, car
   * @permission N/A
   */
  export enum ContentType {
    /**
    * Normal text notification.
    */
    NOTIFICATION_CONTENT_BASIC_TEXT,

    /**
    * Long text notification.
    */
    NOTIFICATION_CONTENT_LONG_TEXT,

    /**
    * Picture-attached notification.
    */
    NOTIFICATION_CONTENT_PICTURE,

    /**
    * Conversation notification.
    */
    NOTIFICATION_CONTENT_CONVERSATION,

    /**
    * Multi-line text notification.
    */
    NOTIFICATION_CONTENT_MULTILINE,
  }

  /**
   * Indicates the level of the slot
   */
  export enum SlotLevel {
    /**
    * Indicates that the notification function is disabled.
    */
    LEVEL_NONE = 0,

    /**
    * Indicates that the notification function is enabled but notification
    * icons are not displayed in the status bar, with no banner or prompt tone.
    */
    LEVEL_MIN = 1,

    /**
    * Indicates that the notification function is enabled and notification
    * icons are displayed in the status bar, with no banner or prompt tone.
    */
    LEVEL_LOW = 2,

    /**
    * Indicates that the notification function is enabled and notification
    * icons are displayed in the status bar, with no banner but with a prompt tone.
    */
    LEVEL_DEFAULT = 3,

    /**
    * Indicates that the notification function is enabled and notification
    * icons are displayed in the status bar, with a banner and a prompt tone.
    */
    LEVEL_HIGH = 4,
  }

  /**
   * subscribe
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function subscribe(subscriber: NotificationSubscriber, callback: AsyncCallback<void>): void;

  /**
   * subscribe
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function subscribe(subscriber: NotificationSubscriber, info: NotificationSubscribeInfo, callback: AsyncCallback<void>): void;

  /**
   * subscribe
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function subscribe(subscriber: NotificationSubscriber, info?: NotificationSubscribeInfo): Promise<void>;

  /**
   * unsubscribe
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function unsubscribe(subscriber: NotificationSubscriber, callback: AsyncCallback<void>): void;

  /**
   * unsubscribe
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function unsubscribe(subscriber: NotificationSubscriber): Promise<void>;

  /**
   * enableNotification
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function enableNotification(bundle: BundleOption, enable: boolean, callback: AsyncCallback<void>): void;

  /**
   * enableNotification
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function enableNotification(bundle: BundleOption, enable: boolean): Promise<void>;

  /**
   * isNotificationEnabled
   *
   * @systemapi Hide this for inner system use.
   */
  function isNotificationEnabled(bundle: BundleOption, callback: AsyncCallback<boolean>): void;

  /**
   * isNotificationEnabled
   *
   * @systemapi Hide this for inner system use.
   */
  function isNotificationEnabled(bundle: BundleOption): Promise<boolean>;

  /**
   * isNotificationEnabled
   *
   * @systemapi Hide this for inner system use.
   */
  function isNotificationEnabled(callback: AsyncCallback<boolean>): void;

  /**
   * isNotificationEnabled
   *
   * @systemapi Hide this for inner system use.
   */
  function isNotificationEnabled(): Promise<boolean>;

  /**
   * displayBadge
   *
   * @systemapi Hide this for inner system use.
   */
  function displayBadge(bundle: BundleOption, enable: boolean, callback: AsyncCallback<void>): void;

  /**
   * displayBadge
   *
   * @systemapi Hide this for inner system use.
   */
  function displayBadge(bundle: BundleOption, enable: boolean): Promise<void>;

  /**
   * isBadgeDisplayed
   *
   * @systemapi Hide this for inner system use.
   */
  function isBadgeDisplayed(bundle: BundleOption, callback: AsyncCallback<boolean>): void;

  /**
   * isBadgeDisplayed
   *
   * @systemapi Hide this for inner system use.
   */
  function isBadgeDisplayed(bundle: BundleOption): Promise<boolean>;

  /**
   * setSlotByBundle
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function setSlotByBundle(bundle: BundleOption, slot: NotificationSlot, callback: AsyncCallback<void>): void;

  /**
   * setSlotByBundle
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;

  /**
   * getSlotsByBundle
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getSlotsByBundle(bundle: BundleOption, callback: AsyncCallback<Array<NotificationSlot>>): void;

  /**
   * getSlotsByBundle
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getSlotsByBundle(bundle: BundleOption): Promise<Array<NotificationSlot>>;

  /**
   * getSlotNumByBundle
   *
   * @systemapi Hide this for inner system use.
   */
  function getSlotNumByBundle(bundle: BundleOption, callback: AsyncCallback<number>): void;

  /**
   * getSlotNumByBundle
   *
   * @systemapi Hide this for inner system use.
   */
  function getSlotNumByBundle(bundle: BundleOption): Promise<number>;

  /**
   * remove
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function remove(bundle: BundleOption, notificationKey: NotificationKey, callback: AsyncCallback<void>): void;

  /**
   * remove
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function remove(bundle: BundleOption, notificationKey: NotificationKey): Promise<void>;

  /**
   * remove
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function remove(hashCode: string, callback: AsyncCallback<void>): void;

  /**
   * remove
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function remove(hashCode: string): Promise<void>;

  /**
   * removeAll
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function removeAll(bundle: BundleOption, callback: AsyncCallback<void>): void;

  /**
   * removeAll
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function removeAll(callback: AsyncCallback<void>): void;

  /**
   * removeAll
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function removeAll(bundle?: BundleOption): Promise<void>;

  /**
   * Obtains all active notifications in the current system. The caller must have system permissions to
   * call this method.
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getAllActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;

  /**
   * Obtains all active notifications in the current system. The caller must have system permissions to
   * call this method.
   *
   * @systemapi Hide this for inner system use.
   * @permission ohos.permission.NOTIFICATION_CONTROLLER
   */
  function getAllActiveNotifications(): Promise<Array<NotificationRequest>>;

  /**
   * Obtains the number of all active notifications.
   */
  function getActiveNotificationCount(callback: AsyncCallback<number>): void;
  function getActiveNotificationCount(): Promise<number>;

  /**
   * Obtains an array of active notifications.
   */
  function getActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
  function getActiveNotifications(): Promise<Array<NotificationRequest>>;

  /**
   * Cancel the notification of a specified group for this application.
   */
  function cancelGroup(groupName: string, callback: AsyncCallback<void>): void;
  function cancelGroup(groupName: string): Promise<void>;

  /**
   * Delete the notification of a specified group for this application.
   *
   * @systemapi Hide this for inner system use.
   */
  function removeGroupByBundle(bundle: BundleOption, groupName: string, callback: AsyncCallback<void>): void;
  function removeGroupByBundle(bundle: BundleOption, groupName: string): Promise<void>;

  /**
   * Set the Do Not Disturb date.
   *
   * @systemapi Hide this for inner system use.
   */
  function setDoNotDisturbDate(date: DoNotDisturbDate, callback: AsyncCallback<void>): void;
  function setDoNotDisturbDate(date: DoNotDisturbDate): Promise<void>;

  /**
   * Obtains the Do Not Disturb date.
   *
   * @systemapi Hide this for inner system use.
   */
  function getDoNotDisturbDate(callback: AsyncCallback<DoNotDisturbDate>): void;
  function getDoNotDisturbDate(): Promise<DoNotDisturbDate>;

  /**
   * Obtains whether to support the Do Not Disturb mode.
   *
   * @systemapi Hide this for inner system use.
   */
  function supportDoNotDisturbMode(callback: AsyncCallback<boolean>): void;
  function supportDoNotDisturbMode(): Promise<boolean>;

  /**
   * Describes a BundleOption.
   */
  export interface BundleOption {
    bundle: string;
    uid?: number;
  }

  /**
   * Describes a NotificationKey, which can be used to identify a notification.
   */
  export interface NotificationKey {
    id: number;
    label?: string;
  }

  /**
   * DisturbMode
   *
   * @systemapi Hide this for inner system use.
   */
  export enum DoNotDisturbMode {
    ALLOW_UNKNOWN,

    /**
     * Indicates that all notifications are allowed to interrupt the user in Do Not Disturb mode.
     */
    ALLOW_ALL,

    /**
     * Indicates that only notifications meeting the specified priority criteria are allowed to interrupt
     * the user in Do Not Disturb mode.
     */
    ALLOW_PRIORITY,

    /**
     * Indicates that no notifications are allowed to interrupt the user in Do Not Disturb mode.
     */
    ALLOW_NONE,

    /**
     * Indicates that only notifications of the {@link NotificationRequest#CLASSIFICATION_ALARM} category
     * are allowed to interrupt the user in Do Not Disturb mode.
     */
    ALLOW_ALARMS
  }

  /**
   * The type of the Do Not Disturb.
   *
   * @systemapi Hide this for inner system use.
   */
   export enum DoNotDisturbType {
    TYPE_NONE = 0,         // 非通知勿扰类型
    TYPE_ONCE = 1,         // 以设置时间段(只看小时和分钟)一次执行勿扰
    TYPE_DAILY = 2,        // 以设置时间段(只看小时和分钟)每天执行勿扰
    TYPE_CLEARLY = 3,      // 以设置时间段(明确年月日时分)执行勿扰
  }

  /**
   * Describes a DoNotDisturbDate instance.
   *
   * @systemapi Hide this for inner system use.
   */
  export interface DoNotDisturbDate {
    /**
     * the type of the Do Not Disturb.
     */
    type: DoNotDisturbType;

    /**
     * the start time of the Do Not Disturb.
     */
    begin: Date;

    /**
     * the end time of the Do Not Disturb.
     */
    end: Date;
  }

  /**
   * Notification source type
   *
   * @systemapi Hide this for inner system use.
   */
  export enum SourceType {
    TYPE_NORMAL   = 0x00000000,      // 普通通知
    TYPE_CONTINUOUS = 0x00000001,    // 长时任务通知
    TYPE_TIMER    = 0x00000002,      // 定时通知
  }
}

export default notification;
