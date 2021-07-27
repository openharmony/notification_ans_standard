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

#ifndef BASE_NOTIFICATION_ANS_STANDARD_SERVICES_PREFERENCES_CONSTANT_H
#define BASE_NOTIFICATION_ANS_STANDARD_SERVICES_PREFERENCES_CONSTANT_H

namespace OHOS {
namespace Notification {

const static std::string kDisturbMode = "ans_disturbMode";
const static std::string kEnable = "ans_notificationAll";
const static std::string kBundleLabel = "label_ans_bundle_";
const static std::string kUnderLine = "_";
const static std::string kAnsBundle = "ans_bundle";

const static std::string kBundleName = "name";
const static std::string kBundleImportance = "importance";
const static std::string kBundleShowBadge = "showBadge";
const static std::string kBundleBadgeTotalNum = "badgeTotalNum";
const static std::string kBundlePrivateAllowed = "privateAllowed";
const static std::string kBundleEnableNotification = "enabledNotification";

const static std::string kGroup = "group";
const static std::string kGroupSize = "group_size";
const static std::string kGroupId = "id";
const static std::string kGroupName = "name";
const static std::string kGroupDescription = "groupDescription";
const static std::string kGroupDisable = "isDisable";

const static std::string kSlot = "slot";
const static std::string kSlotType = "type";
const static std::string kSlotId = "id";
const static std::string kSlotGroupId = "groupId";
const static std::string kSlotName = "name";
const static std::string kSlotDescription = "description";
const static std::string kSlotLevel = "level";
const static std::string kSlotShowBadge = "showBadge";
const static std::string kSlotEnableLight = "enableLight";
const static std::string kSlotEnableVrbration = "enableVibration";
const static std::string kSlotLedLightColor = "ledLightColor";
const static std::string kSlotLockscreenVisibleness = "lockscreenVisibleness";
const static std::string kSlotSound = "sound";
const static std::string kSlotVibrationSytle = "vibrationSytle";
const static std::string kSlotEnableBypassDnd = "enableBypassDnd";

const static int32_t MAX_TIMES = 5;                // 500ms
const static int32_t SLEEP_INTERVAL = 100 * 1000;  // 100ms

// distributed database
const std::string APP_ID = "advanced_notification_service";
const std::string STORE_ID = "local_db";

const static int BUNDLE_IMPORTANCE = 3;
const static bool BUNDLE_SHOW_BADGE = true;
const static int BUNDLE_BADGE_TOTAL_NUM = 0;
const static int BUNDLE_PRIVATE_ALLOWED = true;
const static int BUNDLE_ENABLE_NOTIFICATION = true;

enum class BundleType {
    kBundleNameType = 1,
    kBundleImportanceType,
    kBundleShowBadgeType,
    kBundleBadgeTotalNumType,
    kBundlePrivateAllowedType,
    kBundleEnableNotificationType,
};

}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_SERVICES_PREFERENCES_CONSTANT_H