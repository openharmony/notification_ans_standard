/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

import notification from '../@ohos.notification';
import image from '../@ohos.multimedia.image';
import { MessageUser } from './notificationMessageUser';

/**
 * Describes a normal text notification.
 *
 * @name NotificationBasicContent
 * @since 7
 * @syscap SystemCapability.Notification.Notification
 * @permission N/A
 */
export interface NotificationBasicContent {
  /**
   * Title of the normal text notification.
   */
  title: string;

  /**
   * Content of the normal text notification.
   */
  text: string;

  /**
   * Additional information of the normal text notification.
   */
  additionalText?: string;
}

/**
 * Describes a long text notification.
 *
 * @name NotificationLongTextContent
 * @since 7
 * @syscap SystemCapability.Notification.Notification
 * @permission N/A
 */
export interface NotificationLongTextContent extends NotificationBasicContent {
  /**
   * Long text content of the notification.
   */
  longText: string;

  /**
   * Brief text of the long text notification.
   */
  briefText: string;

  /**
   * Title that will be displayed for the long text notification when it is expanded.
   */
  expandedTitle: string;
}

/**
 * Describes a multi-line text notification.
 *
 * @name NotificationMultiLineContent
 * @since 7
 * @syscap SystemCapability.Notification.Notification
 * @permission N/A
 */
export interface NotificationMultiLineContent extends NotificationBasicContent {
  /**
   * Brief text of the multi-line text notification.
   */
  briefText: string;

  /**
   * Brief text of the multi-line text notification.
   */
  longTitle: string;

  /**
   * Multi-line content of the multi-line text notification.
   */
  lines: Array<string>;
}

/**
 * Describes a picture-attached notification.
 *
 * @name NotificationPictureContent
 * @since 7
 * @syscap SystemCapability.Notification.Notification
 * @permission N/A
 */
export interface NotificationPictureContent extends NotificationBasicContent {
  /**
   * Multi-line content of the multi-line text notification.
   */
  briefText: string;

  /**
   * Title that will be displayed for the picture-attached notification when it is expanded.
   */
  expandedTitle: string;

  /**
   * Picture to be included in a notification.
   */
  picture: image.PixelMap;
}

/**
 * Constructs a conversation-like notification that includes message communication among multiple users.
 *
 * @since 8
 */
export interface NotificationConversationalContent extends NotificationBasicContent {
  /**
   * Indicates the {@code MessageUser} who sends all objects in this conversation-like notification.
   * This parameter cannot be null.
   */
  user: MessageUser;

  /**
   * Obtains all messages included in this conversation-like notification.
   */
  messages: Array<ConversationalMessage>;

  /**
   * Checks whether this notification represents a group conversation.
   */
  conversationGroup?: boolean;

  /**
   * Obtains the title to be displayed for the conversation.
   */
  conversationTitle?: string;
}

/**
 * Provides methods for defining a conversational message that is used in notifications created with
 * {@link NotificationConversationalContent}. Each message contains the message content, timestamp, and
 * sender; and the message content and sender information will be displayed in the notification bar.
 *
 * @since 8
 */
export interface ConversationalMessage {
  /**
   * Obtains the text to be displayed as the content of this message.
   */
  text: string;

  /**
   * Obtains the time when this message arrived.
   */
  timestamp: number;

  /**
   * Obtains the sender of this message.
   */
  sender: MessageUser;

  /**
   * Obtains the MIME type of this message.
   */
  mimeType?: string;

  /**
   * Obtains the URI of this message.
   */
  uri?: string;
}

/**
 * Describes notification types.
 *
 * @name NotificationContent
 * @since 7
 * @syscap SystemCapability.Notification.Notification
 * @permission N/A
 */
export interface NotificationContent {
  /**
   * Notification content type.
   */
  contentType: notification.ContentType;

  /**
   * Normal text notification.
   */
  normal?: NotificationBasicContent;

  /**
   * Long text notification.
   */
  longText?: NotificationLongTextContent;

  /**
   * Multi-line text notification.
   */
  multiLine?: NotificationMultiLineContent;

  /**
   * Picture-attached notification.
   */
  picture?: NotificationPictureContent;

  /**
   * Constructs a conversation-like notification that includes message communication among multiple users.
   *
   * @since 8
   */
  conversation?: NotificationConversationalContent;
}
