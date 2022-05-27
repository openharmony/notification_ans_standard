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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_PREFERENCES_CONSTANT_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_PREFERENCES_CONSTANT_H

namespace OHOS {
namespace Notification {
/**
 * Indicates that disturbe key which do not disturbe type.
 */
const static std::string KEY_DO_NOT_DISTURB_TYPE = "ans_doNotDisturbType";

/**
 * Indicates that disturbe key which do not disturbe begin date.
 */
const static std::string KEY_DO_NOT_DISTURB_BEGIN_DATE = "ans_doNotDisturbBeginDate";

/**
 * Indicates that disturbe key which do not disturbe end date.
 */
const static std::string KEY_DO_NOT_DISTURB_END_DATE = "ans_doNotDisturbEndDate";

/**
 * Indicates that disturbe key which enable all notification.
 */
const static std::string KEY_ENABLE_ALL_NOTIFICATION = "ans_notificationAll";

/**
 * Indicates that disturbe key which bundle label.
 */
const static std::string KEY_BUNDLE_LABEL = "label_ans_bundle_";

/**
 * Indicates that disturbe key which under line.
 */
const static std::string KEY_UNDER_LINE = "_";

/**
 * Indicates that disturbe key which bundle begin key.
 */
const static std::string KEY_ANS_BUNDLE = "ans_bundle";

/**
 * Indicates that disturbe key which bundle name.
 */
const static std::string KEY_BUNDLE_NAME = "name";

/**
 * Indicates that disturbe key which bundle imortance.
 */
const static std::string KEY_BUNDLE_IMPORTANCE = "importance";

/**
 * Indicates that disturbe key which bundle show badge.
 */
const static std::string KEY_BUNDLE_SHOW_BADGE = "showBadge";

/**
 * Indicates that disturbe key which bundle total badge num.
 */
const static std::string KEY_BUNDLE_BADGE_TOTAL_NUM = "badgeTotalNum";

/**
 * Indicates that disturbe key which bundle private allowed.
 */
const static std::string KEY_BUNDLE_PRIVATE_ALLOWED = "privateAllowed";

/**
 * Indicates that disturbe key which bundle enable notification.
 */
const static std::string KEY_BUNDLE_ENABLE_NOTIFICATION = "enabledNotification";

/**
 * Indicates that disturbe key which bundle popped dialog.
 */
const static std::string KEY_BUNDLE_POPPED_DIALOG = "poppedDialog";

/**
 * Indicates that disturbe key which bundle uid.
 */
const static std::string KEY_BUNDLE_UID = "uid";

/**
 * Indicates that disturbe key which group.
 */
const static std::string KEY_GROUP = "group";

/**
 * Indicates that disturbe key which group id.
 */
const static std::string KEY_GROUP_ID = "id";

/**
 * Indicates that disturbe key which group name.
 */
const static std::string KEY_GROUP_NAME = "name";

/**
 * Indicates that disturbe key which group description.
 */
const static std::string KEY_GROUP_DESCRIPTION = "groupDescription";

/**
 * Indicates that disturbe key which group is disable.
 */
const static std::string KEY_GROUP_DISABLE = "isDisable";

/**
 * Indicates that disturbe key which slot.
 */
const static std::string KEY_SLOT = "slot";

/**
 * Indicates that disturbe key which slot type.
 */
const static std::string KEY_SLOT_TYPE = "type";

/**
 * Indicates that disturbe key which slot id.
 */
const static std::string KEY_SLOT_ID = "id";

/**
 * Indicates that disturbe key which slot group id.
 */
const static std::string KEY_SLOT_GROUPID = "groupId";

/**
 * Indicates that disturbe key which slot group name.
 */
const static std::string KEY_SLOT_NAME = "name";

/**
 * Indicates that disturbe key which slot description.
 */
const static std::string KEY_SLOT_DESCRIPTION = "description";

/**
 * Indicates that disturbe key which slot level.
 */
const static std::string KEY_SLOT_LEVEL = "level";

/**
 * Indicates that disturbe key which slot show badge.
 */
const static std::string KEY_SLOT_SHOW_BADGE = "showBadge";

/**
 * Indicates that disturbe key which slot enable light.
 */
const static std::string KEY_SLOT_ENABLE_LIGHT = "enableLight";

/**
 * Indicates that disturbe key which slot enable vibration.
 */
const static std::string KEY_SLOT_ENABLE_VRBRATION = "enableVibration";

/**
 * Indicates that disturbe key which slot led light color.
 */
const static std::string KEY_SLOT_LED_LIGHT_COLOR = "ledLightColor";

/**
 * Indicates that disturbe key which slot lockscreen visibleness.
 */
const static std::string KEY_SLOT_LOCKSCREEN_VISIBLENESS = "lockscreenVisibleness";

/**
 * Indicates that disturbe key which slot sound.
 */
const static std::string KEY_SLOT_SOUND = "sound";

/**
 * Indicates that disturbe key which slot vibration style.
 */
const static std::string KEY_SLOT_VIBRATION_STYLE = "vibrationSytle";

/**
 * Indicates that disturbe key which slot enable bypass end.
 */
const static std::string KEY_SLOT_ENABLE_BYPASS_DND = "enableBypassDnd";

/**
 * Indicates whether the type of slot is enabled.
 */
const static std::string KEY_SLOT_ENABLED = "enabled";

/**
 * Indicates distributed database app id.
 */
const std::string APP_ID = "advanced_notification_service";

/**
 * Indicates distributed database store id.
 */
const std::string STORE_ID = "local_db";

/**
 * Default params that bundle importance is LEVEL_DEFAULT.
 */
const static int BUNDLE_IMPORTANCE = 3;

/**
 * Default params that bundle badge total num is zero.
 */
const static int BUNDLE_BADGE_TOTAL_NUM = 0;

/**
 * Default params that bundle private allowed is false.
 */
const static int BUNDLE_PRIVATE_ALLOWED = false;

/**
 * Default params that bundle enable notification is true.
 */
const static int BUNDLE_ENABLE_NOTIFICATION = true;
const static int BUNDLE_POPPED_DIALOG = false;

/**
 * Default params that bundle show badge is false.
 */
const static bool BUNDLE_SHOW_BADGE = false;

/**
 * Indicates bundle type which used to DB store.
 */
enum class BundleType {
    BUNDLE_NAME_TYPE = 1,
    BUNDLE_IMPORTANCE_TYPE,
    BUNDLE_SHOW_BADGE_TYPE,
    BUNDLE_BADGE_TOTAL_NUM_TYPE,
    BUNDLE_PRIVATE_ALLOWED_TYPE,
    BUNDLE_ENABLE_NOTIFICATION_TYPE,
    BUNDLE_POPPED_DIALOG_TYPE,
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_PREFERENCES_CONSTANT_H