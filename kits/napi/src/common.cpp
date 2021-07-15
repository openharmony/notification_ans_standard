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

#include "common.h"
#include "notification_long_text_content.h"
#include "notification_multiline_content.h"
#include "publish.h"
#include "want_agent.h"

namespace OHOS {
namespace NotificationNapi {
static napi_value GetIdByPublish(const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetSlotType(const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationIsOngoing(const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationIsUnremovable(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationDeliveryTime(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationtapDismissed(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationRemovalWantAgent(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationMaxScreenWantAgent(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationAutoDeletedTime(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
// static napi_value GetNotificationSettingsText(
//     const napi_env &env, const napi_value &value, NotificationRequest &request);
// static napi_value GetNotificationGroupValue(
//     const napi_env &env, const napi_value &value, NotificationRequest &request);
// static napi_value GetNotificationGroupAlertType(
//     const napi_env &env, const napi_value &value, NotificationRequest &request);
// static napi_value GetNotificationGroupOverview(
//     const napi_env &env, const napi_value &value, NotificationRequest &request);
// static napi_value GetNotificationClassification(
//     const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationColor(const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationColorEnabled(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationIsAlertOnce(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationIsStopwatch(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationIsCountDown(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
// static napi_value GetNotificationVisibleness(
//     const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationProgressBar(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationStatusBarText(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
// static napi_value GetNotificationOnlyLocal(
//     const napi_env &env, const napi_value &value, NotificationRequest &request);
// static napi_value GetNotificationSortingKey(
//     const napi_env &env, const napi_value &value, NotificationRequest &request);
// static napi_value GetNotificationIsFloatingIcon(
//     const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationLabel(const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationBadgeIconStyle(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
// static napi_value GetNotificationShortcutId(
//     const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationShowDeliveryTime(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationNotificationActionButtons(
    const napi_env &env, const napi_value &value, NotificationRequest &request);
static napi_value GetNotificationContentType(const napi_env &env, const napi_value &result, int32_t &type);
static napi_value GetNotificationBasicContent(
    const napi_env &env, const napi_value &result, NotificationRequest &request);
static napi_value GetNotificationLongTextContent(
    const napi_env &env, const napi_value &result, NotificationRequest &request);
static napi_value GetNotificationMultiLineContent(
    const napi_env &env, const napi_value &result, NotificationRequest &request);
static napi_value GetNotificationMultiLineContentLines(const napi_env &env, const napi_value &result,
    std::shared_ptr<OHOS::Notification::NotificationMultiLineContent> &multiLineContent);

Common::Common()
{}

Common::~Common()
{}

napi_value Common::NapiGetboolean(napi_env env, const bool &isValue)
{
    napi_value result = 0;
    napi_get_boolean(env, isValue, &result);
    return result;
}

napi_value Common::NapiGetNull(napi_env env)
{
    napi_value result = 0;
    napi_get_null(env, &result);
    return result;
}

napi_value Common::GetCallbackErrorValue(napi_env env, int errCode)
{
    napi_value result = nullptr;
    napi_value eCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &eCode));
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_set_named_property(env, result, "code", eCode));
    return result;
}

void Common::ReturnCallbackPromise(const napi_env &env, const CallbackPromiseInfo &info, const napi_value &result)
{
    ANS_LOGI("ReturnCallbackPromise start");

    if (info.isCallback) {
        SetCallback(env, info.callback, info.errorCode, result);
    } else {
        SetPromise(env, info.deferred, result);
    }
    ANS_LOGI("ReturnCallbackPromise end");
}

void Common::SetCallback(
    const napi_env &env, const napi_ref &callbackIn, const int &errorCode, const napi_value &result)
{
    ANS_LOGI("enter");
    napi_value undefined;
    napi_get_undefined(env, &undefined);

    napi_value callback;
    napi_value resultout;
    napi_get_reference_value(env, callbackIn, &callback);
    napi_value results[ARGS_TWO] = {0};
    results[PARAM0] = GetCallbackErrorValue(env, errorCode);
    results[PARAM1] = result;
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &results[PARAM0], &resultout));
    ANS_LOGI("end");
}

void Common::SetPromise(const napi_env &env, const napi_deferred &deferred, const napi_value &result)
{
    ANS_LOGI("enter");
    napi_resolve_deferred(env, deferred, result);
    ANS_LOGI("end");
}

napi_value Common::SetNotificationRequest(
    const napi_env &env, const std::shared_ptr<OHOS::Notification::Notification> &notification, napi_value &result)
{
    ANS_LOGI("SetNotificationRequest start");

    napi_value value;
    NotificationRequest request = notification->GetNotificationRequest();

    // content?NotificationContent
    std::shared_ptr<NotificationContent> content = request.GetContent();
    if (content) {
        napi_value contentResult;
        napi_create_object(env, &contentResult);
        if (!SetNotificationContent(env, content, contentResult)) {
            ANS_LOGE("SetNotificationContent call failed");
            return NapiGetboolean(env, false);
        }
        napi_set_named_property(env, result, "content", contentResult);
    } else {
        ANS_LOGE("content is nullptr");
    }

    // id:number
    napi_create_int32(env, request.GetNotificationId(), &value);
    ANS_LOGE("SetNotificationContent get id is: %{public}d", request.GetNotificationId());
    napi_set_named_property(env, result, "id", value);
    ANS_LOGE("SetNotificationContent id is: %{public}d", request.GetNotificationId());
    // slotType?: SlotType
    enum SlotType outType;
    if (!SlotTypeCToJS(request.GetSlotType(), outType)) {
        return NapiGetboolean(env, false);
    }
    napi_create_int32(env, outType, &value);
    napi_set_named_property(env, result, "slotType", value);

    // isOngoing?: boolean
    napi_get_boolean(env, request.IsInProgress(), &value);
    napi_set_named_property(env, result, "isOngoing", value);

    // isUnremovable?: boolean
    napi_get_boolean(env, request.IsUnremovable(), &value);
    napi_set_named_property(env, result, "isUnremovable", value);

    // // createTime?: number
    // napi_create_int64(env, request.GetCreateTime(), &value);
    // napi_set_named_property(env, result, "createTime", value);

    // hashCode?: string
    napi_create_string_utf8(env, notification->GetKey().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "hashCode", value);

    // deliveryTime?: number
    napi_create_int64(env, request.GetDeliveryTime(), &value);
    napi_set_named_property(env, result, "deliveryTime", value);

    // tapDismissed?: boolean
    napi_get_boolean(env, request.IsTapDismissed(), &value);
    napi_set_named_property(env, result, "tapDismissed", value);

    // autoDeletedTime?: number
    napi_create_int64(env, request.GetAutoDeletedTime(), &value);
    napi_set_named_property(env, result, "autoDeletedTime", value);

    // wantAgent?: WantAgent
    std::shared_ptr<WantAgent::WantAgent> agent = request.GetWantAgent();
    if (agent) {
        napi_value wantAgent;
        wantAgent = CreateWantAgentByJS(env, agent);
        napi_set_named_property(env, result, "wantAgent", wantAgent);
    }
    // extraInfo ?: {[key:string] : any};  //do

    // // settingsText ?: string
    // napi_create_string_utf8(env, request.GetSettingsText().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "settingsText", value);

    // // ownerBundleName ?: string
    // napi_create_string_utf8(env, notification->GetBundleName().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "ownerBundleName", value);

    // // groupValue ?: string
    // napi_create_string_utf8(env, request.GetGroupValue().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "groupValue", value);

    // // groupAlertType ?: number
    // int groupAlertType = (int)request.GetGroupAlertType();
    // napi_create_int32(env, groupAlertType, &value);
    // napi_set_named_property(env, result, "groupAlertType", value);

    // // groupOverview ?: boolean
    // napi_get_boolean(env, request.IsGroupOverview(), &value);
    // napi_set_named_property(env, result, "groupOverview", value);

    // removalWantAgent ?  WantAgent
    std::shared_ptr<WantAgent::WantAgent> removalAgent = request.GetRemovalWantAgent();
    if (removalAgent) {
        napi_value wantAgent;
        wantAgent = CreateWantAgentByJS(env, removalAgent);
        napi_set_named_property(env, result, "removalWantAgent", wantAgent);
    }

    // maxScreenWantAgent ? WantAgent
    std::shared_ptr<WantAgent::WantAgent> maxScreenAgent = request.GetMaxScreenWantAgent();
    if (maxScreenAgent) {
        napi_value wantAgent;
        wantAgent = CreateWantAgentByJS(env, maxScreenAgent);
        napi_set_named_property(env, result, "maxScreenWantAgent", wantAgent);
    }

    // // classification ?: string
    // napi_create_string_utf8(env, request.GetClassification().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "classification", value);

    // color ?: number
    napi_create_int32(env, request.GetColor(), &value);
    napi_set_named_property(env, result, "color", value);

    // colorEnabled ?: boolean
    napi_get_boolean(env, request.IsColorEnabled(), &value);
    napi_set_named_property(env, result, "colorEnabled", value);

    // isAlertOnce ?: boolean
    napi_get_boolean(env, request.IsAlertOneTime(), &value);
    napi_set_named_property(env, result, "isAlertOnce", value);

    // isStopwatch ?: boolean
    napi_get_boolean(env, request.IsShowStopwatch(), &value);
    napi_set_named_property(env, result, "isStopwatch", value);

    // isCountDown ?: boolean
    napi_get_boolean(env, request.IsCountdownTimer(), &value);
    napi_set_named_property(env, result, "isCountDown", value);

    // showCreateTime ?: boolean; do request
    // napi_get_boolean(env, request.IsCountdownTimer(), &value);
    // napi_set_named_property(env, result, "showCreateTime", value);

    // // visibleness ?: number
    // int visibleness = (int)request.GetVisibleness();
    // napi_create_int32(env, visibleness, &value);
    // napi_set_named_property(env, result, "visibleness", value);

    // progressValue ?: number
    napi_create_int32(env, request.GetProgressValue(), &value);
    napi_set_named_property(env, result, "progressValue", value);

    // progressMaxValue ?: number
    napi_create_int32(env, request.GetProgressMax(), &value);
    napi_set_named_property(env, result, "progressMaxValue", value);

    // isIndeterminate ?: boolean
    napi_get_boolean(env, request.IsProgressIndeterminate(), &value);
    napi_set_named_property(env, result, "isIndeterminate", value);

    // statusBarText ?: string
    napi_create_string_utf8(env, request.GetStatusBarText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "statusBarText", value);

    // isFloatingIcon ?: boolean;  do notification

    // // onlyLocal ?: boolean
    // napi_get_boolean(env, request.IsOnlyLocal(), &value);
    // napi_set_named_property(env, result, "onlyLocal", value);

    // // sortingKey ?: string
    // napi_create_string_utf8(env, request.GetSortingKey().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "sortingKey", value);

    // // messageUsers ?: Array<MessageUser>
    // napi_value arr;
    // int count = 0;
    // napi_create_array(env, &arr);
    // for (auto vec : request.GetMessageUsers()) {
    //     if (vec) {
    //         napi_value messageUserResult;
    //         napi_create_object(env, &messageUserResult);
    //         if (SetMessageUser(env, vec, messageUserResult)) {
    //             napi_set_element(env, arr, count, messageUserResult);
    //             count++;
    //         }
    //     }
    // }
    // napi_set_named_property(env, result, "messageUsers", arr);

    // label ?: string
    napi_create_string_utf8(env, request.GetLabel().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "label", value);

    // badgeIconStyle ?: number
    int badgeIconStyle = (int)request.GetBadgeIconStyle();
    napi_create_int32(env, badgeIconStyle, &value);
    napi_set_named_property(env, result, "badgeIconStyle", value);

    // // shortcutId ?: string
    // napi_create_string_utf8(env, request.GetShortcutId().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "shortcutId", value);

    // showDeliveryTime ?: boolean
    napi_get_boolean(env, request.IsShowDeliveryTime(), &value);
    napi_set_named_property(env, result, "showDeliveryTime", value);

    // actionButtons?: Array<NotificationActionButton>
    napi_value arr;
    int count = 0;
    napi_create_array(env, &arr);
    for (auto vec : request.GetActionButtons()) {
        if (vec) {
            napi_value actionButtonResult;
            napi_create_object(env, &actionButtonResult);
            if (SetNotificationActionButton(env, vec, actionButtonResult)) {
                napi_set_element(env, arr, count, actionButtonResult);
                count++;
            }
        }
    }
    napi_set_named_property(env, result, "actionButtons", arr);

    // smallIcon ?: image.PixelMap; do
    // largeIcon ?: image.PixelMap; do

    // readonly creatorBundleName?: string
    napi_create_string_utf8(env, notification->GetCreateBundle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "creatorBundleName", value);

    // readonly creatorUid?: number
    napi_create_int32(env, notification->GetUid(), &value);
    napi_set_named_property(env, result, "creatorUid", value);

    // readonly creatorPid?: number //do  notification
    // napi_create_int32(env, request.GetCreatorPid(), &value);
    // napi_set_named_property(env, result, "creatorPid", value);

    return NapiGetboolean(env, true);
}

napi_value Common::SetNotificationRequest2(
    const napi_env &env, const std::shared_ptr<OHOS::Notification::NotificationRequest> &request, napi_value &result)
{
    ANS_LOGI("SetNotificationRequest2 start");

    napi_value value;

    // content?NotificationContent
    std::shared_ptr<NotificationContent> content = request->GetContent();

    if (content) {
        napi_value contentResult;
        napi_create_object(env, &contentResult);
        if (!SetNotificationContent(env, content, contentResult)) {
            ANS_LOGE("SetNotificationContent call failed");
            return NapiGetboolean(env, false);
        }
        napi_set_named_property(env, result, "content", contentResult);
    } else {
        ANS_LOGE("content is nullptr");
    }

    // id:number
    napi_create_int32(env, request->GetNotificationId(), &value);
    ANS_LOGE("SetNotificationContent get id is: %{public}d", request->GetNotificationId());
    napi_set_named_property(env, result, "id", value);
    ANS_LOGE("SetNotificationContent id is: %{public}d", request->GetNotificationId());

    // slotType?: SlotType
    enum SlotType outType;
    if (!SlotTypeCToJS(request->GetSlotType(), outType)) {
        return NapiGetboolean(env, false);
    }
    napi_create_int32(env, outType, &value);
    napi_set_named_property(env, result, "slotType", value);

    // isOngoing?: boolean
    napi_get_boolean(env, request->IsInProgress(), &value);
    napi_set_named_property(env, result, "isOngoing", value);

    // isUnremovable?: boolean
    napi_get_boolean(env, request->IsUnremovable(), &value);
    napi_set_named_property(env, result, "isUnremovable", value);

    // // createTime?: number
    // napi_create_int64(env, request->GetCreateTime(), &value);
    // napi_set_named_property(env, result, "createTime", value);

    // hashCode?: string
    napi_create_string_utf8(env, request->GetNotificationHashCode().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "hashCode", value);

    // deliveryTime?: number
    napi_create_int64(env, request->GetDeliveryTime(), &value);
    napi_set_named_property(env, result, "deliveryTime", value);

    // tapDismissed?: boolean
    napi_get_boolean(env, request->IsTapDismissed(), &value);
    napi_set_named_property(env, result, "tapDismissed", value);

    // autoDeletedTime?: number
    napi_create_int64(env, request->GetAutoDeletedTime(), &value);
    napi_set_named_property(env, result, "autoDeletedTime", value);

    // wantAgent?: WantAgent
    std::shared_ptr<WantAgent::WantAgent> agent = request->GetWantAgent();
    if (agent) {
        napi_value wantAgent;
        wantAgent = CreateWantAgentByJS(env, agent);
        napi_set_named_property(env, result, "wantAgent", wantAgent);
    }

    // extraInfo ?: {[key:string] : any};  //do

    // // settingsText ?: string
    // napi_create_string_utf8(env, request->GetSettingsText().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "settingsText", value);

    // // ownerBundleName ?: string
    // napi_create_string_utf8(env, notification->GetBundleName().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "ownerBundleName", value);

    // // groupValue ?: string
    // napi_create_string_utf8(env, request->GetGroupValue().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "groupValue", value);

    // // groupAlertType ?: number
    // int groupAlertType = (int)request->GetGroupAlertType();
    // napi_create_int32(env, groupAlertType, &value);
    // napi_set_named_property(env, result, "groupAlertType", value);

    // // groupOverview ?: boolean
    // napi_get_boolean(env, request->IsGroupOverview(), &value);
    // napi_set_named_property(env, result, "groupOverview", value);

    // removalWantAgent ?  WantAgent
    std::shared_ptr<WantAgent::WantAgent> removalAgent = request->GetRemovalWantAgent();
    if (removalAgent) {
        napi_value wantAgent;
        wantAgent = CreateWantAgentByJS(env, removalAgent);
        napi_set_named_property(env, result, "removalWantAgent", wantAgent);
    }

    // maxScreenWantAgent ? WantAgent
    std::shared_ptr<WantAgent::WantAgent> maxScreenAgent = request->GetMaxScreenWantAgent();
    if (maxScreenAgent) {
        napi_value wantAgent;
        wantAgent = CreateWantAgentByJS(env, maxScreenAgent);
        napi_set_named_property(env, result, "maxScreenWantAgent", wantAgent);
    }

    // // classification ?: string
    // napi_create_string_utf8(env, request->GetClassification().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "classification", value);

    // color ?: number
    napi_create_int32(env, request->GetColor(), &value);
    napi_set_named_property(env, result, "color", value);

    // colorEnabled ?: boolean
    napi_get_boolean(env, request->IsColorEnabled(), &value);
    napi_set_named_property(env, result, "colorEnabled", value);

    // isAlertOnce ?: boolean
    napi_get_boolean(env, request->IsAlertOneTime(), &value);
    napi_set_named_property(env, result, "isAlertOnce", value);

    // isStopwatch ?: boolean
    napi_get_boolean(env, request->IsShowStopwatch(), &value);
    napi_set_named_property(env, result, "isStopwatch", value);

    // isCountDown ?: boolean
    napi_get_boolean(env, request->IsCountdownTimer(), &value);
    napi_set_named_property(env, result, "isCountDown", value);

    // showCreateTime ?: boolean; do request
    // napi_get_boolean(env, request->IsCountdownTimer(), &value);
    // napi_set_named_property(env, result, "showCreateTime", value);

    // // visibleness ?: number
    // int visibleness = (int)request->GetVisibleness();
    // napi_create_int32(env, visibleness, &value);
    // napi_set_named_property(env, result, "visibleness", value);

    // progressValue ?: number
    napi_create_int32(env, request->GetProgressValue(), &value);
    napi_set_named_property(env, result, "progressValue", value);

    // progressMaxValue ?: number
    napi_create_int32(env, request->GetProgressMax(), &value);
    napi_set_named_property(env, result, "progressMaxValue", value);

    // isIndeterminate ?: boolean
    napi_get_boolean(env, request->IsProgressIndeterminate(), &value);
    napi_set_named_property(env, result, "isIndeterminate", value);

    // statusBarText ?: string
    napi_create_string_utf8(env, request->GetStatusBarText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "statusBarText", value);

    // isFloatingIcon ?: boolean;  do notification

    // // onlyLocal ?: boolean
    // napi_get_boolean(env, request->IsOnlyLocal(), &value);
    // napi_set_named_property(env, result, "onlyLocal", value);

    // // sortingKey ?: string
    // napi_create_string_utf8(env, request->GetSortingKey().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "sortingKey", value);

    // // messageUsers ?: Array<MessageUser>
    // napi_value arr;
    // int count = 0;
    // napi_create_array(env, &arr);
    // for (auto vec : request->GetMessageUsers()) {notification
    // label ?: string
    napi_create_string_utf8(env, request->GetLabel().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "label", value);

    // badgeIconStyle ?: number
    int badgeIconStyle = (int)request->GetBadgeIconStyle();
    napi_create_int32(env, badgeIconStyle, &value);
    napi_set_named_property(env, result, "badgeIconStyle", value);

    // // shortcutId ?: string
    // napi_create_string_utf8(env, request->GetShortcutId().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "shortcutId", value);

    // showDeliveryTime ?: boolean
    napi_get_boolean(env, request->IsShowDeliveryTime(), &value);
    napi_set_named_property(env, result, "showDeliveryTime", value);

    // actionButtons?: Array<NotificationActionButton>
    napi_value arr;
    int count = 0;
    napi_create_array(env, &arr);
    for (auto vec : request->GetActionButtons()) {
        if (vec) {
            napi_value actionButtonResult;
            napi_create_object(env, &actionButtonResult);
            if (SetNotificationActionButton(env, vec, actionButtonResult)) {
                napi_set_element(env, arr, count, actionButtonResult);
                count++;
            }
        }
    }
    napi_set_named_property(env, result, "actionButtons", arr);

    // smallIcon ?: image.PixelMap; do
    // largeIcon ?: image.PixelMap; do

    // readonly creatorBundleName?: string
    napi_create_string_utf8(env, request->GetCreatorBundleName().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "creatorBundleName", value);

    // readonly creatorUid?: number
    napi_create_int32(env, request->GetCreatorUid(), &value);
    napi_set_named_property(env, result, "creatorUid", value);

    // readonly creatorPid?: number //do  notification
    // napi_create_int32(env, request->GetCreatorPid(), &value);
    // napi_set_named_property(env, result, "creatorPid", value);

    return NapiGetboolean(env, true);
}

napi_value Common::SetNotificationSortingMap(
    const napi_env &env, const std::shared_ptr<NotificationSortingMap> &sortingMap, napi_value &result)
{
    ANS_LOGI("SetNotificationSortingMap start");
    if (sortingMap == nullptr) {
        ANS_LOGE("sortingMap is null");
        return NapiGetboolean(env, false);
    }
    if (sortingMap->GetKey().size() == 0) {
        ANS_LOGE("sortingMap GetKey().size is empty");
        return NapiGetboolean(env, false);
    }

    size_t count = 0;
    napi_value arrSortedHashCode;
    napi_create_array(env, &arrSortedHashCode);
    napi_value sortingsResult;
    napi_create_object(env, &sortingsResult);
    for (auto key : sortingMap->GetKey()) {
        NotificationSorting sorting;
        if (sortingMap->GetNotificationSorting(key, sorting)) {
            // sortedHashCode: Array<string>
            napi_value keyValue;
            ANS_LOGI("sortingMap key = %{public}s", key.c_str());
            napi_create_string_utf8(env, key.c_str(), NAPI_AUTO_LENGTH, &keyValue);
            napi_set_element(env, arrSortedHashCode, count, keyValue);

            // sortings:{[key : string] : NotificationSorting}
            napi_value sortingResult;
            napi_create_object(env, &sortingResult);
            if (!SetNotificationSorting(env, sorting, sortingResult)) {
                ANS_LOGE("SetNotificationSorting call failed");
                return NapiGetboolean(env, false);
            }
            napi_set_named_property(env, sortingsResult, key.c_str(), sortingResult);
            count++;
        } else {
            ANS_LOGW("sortingMap Key: %{public}s match value is empty", key.c_str());
        }
    }
    napi_set_named_property(env, result, "sortedHashCode", arrSortedHashCode);
    napi_set_named_property(env, result, "sortings", sortingsResult);

    return NapiGetboolean(env, true);
}

napi_value Common::SetNotificationSorting(const napi_env &env, const NotificationSorting &sorting, napi_value &result)
{
    ANS_LOGI("SetNotificationSorting start");

    // slot: NotificationSlot
    napi_value slotResult;
    napi_value value = 0;
    napi_create_object(env, &slotResult);
    if (!SetNotificationSlot(env, sorting.GetSlot(), slotResult)) {
        ANS_LOGE("SetNotificationSlot call failed");
        return NapiGetboolean(env, false);
    }
    napi_set_named_property(env, result, "slot", slotResult);

    // hashCode?: string
    napi_create_string_utf8(env, sorting.GetKey().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "hashCode", value);

    // ranking?: number
    napi_create_int32(env, sorting.GetRanking(), &value);
    napi_set_named_property(env, result, "ranking", value);

    // isDisplayBadge?: boolean
    napi_get_boolean(env, sorting.IsDisplayBadge(), &value);
    napi_set_named_property(env, result, "isDisplayBadge", value);

    // isHiddenNotification?: boolean
    napi_get_boolean(env, sorting.IsHiddenNotification(), &value);
    napi_set_named_property(env, result, "isHiddenNotification", value);

    // isSuitInterruptionFilter?: boolean   ???????  do
    // napi_get_boolean(env, sorting->isOrdered, &value);
    // napi_set_named_property(env, result, "isSuitInterruptionFilter", value);

    // importance?: number
    napi_create_int32(env, sorting.GetImportance(), &value);
    napi_set_named_property(env, result, "importance", value);

    // groupKeyOverride?: string
    napi_create_string_utf8(env, sorting.GetGroupKeyOverride().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "groupKeyOverride", value);

    // visiblenessOverride?: number
    napi_create_int32(env, sorting.GetVisiblenessOverride(), &value);
    napi_set_named_property(env, result, "visiblenessOverride", value);

    return NapiGetboolean(env, true);
}

napi_value Common::SetNotificationSlot(const napi_env &env, const NotificationSlot &slot, napi_value &result)
{
    ANS_LOGI("SetNotificationSlot start");

    napi_value value = 0;

    // type: SlotType;
    enum SlotType outType;
    if (!SlotTypeCToJS(slot.GetType(), outType)) {
        return NapiGetboolean(env, false);
    }
    napi_create_int32(env, outType, &value);
    napi_set_named_property(env, result, "type", value);

    // // slotId: string
    // napi_create_string_utf8(env, slot.GetId().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "slotId", value);

    // // slotName: string
    // napi_create_string_utf8(env, slot.GetName().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "slotName", value);

    // level?: number
    napi_create_int32(env, slot.GetLevel(), &value);
    napi_set_named_property(env, result, "level", value);

    // desc?: string
    napi_create_string_utf8(env, slot.GetDescription().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "desc", value);

    // showBadgeFlag?: boolean
    napi_get_boolean(env, slot.IsShowBadge(), &value);
    napi_set_named_property(env, result, "showBadgeFlag", value);

    // bypassDnd?: boolean
    napi_get_boolean(env, slot.IsEnableBypassDnd(), &value);
    napi_set_named_property(env, result, "bypassDnd", value);

    // lockscreenVisibility?: number
    int32_t LockScreenVisibleness = (int32_t)slot.GetLockScreenVisibleness();
    napi_create_int32(env, LockScreenVisibleness, &value);
    napi_set_named_property(env, result, "lockscreenVisibility", value);

    // vibrationEnabled?: boolean
    napi_get_boolean(env, slot.CanVibrate(), &value);
    napi_set_named_property(env, result, "vibrationEnabled", value);

    // sound?: string
    napi_create_string_utf8(env, slot.GetSound().ToString().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "sound", value);

    // lightEnabled?: boolean
    napi_get_boolean(env, slot.CanEnableLight(), &value);
    napi_set_named_property(env, result, "lightEnabled", value);

    // lightColor?: number
    napi_create_int32(env, slot.GetLedLightColor(), &value);
    napi_set_named_property(env, result, "lightColor", value);

    // slotGroupId?: string
    // napi_create_string_utf8(env, slot.GetSlotGroup().c_str(), NAPI_AUTO_LENGTH, &value);
    // napi_set_named_property(env, result, "slotGroupId", value);

    // vibrationValues?: Array<number>
    napi_value arr;
    napi_create_array(env, &arr);
    size_t count = 0;
    for (auto vec : slot.GetVibrationStyle()) {
        napi_create_int64(env, vec, &value);
        napi_set_element(env, arr, count, value);
        count++;
    }
    napi_set_named_property(env, result, "vibrationValues", arr);

    return NapiGetboolean(env, true);
}
napi_value Common::SetNotificationContent(
    const napi_env &env, const std::shared_ptr<NotificationContent> &content, napi_value &result)
{
    ANS_LOGI("SetNotificationContent start");
    if (content == nullptr) {
        ANS_LOGE("content is null");
        return NapiGetboolean(env, false);
    }

    napi_value value;
    // contentType: ContentType;
    NotificationContent::Type type = content->GetContentType();
    napi_create_int32(env, (int32_t)type, &value);
    napi_set_named_property(env, result, "contentType", value);

    ContentType outType;
    if (!ContentTypeCToJS(type, outType)) {
        return NapiGetboolean(env, false);
    }

    if (outType == ContentType::NOTIFICATION_CONTENT_BASIC_TEXT) {
        // normal?: NotificationBasicContent
        std::shared_ptr<NotificationBasicContent> basicContent = content->GetNotificationContent();
        if (basicContent == nullptr) {
            ANS_LOGI("basicContent is null");
            napi_set_named_property(env, result, "normal", NapiGetNull(env));
        } else {
            napi_value basicContentResult;
            napi_create_object(env, &basicContentResult);
            if (SetNotificationBasicContent(env, basicContent.get(), basicContentResult)) {
                napi_set_named_property(env, result, "normal", basicContentResult);
            } else {
                napi_set_named_property(env, result, "normal", NapiGetNull(env));
            }
        }
    } else if (outType == ContentType::NOTIFICATION_CONTENT_LONG_TEXT) {
        // longText?: NotificationLongTextContent
        OHOS::Notification::NotificationLongTextContent *longTextContent =
            static_cast<OHOS::Notification::NotificationLongTextContent *>(content->GetNotificationContent().get());
        if (longTextContent == nullptr) {
            ANS_LOGI("longTextContent is null");
            napi_set_named_property(env, result, "longText", NapiGetNull(env));
        } else {
            napi_value longTextContentResult;
            napi_create_object(env, &longTextContentResult);
            if (SetNotificationLongTextContent(env, longTextContent, longTextContentResult)) {
                napi_set_named_property(env, result, "longText", longTextContentResult);
            } else {
                napi_set_named_property(env, result, "longText", NapiGetNull(env));
            }
        }
    } else if (outType == ContentType::NOTIFICATION_CONTENT_PICTURE) {
        // picture?: NotificationPictureContent
        // OHOS::Notification::NotificationPictureContent * pictureContent =
        //     static_cast<OHOS::Notification::NotificationPictureContent *>(content->GetNotificationContent().get());
        // if (pictureContent == nullptr) {
        //     ANS_LOGI("pictureContent is null");
        //     napi_set_named_property(env, result, "picture", NapiGetNull(env));
        // } else {
        //     napi_value pictureContentResult;
        //     napi_create_object(env, &pictureContentResult);
        //     if (SetNotificationPictureContent(env, pictureContent, pictureContentResult)) {
        //         napi_set_named_property(env, result, "picture", pictureContentResult);
        //     } else {
        //         napi_set_named_property(env, result, "picture", NapiGetNull(env));
        //     }
        // }
    } else if (outType == ContentType::NOTIFICATION_CONTENT_CONVERSATION) {
        // conversational?: NotificationConversationalContent
        // OHOS::Notification::NotificationConversationalContent * conversationalContent =
        //     static_cast<OHOS::Notification::NotificationConversationalContent
        //     *>(content->GetNotificationContent().get());
        // if (conversationalContent == nullptr) {
        //     ANS_LOGI("conversationalContent is null");
        //     napi_set_named_property(env, result, "conversational", NapiGetNull(env));
        // } else {
        //     napi_value conversationalContentResult;
        //     napi_create_object(env, &conversationalContentResult);
        //     if (SetNotificationConversationalContent(env, conversationalContent, conversationalContentResult)) {
        //         napi_set_named_property(env, result, "conversational", conversationalContentResult);
        //     } else {
        //         napi_set_named_property(env, result, "conversational", NapiGetNull(env));
        //     }
        // }
    } else if (outType == ContentType::NOTIFICATION_CONTENT_MULTILINE) {
        // multiLine?: NotificationMultiLineContent
        OHOS::Notification::NotificationMultiLineContent *multiLineContent =
            static_cast<OHOS::Notification::NotificationMultiLineContent *>(content->GetNotificationContent().get());
        if (multiLineContent == nullptr) {
            ANS_LOGI("multiLineContent is null");
            napi_set_named_property(env, result, "multiLine", NapiGetNull(env));
        } else {
            napi_value multiLineContentResult;
            napi_create_object(env, &multiLineContentResult);
            if (SetNotificationMultiLineContent(env, multiLineContent, multiLineContentResult)) {
                napi_set_named_property(env, result, "multiLine", multiLineContentResult);
            } else {
                napi_set_named_property(env, result, "multiLine", NapiGetNull(env));
            }
        }
    } else {
        ANS_LOGE("ContentType is does not exist");
        return NapiGetboolean(env, false);
    }
    return NapiGetboolean(env, true);
}

napi_value Common::SetNotificationBasicContent(
    const napi_env &env, NotificationBasicContent *basicContent, napi_value &result)
{
    ANS_LOGI("SetNotificationBasicContent start");
    if (basicContent == nullptr) {
        ANS_LOGE("basicContent is null");
        return NapiGetboolean(env, false);
    }

    napi_value value;

    // title: string;
    napi_create_string_utf8(env, basicContent->GetTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "title", value);

    // text: string;
    napi_create_string_utf8(env, basicContent->GetText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "text", value);

    // additionalText?: string;
    napi_create_string_utf8(env, basicContent->GetAdditionalText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "additionalText", value);

    return NapiGetboolean(env, true);
}

napi_value Common::SetNotificationLongTextContent(
    const napi_env &env, OHOS::Notification::NotificationLongTextContent *longTextContent, napi_value &result)
{
    ANS_LOGI("SetNotificationLongTextContent start");
    if (longTextContent == nullptr) {
        ANS_LOGE("longTextContent is null");
        return NapiGetboolean(env, false);
    }

    if (!SetNotificationBasicContent(env, longTextContent, result)) {
        ANS_LOGE("SetNotificationBasicContent call failed");
        return NapiGetboolean(env, false);
    }

    napi_value value;

    // longText: string
    napi_create_string_utf8(env, longTextContent->GetLongText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "longText", value);

    // briefText: string
    napi_create_string_utf8(env, longTextContent->GetBriefText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "briefText", value);

    // expandedTitle: string
    napi_create_string_utf8(env, longTextContent->GetExpandedTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "expandedTitle", value);

    return NapiGetboolean(env, true);
}

// napi_value Common::SetNotificationPictureContent(
//     const napi_env &env, OHOS::Notification::NotificationPictureContent  *pictureContent, napi_value &result)
// {
//     ANS_LOGI("SetNotificationPictureContent start");
//     if (pictureContent == nullptr) {
//         ANS_LOGE("pictureContent is null");
//         return NapiGetboolean(env, false);
//     }

//     if (!SetNotificationBasicContent(env, pictureContent, result)) {
//         ANS_LOGE("SetNotificationBasicContent call failed");
//         return NapiGetboolean(env, false);
//     }

//     napi_value value;

//     //   pictureId: number   //c++ no do

//     // briefText: string
//     napi_create_string_utf8(env, pictureContent->GetBriefText().c_str(), NAPI_AUTO_LENGTH, &value);
//     napi_set_named_property(env, result, "briefText", value);

//     // expandedTitle: string
//     napi_create_string_utf8(env, pictureContent->GetExpandedTitle().c_str(), NAPI_AUTO_LENGTH, &value);
//     napi_set_named_property(env, result, "expandedTitle", value);

//     // bigPicture: image.PixelMap //do

//     return NapiGetboolean(env, true);
// }

// napi_value Common::SetNotificationConversationalContent(
//     const napi_env &env, OHOS::Notification::NotificationConversationalContent *conversationalContent, napi_value
//     &result)
// {
//     ANS_LOGI("SetNotificationConversationalContent start");
//     if (conversationalContent == nullptr) {
//         ANS_LOGE("conversationalContent is null");
//         return NapiGetboolean(env, false);
//     }

//     if (!SetNotificationBasicContent(env, conversationalContent, result)) {
//         ANS_LOGE("SetNotificationBasicContent call failed");
//         return NapiGetboolean(env, false);
//     }

//     napi_value value;
//     // conversationTitle: string
//     napi_create_string_utf8(env, conversationalContent->GetConversationTitle().c_str(), NAPI_AUTO_LENGTH, &value);
//     napi_set_named_property(env, result, "conversationTitle", value);

//     // conversationGroup: boolean
//     napi_get_boolean(env, conversationalContent->IsConversationGroup(), &value);
//     napi_set_named_property(env, result, "conversationGroup", value);

//     // messages: Array<ConversationalMessage>
//     napi_value arr;
//     int count = 0;
//     napi_create_array(env, &arr);
//     std::vector<std::shared_ptr<NotificationConversationalMessage>> messages =
//         conversationalContent->GetAllConversationalMessages();
//     for (auto vec : messages) {
//         if (vec) {
//             napi_value conversationalMessageResult;
//             napi_create_object(env, &conversationalMessageResult);
//             if (SetConversationalMessage(env, vec, conversationalMessageResult)) {
//                 napi_set_element(env, arr, count, conversationalMessageResult);
//                 count++;
//             }
//         }
//     }
//     napi_set_named_property(env, result, "messages", arr);

//     // user: MessageUser
//     napi_value messageUserResult;
//     napi_create_object(env, &messageUserResult);
//     if (!SetMessageUser(env, conversationalContent->GetMessageUser(), messageUserResult)) {
//         messageUserResult = NapiGetNull();
//     }
//     napi_set_named_property(env, result, "user", messageUserResult);

//     return NapiGetboolean(env, true);
// }

napi_value Common::SetNotificationMultiLineContent(
    const napi_env &env, OHOS::Notification::NotificationMultiLineContent *multiLineContent, napi_value &result)
{
    ANS_LOGI("SetNotificationMultiLineContent start");
    if (multiLineContent == nullptr) {
        ANS_LOGE("multiLineContent is null");
        return NapiGetboolean(env, false);
    }

    if (!SetNotificationBasicContent(env, multiLineContent, result)) {
        ANS_LOGE("SetNotificationBasicContent call failed");
        return NapiGetboolean(env, false);
    }

    napi_value value;
    // briefText: string
    napi_create_string_utf8(env, multiLineContent->GetBriefText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "briefText", value);

    // longTitle: string
    napi_create_string_utf8(env, multiLineContent->GetExpandedTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "longTitle", value);

    // lines: Array<String>
    napi_value arr;
    int count = 0;
    napi_create_array(env, &arr);
    for (auto vec : multiLineContent->GetAllLines()) {
        napi_create_string_utf8(env, vec.c_str(), NAPI_AUTO_LENGTH, &value);
        napi_set_element(env, arr, count, value);
        count++;
    }
    napi_set_named_property(env, result, "lines", arr);

    return NapiGetboolean(env, true);
}

//  napi_value Common::SetMessageUser(const napi_env &env, std::shared_ptr<MessageUser> &messageUser, napi_value
// &result)
// {
//     ANS_LOGI("SetMessageUser start");
//     if (messageUser == nullptr) {
//         ANS_LOGE("SetMessageUser is null");
//         return NapiGetboolean(env, false);
//     }

//     napi_value value;
//     // name: string
//     napi_create_string_utf8(env, messageUser->GetName().c_str(), NAPI_AUTO_LENGTH, &value);
//     napi_set_named_property(env, result, "name", value);

//     // pixelMap: image.PixelMap  //do

//     // uri: string
//     napi_create_string_utf8(env, pictureContent->GetUri().ToString().c_str(), NAPI_AUTO_LENGTH, &value);
//     napi_set_named_property(env, result, "uri", value);

//     // important: boolean
//     napi_get_boolean(env, pictureContent->IsUserImportant(), &value);
//     napi_set_named_property(env, result, "important", value);

//     // machine: boolean
//     napi_get_boolean(env, pictureContent->IsMachine(), &value);
//     napi_set_named_property(env, result, "machine", value);

//     // key: string
//     napi_create_string_utf8(env, pictureContent->GetKey().c_str(), NAPI_AUTO_LENGTH, &value);
//     napi_set_named_property(env, result, "key", value);

//     return NapiGetboolean(env, true);
// }

//  napi_value Common::SetConversationalMessage(
//     const napi_env &env, std::shared_ptr<NotificationConversationalMessage> &conversationalMessage, napi_value
//     &result)
// {
//     ANS_LOGI("SetConversationalMessage start");
//     if (conversationalMessage == nullptr) {
//         ANS_LOGE("SetConversationalMessage is null");
//         return NapiGetboolean(env, false);
//     }

//     napi_value value;
//     // text: string
//     napi_create_string_utf8(env, conversationalMessage->GetText().c_str(), NAPI_AUTO_LENGTH, &value);
//     napi_set_named_property(env, result, "text", value);

//     // timestamp: number;
//     napi_create_int64(env, conversationalMessage->GetArrivedTime(), &value);
//     napi_set_named_property(env, result, "timestamp", value);

//     // mimeType: string;
//     napi_create_string_utf8(env, conversationalMessage->GetMimeType().c_str(), NAPI_AUTO_LENGTH, &value);
//     napi_set_named_property(env, result, "mimeType", value);

//     // uri: string;
//     napi_create_string_utf8(env, conversationalMessage->GetUri()->ToString().c_str(), NAPI_AUTO_LENGTH, &value);
//     napi_set_named_property(env, result, "uri", value);

//     // sender: MessageUser;
//     napi_value messageUserResult;
//     napi_create_object(env, &messageUserResult);
//     if (SetMessageUser(env, conversationalMessage->GetSender(), messageUserResult)) {
//         messageUserResult = NapiGetNull();
//     }
//     napi_set_named_property(env, result, "sender", messageUserResult);

//     return NapiGetboolean(env, true);
// }

napi_value Common::SetNotificationActionButton(
    const napi_env &env, std::shared_ptr<NotificationActionButton> &actionButton, napi_value &result)
{
    ANS_LOGI("SetNotificationActionButton start");
    if (actionButton == nullptr) {
        ANS_LOGE("actionButton is null");
        return NapiGetboolean(env, false);
    }

    napi_value value;

    // title: string
    napi_create_string_utf8(env, actionButton->GetTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "title", value);

    // wantAgent: WantAgent
    std::shared_ptr<WantAgent::WantAgent> agent = actionButton->GetWantAgent();
    if (!agent) {
        ANS_LOGE("wantAgent is null");
        return NapiGetboolean(env, false);
    }
    napi_value wantAgent;
    wantAgent = CreateWantAgentByJS(env, agent);
    napi_set_named_property(env, result, "wantAgent", wantAgent);

    // extras?: {[key: string]: any}  //do
    // icon?: image.PixelMap //do
    return NapiGetboolean(env, true);
}

napi_value Common::GetNotificationSubscriberInfo(
    const napi_env &env, const napi_value &value, NotificationSubscriberInfo &result)
{
    uint32_t length = 0;
    size_t strLen;
    bool hasProperty = false;
    napi_valuetype valuetype;
    // bundleNames?: Array<string>;
    NAPI_CALL(env, napi_has_named_property(env, value, "bundleNames", &hasProperty));
    if (hasProperty) {
        napi_value nbundleNames;
        napi_get_named_property(env, value, "bundleNames", &nbundleNames);
        napi_get_array_length(env, nbundleNames, &length);
        if (length > 0) {
            for (uint32_t i = 0; i < length; ++i) {
                napi_value nbundleName;
                char str[STR_MAX_SIZE] = {0};
                napi_get_element(env, nbundleNames, i, &nbundleName);
                NAPI_CALL(env, napi_typeof(env, nbundleName, &valuetype));
                NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
                NAPI_CALL(env, napi_get_value_string_utf8(env, nbundleName, str, STR_MAX_SIZE - 1, &strLen));
                result.bundleNames.emplace_back(str);
                result.hasSubscriberInfo = true;
            }
        }
    }

    // deviceIds?: Array<string>;
    // NAPI_CALL(env, napi_has_named_property(env, value, "deviceIds", &hasProperty));
    // if (hasProperty) {
    //     napi_value ndeviceIds;
    //     napi_get_named_property(env, value, "deviceIds", &ndeviceIds);
    //     napi_get_array_length(env, value, &length);
    //     if (length > 0) {
    //         for (uint32_t i = 0; i < length; ++i) {
    //             napi_value ndeviceId;
    //             char str[STR_MAX_SIZE] = {0};
    //             napi_get_element(env, ndeviceIds, i, &ndeviceId);
    //             NAPI_CALL(env, napi_typeof(env, ndeviceId, &valuetype));
    //             NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    //             NAPI_CALL(env, napi_get_value_string_utf8(env, ndeviceId, str, STR_MAX_SIZE - 1, &strLen));
    //             result.deviceIds.emplace_back(str);
    //             result.hasSubscriberInfo = true;
    //         }
    //     }
    // }

    // userId?: number;
    NAPI_CALL(env, napi_has_named_property(env, value, "userId", &hasProperty));
    if (hasProperty) {
        napi_value nuserId;
        napi_get_named_property(env, value, "userId", &nuserId);
        NAPI_CALL(env, napi_typeof(env, nuserId, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        NAPI_CALL(env, napi_get_value_int32(env, nuserId, &result.userId));
        result.hasSubscriberInfo = true;
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationRequest(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    // content: NotificationContent
    if (GetNotificationContent(env, value, request) == nullptr) {
        return nullptr;
    }

    // id?: number
    if (GetIdByPublish(env, value, request) == nullptr) {
        return nullptr;
    }

    // slotType?: notification.SlotType
    if (GetSlotType(env, value, request) == nullptr) {
        return nullptr;
    }

    // isOngoing?: boolean
    if (GetNotificationIsOngoing(env, value, request) == nullptr) {
        return nullptr;
    }

    // isUnremovable?: boolean
    if (GetNotificationIsUnremovable(env, value, request) == nullptr) {
        return nullptr;
    }

    // deliveryTime?: number
    if (GetNotificationDeliveryTime(env, value, request) == nullptr) {
        return nullptr;
    }

    // tapDismissed?: boolean
    if (GetNotificationtapDismissed(env, value, request) == nullptr) {
        return nullptr;
    }

    // wantAgent?: WantAgent
    if (Common::GetNotificationWantAgent(env, value, request) == nullptr) {
        return nullptr;
    }

    // extraInfo?: {[key: string]: any} : do

    // removalWantAgent?: WantAgent
    if (GetNotificationRemovalWantAgent(env, value, request) == nullptr) {
        return nullptr;
    }

    // maxScreenWantAgent?: WantAgent
    if (GetNotificationMaxScreenWantAgent(env, value, request) == nullptr) {
        return nullptr;
    }

    // autoDeletedTime?: number
    if (GetNotificationAutoDeletedTime(env, value, request) == nullptr) {
        return nullptr;
    }

    // // settingsText?: string
    // if (GetNotificationSettingsText(env, value, request) == nullptr) {
    //     return nullptr;
    // }

    // // groupValue?: string
    // if (GetNotificationGroupValue(env, value, request) == nullptr) {
    //     return nullptr;
    // }

    // // groupAlertType?: number
    // if (GetNotificationGroupAlertType(env, value, request) == nullptr) {
    //     return nullptr;
    // }

    // // groupOverview?: boolean
    // if (GetNotificationGroupOverview(env, value, request) == nullptr) {
    //     return nullptr;
    // }

    // // classification?: string
    // if (GetNotificationClassification(env, value, request) == nullptr) {
    //     return nullptr;
    // }

    // color?: number
    if (GetNotificationColor(env, value, request) == nullptr) {
        return nullptr;
    }

    // colorEnabled?: boolean
    if (GetNotificationColorEnabled(env, value, request) == nullptr) {
        return nullptr;
    }

    // isAlertOnce?: boolean
    if (GetNotificationIsAlertOnce(env, value, request) == nullptr) {
        return nullptr;
    }

    // isStopwatch?: boolean
    if (GetNotificationIsStopwatch(env, value, request) == nullptr) {
        return nullptr;
    }

    // isCountDown?: boolean
    if (GetNotificationIsCountDown(env, value, request) == nullptr) {
        return nullptr;
    }

    // // visibleness?: number
    // if (GetNotificationVisibleness(env, value, request) == nullptr) {
    //     return nullptr;
    // }

    // progressValue?: number
    // progressMaxValue?: number
    // isIndeterminate?: boolean
    if (GetNotificationProgressBar(env, value, request) == nullptr) {
        return nullptr;
    }

    // progressMaxValue?: number
    // if (GetNotificationProgressMaxValueByPublish(env, value, notificationRequestData.progressMaxValue, request) ==
    //     nullptr) {
    //     return nullptr;
    // }

    // isIndeterminate?: boolean
    // if (GetNotificationIsIndeterminateByPublish(env, value, notificationRequestData.isIndeterminate, request) ==
    //     nullptr) {
    //     return nullptr;
    // }

    // statusBarText?: string
    if (GetNotificationStatusBarText(env, value, request) == nullptr) {
        return nullptr;
    }

    // // onlyLocal?: boolean
    // if (GetNotificationOnlyLocal(env, value, request) == nullptr) {
    //     return nullptr;
    // }

    // // sortingKey?: string
    // if (GetNotificationSortingKey(env, value, request) == nullptr) {
    //     return nullptr;
    // }

    // isFloatingIcon?: boolean
    // if (GetNotificationIsFloatingIcon(env, value, request) == nullptr) {
    //     return nullptr;
    // }

    // label?: string
    if (GetNotificationLabel(env, value, request) == nullptr) {
        return nullptr;
    }

    // badgeIconStyle?: number
    if (GetNotificationBadgeIconStyle(env, value, request) == nullptr) {
        return nullptr;
    }

    // // shortcutId?: string
    // if (GetNotificationShortcutId(env, value, request) == nullptr) {
    //     return nullptr;
    // }

    // showDeliveryTime?: boolean
    if (GetNotificationShowDeliveryTime(env, value, request) == nullptr) {
        return nullptr;
    }

    // actionButtons?: Array<NotificationActionButton>
    if (GetNotificationNotificationActionButtons(env, value, request) == nullptr) {
        return nullptr;
    }

    return Common::NapiGetNull(env);
}

napi_value GetIdByPublish(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    int32_t notificationId;

    NAPI_CALL(env, napi_has_named_property(env, value, "id", &hasProperty));
    ANS_LOGI("============GetIdByPublish hasProperty = %{public}d", hasProperty);
    if (hasProperty) {
        napi_get_named_property(env, value, "id", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, result, &notificationId);
        request.SetNotificationId(notificationId);
        ANS_LOGI("============GetIdByPublish notificationId = %{public}d", notificationId);
    } else {
        request.SetNotificationId(0);
    }

    ANS_LOGI("end");
    return Common::NapiGetNull(env);
}

napi_value GetSlotType(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    int32_t slotType;

    NAPI_CALL(env, napi_has_named_property(env, value, "slotType", &hasProperty));
    ANS_LOGI("============GetSlotType hasProperty = %{public}d", hasProperty);
    if (hasProperty) {
        napi_get_named_property(env, value, "slotType", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, result, &slotType);

        enum NotificationConstant::SlotType outType;
        if (!Common::SlotTypeJSToC(SlotType(slotType), outType)) {
            return nullptr;
        }
        request.SetSlotType(outType);
        ANS_LOGI("============GetSlotType slotType = %{public}d", slotType);
    } else {
        request.SetSlotType(NotificationConstant::OTHER);
    }
    ANS_LOGI("end");
    return Common::NapiGetNull(env);
}

napi_value Common::GetNotificationContent(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    int32_t type;

    NAPI_CALL(env, napi_has_named_property(env, value, "content", &hasProperty));
    ANS_LOGI("============GetNotificationContent hasProperty = %{public}d", hasProperty);
    if (hasProperty) {
        ANS_LOGI("============GetNotificationContent 000000000000000000000================");
        napi_get_named_property(env, value, "content", &result);
        ANS_LOGI("============GetNotificationContent 000000001111111111111111111================");
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        ANS_LOGI("============GetNotificationContent 0000000022222222222222222================");
        ANS_LOGI("============GetNotificationContent 0000000022222222222222222================ valuetype = "
                 "%{public}d",
            valuetype);
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        ANS_LOGI("============GetNotificationContent 11111111111111================");
        if (GetNotificationContentType(env, result, type) == nullptr) {
            return nullptr;
        }

        NotificationContent::Type outType;
        if (!ContentTypeJSToC(ContentType(type), outType)) {
            return nullptr;
        }
        switch (outType) {
            case NotificationContent::Type::BASIC_TEXT:
                ANS_LOGI("============GetNotificationContent================");
                if (GetNotificationBasicContent(env, result, request) == nullptr) {
                    return nullptr;
                }
                break;
            case NotificationContent::Type::LONG_TEXT:
                ANS_LOGI("============GetNotificationLongContentByPublish================");
                if (GetNotificationLongTextContent(env, result, request) == nullptr) {
                    return nullptr;
                }
                break;
            case NotificationContent::Type::PICTURE:
                break;
            case NotificationContent::Type::CONVERSATION:
                break;
            case NotificationContent::Type::MULTILINE:
                ANS_LOGI("============GetNotificationMultilineContentByPublish================");
                if (GetNotificationMultiLineContent(env, result, request) == nullptr) {
                    return nullptr;
                }
                break;
            case NotificationContent::Type::MEDIA:
                break;
            default:
                return nullptr;
                break;
        }
    }

    ANS_LOGI("end");
    return Common::NapiGetNull(env);
}

napi_value GetNotificationIsOngoing(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    bool isOngoing;

    NAPI_CALL(env, napi_has_named_property(env, value, "isOngoing", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isOngoing", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isOngoing);
        request.SetInProgress(isOngoing);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationIsUnremovable(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    bool isUnremovable;

    NAPI_CALL(env, napi_has_named_property(env, value, "isUnremovable", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isUnremovable", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isUnremovable);
        request.SetUnremovable(isUnremovable);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationDeliveryTime(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    int64_t deliveryTime;

    NAPI_CALL(env, napi_has_named_property(env, value, "deliveryTime", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "deliveryTime", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int64(env, result, &deliveryTime);
        request.SetDeliveryTime(deliveryTime);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationtapDismissed(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    bool tapDismissed;

    NAPI_CALL(env, napi_has_named_property(env, value, "tapDismissed", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "tapDismissed", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &tapDismissed);
        request.SetTapDismissed(tapDismissed);
    }

    return Common::NapiGetNull(env);
}

napi_value Common::GetNotificationWantAgent(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    bool hasProperty = false;
    WantAgent::WantAgent *wantAgent = nullptr;
    napi_value result;
    napi_valuetype valuetype;

    NAPI_CALL(env, napi_has_named_property(env, value, "wantAgent", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "wantAgent", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Bool expected.");
        napi_unwrap(env, result, (void **)&wantAgent);
        if (wantAgent == nullptr) {
            return nullptr;
        }
        std::shared_ptr<WantAgent::WantAgent> sWantAgent{wantAgent};
        request.SetWantAgent(sWantAgent);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationRemovalWantAgent(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    bool hasProperty = false;
    WantAgent::WantAgent *wantAgent = nullptr;
    napi_value result;
    napi_valuetype valuetype;

    NAPI_CALL(env, napi_has_named_property(env, value, "removalWantAgent", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "removalWantAgent", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Bool expected.");
        napi_unwrap(env, result, (void **)&wantAgent);
        if (wantAgent == nullptr) {
            return nullptr;
        }
        std::shared_ptr<WantAgent::WantAgent> removeWantAgent(wantAgent);
        request.SetRemovalWantAgent(removeWantAgent);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationMaxScreenWantAgent(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    bool hasProperty = false;
    WantAgent::WantAgent *wantAgent = nullptr;
    napi_value result;
    napi_valuetype valuetype;

    NAPI_CALL(env, napi_has_named_property(env, value, "maxScreenWantAgent", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "maxScreenWantAgent", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Bool expected.");
        napi_unwrap(env, result, (void **)&wantAgent);
        if (wantAgent == nullptr) {
            return nullptr;
        }
        std::shared_ptr<WantAgent::WantAgent> maxScreenWantAgent(wantAgent);
        request.SetMaxScreenWantAgent(maxScreenWantAgent);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationAutoDeletedTime(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    int64_t autoDeletedTime;

    NAPI_CALL(env, napi_has_named_property(env, value, "autoDeletedTime", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "autoDeletedTime", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int64(env, result, &autoDeletedTime);
        request.SetAutoDeletedTime(autoDeletedTime);
    }

    return Common::NapiGetNull(env);
}

// napi_value GetNotificationSettingsText(
//     const napi_env &env, const napi_value &value, NotificationRequest &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;
//     char str[STR_MAX_SIZE] = {0};
//     size_t strLen = 0;

//     NAPI_CALL(env, napi_has_named_property(env, value, "settingsText", &hasProperty));
//     if (hasProperty) {
//         napi_get_named_property(env, value, "settingsText", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
//         NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
//         request.SetSettingsText(str);
//     }

//     ANS_LOGI("===============GetNotificationSettingsText====settingsText = %{public}s end", str);
//     return Common::NapiGetNull(env);
// }

// napi_value GetNotificationGroupValue(
//     const napi_env &env, const napi_value &value, NotificationRequest &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;
//     char str[STR_MAX_SIZE] = {0};
//     size_t strLen = 0;

//     NAPI_CALL(env, napi_has_named_property(env, value, "groupValue", &hasProperty));
//     if (hasProperty) {
//         napi_get_named_property(env, value, "groupValue", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
//         NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
//         request.SetGroupValue(str);
//     }

//     ANS_LOGI("===============GetNotificationGroupValue====groupValue = %{public}s end", str);
//     return Common::NapiGetNull(env);
// }

// napi_value GetNotificationGroupAlertType(
//     const napi_env &env, const napi_value &value, NotificationRequest &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;
//     int32_t groupAlertType;

//     NAPI_CALL(env, napi_has_named_property(env, value, "groupAlertType", &hasProperty));
//     if (hasProperty) {
//         napi_get_named_property(env, value, "groupAlertType", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
//         napi_get_value_int32(env, result, &groupAlertType);
//         request.SetGroupAlertType(static_cast<NotificationRequest::GroupAlertType>(groupAlertType));
//     }

//     return Common::NapiGetNull(env);
// }

// napi_value GetNotificationGroupOverview(
//     const napi_env &env, const napi_value &value, NotificationRequest &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;
//     bool groupOverview;

//     NAPI_CALL(env, napi_has_named_property(env, value, "groupOverview", &hasProperty));
//     if (hasProperty) {
//         napi_get_named_property(env, value, "groupOverview", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
//         napi_get_value_bool(env, result, &groupOverview);
//         request.SetGroupOverview(groupOverview);
//     }

//     return Common::NapiGetNull(env);
// }

// napi_value GetNotificationClassification(
//     const napi_env &env, const napi_value &value, NotificationRequest &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;
//     char str[STR_MAX_SIZE] = {0};
//     size_t strLen = 0;

//     NAPI_CALL(env, napi_has_named_property(env, value, "classification", &hasProperty));
//     if (hasProperty) {
//         napi_get_named_property(env, value, "classification", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
//         NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
//         request.SetClassification(str);
//     }

//     ANS_LOGI("===============GetNotificationClassification====classification = %{public}s end", str);
//     return Common::NapiGetNull(env);
// }

napi_value GetNotificationColor(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    int32_t color;

    NAPI_CALL(env, napi_has_named_property(env, value, "color", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "color", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, result, &color);
        request.SetColor(color);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationColorEnabled(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    bool colorEnabled;

    NAPI_CALL(env, napi_has_named_property(env, value, "colorEnabled", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "colorEnabled", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &colorEnabled);
        request.SetColorEnabled(colorEnabled);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationIsAlertOnce(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    bool isAlertOnce;

    NAPI_CALL(env, napi_has_named_property(env, value, "isAlertOnce", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isAlertOnce", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isAlertOnce);
        request.SetAlertOneTime(isAlertOnce);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationIsStopwatch(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    bool isStopwatch;

    NAPI_CALL(env, napi_has_named_property(env, value, "isStopwatch", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isStopwatch", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isStopwatch);
        request.SetShowStopwatch(isStopwatch);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationIsCountDown(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    bool isCountDown;

    NAPI_CALL(env, napi_has_named_property(env, value, "isCountDown", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isCountDown", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isCountDown);
        request.SetCountdownTimer(isCountDown);
    }

    return Common::NapiGetNull(env);
}

// napi_value GetNotificationVisibleness(
//     const napi_env &env, const napi_value &value, NotificationRequest &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;
//     int32_t visibleness;

//     NAPI_CALL(env, napi_has_named_property(env, value, "visibleness", &hasProperty));
//     if (hasProperty) {
//         napi_get_named_property(env, value, "visibleness", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
//         napi_get_value_int32(env, result, &visibleness);
//         request.SetVisibleness(static_cast<NotificationConstant::VisiblenessType>(visibleness));
//     }

//     return Common::NapiGetNull(env);
// }
napi_value GetNotificationProgressBar(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value resultProgressValue, resultProgressMaxValue, resultIsIndeterminate;
    bool hasPropertyProgressValue = false;
    bool hasPropertyProgressMaxValue = false;
    bool hasPropertyIsIndeterminate = false;
    int32_t progressValue = 0;
    int32_t progressMaxValue = 0;
    bool isIndeterminate = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "progressValue", &hasPropertyProgressValue));
    if (hasPropertyProgressValue) {
        NAPI_CALL(env, napi_has_named_property(env, value, "progressMaxValue", &hasPropertyProgressMaxValue));
        if (hasPropertyProgressMaxValue) {
            NAPI_CALL(env, napi_has_named_property(env, value, "isIndeterminate", &hasPropertyIsIndeterminate));
            if (hasPropertyIsIndeterminate) {
                napi_get_named_property(env, value, "progressValue", &resultProgressValue);
                NAPI_CALL(env, napi_typeof(env, resultProgressValue, &valuetype));
                NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
                napi_get_value_int32(env, resultProgressValue, &progressValue);

                napi_get_named_property(env, value, "progressMaxValue", &resultProgressMaxValue);
                NAPI_CALL(env, napi_typeof(env, resultProgressMaxValue, &valuetype));
                NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
                napi_get_value_int32(env, resultProgressMaxValue, &progressMaxValue);

                napi_get_named_property(env, value, "isIndeterminate", &resultIsIndeterminate);
                NAPI_CALL(env, napi_typeof(env, resultIsIndeterminate, &valuetype));
                NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. bool expected.");
                napi_get_value_bool(env, resultIsIndeterminate, &isIndeterminate);
            }
        }
        request.SetProgressBar(progressValue, progressMaxValue, isIndeterminate);
    }

    return Common::NapiGetNull(env);
}

// napi_value GetNotificationProgressMaxValueByPublish(
//     const napi_env &env, const napi_value &value, int32_t &progressMaxValue, NotificationRequest &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;

//     NAPI_CALL(env, napi_has_named_property(env, value, "progressMaxValue", &hasProperty));
//     if (hasProperty) {
//         // isSetRequestProperty[PROGRESSMAXVALUE] = true;
//         napi_get_named_property(env, value, "progressMaxValue", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. int expected.");
//         napi_get_value_int32(env, result, &progressMaxValue);
//     } else {
//         // isSetRequestProperty[PROGRESSMAXVALUE] = false;
//     }

//     return Common::NapiGetNull(env);
// }

// napi_value GetNotificationIsIndeterminateByPublish(
//     const napi_env &env, const napi_value &value, bool &isIndeterminate, NotificationRequest &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;

//     NAPI_CALL(env, napi_has_named_property(env, value, "isIndeterminate", &hasProperty));
//     if (hasProperty) {
//         // isSetRequestProperty[ISINDETERMINATE] = true;
//         napi_get_named_property(env, value, "isIndeterminate", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. bool expected.");
//         napi_get_value_bool(env, result, &isIndeterminate);
//     } else {
//         // isSetRequestProperty[ISINDETERMINATE] = false;
//     }

//     return Common::NapiGetNull(env);
// }

napi_value GetNotificationStatusBarText(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "statusBarText", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "statusBarText", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
        request.SetStatusBarText(str);
    }

    ANS_LOGI("===============GetNotificationStatusBarText====statusBarText = %{public}s end", str);
    return Common::NapiGetNull(env);
}

// napi_value GetNotificationOnlyLocal(const napi_env &env, const napi_value &value, NotificationRequest
// &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;
//     bool onlyLocal;

//     NAPI_CALL(env, napi_has_named_property(env, value, "onlyLocal", &hasProperty));
//     if (hasProperty) {
//         napi_get_named_property(env, value, "onlyLocal", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. bool expected.");
//         napi_get_value_bool(env, result, &onlyLocal);
//         request.SetOnlyLocal(onlyLocal);
//     }

//     return Common::NapiGetNull(env);
// }

// napi_value GetNotificationSortingKey(
//     const napi_env &env, const napi_value &value, NotificationRequest &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;
//     char str[STR_MAX_SIZE] = {0};
//     size_t strLen = 0;

//     NAPI_CALL(env, napi_has_named_property(env, value, "sortingKey", &hasProperty));
//     if (hasProperty) {
//         napi_get_named_property(env, value, "sortingKey", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
//         NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
//         request.SetSortingKey(str);
//     }

//     ANS_LOGI("===============GetNotificationSortingKey====sortingKey = %{public}s end", str);
//     return Common::NapiGetNull(env);
// }

napi_value GetNotificationIsFloatingIcon(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    bool isFloatingIcon;

    NAPI_CALL(env, napi_has_named_property(env, value, "isFloatingIcon", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isFloatingIcon", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. bool expected.");
        napi_get_value_bool(env, result, &isFloatingIcon);
        request.SetOnlyLocal(isFloatingIcon);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationLabel(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "label", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "label", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
        request.SetLabel(str);
    }

    ANS_LOGI("===============GetNotificationLabel====label = %{public}s end", str);
    return Common::NapiGetNull(env);
}

napi_value GetNotificationBadgeIconStyle(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    int32_t badgeIconStyle;

    NAPI_CALL(env, napi_has_named_property(env, value, "badgeIconStyle", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "badgeIconStyle", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, result, &badgeIconStyle);
        request.SetBadgeIconStyle(static_cast<NotificationRequest::BadgeStyle>(badgeIconStyle));
    }

    return Common::NapiGetNull(env);
}

// napi_value GetNotificationShortcutId(
//     const napi_env &env, const napi_value &value, NotificationRequest &request)
// {
//     ANS_LOGI("enter");

//     napi_valuetype valuetype;
//     napi_value result;
//     bool hasProperty = false;
//     char str[STR_MAX_SIZE] = {0};
//     size_t strLen = 0;

//     NAPI_CALL(env, napi_has_named_property(env, value, "shortcutId", &hasProperty));
//     if (hasProperty) {
//         napi_get_named_property(env, value, "shortcutId", &result);
//         NAPI_CALL(env, napi_typeof(env, result, &valuetype));
//         NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
//         NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
//         request.SetShortcutId(str);
//     }

//     ANS_LOGI("===============GetNotificationShortcutId====shortcutId = %{public}s end", str);
//     return Common::NapiGetNull(env);
// }

napi_value GetNotificationShowDeliveryTime(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value result;
    bool hasProperty = false;
    bool showDeliveryTime;

    NAPI_CALL(env, napi_has_named_property(env, value, "showDeliveryTime", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "showDeliveryTime", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. bool expected.");
        napi_get_value_bool(env, result, &showDeliveryTime);
        request.SetShowDeliveryTime(showDeliveryTime);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationNotificationActionButtons(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    bool isArray = false;
    napi_valuetype valuetype;
    napi_value actionButtons;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    uint32_t length = 0;
    bool hasProperty = false;
    napi_value titelActionButton, wantAgentActionButton;
    std::string title;
    WantAgent::WantAgent *wantAgentPtr = nullptr;
    std::shared_ptr<WantAgent::WantAgent> wantAgent;

    napi_get_named_property(env, value, "actionButtons", &actionButtons);
    napi_is_array(env, actionButtons, &isArray);
    if (isArray) {
        napi_get_array_length(env, actionButtons, &length);
        NAPI_ASSERT(env, length > 0, "The array is empty.");
        for (size_t i = 0; i < length; i++) {
            napi_value actionButton;
            napi_get_element(env, actionButtons, i, &actionButton);
            NAPI_CALL(env, napi_typeof(env, actionButton, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

            NAPI_CALL(env, napi_has_named_property(env, actionButton, "title", &hasProperty));
            if (hasProperty) {
                napi_get_named_property(env, actionButton, "title", &titelActionButton);
                NAPI_CALL(env, napi_typeof(env, titelActionButton, &valuetype));
                NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
                NAPI_CALL(env, napi_get_value_string_utf8(env, titelActionButton, str, STR_MAX_SIZE - 1, &strLen));
                title = str;
            }

            NAPI_CALL(env, napi_has_named_property(env, actionButton, "wantAgent", &hasProperty));
            if (hasProperty) {
                napi_get_named_property(env, actionButton, "wantAgent", &wantAgentActionButton);
                NAPI_CALL(env, napi_typeof(env, wantAgentActionButton, &valuetype));
                NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. String expected.");
                napi_unwrap(env, wantAgentActionButton, (void **)&wantAgentPtr);
                if (wantAgentPtr == nullptr) {
                    return nullptr;
                }
                std::shared_ptr<WantAgent::WantAgent> wantAgentOut(wantAgentPtr);
                wantAgent = wantAgentOut;
            }

            std::vector<std::shared_ptr<NotificationUserInput>> vec{nullptr};

            auto pActionButton = NotificationActionButton::Create(
                nullptr, title, wantAgent, nullptr, NotificationConstant::ARCHIVE_ACTION_BUTTON, false, vec, false);
            request.AddActionButton(pActionButton);
        }
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationContentType(const napi_env &env, const napi_value &result, int32_t &type)
{
    ANS_LOGI("enter");

    napi_value contentResult;
    napi_valuetype valuetype;
    bool hasProperty = false;

    NAPI_CALL(env, napi_has_named_property(env, result, "contentType", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, result, "contentType", &contentResult);
        NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, contentResult, &type);
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationBasicContent(const napi_env &env, const napi_value &result, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value contentResult, basicContentResult;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    NAPI_CALL(env, napi_has_named_property(env, result, "normal", &hasProperty));
    if (hasProperty) {
        std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
        napi_get_named_property(env, result, "normal", &contentResult);
        NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "title", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "title", &basicContentResult);
            NAPI_CALL(env, napi_typeof(env, basicContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, basicContentResult, str, STR_MAX_SIZE - 1, &strLen));
            normalContent->SetTitle(str);
            ANS_LOGI("normal::title = %{public}s", str);
        }

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "text", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "text", &basicContentResult);
            NAPI_CALL(env, napi_typeof(env, basicContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, basicContentResult, str, STR_MAX_SIZE - 1, &strLen));
            normalContent->SetText(str);
            ANS_LOGI("normal::text = %{public}s", str);
        }

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "additionalText", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "additionalText", &basicContentResult);
            NAPI_CALL(env, napi_typeof(env, basicContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, basicContentResult, str, STR_MAX_SIZE - 1, &strLen));
            normalContent->SetAdditionalText(str);
            ANS_LOGI("normal::additionalText = %{public}s", str);
        }
        request.SetContent(std::make_shared<NotificationContent>(normalContent));
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationLongTextContent(const napi_env &env, const napi_value &result, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value contentResult, longContentResult, basicContentResult;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    NAPI_CALL(env, napi_has_named_property(env, result, "longText", &hasProperty));
    if (hasProperty) {
        std::shared_ptr<OHOS::Notification::NotificationLongTextContent> longContent =
            std::make_shared<OHOS::Notification::NotificationLongTextContent>();
        napi_get_named_property(env, result, "longText", &contentResult);
        NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "title", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "title", &basicContentResult);
            NAPI_CALL(env, napi_typeof(env, basicContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, basicContentResult, str, STR_MAX_SIZE - 1, &strLen));
            longContent->SetTitle(str);
            ANS_LOGI("longText::title = %{public}s", str);
        }

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "text", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "text", &basicContentResult);
            NAPI_CALL(env, napi_typeof(env, basicContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, basicContentResult, str, STR_MAX_SIZE - 1, &strLen));
            longContent->SetText(str);
            ANS_LOGI("longText::text = %{public}s", str);
        }

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "additionalText", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "additionalText", &basicContentResult);
            NAPI_CALL(env, napi_typeof(env, basicContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, basicContentResult, str, STR_MAX_SIZE - 1, &strLen));
            longContent->SetAdditionalText(str);
            ANS_LOGI("longText::additionalText = %{public}s", str);
        }

        napi_get_named_property(env, result, "longText", &contentResult);
        NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "longText", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "longText", &longContentResult);
            NAPI_CALL(env, napi_typeof(env, longContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, longContentResult, str, STR_MAX_SIZE - 1, &strLen));
            longContent->SetLongText(str);
            ANS_LOGI("longText::longText = %{public}s", str);
        }

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "briefText", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "briefText", &longContentResult);
            NAPI_CALL(env, napi_typeof(env, longContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, longContentResult, str, STR_MAX_SIZE - 1, &strLen));
            longContent->SetBriefText(str);
            ANS_LOGI("longText::briefText = %{public}s", str);
        }

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "expandedTitle", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "expandedTitle", &longContentResult);
            NAPI_CALL(env, napi_typeof(env, longContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, longContentResult, str, STR_MAX_SIZE - 1, &strLen));
            longContent->SetExpandedTitle(str);
            ANS_LOGI("longText::expandedTitle = %{public}s", str);
        }
        request.SetContent(std::make_shared<NotificationContent>(longContent));
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationMultiLineContent(const napi_env &env, const napi_value &result, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype;
    napi_value contentResult, multiLineContentResult, basicContentResult;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    NAPI_CALL(env, napi_has_named_property(env, result, "multiLine", &hasProperty));
    if (hasProperty) {
        std::shared_ptr<OHOS::Notification::NotificationMultiLineContent> multiLineContent =
            std::make_shared<OHOS::Notification::NotificationMultiLineContent>();
        napi_get_named_property(env, result, "multiLine", &contentResult);
        NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "title", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "title", &basicContentResult);
            NAPI_CALL(env, napi_typeof(env, basicContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, basicContentResult, str, STR_MAX_SIZE - 1, &strLen));
            multiLineContent->SetTitle(str);
            ANS_LOGI("multiLine::title = %{public}s", str);
        }

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "text", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "text", &basicContentResult);
            NAPI_CALL(env, napi_typeof(env, basicContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, basicContentResult, str, STR_MAX_SIZE - 1, &strLen));
            multiLineContent->SetText(str);
            ANS_LOGI("multiLine::text = %{public}s", str);
        }

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "additionalText", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "additionalText", &basicContentResult);
            NAPI_CALL(env, napi_typeof(env, basicContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, basicContentResult, str, STR_MAX_SIZE - 1, &strLen));
            multiLineContent->SetAdditionalText(str);
            ANS_LOGI("multiLine::additionalText = %{public}s", str);
        }

        napi_get_named_property(env, result, "multiLine", &contentResult);
        NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "briefText", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "briefText", &multiLineContentResult);
            NAPI_CALL(env, napi_typeof(env, multiLineContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, multiLineContentResult, str, STR_MAX_SIZE - 1, &strLen));
            multiLineContent->SetBriefText(str);
            ANS_LOGI("multiLine: briefText = %{public}s", str);
        }

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "longTitle", &hasProperty));
        if (hasProperty) {
            napi_get_named_property(env, contentResult, "longTitle", &multiLineContentResult);
            NAPI_CALL(env, napi_typeof(env, multiLineContentResult, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, multiLineContentResult, str, STR_MAX_SIZE - 1, &strLen));
            multiLineContent->SetExpandedTitle(str);
            ANS_LOGI("multiLine: longTitle = %{public}s", str);
        }

        NAPI_CALL(env, napi_has_named_property(env, contentResult, "lines", &hasProperty));
        if (hasProperty) {
            if (GetNotificationMultiLineContentLines(env, contentResult, multiLineContent) == nullptr) {
                return nullptr;
            }
        }

        request.SetContent(std::make_shared<NotificationContent>(multiLineContent));
    }

    return Common::NapiGetNull(env);
}

napi_value GetNotificationMultiLineContentLines(const napi_env &env, const napi_value &result,
    std::shared_ptr<OHOS::Notification::NotificationMultiLineContent> &multiLineContent)
{
    ANS_LOGI("enter");

    bool isArray = false;
    napi_valuetype valuetype;
    napi_value multilines;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    uint32_t length = 0;

    napi_get_named_property(env, result, "lines", &multilines);
    napi_is_array(env, multilines, &isArray);
    if (isArray) {
        napi_get_array_length(env, multilines, &length);
        NAPI_ASSERT(env, length > 0, "The array is empty.");
        for (size_t i = 0; i < length; i++) {
            napi_value line;
            napi_get_element(env, multilines, i, &line);
            NAPI_CALL(env, napi_typeof(env, line, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, line, str, STR_MAX_SIZE - 1, &strLen));
            multiLineContent->AddSingleLine(str);
            ANS_LOGI("multiLine: lines : addSingleLine = %{public}s", str);
        }
    }

    return Common::NapiGetNull(env);
}

napi_value Common::GetNotificationSlot(const napi_env &env, NotificationSlot &slot, const napi_value &result)
{
    ANS_LOGI("enter");
    napi_value nobj = nullptr;
    napi_valuetype valuetype;
    bool hasProperty = false;
    size_t strLen = 0;
    uint32_t length = 0;

    // type: notification.SlotType
    int slotType = 0;
    napi_get_named_property(env, result, "type", &nobj);
    NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
    napi_get_value_int32(env, nobj, &slotType);

    enum NotificationConstant::SlotType outType;
    if (!SlotTypeJSToC(SlotType(slotType), outType)) {
        return nullptr;
    }
    slot.SetType(outType);

    // level?: number
    NAPI_CALL(env, napi_has_named_property(env, result, "level", &hasProperty));
    if (hasProperty) {
        int level = 0;
        napi_get_named_property(env, result, "level", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, nobj, &level);
        ANS_LOGI("level is: %{public}d", level);
        slot.SetLevel(NotificationSlot::NotificationLevel(level));
    }

    // desc?: string
    NAPI_CALL(env, napi_has_named_property(env, result, "desc", &hasProperty));
    if (hasProperty) {
        std::string desc;
        char str[STR_MAX_SIZE] = {0};
        napi_get_named_property(env, result, "desc", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, nobj, str, STR_MAX_SIZE - 1, &strLen));
        desc = str;
        ANS_LOGI("desc is: %{public}s", desc.c_str());
        slot.SetDescription(desc);
    }

    // showBadgeFlag?: boolean
    NAPI_CALL(env, napi_has_named_property(env, result, "showBadgeFlag", &hasProperty));
    if (hasProperty) {
        bool showBadgeFlag = false;
        napi_get_named_property(env, result, "showBadgeFlag", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, nobj, &showBadgeFlag);
        ANS_LOGI("showBadgeFlag is: %{public}d", showBadgeFlag);
        slot.EnableBadge(showBadgeFlag);
    }

    // bypassDnd?: boolean
    NAPI_CALL(env, napi_has_named_property(env, result, "bypassDnd", &hasProperty));
    if (hasProperty) {
        bool bypassDnd = false;
        napi_get_named_property(env, result, "bypassDnd", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, nobj, &bypassDnd);
        ANS_LOGI("bypassDnd is: %{public}d", bypassDnd);
        slot.EnableBypassDnd(bypassDnd);
    }

    // lockscreenVisibility?: number
    NAPI_CALL(env, napi_has_named_property(env, result, "lockscreenVisibility", &hasProperty));
    if (hasProperty) {
        int lockscreenVisibility = 0;
        napi_get_named_property(env, result, "lockscreenVisibility", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, nobj, &lockscreenVisibility);
        ANS_LOGI("lockscreenVisibility is: %{public}d", lockscreenVisibility);
        slot.SetLockscreenVisibleness(NotificationConstant::VisiblenessType(lockscreenVisibility));
    }

    // vibrationEnabled?: boolean
    NAPI_CALL(env, napi_has_named_property(env, result, "vibrationEnabled", &hasProperty));
    if (hasProperty) {
        bool vibrationEnabled = false;
        napi_get_named_property(env, result, "vibrationEnabled", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, nobj, &vibrationEnabled);
        ANS_LOGI("vibrationEnabled is: %{public}d", vibrationEnabled);
        slot.SetEnableVibration(vibrationEnabled);
    }

    // sound?: string
    NAPI_CALL(env, napi_has_named_property(env, result, "sound", &hasProperty));
    if (hasProperty) {
        std::string sound;
        char str[STR_MAX_SIZE] = {0};
        napi_get_named_property(env, result, "sound", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, nobj, str, STR_MAX_SIZE - 1, &strLen));
        sound = str;
        ANS_LOGI("sound is: %{public}s", sound.c_str());
        slot.SetSound(Uri(sound));
    }

    // lightEnabled?: boolean
    NAPI_CALL(env, napi_has_named_property(env, result, "lightEnabled", &hasProperty));
    if (hasProperty) {
        bool lightEnabled = false;
        napi_get_named_property(env, result, "lightEnabled", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, nobj, &lightEnabled);
        ANS_LOGI("lightEnabled is: %{public}d", lightEnabled);
        slot.SetEnableLight(lightEnabled);
    }

    // lightColor?: number
    NAPI_CALL(env, napi_has_named_property(env, result, "lightColor", &hasProperty));
    if (hasProperty) {
        int lightColor = 0;
        napi_get_named_property(env, result, "lightColor", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, nobj, &lightColor);
        ANS_LOGI("lightColor is: %{public}d", lightColor);
        slot.SetLedLightColor(lightColor);
    }

    // vibrationValues?: Array<number>
    NAPI_CALL(env, napi_has_named_property(env, result, "vibrationValues", &hasProperty));
    if (hasProperty) {
        bool isArray = false;
        napi_get_named_property(env, result, "vibrationValues", &nobj);
        napi_is_array(env, nobj, &isArray);
        if (isArray) {
            napi_get_array_length(env, nobj, &length);
            std::vector<int64_t> vibrationValues;
            for (size_t i = 0; i < length; i++) {
                napi_value nVibrationValue;
                int64_t vibrationValue = 0;
                napi_get_element(env, nobj, i, &nVibrationValue);
                NAPI_CALL(env, napi_typeof(env, nVibrationValue, &valuetype));
                NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
                napi_get_value_int64(env, nVibrationValue, &vibrationValue);
                ANS_LOGI("vibrationValue is: %{public}lld", vibrationValue);
                vibrationValues.emplace_back(vibrationValue);
            }
            if (vibrationValues.size() > 0) {
                slot.SetVibrationStyle(vibrationValues);
            }
        }
    }
    return NapiGetNull(env);
}

bool Common::ContentTypeJSToC(const enum ContentType &inType, enum NotificationContent::Type &outType)
{
    switch (inType) {
        case ContentType::NOTIFICATION_CONTENT_BASIC_TEXT:
            outType = NotificationContent::Type::BASIC_TEXT;
            break;
        case ContentType::NOTIFICATION_CONTENT_LONG_TEXT:
            outType = NotificationContent::Type::LONG_TEXT;
            break;
        case ContentType::NOTIFICATION_CONTENT_PICTURE:
            outType = NotificationContent::Type::PICTURE;
            break;
        case ContentType::NOTIFICATION_CONTENT_CONVERSATION:
            outType = NotificationContent::Type::CONVERSATION;
            break;
        case ContentType::NOTIFICATION_CONTENT_MULTILINE:
            outType = NotificationContent::Type::MULTILINE;
            break;
        case ContentType::NOTIFICATION_CONTENT_MEDIA:
            outType = NotificationContent::Type::MEDIA;
            break;
        default:
            ANS_LOGI("contentType is error");
            return false;
    }
    return true;
}

bool Common::ContentTypeCToJS(const enum NotificationContent::Type &inType, enum ContentType &outType)
{
    switch (inType) {
        case NotificationContent::Type::BASIC_TEXT:
            outType = ContentType::NOTIFICATION_CONTENT_BASIC_TEXT;
            break;
        case NotificationContent::Type::LONG_TEXT:
            outType = ContentType::NOTIFICATION_CONTENT_LONG_TEXT;
            break;
        case NotificationContent::Type::PICTURE:
            outType = ContentType::NOTIFICATION_CONTENT_PICTURE;
            break;
        case NotificationContent::Type::CONVERSATION:
            outType = ContentType::NOTIFICATION_CONTENT_CONVERSATION;
            break;
        case NotificationContent::Type::MULTILINE:
            outType = ContentType::NOTIFICATION_CONTENT_MULTILINE;
            break;
        case NotificationContent::Type::MEDIA:
            outType = ContentType::NOTIFICATION_CONTENT_MEDIA;
            break;
        default:
            ANS_LOGI("contentType is error");
            return false;
    }
    return true;
}

bool Common::SlotTypeJSToC(const enum SlotType &inType, enum NotificationConstant::SlotType &outType)
{
    switch (inType) {
        case SlotType::UNKNOWN_TYPE:
            outType = NotificationConstant::SlotType::CUSTOM;
            break;
        case SlotType::SOCIAL_COMMUNICATION:
            outType = NotificationConstant::SlotType::SOCIAL_COMMUNICATION;
            break;
        case SlotType::SERVICE_INFORMATION:
            outType = NotificationConstant::SlotType::SERVICE_REMINDER;
            break;
        case SlotType::CONTENT_INFORMATION:
            outType = NotificationConstant::SlotType::CONTENT_INFORMATION;
            break;
        case SlotType::OTHER_TYPES:
            outType = NotificationConstant::SlotType::OTHER;
            break;
        default:
            ANS_LOGI("slotType is error");
            return false;
    }
    return true;
}

bool Common::SlotTypeCToJS(const enum NotificationConstant::SlotType &inType, enum SlotType &outType)
{
    switch (inType) {
        case NotificationConstant::SlotType::CUSTOM:
            outType = SlotType::UNKNOWN_TYPE;
            break;
        case NotificationConstant::SlotType::SOCIAL_COMMUNICATION:
            outType = SlotType::SOCIAL_COMMUNICATION;
            break;
        case NotificationConstant::SlotType::SERVICE_REMINDER:
            outType = SlotType::SERVICE_INFORMATION;
            break;
        case NotificationConstant::SlotType::CONTENT_INFORMATION:
            outType = SlotType::CONTENT_INFORMATION;
            break;
        case NotificationConstant::SlotType::OTHER:
            outType = SlotType::OTHER_TYPES;
            break;
        default:
            ANS_LOGI("slotType is error");
            return false;
    }
    return true;
}

bool Common::ReasonCToJS(const int &inType, int &outType)
{
    switch (inType) {
        case NotificationConstant::CLICK_REASON_DELETE:
            outType = NotificationReason::CLICK_REASON_DELETE;
            break;
        case NotificationConstant::CANCEL_REASON_DELETE:
            outType = NotificationReason::CANCEL_REASON_DELETE;
            break;
        case NotificationConstant::CANCEL_ALL_REASON_DELETE:
            outType = NotificationReason::CANCEL_ALL_REASON_DELETE;
            break;
        case NotificationConstant::ERROR_REASON_DELETE:
            outType = NotificationReason::ERROR_REASON_DELETE;
            break;
        case NotificationConstant::PACKAGE_CHANGED_REASON_DELETE:
            outType = NotificationReason::PACKAGE_CHANGED_REASON_DELETE;
            break;
        case NotificationConstant::USER_STOPPED_REASON_DELETE:
            outType = NotificationReason::USER_STOPPED_REASON_DELETE;
            break;
        case NotificationConstant::APP_CANCEL_REASON_DELETE:
            outType = NotificationReason::APP_CANCEL_REASON_DELETE;
            break;
        case NotificationConstant::APP_CANCEL_ALL_REASON_DELETE:
            outType = NotificationReason::APP_CANCEL_ALL_REASON_DELETE;
            break;
        case NotificationConstant::APP_CANCEL_REASON_OTHER:
            outType = NotificationReason::APP_CANCEL_REASON_OTHER;
            break;
        default:
            ANS_LOGI("reason is error");
            return false;
    }
    return true;
}

bool Common::DisturbModeJSToC(const enum DisturbMode &inType, enum NotificationConstant::DisturbMode &outType)
{
    switch (inType) {
        case DisturbMode::ALLOW_UNKNOWN:
            outType = NotificationConstant::DisturbMode::ALLOW_UNKNOWN;
            break;
        case DisturbMode::ALLOW_ALL:
            outType = NotificationConstant::DisturbMode::ALLOW_ALL;
            break;
        case DisturbMode::ALLOW_PRIORITY:
            outType = NotificationConstant::DisturbMode::ALLOW_PRIORITY;
            break;
        case DisturbMode::ALLOW_NONE:
            outType = NotificationConstant::DisturbMode::ALLOW_NONE;
            break;
        case DisturbMode::ALLOW_ALARMS:
            outType = NotificationConstant::DisturbMode::ALLOW_ALARMS;
            break;
        default:
            ANS_LOGI("disturbMode is error");
            return false;
    }
    return true;
}

napi_value Common::CreateWantAgentByJS(const napi_env &env, const std::shared_ptr<WantAgent::WantAgent> &agent)
{
    if (agent == nullptr) {
        ANS_LOGI("agent is nullptr");
        return nullptr;
    }
    napi_value wantAgent = nullptr;
    napi_value wantAgentClass = nullptr;
    napi_define_class(env,
        "wantAgentClass",
        NAPI_AUTO_LENGTH,
        [](napi_env env, napi_callback_info info) -> napi_value {
            napi_value thisVar = nullptr;
            napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
            return thisVar;
        },
        nullptr,
        0,
        nullptr,
        &wantAgentClass);
    napi_new_instance(env, wantAgentClass, 0, nullptr, &wantAgent);
    napi_wrap(env, wantAgent, (void *)agent.get(), [](napi_env env, void *data, void *hint) {}, nullptr, nullptr);

    return wantAgent;
}
}  // namespace NotificationNapi
}  // namespace OHOS
