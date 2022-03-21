/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "napi_common.h"
#include "pixel_map_napi.h"

namespace OHOS {
namespace NotificationNapi {
std::set<std::shared_ptr<AbilityRuntime::WantAgent::WantAgent>> Common::wantAgent_;

Common::Common()
{}

Common::~Common()
{}

napi_value Common::NapiGetBoolean(napi_env env, const bool &isValue)
{
    napi_value result = nullptr;
    napi_get_boolean(env, isValue, &result);
    return result;
}

napi_value Common::NapiGetNull(napi_env env)
{
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

napi_value Common::NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
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

void Common::PaddingCallbackPromiseInfo(
    const napi_env &env, const napi_ref &callback, CallbackPromiseInfo &info, napi_value &promise)
{
    ANS_LOGI("enter");

    if (callback) {
        info.callback = callback;
        info.isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        info.deferred = deferred;
        info.isCallback = false;
    }
}

void Common::ReturnCallbackPromise(const napi_env &env, const CallbackPromiseInfo &info, const napi_value &result)
{
    ANS_LOGI("enter errorCode=%{public}d", info.errorCode);
    if (info.isCallback) {
        SetCallback(env, info.callback, info.errorCode, result);
    } else {
        SetPromise(env, info.deferred, info.errorCode, result);
    }
    ANS_LOGI("end");
}

void Common::SetCallback(
    const napi_env &env, const napi_ref &callbackIn, const int &errorCode, const napi_value &result)
{
    ANS_LOGI("enter");
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);

    napi_value callback = nullptr;
    napi_value resultout = nullptr;
    napi_get_reference_value(env, callbackIn, &callback);
    napi_value results[ARGS_TWO] = {nullptr};
    results[PARAM0] = GetCallbackErrorValue(env, errorCode);
    results[PARAM1] = result;
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &results[PARAM0], &resultout));
    ANS_LOGI("end");
}

void Common::SetCallback(
    const napi_env &env, const napi_ref &callbackIn, const napi_value &result)
{
    ANS_LOGI("enter");
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);

    napi_value callback = nullptr;
    napi_value resultout = nullptr;
    napi_get_reference_value(env, callbackIn, &callback);
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_ONE, &result, &resultout));
    ANS_LOGI("end");
}

void Common::SetPromise(
    const napi_env &env, const napi_deferred &deferred, const int &errorCode, const napi_value &result)
{
    ANS_LOGI("enter");
    if (errorCode == ERR_OK) {
        napi_resolve_deferred(env, deferred, result);
    } else {
        napi_reject_deferred(env, deferred, GetCallbackErrorValue(env, errorCode));
    }
    ANS_LOGI("end");
}

napi_value Common::JSParaError(const napi_env &env, const napi_ref &callback)
{
    if (callback) {
        return Common::NapiGetNull(env);
    }
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    napi_create_promise(env, &deferred, &promise);
    SetPromise(env, deferred, ERROR, Common::NapiGetNull(env));
    return promise;
}

napi_value Common::ParseParaOnlyCallback(const napi_env &env, const napi_callback_info &info, napi_ref &callback)
{
    ANS_LOGI("enter");

    size_t argc = ONLY_CALLBACK_MAX_PARA;
    napi_value argv[ONLY_CALLBACK_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ONLY_CALLBACK_MIN_PARA, "Wrong number of arguments");

    // argv[0]:callback
    napi_valuetype valuetype = napi_undefined;
    if (argc >= ONLY_CALLBACK_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM0], 1, &callback);
    }

    return Common::NapiGetNull(env);
}

