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

#include "constant.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;

void SetNamedPropertyByInteger(napi_env env, napi_value dstObj, int32_t objName, const char *propName)
{
    napi_value prop = nullptr;
    if (napi_create_int32(env, objName, &prop) == napi_ok) {
        napi_set_named_property(env, dstObj, propName, prop);
    }
}

napi_value NotificationReasonInit(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, (int32_t)NotificationReason::CLICK_REASON_DELETE, "CLICK_REASON_DELETE");
    SetNamedPropertyByInteger(env, obj, (int32_t)NotificationReason::CANCEL_REASON_DELETE, "CANCEL_REASON_DELETE");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)NotificationReason::CANCEL_ALL_REASON_DELETE, "CANCEL_ALL_REASON_DELETE");
    SetNamedPropertyByInteger(env, obj, (int32_t)NotificationReason::ERROR_REASON_DELETE, "ERROR_REASON_DELETE");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)NotificationReason::PACKAGE_CHANGED_REASON_DELETE, "PACKAGE_CHANGED_REASON_DELETE");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)NotificationReason::USER_STOPPED_REASON_DELETE, "USER_STOPPED_REASON_DELETE");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)NotificationReason::PACKAGE_BANNED_REASON_DELETE, "PACKAGE_BANNED_REASON_DELETE");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)NotificationReason::APP_CANCEL_REASON_DELETE, "APP_CANCEL_REASON_DELETE");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)NotificationReason::APP_CANCEL_ALL_REASON_DELETE, "APP_CANCEL_ALL_REASON_DELETE");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)NotificationReason::APP_CANCEL_REASON_OTHER, "APP_CANCEL_REASON_OTHER");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("NotificationReason", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value SlotTypeInit(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, (int32_t)SlotType::UNKNOWN_TYPE, "UNKNOWN_TYPE");
    SetNamedPropertyByInteger(env, obj, (int32_t)SlotType::SOCIAL_COMMUNICATION, "SOCIAL_COMMUNICATION");
    SetNamedPropertyByInteger(env, obj, (int32_t)SlotType::SERVICE_INFORMATION, "SERVICE_INFORMATION");
    SetNamedPropertyByInteger(env, obj, (int32_t)SlotType::CONTENT_INFORMATION, "CONTENT_INFORMATION");
    SetNamedPropertyByInteger(env, obj, (int32_t)SlotType::OTHER_TYPES, "OTHER_TYPES");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("SlotType", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value SlotLevelInit(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, (int32_t)SlotLevel::LEVEL_NONE, "LEVEL_NONE");
    SetNamedPropertyByInteger(env, obj, (int32_t)SlotLevel::LEVEL_MIN, "LEVEL_MIN");
    SetNamedPropertyByInteger(env, obj, (int32_t)SlotLevel::LEVEL_LOW, "LEVEL_LOW");
    SetNamedPropertyByInteger(env, obj, (int32_t)SlotLevel::LEVEL_DEFAULT, "LEVEL_DEFAULT");
    SetNamedPropertyByInteger(env, obj, (int32_t)SlotLevel::LEVEL_HIGH, "LEVEL_HIGH");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("SlotLevel", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value SemanticActionButtonInit(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, (int32_t)SemanticActionButton::NONE_ACTION_BUTTON, "NONE_ACTION_BUTTON");
    SetNamedPropertyByInteger(env, obj, (int32_t)SemanticActionButton::REPLY_ACTION_BUTTON, "REPLY_ACTION_BUTTON");
    SetNamedPropertyByInteger(env, obj, (int32_t)SemanticActionButton::READ_ACTION_BUTTON, "READ_ACTION_BUTTON");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)SemanticActionButton::UNREAD_ACTION_BUTTON, "UNREAD_ACTION_BUTTON");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)SemanticActionButton::DELETE_ACTION_BUTTON, "DELETE_ACTION_BUTTON");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)SemanticActionButton::ARCHIVE_ACTION_BUTTON, "ARCHIVE_ACTION_BUTTON");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)SemanticActionButton::MUTE_ACTION_BUTTON, "MUTE_ACTION_BUTTON");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)SemanticActionButton::UNMUTE_ACTION_BUTTON, "UNMUTE_ACTION_BUTTON");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)SemanticActionButton::THUMBS_UP_ACTION_BUTTON, "THUMBS_UP_ACTION_BUTTON");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)SemanticActionButton::THUMBS_DOWN_ACTION_BUTTON, "THUMBS_DOWN_ACTION_BUTTON");
    SetNamedPropertyByInteger(env, obj, (int32_t)SemanticActionButton::CALL_ACTION_BUTTON, "CALL_ACTION_BUTTON");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("SemanticActionButton", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value InputsSourceInit(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, (int32_t)InputsSource::FREE_FORM_INPUT, "FREE_FORM_INPUT");
    SetNamedPropertyByInteger(env, obj, (int32_t)InputsSource::OPTION, "OPTION");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("InputsSource", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value DoNotDisturbMode(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, (int32_t)DisturbMode::ALLOW_UNKNOWN, "ALLOW_UNKNOWN");
    SetNamedPropertyByInteger(env, obj, (int32_t)DisturbMode::ALLOW_ALL, "ALLOW_ALL");
    SetNamedPropertyByInteger(env, obj, (int32_t)DisturbMode::ALLOW_PRIORITY, "ALLOW_PRIORITY");
    SetNamedPropertyByInteger(env, obj, (int32_t)DisturbMode::ALLOW_NONE, "ALLOW_NONE");
    SetNamedPropertyByInteger(env, obj, (int32_t)DisturbMode::ALLOW_ALARMS, "ALLOW_ALARMS");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("DoNotDisturbMode", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value InputEditTypeInit(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, (int32_t)InputEditType::EDIT_AUTO, "EDIT_AUTO");
    SetNamedPropertyByInteger(env, obj, (int32_t)InputEditType::EDIT_DISABLED, "EDIT_DISABLED");
    SetNamedPropertyByInteger(env, obj, (int32_t)InputEditType::EDIT_ENABLED, "EDIT_ENABLED");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("InputEditType", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value ContentTypeInit(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(
        env, obj, (int32_t)ContentType::NOTIFICATION_CONTENT_BASIC_TEXT, "NOTIFICATION_CONTENT_BASIC_TEXT");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)ContentType::NOTIFICATION_CONTENT_LONG_TEXT, "NOTIFICATION_CONTENT_LONG_TEXT");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)ContentType::NOTIFICATION_CONTENT_PICTURE, "NOTIFICATION_CONTENT_PICTURE");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)ContentType::NOTIFICATION_CONTENT_CONVERSATION, "NOTIFICATION_CONTENT_CONVERSATION");
    SetNamedPropertyByInteger(
        env, obj, (int32_t)ContentType::NOTIFICATION_CONTENT_MULTILINE, "NOTIFICATION_CONTENT_MULTILINE");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("ContentType", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value DoNotDisturbTypeInit(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, (int32_t)DoNotDisturbType::TYPE_NONE, "TYPE_NONE");
    SetNamedPropertyByInteger(env, obj, (int32_t)DoNotDisturbType::TYPE_ONCE, "TYPE_ONCE");
    SetNamedPropertyByInteger(env, obj, (int32_t)DoNotDisturbType::TYPE_DAILY, "TYPE_DAILY");
    SetNamedPropertyByInteger(env, obj, (int32_t)DoNotDisturbType::TYPE_CLEARLY, "TYPE_CLEARLY");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("DoNotDisturbType", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value DeviceRemindTypeInit(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, (int32_t)DeviceRemindType::IDLE_DONOT_REMIND, "IDLE_DONOT_REMIND");
    SetNamedPropertyByInteger(env, obj, (int32_t)DeviceRemindType::IDLE_REMIND, "IDLE_REMIND");
    SetNamedPropertyByInteger(env, obj, (int32_t)DeviceRemindType::ACTIVE_DONOT_REMIND, "ACTIVE_DONOT_REMIND");
    SetNamedPropertyByInteger(env, obj, (int32_t)DeviceRemindType::ACTIVE_REMIND, "ACTIVE_REMIND");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("DeviceRemindType", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value SourceTypeInit(napi_env env, napi_value exports)
{
    ANS_LOGI("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, (int32_t)SourceType::TYPE_NORMAL, "TYPE_NORMAL");
    SetNamedPropertyByInteger(env, obj, (int32_t)SourceType::TYPE_CONTINUOUS, "TYPE_CONTINUOUS");
    SetNamedPropertyByInteger(env, obj, (int32_t)SourceType::TYPE_TIMER, "TYPE_TIMER");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("SourceType", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value ConstantInit(napi_env env, napi_value exports)
{
    NotificationReasonInit(env, exports);
    SlotTypeInit(env, exports);
    SlotLevelInit(env, exports);
    SemanticActionButtonInit(env, exports);
    InputsSourceInit(env, exports);
    DoNotDisturbMode(env, exports);
    InputEditTypeInit(env, exports);
    ContentTypeInit(env, exports);
    SourceTypeInit(env, exports);
    DoNotDisturbTypeInit(env, exports);
    DeviceRemindTypeInit(env, exports);
    return exports;
}
}  // namespace NotificationNapi
}  // namespace OHOS
