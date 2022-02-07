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
import notification from './@ohos.notification';
import { NotificationSlot } from './notification/notificationSlot';

/**
 * Providers static methods for managing reminders, including publishing or canceling a reminder.
 * adding or removing a notification slot, and obtaining or cancelling all reminders of the current application.
 *
 * @since 7
 * @sysCap SystemCapability.Notification.ReminderAgent
 * @import reminderAgent from '@ohos.reminderAgent';
 */
declare namespace reminderAgent {
  /**
   * Publishes a scheduled reminder.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   * @permission ohos.permission.PUBLISH_AGENT_REMINDER
   * @param reminderReq Indicates the reminder instance to publish.
   * @param callback Indicates the callback function.
   * @returns reminder id.
   */
  function publishReminder(reminderReq: ReminderRequest, callback: AsyncCallback<number>): void;
  function publishReminder(reminderReq: ReminderRequest): Promise<number>;

  /**
   * Cancels a reminder.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   * @param reminderId Indicates the reminder id.
   * @param callback Indicates the callback function.
   */
  function cancelReminder(reminderId: number, callback: AsyncCallback<void>): void;
  function cancelReminder(reminderId: number): Promise<void>;

  /**
   * Obtains all the valid reminders of current application.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   * @param callback Indicates the callback function.
   */
  function getValidReminders(callback: AsyncCallback<Array<ReminderRequest>>): void;
  function getValidReminders(): Promise<Array<ReminderRequest>>;

  /**
   * Cancels all the reminders of current application.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   * @param callback Indicates the callback function.
   */
  function cancelAllReminders(callback: AsyncCallback<void>): void;
  function cancelAllReminders(): Promise<void>;

  /**
   * Add notification slot.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   * @param slot Indicates the slot.
   * @param callback Indicates the callback function.
   */
  function addNotificationSlot(slot: NotificationSlot, callback: AsyncCallback<void>): void;
  function addNotificationSlot(slot: NotificationSlot): Promise<void>;

  /**
   * Deletes a created notification slot based on the slot type.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   * @param slotType Indicates the type of the slot.
   * @param callback Indicates the callback function.
   */
  function removeNotificationSlot(slotType: notification.SlotType, callback: AsyncCallback<void>): void;
  function removeNotificationSlot(slotType: notification.SlotType): Promise<void>;

  /**
   * Declares action button type.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   */
  export enum ActionButtonType {
    /**
     * Button for closing the reminder.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    ACTION_BUTTON_TYPE_CLOSE = 0,

    /**
     * Button for snoozing the reminder.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    ACTION_BUTTON_TYPE_SNOOZE = 1
  }

  /**
   * Declares reminder type.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   */
  export enum ReminderType {
    /**
     * Countdown reminder.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    REMINDER_TYPE_TIMER = 0,

    /**
     * Calendar reminder.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    REMINDER_TYPE_CALENDAR = 1,

    /**
     * Alarm reminder.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    REMINDER_TYPE_ALARM = 2
  }

  /**
   * Action button information. The button will show on displayed reminder.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   */
  interface ActionButton {
    /**
     * Text on the button.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    title: string;

    /**
     * Button type.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    type: ActionButtonType;
  }

  /**
   * Want agent information.
   * It will switch to target ability when you click the displayed reminder.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   */
  interface WantAgent {
    /**
     * Name of the package redirected to when the reminder notification is clicked.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    pkgName: string;

    /**
     * Name of the ability that is redirected to when the reminder notification is clicked.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    abilityName: string;
  }

  /**
   * Reminder Common information.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   */
  interface ReminderRequest {
    /**
     * Type of the reminder.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    reminderType: ReminderType;

    /**
     * Action button displayed on the reminder notification.
     * (The parameter is optional. Up to two buttons are supported).
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    actionButton?: [ActionButton?, ActionButton?];

    /**
     * Information about the ability that is redirected to when the notification is clicked.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    wantAgent?: WantAgent;

    /**
     * Reminder title.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    title?: string;

    /**
     * Reminder content.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    content?: string;

    /**
     * Content to be displayed when the reminder is snoozing.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    expiredContent?: string;

    /**
     * notification id. If there are reminders with the same ID, the later one will overwrite the earlier one.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    notificationId?: number;

    /**
     * Type of the slot used by the reminder.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    slotType?: notification.SlotType;
  }

  /**
   * Alarm reminder information.
   *
   * @since 7
   * @sysCap Define alarm reminder object.
   */
  interface ReminderRequestAlarm extends ReminderRequest {
    /**
     * Hour portion of the reminder time.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    hour: number;

    /**
     * minute portion of the remidner time.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    minute: number;

    /**
     * Days of a week when the reminder repeates.
     * @since 7
     * @sysCap SystemCapability.Notification.ReminderAgent
     */
    daysOfWeek?: Array<number>;
  }

  /**
   * CountDown reminder information.
   *
   * @since 7
   * @sysCap SystemCapability.Notification.ReminderAgent
   */
  interface ReminderRequestTimer extends ReminderRequest {
    triggerTimeInSeconds: number;
  }
}
export default reminderAgent;