napi_value Common::SetNotificationByDistributedOptions(
    const napi_env &env, const OHOS::Notification::Notification *notification, napi_value &result)
{
    ANS_LOGI("enter");
    if (notification == nullptr) {
        ANS_LOGE("notification is nullptr");
        return NapiGetBoolean(env, false);
    }

    NotificationDistributedOptions options = notification->GetNotificationRequest().GetNotificationDistributedOptions();
    napi_value value = nullptr;
    // isDistributed?: boolean
    if (notification->GetDeviceId().empty()) {
        napi_get_boolean(env, false, &value);
    } else {
        napi_get_boolean(env, options.IsDistributed(), &value);
    }
    napi_set_named_property(env, result, "isDistributed", value);

    // supportDisplayDevices?: Array<string>
    int count = 0;
    napi_value arrSupportDisplayDevices = nullptr;
    napi_create_array(env, &arrSupportDisplayDevices);
    std::vector<std::string> displayDevices = options.GetDevicesSupportDisplay();
    for (auto vec : displayDevices) {
        napi_value vecValue = nullptr;
        ANS_LOGI("supportDisplayDevices = %{public}s", vec.c_str());
        napi_create_string_utf8(env, vec.c_str(), NAPI_AUTO_LENGTH, &vecValue);
        napi_set_element(env, arrSupportDisplayDevices, count, vecValue);
        count ++;
    }
    napi_set_named_property(env, result, "supportDisplayDevices", arrSupportDisplayDevices);

    // supportOperateDevices?: Array<string>
    count = 0;
    napi_value arrSupportOperateDevices = nullptr;
    napi_create_array(env, &arrSupportOperateDevices);
    std::vector<std::string> operateDevices = options.GetDevicesSupportOperate();
    for (auto vec : operateDevices) {
        napi_value vecValue = nullptr;
        ANS_LOGI("supportOperateDevices  = %{public}s", vec.c_str());
        napi_create_string_utf8(env, vec.c_str(), NAPI_AUTO_LENGTH, &vecValue);
        napi_set_element(env, arrSupportOperateDevices, count, vecValue);
        count ++;
    }
    napi_set_named_property(env, result, "supportOperateDevices", arrSupportOperateDevices);

    // readonly remindType?: number
    enum DeviceRemindType outType = DeviceRemindType::IDLE_DONOT_REMIND;
    if (!DeviceRemindTypeCToJS(notification->GetRemindType(), outType)) {
        return NapiGetBoolean(env, false);
    }
    napi_create_int32(env, (int32_t)outType, &value);
    napi_set_named_property(env, result, "remindType", value);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotification(
    const napi_env &env, const OHOS::Notification::Notification *notification, napi_value &result)
{
    ANS_LOGI("enter");

    if (notification == nullptr) {
        ANS_LOGE("notification is nullptr");
        return NapiGetBoolean(env, false);
    }
    napi_value value = nullptr;
    NotificationRequest request = notification->GetNotificationRequest();
    if (!SetNotificationRequest(env, &request, result)) {
        return NapiGetBoolean(env, false);
    }

    // hashCode?: string
    napi_create_string_utf8(env, notification->GetKey().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "hashCode", value);

    // isFloatingIcon ?: boolean
    napi_get_boolean(env, notification->IsFloatingIcon(), &value);
    napi_set_named_property(env, result, "isFloatingIcon", value);

    // readonly creatorBundleName?: string
    napi_create_string_utf8(env, notification->GetCreateBundle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "creatorBundleName", value);

    // readonly creatorUid?: number
    napi_create_int32(env, notification->GetUid(), &value);
    napi_set_named_property(env, result, "creatorUid", value);

    // readonly creatorPid?: number
    napi_create_int32(env, notification->GetPid(), &value);
    napi_set_named_property(env, result, "creatorPid", value);

    // readonly creatorUserId?: number
    napi_create_int32(env, notification->GetUserId(), &value);
    napi_set_named_property(env, result, "creatorUserId", value);

    // distributedOption?:DistributedOptions
    napi_value distributedResult = nullptr;
    napi_create_object(env, &distributedResult);
    if (!SetNotificationByDistributedOptions(env, notification, distributedResult)) {
        return NapiGetBoolean(env, false);
    }
    napi_set_named_property(env, result, "distributedOption", distributedResult);

    // readonly isRemoveAllowed?: boolean
    napi_get_boolean(env, notification->IsRemoveAllowed(), &value);
    napi_set_named_property(env, result, "isRemoveAllowed", value);

    // readonly source?: number
    SourceType sourceType = SourceType::TYPE_NORMAL;
    if (!SourceTypeCToJS(notification->GetSourceType(), sourceType)) {
        return NapiGetBoolean(env, false);
    }
    napi_create_int32(env, (int32_t)sourceType, &value);
    napi_set_named_property(env, result, "source", value);

    // readonly deviceId?: string
    napi_create_string_utf8(env, notification->GetDeviceId().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "deviceId", value);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationRequestByString(
    const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result)
{
    ANS_LOGI("enter");

    napi_value value = nullptr;

    if (request == nullptr) {
        ANS_LOGE("request is nullptr");
        return NapiGetBoolean(env, false);
    }

    // classification?: string
    napi_create_string_utf8(env, request->GetClassification().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "classification", value);

    // statusBarText?: string
    napi_create_string_utf8(env, request->GetStatusBarText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "statusBarText", value);

    // label?: string
    napi_create_string_utf8(env, request->GetLabel().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "label", value);

    // groupName?: string
    napi_create_string_utf8(env, request->GetGroupName().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "groupName", value);

    // readonly creatorBundleName?: string
    napi_create_string_utf8(env, request->GetCreatorBundleName().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "creatorBundleName", value);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationRequestByNumber(
    const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result)
{
    ANS_LOGI("enter");

    napi_value value = nullptr;

    if (request == nullptr) {
        ANS_LOGE("request is nullptr");
        return NapiGetBoolean(env, false);
    }

    // id?: number
    napi_create_int32(env, request->GetNotificationId(), &value);
    napi_set_named_property(env, result, "id", value);

    // slotType?: SlotType
    SlotType outType = SlotType::UNKNOWN_TYPE;
    if (!SlotTypeCToJS(request->GetSlotType(), outType)) {
        return NapiGetBoolean(env, false);
    }
    napi_create_int32(env, (int32_t)outType, &value);
    napi_set_named_property(env, result, "slotType", value);

    // deliveryTime?: number
    napi_create_int64(env, request->GetDeliveryTime(), &value);
    napi_set_named_property(env, result, "deliveryTime", value);

    // autoDeletedTime?: number
    napi_create_int64(env, request->GetAutoDeletedTime(), &value);
    napi_set_named_property(env, result, "autoDeletedTime", value);

    // color ?: number
    napi_create_uint32(env, request->GetColor(), &value);
    napi_set_named_property(env, result, "color", value);

    // badgeIconStyle ?: number
    int badgeIconStyle = static_cast<int>(request->GetBadgeIconStyle());
    napi_create_int32(env, badgeIconStyle, &value);
    napi_set_named_property(env, result, "badgeIconStyle", value);

    // readonly creatorUid?: number
    napi_create_int32(env, request->GetCreatorUid(), &value);
    napi_set_named_property(env, result, "creatorUid", value);

    // readonly creatorPid?: number
    napi_create_int32(env, request->GetCreatorPid(), &value);
    napi_set_named_property(env, result, "creatorPid", value);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationRequestByBool(
    const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result)
{
    ANS_LOGI("enter");

    napi_value value = nullptr;

    if (request == nullptr) {
        ANS_LOGE("request is nullptr");
        return NapiGetBoolean(env, false);
    }
    // isOngoing?: boolean
    napi_get_boolean(env, request->IsInProgress(), &value);
    napi_set_named_property(env, result, "isOngoing", value);

    // isUnremovable?: boolean
    napi_get_boolean(env, request->IsUnremovable(), &value);
    napi_set_named_property(env, result, "isUnremovable", value);

    // tapDismissed?: boolean
    napi_get_boolean(env, request->IsTapDismissed(), &value);
    napi_set_named_property(env, result, "tapDismissed", value);

    // colorEnabled?: boolean
    napi_get_boolean(env, request->IsColorEnabled(), &value);
    napi_set_named_property(env, result, "colorEnabled", value);

    // isAlertOnce?: boolean
    napi_get_boolean(env, request->IsAlertOneTime(), &value);
    napi_set_named_property(env, result, "isAlertOnce", value);

    // isStopwatch?: boolean
    napi_get_boolean(env, request->IsShowStopwatch(), &value);
    napi_set_named_property(env, result, "isStopwatch", value);

    // isCountDown?: boolean
    napi_get_boolean(env, request->IsCountdownTimer(), &value);
    napi_set_named_property(env, result, "isCountDown", value);

    // isFloatingIcon?: boolean
    napi_get_boolean(env, request->IsFloatingIcon(), &value);
    napi_set_named_property(env, result, "isFloatingIcon", value);

    // showDeliveryTime?: boolean
    napi_get_boolean(env, request->IsShowDeliveryTime(), &value);
    napi_set_named_property(env, result, "showDeliveryTime", value);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationRequestByWantAgent(
    const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result)
{
    ANS_LOGI("enter");
    if (request == nullptr) {
        ANS_LOGE("request is nullptr");
        return NapiGetBoolean(env, false);
    }
    // wantAgent?: WantAgent
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> agent = request->GetWantAgent();
    if (agent) {
        napi_value wantAgent = nullptr;
        wantAgent = CreateWantAgentByJS(env, agent);
        napi_set_named_property(env, result, "wantAgent", wantAgent);
    } else {
        napi_set_named_property(env, result, "wantAgent", NapiGetNull(env));
    }

    // removalWantAgent?: WantAgent
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> removalAgent = request->GetRemovalWantAgent();
    if (removalAgent) {
        napi_value wantAgent = nullptr;
        wantAgent = CreateWantAgentByJS(env, removalAgent);
        napi_set_named_property(env, result, "removalWantAgent", wantAgent);
    } else {
        napi_set_named_property(env, result, "removalWantAgent", NapiGetNull(env));
    }

    // maxScreenWantAgent?: WantAgent
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> maxScreenAgent = request->GetMaxScreenWantAgent();
    if (maxScreenAgent) {
        napi_value wantAgent = nullptr;
        wantAgent = CreateWantAgentByJS(env, maxScreenAgent);
        napi_set_named_property(env, result, "maxScreenWantAgent", wantAgent);
    } else {
        napi_set_named_property(env, result, "maxScreenWantAgent", NapiGetNull(env));
    }

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationRequestByPixelMap(
    const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result)
{
    ANS_LOGI("enter");

    if (request == nullptr) {
        ANS_LOGE("request is nullptr");
        return NapiGetBoolean(env, false);
    }

    // smallIcon?: image.PixelMap
    std::shared_ptr<Media::PixelMap> littleIcon = request->GetLittleIcon();
    if (littleIcon) {
        napi_value smallIconResult = nullptr;
        napi_valuetype valuetype = napi_undefined;
        smallIconResult = Media::PixelMapNapi::CreatePixelMap(env, littleIcon);
        NAPI_CALL(env, napi_typeof(env, smallIconResult, &valuetype));
        if (valuetype == napi_undefined) {
            ANS_LOGI("smallIconResult is undefined");
            napi_set_named_property(env, result, "smallIcon", NapiGetNull(env));
        } else {
            napi_set_named_property(env, result, "smallIcon", smallIconResult);
        }
    }

    // largeIcon?: image.PixelMap
    std::shared_ptr<Media::PixelMap> largeIcon = request->GetBigIcon();
    if (largeIcon) {
        napi_value largeIconResult = nullptr;
        napi_valuetype valuetype = napi_undefined;
        largeIconResult = Media::PixelMapNapi::CreatePixelMap(env, largeIcon);
        NAPI_CALL(env, napi_typeof(env, largeIconResult, &valuetype));
        if (valuetype == napi_undefined) {
            ANS_LOGI("largeIconResult is undefined");
            napi_set_named_property(env, result, "largeIcon", NapiGetNull(env));
        } else {
            napi_set_named_property(env, result, "largeIcon", largeIconResult);
        }
    }

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationRequestByCustom(
    const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result)
{
    ANS_LOGI("enter");

    if (request == nullptr) {
        ANS_LOGE("request is nullptr");
        return NapiGetBoolean(env, false);
    }

    // content: NotificationContent
    std::shared_ptr<NotificationContent> content = request->GetContent();
    if (content) {
        napi_value contentResult = nullptr;
        napi_create_object(env, &contentResult);
        if (!SetNotificationContent(env, content, contentResult)) {
            ANS_LOGE("SetNotificationContent call failed");
            return NapiGetBoolean(env, false);
        }
        napi_set_named_property(env, result, "content", contentResult);
    } else {
        ANS_LOGE("content is nullptr");
        return NapiGetBoolean(env, false);
    }

    // extraInfo?: {[key:string] : any}
    std::shared_ptr<AAFwk::WantParams> additionalData = request->GetAdditionalData();
    if (additionalData) {
        napi_value extraInfo = nullptr;
        extraInfo = OHOS::AppExecFwk::WrapWantParams(env, *additionalData);
        napi_set_named_property(env, result, "extraInfo", extraInfo);
    }

    // actionButtons?: Array<NotificationActionButton>
    napi_value arr = nullptr;
    int count = 0;
    napi_create_array(env, &arr);
    for (auto vec : request->GetActionButtons()) {
        if (vec) {
            napi_value actionButtonResult = nullptr;
            napi_create_object(env, &actionButtonResult);
            if (SetNotificationActionButton(env, vec, actionButtonResult)) {
                napi_set_element(env, arr, count, actionButtonResult);
                count++;
            }
        }
    }
    napi_set_named_property(env, result, "actionButtons", arr);

    // template?: NotificationTemplate
    std::shared_ptr<NotificationTemplate> templ = request->GetTemplate();
    if (templ) {
        napi_value templateResult = nullptr;
        napi_create_object(env, &templateResult);
        if (!SetNotificationTemplateInfo(env, templ, templateResult)) {
            ANS_LOGE("SetNotificationTemplate call failed");
            return NapiGetBoolean(env, false);
        }
        napi_set_named_property(env, result, "template", templateResult);
    }

    // readonly notificationFlags?: NotificationFlags
    std::shared_ptr<NotificationFlags> flags = request->GetFlags();
    if (flags) {
        napi_value flagsResult = nullptr;
        napi_create_object(env, &flagsResult);
        if (!SetNotificationFlags(env, flags, flagsResult)) {
            ANS_LOGE("SetNotificationFlags call failed");
            return NapiGetBoolean(env, false);
        }
        napi_set_named_property(env, result, "notificationFlags", flagsResult);
    }

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationRequest(
    const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result)
{
    ANS_LOGI("enter");

    if (request == nullptr) {
        ANS_LOGE("request is nullptr");
        return NapiGetBoolean(env, false);
    }

    if (!SetNotificationRequestByString(env, request, result)) {
        return NapiGetBoolean(env, false);
    }
    if (!SetNotificationRequestByNumber(env, request, result)) {
        return NapiGetBoolean(env, false);
    }
    if (!SetNotificationRequestByBool(env, request, result)) {
        return NapiGetBoolean(env, false);
    }
    if (!SetNotificationRequestByWantAgent(env, request, result)) {
        return NapiGetBoolean(env, false);
    }
    if (!SetNotificationRequestByPixelMap(env, request, result)) {
        return NapiGetBoolean(env, false);
    }
    if (!SetNotificationRequestByCustom(env, request, result)) {
        return NapiGetBoolean(env, false);
    }

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationSortingMap(
    const napi_env &env, const std::shared_ptr<NotificationSortingMap> &sortingMap, napi_value &result)
{
    ANS_LOGI("enter");
    if (sortingMap == nullptr) {
        ANS_LOGE("sortingMap is null");
        return NapiGetBoolean(env, false);
    }
    if (sortingMap->GetKey().size() == 0) {
        ANS_LOGE("sortingMap GetKey().size is empty");
        return NapiGetBoolean(env, false);
    }

    size_t count = 0;
    napi_value arrSortedHashCode = nullptr;
    napi_create_array(env, &arrSortedHashCode);
    napi_value sortingsResult = nullptr;
    napi_create_object(env, &sortingsResult);
    for (auto key : sortingMap->GetKey()) {
        NotificationSorting sorting;
        if (sortingMap->GetNotificationSorting(key, sorting)) {
            // sortedHashCode: Array<string>
            napi_value keyValue = nullptr;
            ANS_LOGI("sortingMap key = %{public}s", key.c_str());
            napi_create_string_utf8(env, key.c_str(), NAPI_AUTO_LENGTH, &keyValue);
            napi_set_element(env, arrSortedHashCode, count, keyValue);

            // sortings:{[key : string] : NotificationSorting}
            napi_value sortingResult = nullptr;
            napi_create_object(env, &sortingResult);
            if (!SetNotificationSorting(env, sorting, sortingResult)) {
                ANS_LOGE("SetNotificationSorting call failed");
                return NapiGetBoolean(env, false);
            }
            napi_set_named_property(env, sortingsResult, key.c_str(), sortingResult);
            count++;
        } else {
            ANS_LOGW("sortingMap Key: %{public}s match value is empty", key.c_str());
        }
    }
    napi_set_named_property(env, result, "sortedHashCode", arrSortedHashCode);
    napi_set_named_property(env, result, "sortings", sortingsResult);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationSorting(const napi_env &env, const NotificationSorting &sorting, napi_value &result)
{
    ANS_LOGI("enter");

    // slot: NotificationSlot
    napi_value slotResult = nullptr;
    napi_value value = nullptr;
    napi_create_object(env, &slotResult);
    if (!SetNotificationSlot(env, sorting.GetSlot(), slotResult)) {
        ANS_LOGE("SetNotificationSlot call failed");
        return NapiGetBoolean(env, false);
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

    // importance?: number
    napi_create_int32(env, sorting.GetImportance(), &value);
    napi_set_named_property(env, result, "importance", value);

    // groupKeyOverride?: string
    napi_create_string_utf8(env, sorting.GetGroupKeyOverride().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "groupKeyOverride", value);

    // visiblenessOverride?: number
    napi_create_int32(env, sorting.GetVisiblenessOverride(), &value);
    napi_set_named_property(env, result, "visiblenessOverride", value);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationSlot(const napi_env &env, const NotificationSlot &slot, napi_value &result)
{
    ANS_LOGI("enter");

    napi_value value = nullptr;
    // type: SlotType
    SlotType outType = SlotType::UNKNOWN_TYPE;
    if (!SlotTypeCToJS(slot.GetType(), outType)) {
        return NapiGetBoolean(env, false);
    }
    napi_create_int32(env, (int32_t)outType, &value);
    napi_set_named_property(env, result, "type", value);

    // level?: number
    SlotLevel outLevel = SlotLevel::LEVEL_NONE;
    if (!SlotLevelCToJS(slot.GetLevel(), outLevel)) {
        return NapiGetBoolean(env, false);
    }
    napi_create_int32(env, (int32_t)outLevel, &value);
    napi_set_named_property(env, result, "level", value);

    // desc?: string
    napi_create_string_utf8(env, slot.GetDescription().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "desc", value);

    // badgeFlag?: boolean
    napi_get_boolean(env, slot.IsShowBadge(), &value);
    napi_set_named_property(env, result, "badgeFlag", value);

    // bypassDnd?: boolean
    napi_get_boolean(env, slot.IsEnableBypassDnd(), &value);
    napi_set_named_property(env, result, "bypassDnd", value);

    // lockscreenVisibility?: number
    int32_t lockScreenVisibleness = (int32_t)slot.GetLockScreenVisibleness();
    napi_create_int32(env, lockScreenVisibleness, &value);
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

    // vibrationValues?: Array<number>
    napi_value arr = nullptr;
    napi_create_array(env, &arr);
    size_t count = 0;
    for (auto vec : slot.GetVibrationStyle()) {
        napi_create_int64(env, vec, &value);
        napi_set_element(env, arr, count, value);
        count++;
    }
    napi_set_named_property(env, result, "vibrationValues", arr);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationContentDetailed(const napi_env &env, const ContentType &type,
    const std::shared_ptr<NotificationContent> &content, napi_value &result)
{
    ANS_LOGI("enter");
    napi_value ret = NapiGetBoolean(env, false);
    if (!content) {
        ANS_LOGE("content is null");
        return ret;
    }

    std::shared_ptr<NotificationBasicContent> basicContent = content->GetNotificationContent();
    if (basicContent == nullptr) {
        ANS_LOGE("content is null");
        return ret;
    }

    napi_value contentResult = nullptr;
    napi_create_object(env, &contentResult);
    if (type == ContentType::NOTIFICATION_CONTENT_BASIC_TEXT) {
        // normal?: NotificationBasicContent
        ret = SetNotificationBasicContent(env, basicContent.get(), contentResult);
        if (ret) {
            napi_set_named_property(env, result, "normal", contentResult);
        }
    } else if (type == ContentType::NOTIFICATION_CONTENT_LONG_TEXT) {
        // longText?: NotificationLongTextContent
        ret = SetNotificationLongTextContent(env, basicContent.get(), contentResult);
        if (ret) {
            napi_set_named_property(env, result, "longText", contentResult);
        }
    } else if (type == ContentType::NOTIFICATION_CONTENT_PICTURE) {
        // picture?: NotificationPictureContent
        ret = SetNotificationPictureContent(env, basicContent.get(), contentResult);
        if (ret) {
            napi_set_named_property(env, result, "picture", contentResult);
        }
    } else if (type == ContentType::NOTIFICATION_CONTENT_CONVERSATION) {
        // conversation?: NotificationConversationalContent
        ret = SetNotificationConversationalContent(env, basicContent.get(), contentResult);
        if (ret) {
            napi_set_named_property(env, result, "conversation", contentResult);
        }
    } else if (type == ContentType::NOTIFICATION_CONTENT_MULTILINE) {
        // multiLine?: NotificationMultiLineContent
        ret = SetNotificationMultiLineContent(env, basicContent.get(), contentResult);
        if (ret) {
            napi_set_named_property(env, result, "multiLine", contentResult);
        }
    } else {
        ANS_LOGE("ContentType is does not exist");
    }
    return ret;
}

napi_value Common::SetNotificationContent(
    const napi_env &env, const std::shared_ptr<NotificationContent> &content, napi_value &result)
{
    ANS_LOGI("enter");
    napi_value value = nullptr;
    if (content == nullptr) {
        ANS_LOGE("content is null");
        return NapiGetBoolean(env, false);
    }

    // contentType: ContentType
    NotificationContent::Type type = content->GetContentType();
    ContentType outType = ContentType::NOTIFICATION_CONTENT_BASIC_TEXT;
    if (!ContentTypeCToJS(type, outType)) {
        return NapiGetBoolean(env, false);
    }
    napi_create_int32(env, (int32_t)outType, &value);
    napi_set_named_property(env, result, "contentType", value);

    if (!SetNotificationContentDetailed(env, outType, content, result)) {
        return NapiGetBoolean(env, false);
    }

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationBasicContent(
    const napi_env &env, const NotificationBasicContent *basicContent, napi_value &result)
{
    ANS_LOGI("enter");
    napi_value value = nullptr;
    if (basicContent == nullptr) {
        ANS_LOGE("basicContent is null");
        return NapiGetBoolean(env, false);
    }

    // title: string
    napi_create_string_utf8(env, basicContent->GetTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "title", value);

    // text: string
    napi_create_string_utf8(env, basicContent->GetText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "text", value);

    // additionalText?: string
    napi_create_string_utf8(env, basicContent->GetAdditionalText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "additionalText", value);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationLongTextContent(
    const napi_env &env, NotificationBasicContent *basicContent, napi_value &result)
{
    ANS_LOGI("enter");
    napi_value value = nullptr;
    if (basicContent == nullptr) {
        ANS_LOGE("basicContent is null");
        return NapiGetBoolean(env, false);
    }

    OHOS::Notification::NotificationLongTextContent *longTextContent =
        static_cast<OHOS::Notification::NotificationLongTextContent *>(basicContent);
    if (longTextContent == nullptr) {
        ANS_LOGE("longTextContent is null");
        return NapiGetBoolean(env, false);
    }

    if (!SetNotificationBasicContent(env, longTextContent, result)) {
        ANS_LOGE("SetNotificationBasicContent call failed");
        return NapiGetBoolean(env, false);
    }

    // longText: string
    napi_create_string_utf8(env, longTextContent->GetLongText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "longText", value);

    // briefText: string
    napi_create_string_utf8(env, longTextContent->GetBriefText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "briefText", value);

    // expandedTitle: string
    napi_create_string_utf8(env, longTextContent->GetExpandedTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "expandedTitle", value);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationPictureContent(
    const napi_env &env, NotificationBasicContent *basicContent, napi_value &result)
{
    ANS_LOGI("enter");
    napi_value value = nullptr;
    if (basicContent == nullptr) {
        ANS_LOGE("basicContent is null");
        return NapiGetBoolean(env, false);
    }
    OHOS::Notification::NotificationPictureContent *pictureContent =
        static_cast<OHOS::Notification::NotificationPictureContent *>(basicContent);
    if (pictureContent == nullptr) {
        ANS_LOGE("pictureContent is null");
        return NapiGetBoolean(env, false);
    }

    if (!SetNotificationBasicContent(env, pictureContent, result)) {
        ANS_LOGE("SetNotificationBasicContent call failed");
        return NapiGetBoolean(env, false);
    }

    // briefText: string
    napi_create_string_utf8(env, pictureContent->GetBriefText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "briefText", value);

    // expandedTitle: string
    napi_create_string_utf8(env, pictureContent->GetExpandedTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "expandedTitle", value);

    // picture: image.PixelMap
    std::shared_ptr<Media::PixelMap> picture = pictureContent->GetBigPicture();
    if (picture) {
        napi_value pictureResult = nullptr;
        napi_valuetype valuetype = napi_undefined;
        pictureResult = Media::PixelMapNapi::CreatePixelMap(env, picture);
        NAPI_CALL(env, napi_typeof(env, pictureResult, &valuetype));
        if (valuetype == napi_undefined) {
            ANS_LOGW("pictureResult is undefined");
            napi_set_named_property(env, result, "picture", NapiGetNull(env));
        } else {
            napi_set_named_property(env, result, "picture", pictureResult);
        }
    }
    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationConversationalContent(const napi_env &env,
    NotificationBasicContent *basicContent, napi_value &result)
{
    ANS_LOGI("enter");
    napi_value value = nullptr;
    if (basicContent == nullptr) {
        ANS_LOGE("basicContent is null");
        return NapiGetBoolean(env, false);
    }
    OHOS::Notification::NotificationConversationalContent *conversationalContent =
        static_cast<OHOS::Notification::NotificationConversationalContent *>(basicContent);
    if (conversationalContent == nullptr) {
        ANS_LOGE("conversationalContent is null");
        return NapiGetBoolean(env, false);
    }

    if (!SetNotificationBasicContent(env, conversationalContent, result)) {
        ANS_LOGE("SetNotificationBasicContent call failed");
        return NapiGetBoolean(env, false);
    }

    // conversationTitle: string
    napi_create_string_utf8(env, conversationalContent->GetConversationTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "conversationTitle", value);

    // conversationGroup: boolean
    napi_get_boolean(env, conversationalContent->IsConversationGroup(), &value);
    napi_set_named_property(env, result, "conversationGroup", value);

    // messages: Array<ConversationalMessage>
    napi_value arr = nullptr;
    if (!SetConversationalMessages(env, conversationalContent, arr)) {
        ANS_LOGE("SetConversationalMessages call failed");
        return NapiGetBoolean(env, false);
    }
    napi_set_named_property(env, result, "messages", arr);

    // user: MessageUser
    napi_value messageUserResult = nullptr;
    napi_create_object(env, &messageUserResult);
    if (!SetMessageUser(env, conversationalContent->GetMessageUser(), messageUserResult)) {
        ANS_LOGE("SetMessageUser call failed");
        return NapiGetBoolean(env, false);
    }
    napi_set_named_property(env, result, "user", messageUserResult);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationMultiLineContent(
    const napi_env &env, NotificationBasicContent *basicContent, napi_value &result)
{
    ANS_LOGI("enter");
    napi_value value = nullptr;
    if (basicContent == nullptr) {
        ANS_LOGE("basicContent is null");
        return NapiGetBoolean(env, false);
    }
    OHOS::Notification::NotificationMultiLineContent *multiLineContent =
        static_cast<OHOS::Notification::NotificationMultiLineContent *>(basicContent);
    if (multiLineContent == nullptr) {
        ANS_LOGE("multiLineContent is null");
        return NapiGetBoolean(env, false);
    }

    if (!SetNotificationBasicContent(env, multiLineContent, result)) {
        ANS_LOGE("SetNotificationBasicContent call failed");
        return NapiGetBoolean(env, false);
    }

    // briefText: string
    napi_create_string_utf8(env, multiLineContent->GetBriefText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "briefText", value);

    // longTitle: string
    napi_create_string_utf8(env, multiLineContent->GetExpandedTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "longTitle", value);

    // lines: Array<String>
    napi_value arr = nullptr;
    int count = 0;
    napi_create_array(env, &arr);
    for (auto vec : multiLineContent->GetAllLines()) {
        napi_create_string_utf8(env, vec.c_str(), NAPI_AUTO_LENGTH, &value);
        napi_set_element(env, arr, count, value);
        count++;
    }
    napi_set_named_property(env, result, "lines", arr);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetMessageUser(const napi_env &env, const MessageUser &messageUser, napi_value &result)
{
    ANS_LOGI("enter");

    napi_value value = nullptr;
    // name: string
    napi_create_string_utf8(env, messageUser.GetName().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "name", value);

    // key: string
    napi_create_string_utf8(env, messageUser.GetKey().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "key", value);

    // uri: string
    napi_create_string_utf8(env, messageUser.GetUri().ToString().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "uri", value);

    // isMachine: boolean
    napi_get_boolean(env, messageUser.IsMachine(), &value);
    napi_set_named_property(env, result, "isMachine", value);

    // isUserImportant: boolean
    napi_get_boolean(env, messageUser.IsUserImportant(), &value);
    napi_set_named_property(env, result, "isUserImportant", value);

    // icon?: image.PixelMap
    std::shared_ptr<Media::PixelMap> icon = messageUser.GetPixelMap();
    if (icon) {
        napi_value iconResult = nullptr;
        napi_valuetype valuetype = napi_undefined;
        iconResult = Media::PixelMapNapi::CreatePixelMap(env, icon);
        NAPI_CALL(env, napi_typeof(env, iconResult, &valuetype));
        if (valuetype == napi_undefined) {
            ANS_LOGW("iconResult is undefined");
            napi_set_named_property(env, result, "icon", NapiGetNull(env));
        } else {
            napi_set_named_property(env, result, "icon", iconResult);
        }
    }
    return NapiGetBoolean(env, true);
}

napi_value Common::SetConversationalMessages(const napi_env &env,
    const OHOS::Notification::NotificationConversationalContent *conversationalContent, napi_value &arr)
{
    ANS_LOGI("enter");
    if (!conversationalContent) {
        ANS_LOGE("conversationalContent is null");
        return NapiGetBoolean(env, false);
    }

    int count = 0;
    napi_create_array(env, &arr);
    std::vector<std::shared_ptr<NotificationConversationalMessage>> messages =
        conversationalContent->GetAllConversationalMessages();
    for (auto vec : messages) {
        if (!vec) {
            continue;
        }
        napi_value conversationalMessageResult = nullptr;
        napi_create_object(env, &conversationalMessageResult);
        if (!SetConversationalMessage(env, vec, conversationalMessageResult)) {
            ANS_LOGE("SetConversationalMessage call failed");
            return NapiGetBoolean(env, false);
        }
        napi_set_element(env, arr, count, conversationalMessageResult);
        count++;
    }
    return NapiGetBoolean(env, true);
}

napi_value Common::SetConversationalMessage(const napi_env &env,
    const std::shared_ptr<NotificationConversationalMessage> &conversationalMessage, napi_value &result)
{
    ANS_LOGI("enter");
    napi_value value = nullptr;
    if (conversationalMessage == nullptr) {
        ANS_LOGE("conversationalMessage is null");
        return NapiGetBoolean(env, false);
    }

    // text: string
    napi_create_string_utf8(env, conversationalMessage->GetText().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "text", value);

    // timestamp: number
    napi_create_int64(env, conversationalMessage->GetArrivedTime(), &value);
    napi_set_named_property(env, result, "timestamp", value);

    // sender: MessageUser
    napi_value messageUserResult = nullptr;
    napi_create_object(env, &messageUserResult);
    if (!SetMessageUser(env, conversationalMessage->GetSender(), messageUserResult)) {
        ANS_LOGE("SetMessageUser call failed");
        return NapiGetBoolean(env, false);
    }
    napi_set_named_property(env, result, "sender", messageUserResult);

    // mimeType: string
    napi_create_string_utf8(env, conversationalMessage->GetMimeType().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "mimeType", value);

    // uri: string
    napi_create_string_utf8(env, conversationalMessage->GetUri()->ToString().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "uri", value);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationActionButton(
    const napi_env &env, const std::shared_ptr<NotificationActionButton> &actionButton, napi_value &result)
{
    ANS_LOGI("enter");
    if (actionButton == nullptr) {
        ANS_LOGE("actionButton is null");
        return NapiGetBoolean(env, false);
    }

    napi_value value = nullptr;

    // title: string
    napi_create_string_utf8(env, actionButton->GetTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "title", value);

    // wantAgent: WantAgent
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> agent = actionButton->GetWantAgent();
    if (agent == nullptr) {
        ANS_LOGI("wantAgent is null");
        napi_set_named_property(env, result, "wantAgent", NapiGetNull(env));
        return NapiGetBoolean(env, false);
    }
    napi_value wantAgent = nullptr;
    wantAgent = CreateWantAgentByJS(env, agent);
    napi_set_named_property(env, result, "wantAgent", wantAgent);

    // icon?: image.PixelMap
    std::shared_ptr<Media::PixelMap> icon = actionButton->GetIcon();
    if (icon) {
        napi_value iconResult = nullptr;
        napi_valuetype valuetype = napi_undefined;
        iconResult = Media::PixelMapNapi::CreatePixelMap(env, icon);
        NAPI_CALL(env, napi_typeof(env, iconResult, &valuetype));
        if (valuetype == napi_undefined) {
            ANS_LOGW("iconResult is undefined");
            napi_set_named_property(env, result, "icon", NapiGetNull(env));
        } else {
            napi_set_named_property(env, result, "icon", iconResult);
        }
    }

    if (!SetNotificationActionButtonByExtras(env, actionButton, result)) {
        return NapiGetBoolean(env, false);
    }

    // userInput?: NotificationUserInput
    napi_value userInputResult = nullptr;
    napi_create_object(env, &userInputResult);
    if (!SetNotificationActionButtonByUserInput(env, actionButton->GetUserInput(), userInputResult)) {
        return NapiGetBoolean(env, false);
    }
    napi_set_named_property(env, result, "userInput", userInputResult);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationActionButtonByExtras(
    const napi_env &env, const std::shared_ptr<NotificationActionButton> &actionButton, napi_value &result)
{
    ANS_LOGI("enter");
    if (!actionButton) {
        ANS_LOGE("actionButton is null");
        return NapiGetBoolean(env, false);
    }
    // extras?: {[key: string]: any}
    auto extras = actionButton->GetAdditionalData();
    if (extras) {
        napi_value nExtras = nullptr;
        nExtras = OHOS::AppExecFwk::WrapWantParams(env, *extras);
        napi_set_named_property(env, result, "extras", nExtras);
    }
    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationActionButtonByUserInput(
    const napi_env &env, const std::shared_ptr<NotificationUserInput> &userInput, napi_value &result)
{
    ANS_LOGI("enter");

    if (!userInput) {
        ANS_LOGE("userInput is null");
        return NapiGetBoolean(env, false);
    }

    napi_value value = nullptr;
    napi_value arr = nullptr;
    int count = 0;

    // inputKey: string
    napi_create_string_utf8(env, userInput->GetInputKey().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "inputKey", value);

    // tag: string
    napi_create_string_utf8(env, userInput->GetTag().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "tag", value);

    // options: Array<string>
    napi_create_array(env, &arr);
    for (auto vec : userInput->GetOptions()) {
        napi_create_string_utf8(env, vec.c_str(), NAPI_AUTO_LENGTH, &value);
        napi_set_element(env, arr, count, value);
        count++;
    }
    if (count > 0) {
        napi_set_named_property(env, result, "options", arr);
    }

    // permitFreeFormInput?: boolean
    napi_get_boolean(env, userInput->IsPermitFreeFormInput(), &value);
    napi_set_named_property(env, result, "permitFreeFormInput", value);

    // permitMimeTypes?: Array<string>
    count = 0;
    napi_create_array(env, &arr);
    for (auto vec : userInput->GetPermitMimeTypes()) {
        napi_create_string_utf8(env, vec.c_str(), NAPI_AUTO_LENGTH, &value);
        napi_set_element(env, arr, count, value);
        count++;
    }
    if (count > 0) {
        napi_set_named_property(env, result, "permitMimeTypes", arr);
    }

    // editType?: number
    napi_create_int64(env, userInput->GetEditType(), &value);
    napi_set_named_property(env, result, "editType", value);

    // additionalData?: {[key: string]: Object}
    auto additionalData = userInput->GetAdditionalData();
    if (additionalData) {
        napi_value nAdditionalData = nullptr;
        nAdditionalData = OHOS::AppExecFwk::WrapWantParams(env, *additionalData);
        napi_set_named_property(env, result, "additionalData", nAdditionalData);
    }

    return NapiGetBoolean(env, true);
}

napi_value Common::SetDoNotDisturbDate(
    const napi_env &env, const NotificationDoNotDisturbDate &date, napi_value &result)
{
    ANS_LOGI("enter");
    DoNotDisturbType outType = DoNotDisturbType::TYPE_NONE;
    if (!DoNotDisturbTypeCToJS(date.GetDoNotDisturbType(), outType)) {
        return NapiGetBoolean(env, false);
    }

    // type:DoNotDisturbType
    napi_value typeNapi = nullptr;
    napi_create_int32(env, (int32_t)outType, &typeNapi);
    napi_set_named_property(env, result, "type", typeNapi);

    // begin:Date
    double begind = double(date.GetBeginDate());
    napi_value beginNapi = nullptr;
    napi_create_date(env, begind, &beginNapi);
    napi_set_named_property(env, result, "begin", beginNapi);

    // end:Date
    double endd = double(date.GetEndDate());
    napi_value endNapi = nullptr;
    napi_create_date(env, endd, &endNapi);
    napi_set_named_property(env, result, "end", endNapi);

    return NapiGetBoolean(env, true);
}

napi_value Common::SetEnabledNotificationCallbackData(const napi_env &env, const EnabledNotificationCallbackData &data,
    napi_value &result)
{
    ANS_LOGI("enter");
    // bundle: string
    napi_value bundleNapi = nullptr;
    napi_create_string_utf8(env, data.GetBundle().c_str(), NAPI_AUTO_LENGTH, &bundleNapi);
    napi_set_named_property(env, result, "bundle", bundleNapi);

    // uid: uid_t
    napi_value uidNapi = nullptr;
    napi_create_int32(env, data.GetUid(), &uidNapi);
    napi_set_named_property(env, result, "uid", uidNapi);

    // enable: bool
    napi_value enableNapi = nullptr;
    napi_get_boolean(env, data.GetEnable(), &enableNapi);
    napi_set_named_property(env, result, "enable", enableNapi);

    return NapiGetBoolean(env, true);
}

napi_value Common::GetNotificationSubscriberInfo(
    const napi_env &env, const napi_value &value, NotificationSubscribeInfo &subscriberInfo)
{
    ANS_LOGI("enter");
    uint32_t length = 0;
    size_t strLen = 0;
    bool hasProperty = false;
    bool isArray = false;
    napi_valuetype valuetype = napi_undefined;

    // bundleNames?: Array<string>
    NAPI_CALL(env, napi_has_named_property(env, value, "bundleNames", &hasProperty));
    if (hasProperty) {
        napi_value nBundleNames = nullptr;
        napi_get_named_property(env, value, "bundleNames", &nBundleNames);
        napi_is_array(env, nBundleNames, &isArray);
        NAPI_ASSERT(env, isArray, "Property bundleNames is expected to be an array.");
        napi_get_array_length(env, nBundleNames, &length);
        NAPI_ASSERT(env, length > 0, "The array is empty.");
        for (uint32_t i = 0; i < length; ++i) {
            napi_value nBundleName = nullptr;
            char str[STR_MAX_SIZE] = {0};
            napi_get_element(env, nBundleNames, i, &nBundleName);
            NAPI_CALL(env, napi_typeof(env, nBundleName, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, nBundleName, str, STR_MAX_SIZE - 1, &strLen));
            subscriberInfo.bundleNames.emplace_back(str);
            subscriberInfo.hasSubscribeInfo = true;
        }
    }

    // userId?: number
    NAPI_CALL(env, napi_has_named_property(env, value, "userId", &hasProperty));
    if (hasProperty) {
        napi_value nUserId = nullptr;
        napi_get_named_property(env, value, "userId", &nUserId);
        NAPI_CALL(env, napi_typeof(env, nUserId, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        NAPI_CALL(env, napi_get_value_int32(env, nUserId, &subscriberInfo.userId));
        subscriberInfo.hasSubscribeInfo = true;
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationRequestByNumber(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");
    // id?: number
    if (GetNotificationId(env, value, request) == nullptr) {
        return nullptr;
    }
    // deliveryTime?: number
    if (GetNotificationDeliveryTime(env, value, request) == nullptr) {
        return nullptr;
    }
    // autoDeletedTime?: number
    if (GetNotificationAutoDeletedTime(env, value, request) == nullptr) {
        return nullptr;
    }
    // color?: number
    if (GetNotificationColor(env, value, request) == nullptr) {
        return nullptr;
    }
    // badgeIconStyle?: number
    if (GetNotificationBadgeIconStyle(env, value, request) == nullptr) {
        return nullptr;
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationRequestByString(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");
    // classification?: string
    if (GetNotificationClassification(env, value, request) == nullptr) {
        return nullptr;
    }
    // statusBarText?: string
    if (GetNotificationStatusBarText(env, value, request) == nullptr) {
        return nullptr;
    }
    // label?: string
    if (GetNotificationLabel(env, value, request) == nullptr) {
        return nullptr;
    }
    // groupName?: string
    if (GetNotificationGroupName(env, value, request) == nullptr) {
        return nullptr;
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationRequestByBool(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");
    // isOngoing?: boolean
    if (GetNotificationIsOngoing(env, value, request) == nullptr) {
        return nullptr;
    }
    // isUnremovable?: boolean
    if (GetNotificationIsUnremovable(env, value, request) == nullptr) {
        return nullptr;
    }
    // tapDismissed?: boolean
    if (GetNotificationtapDismissed(env, value, request) == nullptr) {
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
    // showDeliveryTime?: boolean
    if (GetNotificationShowDeliveryTime(env, value, request) == nullptr) {
        return nullptr;
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationRequestByCustom(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");
    // content: NotificationContent
    if (GetNotificationContent(env, value, request) == nullptr) {
        return nullptr;
    }
    // slotType?: notification.SlotType
    if (GetNotificationSlotType(env, value, request) == nullptr) {
        return nullptr;
    }
    // wantAgent?: WantAgent
    if (GetNotificationWantAgent(env, value, request) == nullptr) {
        return nullptr;
    }
    // extraInfo?: {[key: string]: any}
    if (GetNotificationExtraInfo(env, value, request) == nullptr) {
        return nullptr;
    }
    // removalWantAgent?: WantAgent
    if (GetNotificationRemovalWantAgent(env, value, request) == nullptr) {
        return nullptr;
    }
    // maxScreenWantAgent?: WantAgent
    if (GetNotificationMaxScreenWantAgent(env, value, request) == nullptr) {
        return nullptr;
    }
    // actionButtons?: Array<NotificationActionButton>
    if (GetNotificationActionButtons(env, value, request) == nullptr) {
        return nullptr;
    }
    // smallIcon?: image.PixelMap
    if (GetNotificationSmallIcon(env, value, request) == nullptr) {
        return nullptr;
    }
    // largeIcon?: image.PixelMap
    if (GetNotificationLargeIcon(env, value, request) == nullptr) {
        return nullptr;
    }
    // distributedOption?:DistributedOptions
    if (GetNotificationRequestDistributedOptions(env, value, request) == nullptr) {
        return nullptr;
    }
    // template?: NotificationTemplate
    if (GetNotificationTemplate(env, value, request) == nullptr) {
        return nullptr;
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationRequest(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");
    if (!GetNotificationRequestByNumber(env, value, request)) {
        return nullptr;
    }
    if (!GetNotificationRequestByString(env, value, request)) {
        return nullptr;
    }
    if (!GetNotificationRequestByBool(env, value, request)) {
        return nullptr;
    }
    if (!GetNotificationRequestByCustom(env, value, request)) {
        return nullptr;
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationId(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    int32_t notificationId = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "id", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "id", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, result, &notificationId);
        request.SetNotificationId(notificationId);
        ANS_LOGI("notificationId = %{public}d", notificationId);
    } else {
        ANS_LOGI("default notificationId = 0");
        request.SetNotificationId(0);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationSlotType(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    int32_t slotType = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "slotType", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "slotType", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, result, &slotType);

        NotificationConstant::SlotType outType = NotificationConstant::SlotType::OTHER;
        if (!SlotTypeJSToC(SlotType(slotType), outType)) {
            return nullptr;
        }
        request.SetSlotType(outType);
        ANS_LOGI("slotType = %{public}d", slotType);
    } else {
        ANS_LOGI("default slotType = OTHER");
        request.SetSlotType(NotificationConstant::OTHER);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationContent(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    int32_t type = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "content", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property content expected.");

    napi_get_named_property(env, value, "content", &result);
    NAPI_CALL(env, napi_typeof(env, result, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    if (GetNotificationContentType(env, result, type) == nullptr) {
        return nullptr;
    }
    NotificationContent::Type outType = NotificationContent::Type::NONE;
    if (!ContentTypeJSToC(ContentType(type), outType)) {
        return nullptr;
    }
    switch (outType) {
        case NotificationContent::Type::BASIC_TEXT:
            if (GetNotificationBasicContent(env, result, request) == nullptr) {
                return nullptr;
            }
            break;
        case NotificationContent::Type::LONG_TEXT:
            if (GetNotificationLongTextContent(env, result, request) == nullptr) {
                return nullptr;
            }
            break;
        case NotificationContent::Type::PICTURE:
            if (GetNotificationPictureContent(env, result, request) == nullptr) {
                return nullptr;
            }
            break;
        case NotificationContent::Type::CONVERSATION:
            if (GetNotificationConversationalContent(env, result, request) == nullptr) {
                return nullptr;
            }
            break;
        case NotificationContent::Type::MULTILINE:
            if (GetNotificationMultiLineContent(env, result, request) == nullptr) {
                return nullptr;
            }
            break;
        default:
            return nullptr;
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationIsOngoing(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    bool isOngoing = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "isOngoing", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isOngoing", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isOngoing);
        request.SetInProgress(isOngoing);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationIsUnremovable(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    bool isUnremovable = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "isUnremovable", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isUnremovable", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isUnremovable);
        request.SetUnremovable(isUnremovable);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationDeliveryTime(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    int64_t deliveryTime = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "deliveryTime", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "deliveryTime", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int64(env, result, &deliveryTime);
        request.SetDeliveryTime(deliveryTime);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationtapDismissed(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    bool tapDismissed = true;

    NAPI_CALL(env, napi_has_named_property(env, value, "tapDismissed", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "tapDismissed", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &tapDismissed);
        request.SetTapDismissed(tapDismissed);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationWantAgent(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    bool hasProperty = false;
    AbilityRuntime::WantAgent::WantAgent *wantAgent = nullptr;
    napi_value result = nullptr;
    napi_valuetype valuetype = napi_undefined;

    NAPI_CALL(env, napi_has_named_property(env, value, "wantAgent", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "wantAgent", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        napi_unwrap(env, result, (void **)&wantAgent);
        if (wantAgent == nullptr) {
            ANS_LOGE("Invalid object wantAgent");
            return nullptr;
        }
        std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> sWantAgent =
            std::make_shared<AbilityRuntime::WantAgent::WantAgent>(*wantAgent);
        request.SetWantAgent(sWantAgent);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationExtraInfo(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "extraInfo", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "extraInfo", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        AAFwk::WantParams wantParams;
        if (!OHOS::AppExecFwk::UnwrapWantParams(env, result, wantParams)) {
            return nullptr;
        }

        std::shared_ptr<AAFwk::WantParams> extras = std::make_shared<AAFwk::WantParams>(wantParams);
        request.SetAdditionalData(extras);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationGroupName(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "groupName", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "groupName", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
        request.SetGroupName(str);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationRemovalWantAgent(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    bool hasProperty = false;
    AbilityRuntime::WantAgent::WantAgent *wantAgent = nullptr;
    napi_value result = nullptr;
    napi_valuetype valuetype = napi_undefined;

    NAPI_CALL(env, napi_has_named_property(env, value, "removalWantAgent", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "removalWantAgent", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        napi_unwrap(env, result, (void **)&wantAgent);
        if (wantAgent == nullptr) {
            ANS_LOGE("Invalid object removalWantAgent");
            return nullptr;
        }
        std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> removeWantAgent =
            std::make_shared<AbilityRuntime::WantAgent::WantAgent>(*wantAgent);
        request.SetRemovalWantAgent(removeWantAgent);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationMaxScreenWantAgent(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    bool hasProperty = false;
    AbilityRuntime::WantAgent::WantAgent *wantAgent = nullptr;
    napi_value result = nullptr;
    napi_valuetype valuetype = napi_undefined;

    NAPI_CALL(env, napi_has_named_property(env, value, "maxScreenWantAgent", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "maxScreenWantAgent", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        napi_unwrap(env, result, (void **)&wantAgent);
        if (wantAgent == nullptr) {
            ANS_LOGE("Invalid object maxScreenWantAgent");
            return nullptr;
        }
        std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> maxScreenWantAgent =
            std::make_shared<AbilityRuntime::WantAgent::WantAgent>(*wantAgent);
        request.SetMaxScreenWantAgent(maxScreenWantAgent);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationAutoDeletedTime(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    int64_t autoDeletedTime = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "autoDeletedTime", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "autoDeletedTime", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int64(env, result, &autoDeletedTime);
        request.SetAutoDeletedTime(autoDeletedTime);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationClassification(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "classification", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "classification", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
        request.SetClassification(str);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationColor(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    int32_t color = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "color", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "color", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, result, &color);
        NAPI_ASSERT(env, color >= 0, "Wrong argument type. Natural number expected.");
        request.SetColor(color);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationColorEnabled(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    bool colorEnabled = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "colorEnabled", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "colorEnabled", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &colorEnabled);
        request.SetColorEnabled(colorEnabled);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationIsAlertOnce(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    bool isAlertOnce = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "isAlertOnce", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isAlertOnce", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isAlertOnce);
        request.SetAlertOneTime(isAlertOnce);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationIsStopwatch(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    bool isStopwatch = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "isStopwatch", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isStopwatch", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isStopwatch);
        request.SetShowStopwatch(isStopwatch);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationIsCountDown(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    bool isCountDown = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "isCountDown", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isCountDown", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isCountDown);
        request.SetCountdownTimer(isCountDown);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationStatusBarText(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
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

    return NapiGetNull(env);
}

napi_value Common::GetNotificationLabel(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
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

    return NapiGetNull(env);
}

napi_value Common::GetNotificationBadgeIconStyle(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    int32_t badgeIconStyle = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "badgeIconStyle", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "badgeIconStyle", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, result, &badgeIconStyle);
        request.SetBadgeIconStyle(static_cast<NotificationRequest::BadgeStyle>(badgeIconStyle));
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationShowDeliveryTime(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    bool showDeliveryTime = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "showDeliveryTime", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "showDeliveryTime", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &showDeliveryTime);
        request.SetShowDeliveryTime(showDeliveryTime);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationActionButtons(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    bool isArray = false;
    napi_valuetype valuetype = napi_undefined;
    napi_value actionButtons = nullptr;
    uint32_t length = 0;
    bool hasProperty = false;

    napi_has_named_property(env, value, "actionButtons", &hasProperty);
    if (!hasProperty) {
        return Common::NapiGetNull(env);
    }

    napi_get_named_property(env, value, "actionButtons", &actionButtons);
    napi_is_array(env, actionButtons, &isArray);
    NAPI_ASSERT(env, isArray, "Property actionButtons is expected to be an array.");
    napi_get_array_length(env, actionButtons, &length);
    NAPI_ASSERT(env, length > 0, "The array is empty.");
    for (size_t i = 0; i < length; i++) {
        napi_value actionButton = nullptr;
        napi_get_element(env, actionButtons, i, &actionButton);
        NAPI_CALL(env, napi_typeof(env, actionButton, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

        std::shared_ptr<NotificationActionButton> pActionButton = nullptr;
        if (GetNotificationActionButtonsDetailed(env, actionButton, pActionButton) == nullptr) {
            return nullptr;
        }
        request.AddActionButton(pActionButton);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationActionButtonsDetailed(
    const napi_env &env, const napi_value &actionButton, std::shared_ptr<NotificationActionButton> &pActionButton)
{
    ANS_LOGI("enter");

    if (!GetNotificationActionButtonsDetailedBasicInfo(env, actionButton, pActionButton)) {
        return nullptr;
    }
    if (!GetNotificationActionButtonsDetailedByExtras(env, actionButton, pActionButton)) {
        return nullptr;
    }
    if (!GetNotificationUserInput(env, actionButton, pActionButton)) {
        return nullptr;
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationActionButtonsDetailedBasicInfo(
    const napi_env &env, const napi_value &actionButton, std::shared_ptr<NotificationActionButton> &pActionButton)
{
    ANS_LOGI("enter");
    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    napi_value value = nullptr;
    std::string title;
    AbilityRuntime::WantAgent::WantAgent *wantAgentPtr = nullptr;
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent;

    // title: string
    NAPI_CALL(env, napi_has_named_property(env, actionButton, "title", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property title expected.");
    napi_get_named_property(env, actionButton, "title", &value);
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, value, str, STR_MAX_SIZE - 1, &strLen));
    title = str;

    // wantAgent: WantAgent
    NAPI_CALL(env, napi_has_named_property(env, actionButton, "wantAgent", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property wantAgent expected.");
    napi_get_named_property(env, actionButton, "wantAgent", &value);
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    napi_unwrap(env, value, (void **)&wantAgentPtr);
    if (wantAgentPtr == nullptr) {
        ANS_LOGE("Invalid object wantAgent");
        return nullptr;
    }
    wantAgent = std::make_shared<AbilityRuntime::WantAgent::WantAgent>(*wantAgentPtr);

    // icon?: image.PixelMap
    NAPI_CALL(env, napi_has_named_property(env, actionButton, "icon", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, actionButton, "icon", &value);
        NAPI_CALL(env, napi_typeof(env, value, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        pixelMap = Media::PixelMapNapi::GetPixelMap(env, value);
        if (pixelMap == nullptr) {
            ANS_LOGE("Invalid object pixelMap");
            return nullptr;
        }
    }
    pActionButton = NotificationActionButton::Create(pixelMap, title, wantAgent);

    return NapiGetNull(env);
}

napi_value Common::GetNotificationActionButtonsDetailedByExtras(
    const napi_env &env, const napi_value &actionButton, std::shared_ptr<NotificationActionButton> &pActionButton)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;

    if (!pActionButton) {
        ANS_LOGE("pActionButton is nullptr");
        return nullptr;
    }

    // extras?: {[key: string]: any}
    NAPI_CALL(env, napi_has_named_property(env, actionButton, "extras", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, actionButton, "extras", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        AAFwk::WantParams wantParams;
        if (!OHOS::AppExecFwk::UnwrapWantParams(env, result, wantParams)) {
            return nullptr;
        }
        pActionButton->AddAdditionalData(wantParams);
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationUserInput(
    const napi_env &env, const napi_value &actionButton, std::shared_ptr<NotificationActionButton> &pActionButton)
{
    ANS_LOGI("enter");
    napi_valuetype valuetype = napi_undefined;
    napi_value userInputResult = nullptr;
    bool hasProperty = false;

    // userInput?: NotificationUserInput
    NAPI_CALL(env, napi_has_named_property(env, actionButton, "userInput", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, actionButton, "userInput", &userInputResult);
        NAPI_CALL(env, napi_typeof(env, userInputResult, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        std::shared_ptr<NotificationUserInput> userInput = nullptr;

        if (!GetNotificationUserInputByInputKey(env, userInputResult, userInput)) {
            return nullptr;
        }
        if (!GetNotificationUserInputByTag(env, userInputResult, userInput)) {
            return nullptr;
        }
        if (!GetNotificationUserInputByOptions(env, userInputResult, userInput)) {
            return nullptr;
        }
        if (!GetNotificationUserInputByPermitFreeFormInput(env, userInputResult, userInput)) {
            return nullptr;
        }
        if (!GetNotificationUserInputByPermitMimeTypes(env, userInputResult, userInput)) {
            return nullptr;
        }
        if (!GetNotificationUserInputByEditType(env, userInputResult, userInput)) {
            return nullptr;
        }
        if (!GetNotificationUserInputByAdditionalData(env, userInputResult, userInput)) {
            return nullptr;
        }
        pActionButton->AddNotificationUserInput(userInput);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationUserInputByInputKey(
    const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput)
{
    ANS_LOGI("enter");
    napi_valuetype valuetype = napi_undefined;
    napi_value value = nullptr;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    // inputKey: string
    NAPI_CALL(env, napi_has_named_property(env, userInputResult, "inputKey", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property inputKey expected.");
    napi_get_named_property(env, userInputResult, "inputKey", &value);
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, value, str, STR_MAX_SIZE - 1, &strLen));
    ANS_LOGI("NotificationUserInput::inputKey = %{public}s", str);
    userInput = NotificationUserInput::Create(str);
    if (!userInput) {
        ANS_LOGI("Failed to create NotificationUserInput by inputKey=%{public}s", str);
        return nullptr;
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationUserInputByTag(
    const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value value = nullptr;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    if (!userInput) {
        ANS_LOGE("userInput is nullptr");
        return nullptr;
    }

    // tag: string
    NAPI_CALL(env, napi_has_named_property(env, userInputResult, "tag", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property tag expected.");
    napi_get_named_property(env, userInputResult, "tag", &value);
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, value, str, STR_MAX_SIZE - 1, &strLen));
    userInput->SetTag(str);
    ANS_LOGI("NotificationUserInput::tag = %{public}s", str);

    return NapiGetNull(env);
}

napi_value Common::GetNotificationUserInputByOptions(
    const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value value = nullptr;
    bool hasProperty = false;
    size_t strLen = 0;
    uint32_t length = 0;
    bool isArray = false;

    if (!userInput) {
        ANS_LOGE("userInput is nullptr");
        return nullptr;
    }

    // options: Array<string>
    NAPI_CALL(env, napi_has_named_property(env, userInputResult, "options", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property options expected.");
    napi_get_named_property(env, userInputResult, "options", &value);
    napi_is_array(env, value, &isArray);
    NAPI_ASSERT(env, isArray, "Property options is expected to be an array.");
    napi_get_array_length(env, value, &length);
    NAPI_ASSERT(env, length > 0, "The array is empty.");
    std::vector<std::string> options;
    for (uint32_t i = 0; i < length; ++i) {
        napi_value option = nullptr;
        char str[STR_MAX_SIZE] = {0};
        napi_get_element(env, value, i, &option);
        NAPI_CALL(env, napi_typeof(env, option, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, option, str, STR_MAX_SIZE - 1, &strLen));
        options.emplace_back(str);
    }
    userInput->SetOptions(options);

    return NapiGetNull(env);
}

napi_value Common::GetNotificationUserInputByPermitMimeTypes(
    const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value value = nullptr;
    bool hasProperty = false;
    size_t strLen = 0;
    uint32_t length = 0;
    bool isArray = false;

    if (!userInput) {
        ANS_LOGE("userInput is nullptr");
        return nullptr;
    }

    // permitMimeTypes?: Array<string>
    NAPI_CALL(env, napi_has_named_property(env, userInputResult, "permitMimeTypes", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, userInputResult, "permitMimeTypes", &value);
        napi_is_array(env, value, &isArray);
        NAPI_ASSERT(env, isArray, "Property options is expected to be an array.");
        napi_get_array_length(env, value, &length);
        NAPI_ASSERT(env, length > 0, "The array is empty.");
        for (uint32_t i = 0; i < length; ++i) {
            napi_value permitMimeType = nullptr;
            char str[STR_MAX_SIZE] = {0};
            napi_get_element(env, value, i, &permitMimeType);
            NAPI_CALL(env, napi_typeof(env, permitMimeType, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, permitMimeType, str, STR_MAX_SIZE - 1, &strLen));
            userInput->SetPermitMimeTypes(str, true);
        }
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationUserInputByPermitFreeFormInput(
    const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput)
{
    ANS_LOGI("enter");
    napi_value value = nullptr;
    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;

    if (!userInput) {
        ANS_LOGE("userInput is nullptr");
        return nullptr;
    }

    // permitFreeFormInput?: boolean
    NAPI_CALL(env, napi_has_named_property(env, userInputResult, "permitFreeFormInput", &hasProperty));
    if (hasProperty) {
        bool permitFreeFormInput = false;
        napi_get_named_property(env, userInputResult, "permitFreeFormInput", &value);
        NAPI_CALL(env, napi_typeof(env, value, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, value, &permitFreeFormInput);
        ANS_LOGI("permitFreeFormInput is: %{public}d", permitFreeFormInput);
        userInput->SetPermitFreeFormInput(permitFreeFormInput);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationUserInputByEditType(
    const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput)
{
    ANS_LOGI("enter");
    napi_value value = nullptr;
    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;
    int32_t editType = 0;

    if (!userInput) {
        ANS_LOGE("userInput is nullptr");
        return nullptr;
    }

    // editType?: number
    NAPI_CALL(env, napi_has_named_property(env, userInputResult, "editType", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, userInputResult, "editType", &value);
        NAPI_CALL(env, napi_typeof(env, value, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, value, &editType);
        userInput->SetEditType(NotificationConstant::InputEditType(editType));
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationUserInputByAdditionalData(
    const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;

    if (!userInput) {
        ANS_LOGE("userInput is nullptr");
        return nullptr;
    }

    // additionalData?: {[key: string]: Object}
    NAPI_CALL(env, napi_has_named_property(env, userInputResult, "additionalData", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, userInputResult, "additionalData", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        AAFwk::WantParams wantParams;
        if (!OHOS::AppExecFwk::UnwrapWantParams(env, result, wantParams)) {
            return nullptr;
        }
        userInput->AddAdditionalData(wantParams);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationSmallIcon(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "smallIcon", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "smallIcon", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
        pixelMap = Media::PixelMapNapi::GetPixelMap(env, result);
        if (pixelMap == nullptr) {
            ANS_LOGE("Invalid object pixelMap");
            return nullptr;
        }
        request.SetLittleIcon(pixelMap);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationLargeIcon(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "largeIcon", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "largeIcon", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
        pixelMap = Media::PixelMapNapi::GetPixelMap(env, result);
        if (pixelMap == nullptr) {
            ANS_LOGE("Invalid object pixelMap");
            return nullptr;
        }
        request.SetBigIcon(pixelMap);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationRequestDistributedOptions(const napi_env &env,
    const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");
    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;

    // distributedOption?: DistributedOptions
    NAPI_CALL(env, napi_has_named_property(env, value, "distributedOption", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "distributedOption", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

        // isDistributed?: boolean
        if (GetNotificationIsDistributed(env, result, request) == nullptr) {
            return nullptr;
        }

        // supportDisplayDevices?: Array<string>
        if (GetNotificationSupportDisplayDevices(env, result, request) == nullptr) {
            return nullptr;
        }

        // supportOperateDevices?: Array<string>
        if (GetNotificationSupportOperateDevices(env, result, request) == nullptr) {
            return nullptr;
        }
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationIsDistributed(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    bool isDistributed = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "isDistributed", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "isDistributed", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, result, &isDistributed);
        request.SetDistributed(isDistributed);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationSupportDisplayDevices(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    bool isArray = false;
    bool hasProperty = false;
    napi_valuetype valuetype = napi_undefined;
    napi_value supportDisplayDevices = nullptr;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    uint32_t length = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "supportDisplayDevices", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "supportDisplayDevices", &supportDisplayDevices);
        napi_is_array(env, supportDisplayDevices, &isArray);
        NAPI_ASSERT(env, isArray, "Property supportDisplayDevices is expected to be an array.");

        napi_get_array_length(env, supportDisplayDevices, &length);
        NAPI_ASSERT(env, length > 0, "The array is empty.");
        std::vector<std::string> devices;
        for (size_t i = 0; i < length; i++) {
            napi_value line = nullptr;
            napi_get_element(env, supportDisplayDevices, i, &line);
            NAPI_CALL(env, napi_typeof(env, line, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, line, str, STR_MAX_SIZE - 1, &strLen));
            devices.emplace_back(str);
            ANS_LOGI("supportDisplayDevices = %{public}s", str);
        }
        request.SetDevicesSupportDisplay(devices);
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationSupportOperateDevices(
    const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    bool isArray = false;
    bool hasProperty = false;
    napi_valuetype valuetype = napi_undefined;
    napi_value supportOperateDevices = nullptr;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    uint32_t length = 0;

    NAPI_CALL(env, napi_has_named_property(env, value, "supportOperateDevices", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "supportOperateDevices", &supportOperateDevices);
        napi_is_array(env, supportOperateDevices, &isArray);
        NAPI_ASSERT(env, isArray, "Property supportOperateDevices is expected to be an array.");

        napi_get_array_length(env, supportOperateDevices, &length);
        NAPI_ASSERT(env, length > 0, "The array is empty.");
        std::vector<std::string> devices;
        for (size_t i = 0; i < length; i++) {
            napi_value line = nullptr;
            napi_get_element(env, supportOperateDevices, i, &line);
            NAPI_CALL(env, napi_typeof(env, line, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
            NAPI_CALL(env, napi_get_value_string_utf8(env, line, str, STR_MAX_SIZE - 1, &strLen));
            devices.emplace_back(str);
            ANS_LOGI("supportOperateDevices = %{public}s", str);
        }
        request.SetDevicesSupportOperate(devices);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationContentType(const napi_env &env, const napi_value &result, int32_t &type)
{
    ANS_LOGI("enter");

    napi_value contentResult = nullptr;
    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;

    NAPI_CALL(env, napi_has_named_property(env, result, "contentType", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property contentType expected.");

    napi_get_named_property(env, result, "contentType", &contentResult);
    NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
    napi_get_value_int32(env, contentResult, &type);

    return NapiGetNull(env);
}

napi_value Common::GetNotificationBasicContent(
    const napi_env &env, const napi_value &result, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value contentResult = nullptr;
    bool hasProperty = false;
    NAPI_CALL(env, napi_has_named_property(env, result, "normal", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property normal expected.");
    napi_get_named_property(env, result, "normal", &contentResult);
    NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    if (normalContent == nullptr) {
        ANS_LOGE("normalContent is null");
        return nullptr;
    }

    if (GetNotificationBasicContentDetailed(env, contentResult, normalContent) == nullptr) {
        return nullptr;
    }

    request.SetContent(std::make_shared<NotificationContent>(normalContent));

    return NapiGetNull(env);
}

napi_value Common::GetNotificationBasicContentDetailed(
    const napi_env &env, const napi_value &contentResult, std::shared_ptr<NotificationBasicContent> basicContent)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value value = nullptr;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    // title: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "title", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property title expected.");
    napi_get_named_property(env, contentResult, "title", &value);
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, value, str, STR_MAX_SIZE - 1, &strLen));
    basicContent->SetTitle(str);
    ANS_LOGI("normal::title = %{public}s", str);

    // text: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "text", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property text expected.");
    napi_get_named_property(env, contentResult, "text", &value);
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, value, str, STR_MAX_SIZE - 1, &strLen));
    basicContent->SetText(str);
    ANS_LOGI("normal::text = %{public}s", str);

    // additionalText?: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "additionalText", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, contentResult, "additionalText", &value);
        NAPI_CALL(env, napi_typeof(env, value, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, value, str, STR_MAX_SIZE - 1, &strLen));
        basicContent->SetAdditionalText(str);
        ANS_LOGI("normal::additionalText = %{public}s", str);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationLongTextContent(
    const napi_env &env, const napi_value &result, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value contentResult = nullptr;
    bool hasProperty = false;

    NAPI_CALL(env, napi_has_named_property(env, result, "longText", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property longText expected.");

    napi_get_named_property(env, result, "longText", &contentResult);
    NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

    std::shared_ptr<OHOS::Notification::NotificationLongTextContent> longContent =
        std::make_shared<OHOS::Notification::NotificationLongTextContent>();
    if (longContent == nullptr) {
        ANS_LOGE("longContent is null");
        return nullptr;
    }

    if (GetNotificationLongTextContentDetailed(env, contentResult, longContent) == nullptr) {
        return nullptr;
    }

    request.SetContent(std::make_shared<NotificationContent>(longContent));

    return NapiGetNull(env);
}

napi_value Common::GetNotificationLongTextContentDetailed(
    const napi_env &env, const napi_value &contentResult,
    std::shared_ptr<OHOS::Notification::NotificationLongTextContent> &longContent)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value longContentResult = nullptr;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    char long_str[LONG_STR_MAX_SIZE + 1] = {0};
    size_t strLen = 0;

    if (GetNotificationBasicContentDetailed(env, contentResult, longContent) == nullptr) {
        return nullptr;
    }

    // longText: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "longText", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property longText expected.");
    napi_get_named_property(env, contentResult, "longText", &longContentResult);
    NAPI_CALL(env, napi_typeof(env, longContentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, longContentResult, long_str, LONG_STR_MAX_SIZE, &strLen));
    longContent->SetLongText(long_str);
    ANS_LOGI("longText::longText = %{public}s", long_str);

    // briefText: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "briefText", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property briefText expected.");
    napi_get_named_property(env, contentResult, "briefText", &longContentResult);
    NAPI_CALL(env, napi_typeof(env, longContentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, longContentResult, str, STR_MAX_SIZE - 1, &strLen));
    longContent->SetBriefText(str);
    ANS_LOGI("longText::briefText = %{public}s", str);

    // expandedTitle: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "expandedTitle", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property expandedTitle expected.");
    napi_get_named_property(env, contentResult, "expandedTitle", &longContentResult);
    NAPI_CALL(env, napi_typeof(env, longContentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, longContentResult, str, STR_MAX_SIZE - 1, &strLen));
    longContent->SetExpandedTitle(str);
    ANS_LOGI("longText::expandedTitle = %{public}s", str);

    return NapiGetNull(env);
}

napi_value Common::GetNotificationPictureContent(
    const napi_env &env, const napi_value &result, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value contentResult = nullptr;
    bool hasProperty = false;

    NAPI_CALL(env, napi_has_named_property(env, result, "picture", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property picture expected.");
    napi_get_named_property(env, result, "picture", &contentResult);
    NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

    std::shared_ptr<OHOS::Notification::NotificationPictureContent> pictureContent =
        std::make_shared<OHOS::Notification::NotificationPictureContent>();
    if (pictureContent == nullptr) {
        ANS_LOGE("pictureContent is null");
        return nullptr;
    }
    if (GetNotificationPictureContentDetailed(env, contentResult, pictureContent) == nullptr) {
        return nullptr;
    }

    request.SetContent(std::make_shared<NotificationContent>(pictureContent));

    return NapiGetNull(env);
}

napi_value Common::GetNotificationPictureContentDetailed(const napi_env &env,
    const napi_value &contentResult, std::shared_ptr<OHOS::Notification::NotificationPictureContent> &pictureContent)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value pictureContentResult = nullptr;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    if (GetNotificationBasicContentDetailed(env, contentResult, pictureContent) == nullptr) {
        return nullptr;
    }

    // briefText: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "briefText", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property briefText expected.");
    napi_get_named_property(env, contentResult, "briefText", &pictureContentResult);
    NAPI_CALL(env, napi_typeof(env, pictureContentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, pictureContentResult, str, STR_MAX_SIZE - 1, &strLen));
    pictureContent->SetBriefText(str);

    // expandedTitle: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "expandedTitle", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property briefText expected.");
    napi_get_named_property(env, contentResult, "expandedTitle", &pictureContentResult);
    NAPI_CALL(env, napi_typeof(env, pictureContentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, pictureContentResult, str, STR_MAX_SIZE - 1, &strLen));
    pictureContent->SetExpandedTitle(str);

    // picture: image.PixelMap
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "picture", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property picture expected.");
    napi_get_named_property(env, contentResult, "picture", &pictureContentResult);
    NAPI_CALL(env, napi_typeof(env, pictureContentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    pixelMap = Media::PixelMapNapi::GetPixelMap(env, pictureContentResult);
    if (pixelMap == nullptr) {
        ANS_LOGE("Invalid object pixelMap");
        return nullptr;
    }
    pictureContent->SetBigPicture(pixelMap);

    return Common::NapiGetNull(env);
}

napi_value Common::GetNotificationConversationalContent(
    const napi_env &env, const napi_value &result, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value contentResult = nullptr;
    bool hasProperty = false;
    MessageUser user;

    NAPI_CALL(env, napi_has_named_property(env, result, "conversation", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property conversation expected.");
    napi_get_named_property(env, result, "conversation", &contentResult);
    NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

    if (GetNotificationConversationalContentByUser(env, contentResult, user) == nullptr) {
        return nullptr;
    }

    std::shared_ptr<OHOS::Notification::NotificationConversationalContent> conversationalContent =
        std::make_shared<OHOS::Notification::NotificationConversationalContent>(user);
    if (conversationalContent == nullptr) {
        ANS_LOGE("conversationalContent is null");
        return nullptr;
    }

    if (GetNotificationBasicContentDetailed(env, contentResult, conversationalContent) == nullptr) {
        return nullptr;
    }
    if (GetNotificationConversationalContentTitle(env, contentResult, conversationalContent) == nullptr) {
        return nullptr;
    }
    if (GetNotificationConversationalContentGroup(env, contentResult, conversationalContent) == nullptr) {
        return nullptr;
    }
    if (GetNotificationConversationalContentMessages(env, contentResult, conversationalContent) == nullptr) {
        return nullptr;
    }

    request.SetContent(std::make_shared<NotificationContent>(conversationalContent));

    return NapiGetNull(env);
}

napi_value Common::GetNotificationConversationalContentByUser(
    const napi_env &env, const napi_value &contentResult, MessageUser &user)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;

    // user: MessageUser
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "user", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property user expected.");
    napi_value userResult = nullptr;
    napi_get_named_property(env, contentResult, "user", &userResult);
    NAPI_CALL(env, napi_typeof(env, userResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    if (!GetMessageUser(env, userResult, user)) {
        return nullptr;
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationConversationalContentTitle(
    const napi_env &env, const napi_value &contentResult,
    std::shared_ptr<OHOS::Notification::NotificationConversationalContent> &conversationalContent)
{
    ANS_LOGI("enter");
    napi_valuetype valuetype = napi_undefined;
    napi_value conversationalContentResult = nullptr;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    // conversationTitle: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "conversationTitle", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property conversationTitle expected.");
    napi_get_named_property(env, contentResult, "conversationTitle", &conversationalContentResult);
    NAPI_CALL(env, napi_typeof(env, conversationalContentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, conversationalContentResult, str, STR_MAX_SIZE - 1, &strLen));
    conversationalContent->SetConversationTitle(str);
    ANS_LOGI("conversationTitle = %{public}s", str);

    return NapiGetNull(env);
}

napi_value Common::GetNotificationConversationalContentGroup(
    const napi_env &env, const napi_value &contentResult,
    std::shared_ptr<OHOS::Notification::NotificationConversationalContent> &conversationalContent)
{
    ANS_LOGI("enter");
    napi_valuetype valuetype = napi_undefined;
    napi_value conversationalContentResult = nullptr;
    bool hasProperty = false;

    // conversationGroup: boolean
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "conversationGroup", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property conversationGroup expected.");
    napi_get_named_property(env, contentResult, "conversationGroup", &conversationalContentResult);
    NAPI_CALL(env, napi_typeof(env, conversationalContentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
    bool conversationGroup = false;
    napi_get_value_bool(env, conversationalContentResult, &conversationGroup);
    conversationalContent->SetConversationGroup(conversationGroup);
    ANS_LOGI("conversationalText::conversationGroup = %{public}d", conversationGroup);

    return NapiGetNull(env);
}

napi_value Common::GetNotificationConversationalContentMessages(
    const napi_env &env, const napi_value &contentResult,
    std::shared_ptr<OHOS::Notification::NotificationConversationalContent> &conversationalContent)
{
    ANS_LOGI("enter");
    napi_valuetype valuetype = napi_undefined;
    napi_value conversationalContentResult = nullptr;
    bool hasProperty = false;

    // messages: Array<ConversationalMessage>
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "messages", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property messages expected.");
    napi_get_named_property(env, contentResult, "messages", &conversationalContentResult);
    bool isArray = false;
    napi_is_array(env, conversationalContentResult, &isArray);
    NAPI_ASSERT(env, isArray, "Property messages is expected to be an array.");
    uint32_t length = 0;
    napi_get_array_length(env, conversationalContentResult, &length);
    NAPI_ASSERT(env, length > 0, "The array is empty.");
    for (size_t i = 0; i < length; i++) {
        napi_value conversationalMessage = nullptr;
        napi_get_element(env, conversationalContentResult, i, &conversationalMessage);
        NAPI_CALL(env, napi_typeof(env, conversationalMessage, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        std::shared_ptr<NotificationConversationalMessage> message = nullptr;
        if (!GetConversationalMessage(env, conversationalMessage, message)) {
            return nullptr;
        }
        conversationalContent->AddConversationalMessage(message);
    }

    return NapiGetNull(env);
}

napi_value Common::GetConversationalMessage(const napi_env &env, const napi_value &conversationalMessage,
    std::shared_ptr<NotificationConversationalMessage> &message)
{
    ANS_LOGI("enter");

    if (GetConversationalMessageBasicInfo(env, conversationalMessage, message) == nullptr) {
        return nullptr;
    }
    if (GetConversationalMessageOtherInfo(env, conversationalMessage, message) == nullptr) {
        return nullptr;
    }
    return NapiGetNull(env);
}

napi_value Common::GetConversationalMessageBasicInfo(const napi_env &env, const napi_value &conversationalMessage,
    std::shared_ptr<NotificationConversationalMessage> &message)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    std::string text;
    int64_t timestamp = 0;
    MessageUser sender;

    // text: string
    NAPI_CALL(env, napi_has_named_property(env, conversationalMessage, "text", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property text expected.");
    napi_value textResult = nullptr;
    napi_get_named_property(env, conversationalMessage, "text", &textResult);
    NAPI_CALL(env, napi_typeof(env, textResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, textResult, str, STR_MAX_SIZE - 1, &strLen));
    text = str;
    ANS_LOGI("conversationalMessage::text = %{public}s", str);

    // timestamp: number
    NAPI_CALL(env, napi_has_named_property(env, conversationalMessage, "timestamp", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property timestamp expected.");
    napi_value timestampResult = nullptr;
    napi_get_named_property(env, conversationalMessage, "timestamp", &timestampResult);
    NAPI_CALL(env, napi_typeof(env, timestampResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
    napi_get_value_int64(env, timestampResult, &timestamp);
    ANS_LOGI("conversationalMessage::timestamp = %{public}" PRId64, timestamp);

    // sender: MessageUser
    NAPI_CALL(env, napi_has_named_property(env, conversationalMessage, "sender", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property sender expected.");
    napi_value senderResult = nullptr;
    napi_get_named_property(env, conversationalMessage, "sender", &senderResult);
    NAPI_CALL(env, napi_typeof(env, senderResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    if (!GetMessageUser(env, senderResult, sender)) {
        return nullptr;
    }

    message = std::make_shared<NotificationConversationalMessage>(text, timestamp, sender);
    if (!message) {
        ANS_LOGE("Failed to create NotificationConversationalMessage object");
        return nullptr;
    }

    return NapiGetNull(env);
}

napi_value Common::GetConversationalMessageOtherInfo(const napi_env &env, const napi_value &conversationalMessage,
    std::shared_ptr<NotificationConversationalMessage> &message)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    std::string mimeType;
    std::string uri;

    // mimeType: string
    NAPI_CALL(env, napi_has_named_property(env, conversationalMessage, "mimeType", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property mimeType expected.");
    napi_value mimeTypeResult = nullptr;
    napi_get_named_property(env, conversationalMessage, "mimeType", &mimeTypeResult);
    NAPI_CALL(env, napi_typeof(env, mimeTypeResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, mimeTypeResult, str, STR_MAX_SIZE - 1, &strLen));
    mimeType = str;
    ANS_LOGI("conversationalMessage::mimeType = %{public}s", str);

    // uri?: string
    NAPI_CALL(env, napi_has_named_property(env, conversationalMessage, "uri", &hasProperty));
    if (hasProperty) {
        napi_value uriResult = nullptr;
        napi_get_named_property(env, conversationalMessage, "uri", &uriResult);
        NAPI_CALL(env, napi_typeof(env, uriResult, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, uriResult, str, STR_MAX_SIZE - 1, &strLen));
        uri = str;
    }

    std::shared_ptr<Uri> uriPtr = std::make_shared<Uri>(uri);
    message->SetData(mimeType, uriPtr);

    return NapiGetNull(env);
}

napi_value Common::GetMessageUser(const napi_env &env, const napi_value &result, MessageUser &messageUser)
{
    ANS_LOGI("enter");

    if (GetMessageUserByString(env, result, messageUser) == nullptr) {
        return nullptr;
    }

    if (GetMessageUserByBool(env, result, messageUser) == nullptr) {
        return nullptr;
    }

    if (GetMessageUserByCustom(env, result, messageUser) == nullptr) {
        return nullptr;
    }

    return NapiGetNull(env);
}

napi_value Common::GetMessageUserByString(const napi_env &env, const napi_value &result, MessageUser &messageUser)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    // name: string
    NAPI_CALL(env, napi_has_named_property(env, result, "name", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property name expected.");
    napi_value nameResult = nullptr;
    napi_get_named_property(env, result, "name", &nameResult);
    NAPI_CALL(env, napi_typeof(env, nameResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, nameResult, str, STR_MAX_SIZE - 1, &strLen));
    messageUser.SetName(str);
    ANS_LOGI("MessageUser::name = %{public}s", str);

    // key: string
    NAPI_CALL(env, napi_has_named_property(env, result, "key", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property key expected.");
    napi_value keyResult = nullptr;
    napi_get_named_property(env, result, "key", &keyResult);
    NAPI_CALL(env, napi_typeof(env, keyResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, keyResult, str, STR_MAX_SIZE - 1, &strLen));
    messageUser.SetKey(str);
    ANS_LOGI("MessageUser::key = %{public}s", str);

    // uri: string
    NAPI_CALL(env, napi_has_named_property(env, result, "uri", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property uri expected.");
    napi_value uriResult = nullptr;
    napi_get_named_property(env, result, "uri", &uriResult);
    NAPI_CALL(env, napi_typeof(env, uriResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, uriResult, str, STR_MAX_SIZE - 1, &strLen));
    Uri uri(str);
    messageUser.SetUri(uri);

    return NapiGetNull(env);
}

napi_value Common::GetMessageUserByBool(const napi_env &env, const napi_value &result, MessageUser &messageUser)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;

    // isMachine: boolean
    NAPI_CALL(env, napi_has_named_property(env, result, "isMachine", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property isMachine expected.");
    napi_value machineResult = nullptr;
    napi_get_named_property(env, result, "isMachine", &machineResult);
    NAPI_CALL(env, napi_typeof(env, machineResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
    bool machine = false;
    napi_get_value_bool(env, machineResult, &machine);
    messageUser.SetMachine(machine);
    ANS_LOGI("MessageUser::isMachine = %{public}d", machine);

    // isUserImportant: boolean
    NAPI_CALL(env, napi_has_named_property(env, result, "isUserImportant", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property isUserImportant expected.");
    napi_value importantResult = nullptr;
    napi_get_named_property(env, result, "isUserImportant", &importantResult);
    NAPI_CALL(env, napi_typeof(env, importantResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
    bool important = false;
    napi_get_value_bool(env, importantResult, &important);
    messageUser.SetUserAsImportant(important);
    ANS_LOGI("MessageUser::isUserImportant = %{public}d", important);

    return NapiGetNull(env);
}

napi_value Common::GetMessageUserByCustom(const napi_env &env, const napi_value &result, MessageUser &messageUser)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;

    // icon?: image.PixelMap
    NAPI_CALL(env, napi_has_named_property(env, result, "icon", &hasProperty));
    if (hasProperty) {
        napi_value iconResult = nullptr;
        napi_get_named_property(env, result, "icon", &iconResult);
        NAPI_CALL(env, napi_typeof(env, iconResult, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
        pixelMap = Media::PixelMapNapi::GetPixelMap(env, iconResult);
        if (pixelMap == nullptr) {
            ANS_LOGE("Invalid object pixelMap");
            return nullptr;
        }
        messageUser.SetPixelMap(pixelMap);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationMultiLineContent(
    const napi_env &env, const napi_value &result, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value contentResult = nullptr;
    napi_value multiLineContentResult = nullptr;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    NAPI_CALL(env, napi_has_named_property(env, result, "multiLine", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property multiLine expected.");
    napi_get_named_property(env, result, "multiLine", &contentResult);
    NAPI_CALL(env, napi_typeof(env, contentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

    std::shared_ptr<OHOS::Notification::NotificationMultiLineContent> multiLineContent =
        std::make_shared<OHOS::Notification::NotificationMultiLineContent>();
    if (multiLineContent == nullptr) {
        ANS_LOGE("multiLineContent is null");
        return nullptr;
    }

    if (GetNotificationBasicContentDetailed(env, contentResult, multiLineContent) == nullptr) {
        return nullptr;
    }

    // briefText: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "briefText", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property briefText expected.");
    napi_get_named_property(env, contentResult, "briefText", &multiLineContentResult);
    NAPI_CALL(env, napi_typeof(env, multiLineContentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, multiLineContentResult, str, STR_MAX_SIZE - 1, &strLen));
    multiLineContent->SetBriefText(str);
    ANS_LOGI("multiLine: briefText = %{public}s", str);

    // longTitle: string
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "longTitle", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property longTitle expected.");
    napi_get_named_property(env, contentResult, "longTitle", &multiLineContentResult);
    NAPI_CALL(env, napi_typeof(env, multiLineContentResult, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, multiLineContentResult, str, STR_MAX_SIZE - 1, &strLen));
    multiLineContent->SetExpandedTitle(str);
    ANS_LOGI("multiLine: longTitle = %{public}s", str);

    // lines: Array<String>
    NAPI_CALL(env, napi_has_named_property(env, contentResult, "lines", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property lines expected.");
    if (GetNotificationMultiLineContentLines(env, contentResult, multiLineContent) == nullptr) {
        return nullptr;
    }

    request.SetContent(std::make_shared<NotificationContent>(multiLineContent));

    return NapiGetNull(env);
}

napi_value Common::GetNotificationMultiLineContentLines(const napi_env &env, const napi_value &result,
    std::shared_ptr<OHOS::Notification::NotificationMultiLineContent> &multiLineContent)
{
    ANS_LOGI("enter");

    bool isArray = false;
    napi_valuetype valuetype = napi_undefined;
    napi_value multilines = nullptr;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    uint32_t length = 0;

    napi_get_named_property(env, result, "lines", &multilines);
    napi_is_array(env, multilines, &isArray);
    NAPI_ASSERT(env, isArray, "Property lines is expected to be an array.");

    napi_get_array_length(env, multilines, &length);
    NAPI_ASSERT(env, length > 0, "The array is empty.");
    for (size_t i = 0; i < length; i++) {
        napi_value line = nullptr;
        napi_get_element(env, multilines, i, &line);
        NAPI_CALL(env, napi_typeof(env, line, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, line, str, STR_MAX_SIZE - 1, &strLen));
        multiLineContent->AddSingleLine(str);
        ANS_LOGI("multiLine: lines : addSingleLine = %{public}s", str);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationSlot(const napi_env &env, const napi_value &value, NotificationSlot &slot)
{
    ANS_LOGI("enter");

    napi_value nobj = nullptr;
    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;

    // type: notification.SlotType
    int slotType = 0;
    NAPI_CALL(env, napi_has_named_property(env, value, "type", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property type expected.");
    napi_get_named_property(env, value, "type", &nobj);
    NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
    napi_get_value_int32(env, nobj, &slotType);
    NotificationConstant::SlotType outType = NotificationConstant::SlotType::OTHER;
    if (!Common::SlotTypeJSToC(SlotType(slotType), outType)) {
        return nullptr;
    }
    slot.SetType(outType);

    if (GetNotificationSlotByString(env, value, slot) == nullptr) {
        return nullptr;
    }
    if (GetNotificationSlotByNumber(env, value, slot) == nullptr) {
        return nullptr;
    }
    if (GetNotificationSlotByVibration(env, value, slot) == nullptr) {
        return nullptr;
    }
    if (GetNotificationSlotByBool(env, value, slot) == nullptr) {
        return nullptr;
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationSlotByString(const napi_env &env, const napi_value &value, NotificationSlot &slot)
{
    ANS_LOGI("enter");

    napi_value nobj = nullptr;
    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;
    size_t strLen = 0;

    // desc?: string
    NAPI_CALL(env, napi_has_named_property(env, value, "desc", &hasProperty));
    if (hasProperty) {
        std::string desc;
        char str[STR_MAX_SIZE] = {0};
        napi_get_named_property(env, value, "desc", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, nobj, str, STR_MAX_SIZE - 1, &strLen));
        desc = str;
        ANS_LOGI("desc is: %{public}s", desc.c_str());
        slot.SetDescription(desc);
    }

    // sound?: string
    NAPI_CALL(env, napi_has_named_property(env, value, "sound", &hasProperty));
    if (hasProperty) {
        std::string sound;
        char str[STR_MAX_SIZE] = {0};
        napi_get_named_property(env, value, "sound", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, nobj, str, STR_MAX_SIZE - 1, &strLen));
        sound = str;
        ANS_LOGI("sound is: %{public}s", sound.c_str());
        slot.SetSound(Uri(sound));
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationSlotByBool(const napi_env &env, const napi_value &value, NotificationSlot &slot)
{
    ANS_LOGI("enter");
    napi_value nobj = nullptr;
    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;

    // badgeFlag?: boolean
    NAPI_CALL(env, napi_has_named_property(env, value, "badgeFlag", &hasProperty));
    if (hasProperty) {
        bool badgeFlag = false;
        napi_get_named_property(env, value, "badgeFlag", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, nobj, &badgeFlag);
        ANS_LOGI("badgeFlag is: %{public}d", badgeFlag);
        slot.EnableBadge(badgeFlag);
    }

    // bypassDnd?: boolean
    NAPI_CALL(env, napi_has_named_property(env, value, "bypassDnd", &hasProperty));
    if (hasProperty) {
        bool bypassDnd = false;
        napi_get_named_property(env, value, "bypassDnd", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, nobj, &bypassDnd);
        ANS_LOGI("bypassDnd is: %{public}d", bypassDnd);
        slot.EnableBypassDnd(bypassDnd);
    }

    // lightEnabled?: boolean
    NAPI_CALL(env, napi_has_named_property(env, value, "lightEnabled", &hasProperty));
    if (hasProperty) {
        bool lightEnabled = false;
        napi_get_named_property(env, value, "lightEnabled", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, nobj, &lightEnabled);
        ANS_LOGI("lightEnabled is: %{public}d", lightEnabled);
        slot.SetEnableLight(lightEnabled);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationSlotByNumber(const napi_env &env, const napi_value &value, NotificationSlot &slot)
{
    ANS_LOGI("enter");

    napi_value nobj = nullptr;
    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;

    // level?: number
    NAPI_CALL(env, napi_has_named_property(env, value, "level", &hasProperty));
    if (hasProperty) {
        int inLevel = 0;
        napi_get_named_property(env, value, "level", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, nobj, &inLevel);
        ANS_LOGI("level is: %{public}d", inLevel);

        NotificationSlot::NotificationLevel outLevel {NotificationSlot::NotificationLevel::LEVEL_NONE};
        if (!Common::SlotLevelJSToC(SlotLevel(inLevel), outLevel)) {
            return nullptr;
        }
        slot.SetLevel(outLevel);
    }

    // lockscreenVisibility?: number
    NAPI_CALL(env, napi_has_named_property(env, value, "lockscreenVisibility", &hasProperty));
    if (hasProperty) {
        int lockscreenVisibility = 0;
        napi_get_named_property(env, value, "lockscreenVisibility", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, nobj, &lockscreenVisibility);
        ANS_LOGI("lockscreenVisibility is: %{public}d", lockscreenVisibility);
        slot.SetLockscreenVisibleness(NotificationConstant::VisiblenessType(lockscreenVisibility));
    }

    // lightColor?: number
    NAPI_CALL(env, napi_has_named_property(env, value, "lightColor", &hasProperty));
    if (hasProperty) {
        int lightColor = 0;
        napi_get_named_property(env, value, "lightColor", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, nobj, &lightColor);
        ANS_LOGI("lightColor is: %{public}d", lightColor);
        slot.SetLedLightColor(lightColor);
    }
    return NapiGetNull(env);
}

napi_value Common::GetNotificationSlotByVibration(const napi_env &env, const napi_value &value, NotificationSlot &slot)
{
    ANS_LOGI("enter");
    napi_value nobj = nullptr;
    napi_valuetype valuetype = napi_undefined;
    bool hasProperty = false;
    uint32_t length = 0;

    // vibrationValues?: Array<number>
    NAPI_CALL(env, napi_has_named_property(env, value, "vibrationValues", &hasProperty));
    if (hasProperty) {
        bool isArray = false;
        napi_get_named_property(env, value, "vibrationValues", &nobj);
        napi_is_array(env, nobj, &isArray);
        NAPI_ASSERT(env, isArray, "Property vibrationValues is expected to be an array.");
        napi_get_array_length(env, nobj, &length);
        NAPI_ASSERT(env, length > 0, "The array is empty.");
        std::vector<int64_t> vibrationValues;
        for (size_t i = 0; i < length; i++) {
            napi_value nVibrationValue = nullptr;
            int64_t vibrationValue = 0;
            napi_get_element(env, nobj, i, &nVibrationValue);
            NAPI_CALL(env, napi_typeof(env, nVibrationValue, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
            napi_get_value_int64(env, nVibrationValue, &vibrationValue);
            ANS_LOGI("vibrationValue is: %{public}" PRId64, vibrationValue);
            vibrationValues.emplace_back(vibrationValue);
        }
        slot.SetVibrationStyle(vibrationValues);
    }

    // vibrationEnabled?: boolean
    NAPI_CALL(env, napi_has_named_property(env, value, "vibrationEnabled", &hasProperty));
    if (hasProperty) {
        bool vibrationEnabled = false;
        napi_get_named_property(env, value, "vibrationEnabled", &nobj);
        NAPI_CALL(env, napi_typeof(env, nobj, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, nobj, &vibrationEnabled);
        ANS_LOGI("vibrationEnabled is: %{public}d", vibrationEnabled);
        slot.SetEnableVibration(vibrationEnabled);
    }

    return NapiGetNull(env);
}

napi_value Common::GetBundleOption(const napi_env &env, const napi_value &value, NotificationBundleOption &option)
{
    ANS_LOGI("enter");

    bool hasProperty {false};
    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;

    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;
    // bundle: string
    NAPI_CALL(env, napi_has_named_property(env, value, "bundle", &hasProperty));
    if (!hasProperty) {
        ANS_LOGE("Property bundle expected.");
        return nullptr;
    }
    napi_get_named_property(env, value, "bundle", &result);
    NAPI_CALL(env, napi_typeof(env, result, &valuetype));
    if (valuetype != napi_string) {
        ANS_LOGE("Wrong argument type. String expected.");
        return nullptr;
    }
    NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
    option.SetBundleName(str);

    // uid?: number
    NAPI_CALL(env, napi_has_named_property(env, value, "uid", &hasProperty));
    if (hasProperty) {
        int32_t uid = 0;
        napi_get_named_property(env, value, "uid", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        if (valuetype != napi_number) {
            ANS_LOGE("Wrong argument type. Number expected.");
            return nullptr;
        }
        napi_get_value_int32(env, result, &uid);
        option.SetUid(uid);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationKey(const napi_env &env, const napi_value &value, NotificationKey &key)
{
    ANS_LOGI("enter");

    bool hasProperty {false};
    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;

    // id: number
    NAPI_CALL(env, napi_has_named_property(env, value, "id", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property id expected.");
    napi_get_named_property(env, value, "id", &result);
    NAPI_CALL(env, napi_typeof(env, result, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
    napi_get_value_int32(env, result, &key.id);

    // label?: string
    NAPI_CALL(env, napi_has_named_property(env, value, "label", &hasProperty));
    if (hasProperty) {
        char str[STR_MAX_SIZE] = {0};
        size_t strLen = 0;
        napi_get_named_property(env, value, "label", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
        key.label = str;
    }

    return NapiGetNull(env);
}

bool Common::ContentTypeJSToC(const ContentType &inType, NotificationContent::Type &outType)
{
    switch (inType) {
        case ContentType::NOTIFICATION_CONTENT_BASIC_TEXT:
            outType = NotificationContent::Type::BASIC_TEXT;
            break;
        case ContentType::NOTIFICATION_CONTENT_LONG_TEXT:
            outType = NotificationContent::Type::LONG_TEXT;
            break;
        case ContentType::NOTIFICATION_CONTENT_MULTILINE:
            outType = NotificationContent::Type::MULTILINE;
            break;
        case ContentType::NOTIFICATION_CONTENT_PICTURE:
            outType = NotificationContent::Type::PICTURE;
            break;
        case ContentType::NOTIFICATION_CONTENT_CONVERSATION:
            outType = NotificationContent::Type::CONVERSATION;
            break;
        default:
            ANS_LOGE("ContentType %{public}d is an invalid value", inType);
            return false;
    }
    return true;
}

bool Common::ContentTypeCToJS(const NotificationContent::Type &inType, ContentType &outType)
{
    switch (inType) {
        case NotificationContent::Type::BASIC_TEXT:
            outType = ContentType::NOTIFICATION_CONTENT_BASIC_TEXT;
            break;
        case NotificationContent::Type::LONG_TEXT:
            outType = ContentType::NOTIFICATION_CONTENT_LONG_TEXT;
            break;
        case NotificationContent::Type::MULTILINE:
            outType = ContentType::NOTIFICATION_CONTENT_MULTILINE;
            break;
        case NotificationContent::Type::PICTURE:
            outType = ContentType::NOTIFICATION_CONTENT_PICTURE;
            break;
        case NotificationContent::Type::CONVERSATION:
            outType = ContentType::NOTIFICATION_CONTENT_CONVERSATION;
            break;
        default:
            ANS_LOGE("ContentType %{public}d is an invalid value", inType);
            return false;
    }
    return true;
}

bool Common::SlotTypeJSToC(const SlotType &inType, NotificationConstant::SlotType &outType)
{
    switch (inType) {
        case SlotType::SOCIAL_COMMUNICATION:
            outType = NotificationConstant::SlotType::SOCIAL_COMMUNICATION;
            break;
        case SlotType::SERVICE_INFORMATION:
            outType = NotificationConstant::SlotType::SERVICE_REMINDER;
            break;
        case SlotType::CONTENT_INFORMATION:
            outType = NotificationConstant::SlotType::CONTENT_INFORMATION;
            break;
        case SlotType::UNKNOWN_TYPE:
        case SlotType::OTHER_TYPES:
            outType = NotificationConstant::SlotType::OTHER;
            break;
        default:
            ANS_LOGE("SlotType %{public}d is an invalid value", inType);
            return false;
    }
    return true;
}

bool Common::SlotTypeCToJS(const NotificationConstant::SlotType &inType, SlotType &outType)
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
            ANS_LOGE("SlotType %{public}d is an invalid value", inType);
            return false;
    }
    return true;
}

bool Common::SlotLevelJSToC(const SlotLevel &inLevel, NotificationSlot::NotificationLevel &outLevel)
{
    switch (inLevel) {
        case SlotLevel::LEVEL_NONE:
            outLevel = NotificationSlot::NotificationLevel::LEVEL_NONE;
            break;
        case SlotLevel::LEVEL_MIN:
            outLevel = NotificationSlot::NotificationLevel::LEVEL_MIN;
            break;
        case SlotLevel::LEVEL_LOW:
            outLevel = NotificationSlot::NotificationLevel::LEVEL_LOW;
            break;
        case SlotLevel::LEVEL_DEFAULT:
            outLevel = NotificationSlot::NotificationLevel::LEVEL_DEFAULT;
            break;
        case SlotLevel::LEVEL_HIGH:
            outLevel = NotificationSlot::NotificationLevel::LEVEL_HIGH;
            break;
        default:
            ANS_LOGE("SlotLevel %{public}d is an invalid value", inLevel);
            return false;
    }
    return true;
}

bool Common::SlotLevelCToJS(const NotificationSlot::NotificationLevel &inLevel, SlotLevel &outLevel)
{
    switch (inLevel) {
        case NotificationSlot::NotificationLevel::LEVEL_NONE:
        case NotificationSlot::NotificationLevel::LEVEL_UNDEFINED:
            outLevel = SlotLevel::LEVEL_NONE;
            break;
        case NotificationSlot::NotificationLevel::LEVEL_MIN:
            outLevel = SlotLevel::LEVEL_MIN;
            break;
        case NotificationSlot::NotificationLevel::LEVEL_LOW:
            outLevel = SlotLevel::LEVEL_LOW;
            break;
        case NotificationSlot::NotificationLevel::LEVEL_DEFAULT:
            outLevel = SlotLevel::LEVEL_DEFAULT;
            break;
        case NotificationSlot::NotificationLevel::LEVEL_HIGH:
            outLevel = SlotLevel::LEVEL_HIGH;
            break;
        default:
            ANS_LOGE("SlotLevel %{public}d is an invalid value", inLevel);
            return false;
    }
    return true;
}

bool Common::ReasonCToJS(const int &inType, int &outType)
{
    switch (inType) {
        case NotificationConstant::CLICK_REASON_DELETE:
            outType = (int)NotificationReason::CLICK_REASON_DELETE;
            break;
        case NotificationConstant::CANCEL_REASON_DELETE:
            outType = (int)NotificationReason::CANCEL_REASON_DELETE;
            break;
        case NotificationConstant::CANCEL_ALL_REASON_DELETE:
            outType = (int)NotificationReason::CANCEL_ALL_REASON_DELETE;
            break;
        case NotificationConstant::ERROR_REASON_DELETE:
            outType = (int)NotificationReason::ERROR_REASON_DELETE;
            break;
        case NotificationConstant::PACKAGE_CHANGED_REASON_DELETE:
            outType = (int)NotificationReason::PACKAGE_CHANGED_REASON_DELETE;
            break;
        case NotificationConstant::USER_STOPPED_REASON_DELETE:
            outType = (int)NotificationReason::USER_STOPPED_REASON_DELETE;
            break;
        case NotificationConstant::APP_CANCEL_REASON_DELETE:
            outType = (int)NotificationReason::APP_CANCEL_REASON_DELETE;
            break;
        case NotificationConstant::APP_CANCEL_ALL_REASON_DELETE:
            outType = (int)NotificationReason::APP_CANCEL_ALL_REASON_DELETE;
            break;
        case NotificationConstant::APP_CANCEL_REASON_OTHER:
            outType = (int)NotificationReason::APP_CANCEL_REASON_OTHER;
            break;
        default:
            ANS_LOGE("Reason %{public}d is an invalid value", inType);
            return false;
    }
    return true;
}

bool Common::DoNotDisturbTypeJSToC(const DoNotDisturbType &inType, NotificationConstant::DoNotDisturbType &outType)
{
    switch (inType) {
        case DoNotDisturbType::TYPE_NONE:
            outType = NotificationConstant::DoNotDisturbType::NONE;
            break;
        case DoNotDisturbType::TYPE_ONCE:
            outType = NotificationConstant::DoNotDisturbType::ONCE;
            break;
        case DoNotDisturbType::TYPE_DAILY:
            outType = NotificationConstant::DoNotDisturbType::DAILY;
            break;
        case DoNotDisturbType::TYPE_CLEARLY:
            outType = NotificationConstant::DoNotDisturbType::CLEARLY;
            break;
        default:
            ANS_LOGE("DoNotDisturbType %{public}d is an invalid value", inType);
            return false;
    }
    return true;
}

bool Common::DoNotDisturbTypeCToJS(const NotificationConstant::DoNotDisturbType &inType, DoNotDisturbType &outType)
{
    switch (inType) {
        case NotificationConstant::DoNotDisturbType::NONE:
            outType = DoNotDisturbType::TYPE_NONE;
            break;
        case NotificationConstant::DoNotDisturbType::ONCE:
            outType = DoNotDisturbType::TYPE_ONCE;
            break;
        case NotificationConstant::DoNotDisturbType::DAILY:
            outType = DoNotDisturbType::TYPE_DAILY;
            break;
        case NotificationConstant::DoNotDisturbType::CLEARLY:
            outType = DoNotDisturbType::TYPE_CLEARLY;
            break;
        default:
            ANS_LOGE("DoNotDisturbType %{public}d is an invalid value", inType);
            return false;
    }
    return true;
}

bool Common::DeviceRemindTypeCToJS(const NotificationConstant::RemindType &inType, DeviceRemindType &outType)
{
    switch (inType) {
        case NotificationConstant::RemindType::DEVICE_IDLE_DONOT_REMIND:
            outType = DeviceRemindType::IDLE_DONOT_REMIND;
            break;
        case NotificationConstant::RemindType::DEVICE_IDLE_REMIND:
            outType = DeviceRemindType::IDLE_REMIND;
            break;
        case NotificationConstant::RemindType::DEVICE_ACTIVE_DONOT_REMIND:
            outType = DeviceRemindType::ACTIVE_DONOT_REMIND;
            break;
        case NotificationConstant::RemindType::DEVICE_ACTIVE_REMIND:
            outType = DeviceRemindType::ACTIVE_REMIND;
            break;
        default:
            ANS_LOGE("DeviceRemindType %{public}d is an invalid value", inType);
            return false;
    }
    return true;
}

bool Common::SourceTypeCToJS(const NotificationConstant::SourceType &inType, SourceType &outType)
{
    switch (inType) {
        case NotificationConstant::SourceType::TYPE_NORMAL:
            outType = SourceType::TYPE_NORMAL;
            break;
        case NotificationConstant::SourceType::TYPE_CONTINUOUS:
            outType = SourceType::TYPE_CONTINUOUS;
            break;
        case NotificationConstant::SourceType::TYPE_TIMER:
            outType = SourceType::TYPE_TIMER;
            break;
        default:
            ANS_LOGE("SourceType %{public}d is an invalid value", inType);
            return false;
    }
    return true;
}

napi_value Common::CreateWantAgentByJS(const napi_env &env,
    const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> &agent)
{
    if (agent == nullptr) {
        ANS_LOGI("agent is nullptr");
        return nullptr;
    }

    wantAgent_.insert(agent);
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
    napi_wrap(env,
        wantAgent,
        (void *)agent.get(),
        [](napi_env env, void *data, void *hint) {
            AbilityRuntime::WantAgent::WantAgent *objectInfo =
                static_cast<AbilityRuntime::WantAgent::WantAgent *>(data);
            if (objectInfo) {
                for (auto it = wantAgent_.begin(); it != wantAgent_.end(); ++it) {
                    if ((*it).get() == objectInfo) {
                        wantAgent_.erase(it);
                        break;
                    }
                }
            }
        },
        nullptr,
        nullptr);

    return wantAgent;
}

napi_value Common::GetNotificationTemplate(const napi_env &env, const napi_value &value, NotificationRequest &request)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;

    NAPI_CALL(env, napi_has_named_property(env, value, "template", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "template", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");

        std::shared_ptr<NotificationTemplate> templ = std::make_shared<NotificationTemplate>();
        if (templ == nullptr) {
            ANS_LOGE("template is null");
            return nullptr;
        }
        if (GetNotificationTemplateInfo(env, result, templ) == nullptr) {
            return nullptr;
        }

        request.SetTemplate(templ);
    }

    return NapiGetNull(env);
}

napi_value Common::GetNotificationTemplateInfo(const napi_env &env, const napi_value &value,
    std::shared_ptr<NotificationTemplate> &templ)
{
    ANS_LOGI("enter");

    napi_valuetype valuetype = napi_undefined;
    napi_value result = nullptr;
    bool hasProperty = false;
    char str[STR_MAX_SIZE] = {0};
    size_t strLen = 0;

    // name: string
    NAPI_CALL(env, napi_has_named_property(env, value, "name", &hasProperty));
    NAPI_ASSERT(env, hasProperty, "Property text expected.");
    napi_get_named_property(env, value, "name", &result);
    NAPI_CALL(env, napi_typeof(env, result, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    NAPI_CALL(env, napi_get_value_string_utf8(env, result, str, STR_MAX_SIZE - 1, &strLen));
    std::string strInput = str;
    templ->SetTemplateName(strInput);

    // data?: {[key: string]: object}
    NAPI_CALL(env, napi_has_named_property(env, value, "data", &hasProperty));
    if (hasProperty) {
        napi_get_named_property(env, value, "data", &result);
        NAPI_CALL(env, napi_typeof(env, result, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        AAFwk::WantParams wantParams;
        if (!OHOS::AppExecFwk::UnwrapWantParams(env, result, wantParams)) {
            return nullptr;
        }

        std::shared_ptr<AAFwk::WantParams> data = std::make_shared<AAFwk::WantParams>(wantParams);
        templ->SetTemplateData(data);
    }

    return NapiGetNull(env);
}

napi_value Common::SetNotificationTemplateInfo(
    const napi_env &env, const std::shared_ptr<NotificationTemplate> &templ, napi_value &result)
{
    ANS_LOGI("enter");

    if (templ == nullptr) {
        ANS_LOGE("templ is null");
        return NapiGetBoolean(env, false);
    }

    napi_value value = nullptr;

    // name: string;
    napi_create_string_utf8(env, templ->GetTemplateName().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "name", value);

    // data?: {[key: string]: object};
    std::shared_ptr<AAFwk::WantParams> data = templ->GetTemplateData();
    if (data) {
        value = OHOS::AppExecFwk::WrapWantParams(env, *data);
        napi_set_named_property(env, result, "data", value);
    }

    return NapiGetBoolean(env, true);
}

napi_value Common::SetNotificationFlags(
    const napi_env &env, const std::shared_ptr<NotificationFlags> &flags, napi_value &result)
{
    ANS_LOGI("enter");

    if (flags == nullptr) {
        ANS_LOGE("flags is null");
        return NapiGetBoolean(env, false);
    }

    napi_value value = nullptr;

    int32_t soundEnabled = static_cast<int32_t>(flags->IsSoundEnabled());
    napi_create_int32(env, soundEnabled, &value);
    napi_set_named_property(env, result, "soundEnabled", value);

    int32_t vibrationEnabled = static_cast<int32_t>(flags->IsVibrationEnabled());
    napi_create_int32(env, vibrationEnabled, &value);
    napi_set_named_property(env, result, "vibrationEnabled", value);

    return NapiGetBoolean(env, true);
}
}  // namespace NotificationNapi
}  // namespace OHOS
