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

/**
 * Describes a NotificationUserInput instance.
 *
 * @name NotificationUserInput
 * @since 8
 * @permission N/A
 * @syscap SystemCapability.Notification.Notification
 */
export interface NotificationUserInput {
  /**
   * Obtains the key used to identify this input when the input is collected from the user.
   */
  inputKey: string;

  /**
   * Obtains the tag to be displayed to the user when collecting this input from the user.
   */
  tag: string;

  /**
   * Obtains the options provided to users to satisfy user input needs. All options are displayed in a single line.
   */
  options: Array<string>;

  /**
   * Obtains whether users can input values of the given MIME type.
   */
  permitMimeTypes?: Array<string>;

  /**
   * Obtains the edit type of the options provided by this {@code NotificationUserInput} object.
   */
  editType?: number;

  /**
   * Obtains additional data to this NotificationUserInput.
   */
  additionalData?: {[key: string]: Object};
}
