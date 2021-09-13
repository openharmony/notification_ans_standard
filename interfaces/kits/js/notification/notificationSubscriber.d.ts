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

import { NotificationRequest } from './notificationRequest';
import { NotificationSortingMap } from './notificationSortingMap';
import notification from '../@ohos.notification';

/**
 * Provides methods that will be called back when the subscriber receives a new notification or
 * a notification is canceled.
 *
 * @name NotificationSubscriber
 * @sysCap ans
 * @devices phone, tablet
 * @permission N/A
 * @systemApi
 * @since 7
 */
export interface NotificationSubscriber {
  onConsumed?:(data: SubscribeCallbackData) => void;
  onCanceled?:(data: SubscribeCallbackData) => void;
  onUpdate?:(data: NotificationSortingMap) => void;
  onConnected?:() => void;
  onDisConnect?:() => void;
  onDied?:() => void;
  onDisturbModeChanged?:(mode: notification.DoNotDisturbMode) => void;
}

/**
 * Provides methods that will be called back when the subscriber receives a new notification or
 * a notification is canceled.
 *
 * @name SubscribeCallbackData
 * @sysCap ans
 * @devices phone, tablet
 * @permission N/A
 * @systemApi
 * @since 7
 */
export interface SubscribeCallbackData {
  readonly request: NotificationRequest;
  readonly sortingMap?: NotificationSortingMap;
  readonly reason?: number;
  readonly sound?: string;
  readonly vibrationValues?: Array<number>;
}