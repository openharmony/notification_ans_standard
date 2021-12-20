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

import image from '../@ohos.multimedia.image';

/**
 * Represents a message sender that can be used by {@link NotificationRequest},
 *
 * @name MessageUser
 * @since 8
 * @devices phone, tablet, tv, wearable, car
 * @permission N/A
 * @sysCap SystemCapability.Notification.ANS
 */
export interface MessageUser {
  /**
   * Obtains the name of this {@code MessageUser}.
   */
  name: string;

  /**
   * Obtains the key of this {@code MessageUser}.
   */
  key: string;

  /**
   * Obtains the URI of this {@code MessageUser}.
   */
  uri: string;

  /**
   * Checks whether this {@code MessageUser} is important.
   */
  isMachine: boolean;

  /**
   * Checks whether this {@code MessageUser} is a machine.
   */
  isUserImportant: boolean;

  /**
   * Obtains the icon of this {@code MessageUser}.
   */
  icon?: image.PixelMap;
}
