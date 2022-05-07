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
#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_COMMON_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_COMMON_H

#include "ans_log_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "notification_helper.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;

const std::int32_t STR_MAX_SIZE = 200;
const std::int32_t LONG_STR_MAX_SIZE = 1024;
const int32_t NO_ERROR = 0;
const int32_t ERROR = -1;
const int32_t PARAM0 = 0;
const int32_t PARAM1 = 1;
const int32_t PARAM2 = 2;
const int32_t PARAM3 = 3;

enum class ContentType {
    NOTIFICATION_CONTENT_BASIC_TEXT,
    NOTIFICATION_CONTENT_LONG_TEXT,
    NOTIFICATION_CONTENT_PICTURE,
    NOTIFICATION_CONTENT_CONVERSATION,
    NOTIFICATION_CONTENT_MULTILINE
};

enum class SlotType {
    UNKNOWN_TYPE = 0,
    SOCIAL_COMMUNICATION = 1,
    SERVICE_INFORMATION = 2,
    CONTENT_INFORMATION = 3,
    OTHER_TYPES = 0xFFFF,
};

enum class SlotLevel {
    LEVEL_NONE = 0,
    LEVEL_MIN = 1,
    LEVEL_LOW = 2,
    LEVEL_DEFAULT = 3,
    LEVEL_HIGH = 4,
};

enum class NotificationReason {
    CLICK_REASON_DELETE,
    CANCEL_REASON_DELETE,
    CANCEL_ALL_REASON_DELETE,
    ERROR_REASON_DELETE,
    PACKAGE_CHANGED_REASON_DELETE,
    USER_STOPPED_REASON_DELETE,
    PACKAGE_BANNED_REASON_DELETE,
    APP_CANCEL_REASON_DELETE,
    APP_CANCEL_ALL_REASON_DELETE,
    APP_CANCEL_REASON_OTHER
};

enum class SemanticActionButton {
    NONE_ACTION_BUTTON,
    REPLY_ACTION_BUTTON,
    READ_ACTION_BUTTON,
    UNREAD_ACTION_BUTTON,
    DELETE_ACTION_BUTTON,
    ARCHIVE_ACTION_BUTTON,
    MUTE_ACTION_BUTTON,
    UNMUTE_ACTION_BUTTON,
    THUMBS_UP_ACTION_BUTTON,
    THUMBS_DOWN_ACTION_BUTTON,
    CALL_ACTION_BUTTON
};

enum class InputsSource {
    FREE_FORM_INPUT,
    OPTION
};

enum class DisturbMode {
    ALLOW_UNKNOWN,
    ALLOW_ALL,
    ALLOW_PRIORITY,
    ALLOW_NONE,
    ALLOW_ALARMS
};

enum class InputEditType {
    EDIT_AUTO,
    EDIT_DISABLED,
    EDIT_ENABLED
};

enum class DoNotDisturbType {
    TYPE_NONE, TYPE_ONCE,
    TYPE_DAILY, TYPE_CLEARLY
};

enum class SourceType {
    TYPE_NORMAL = 0x00000000,
    TYPE_CONTINUOUS = 0x00000001,
    TYPE_TIMER = 0x00000002
};

enum class DeviceRemindType {
    IDLE_DONOT_REMIND,
    IDLE_REMIND,
    ACTIVE_DONOT_REMIND,
    ACTIVE_REMIND
};

enum class NotificationFlagStatus {
    TYPE_NONE,
    TYPE_OPEN,
    TYPE_CLOSE
};

struct NotificationSubscribeInfo {
    std::vector<std::string> bundleNames;
    int32_t userId = 0;
    bool hasSubscribeInfo = false;
};

struct NotificationKey {
    int32_t id {};
    std::string label {};
};

struct CallbackPromiseInfo {
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    bool isCallback = false;
    int32_t errorCode = 0;
};

class Common {
    Common();

    ~Common();

public:
    /**
     * @brief Gets a napi value that is used to represent specified bool value
     *
     * @param env Indicates the environment that the API is invoked under
     * @param isValue Indicates a bool value
     * @return Returns a napi value that is used to represent specified bool value
     */
    static napi_value NapiGetBoolean(napi_env env, const bool &isValue);

    /**
     * @brief Gets the napi value that is used to represent the null object
     *
     * @param env Indicates the environment that the API is invoked under
     * @return Returns the napi value that is used to represent the null object
     */
    static napi_value NapiGetNull(napi_env env);

    /**
     * @brief Gets the napi value that is used to represent the undefined object
     *
     * @param env Indicates the environment that the API is invoked under
     * @return Returns the napi value that is used to represent the undefined object
     */
    static napi_value NapiGetUndefined(napi_env env);

    /**
     * @brief Gets a napi value with specified error code for callback
     *
     * @param env Indicates the environment that the API is invoked under
     * @param errCode Indicates specified err code
     * @return Returns a napi value with specified error code for callback
     */
    static napi_value GetCallbackErrorValue(napi_env env, int32_t errCode);

    /**
     * @brief Pads the CallbackPromiseInfo struct
     *
     * @param env Indicates the environment that the API is invoked under
     * @param callback Indicates a napi_ref for callback
     * @param info Indicates the CallbackPromiseInfo struct to be padded
     * @param promise Indicates the promise to be created when the callback is null
     */
    static void PaddingCallbackPromiseInfo(
        const napi_env &env, const napi_ref &callback, CallbackPromiseInfo &info, napi_value &promise);

    /**
     * @brief Gets the returned result by the CallbackPromiseInfo struct
     *
     * @param env Indicates the environment that the API is invoked under
     * @param info Indicates the CallbackPromiseInfo struct
     * @param result Indicates the returned result
     */
    static void ReturnCallbackPromise(const napi_env &env, const CallbackPromiseInfo &info, const napi_value &result);

    /**
     * @brief Calls the callback with the result and error code
     *
     * @param env Indicates the environment that the API is invoked under
     * @param callbackIn Indicates the callback to be called
     * @param errCode Indicates the error code returned by the callback
     * @param result Indicates the result returned by the callback
     */
    static void SetCallback(
        const napi_env &env, const napi_ref &callbackIn, const int32_t &errorCode, const napi_value &result);

    /**
     * @brief Calls the callback with the result
     *
     * @param env Indicates the environment that the API is invoked under
     * @param callbackIn Indicates the callback to be called
     * @param result Indicates the result returned by the callback
     */
    static void SetCallback(
        const napi_env &env, const napi_ref &callbackIn, const napi_value &result);

    /**
     * @brief Processes the promise with the result and error code
     *
     * @param env Indicates the environment that the API is invoked under
     * @param deferred Indicates the deferred object whose associated promise to resolve
     * @param errorCode Indicates the error code returned by the callback
     * @param result Indicates the result returned by the callback
     */
    static void SetPromise(
        const napi_env &env, const napi_deferred &deferred, const int32_t &errorCode, const napi_value &result);

    /**
     * @brief Gets the returned result by the callback when an error occurs
     *
     * @param env Indicates the environment that the API is invoked under
     * @param callback Indicates a napi_ref for callback
     * @return Returns the null object
     */
    static napi_value JSParaError(const napi_env &env, const napi_ref &callback);

    /**
     * @brief Parses a single parameter for callback
     *
     * @param env Indicates the environment that the API is invoked under
     * @param info Indicates the callback info passed into the callback function
     * @param callback Indicates the napi_ref for the callback parameter
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value ParseParaOnlyCallback(const napi_env &env, const napi_callback_info &info, napi_ref &callback);

    /**
     * @brief Sets a js object by specified Notification object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param notification Indicates a Notification object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotification(
        const napi_env &env, const OHOS::Notification::Notification *notification, napi_value &result);

    /**
     * @brief Sets a js object by specified NotificationRequest object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param request Indicates a NotificationRequest object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationRequest(
        const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result);
    
    /**
     * @brief Sets a js object by the string obejcts of specified NotificationRequest object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param request Indicates a NotificationRequest object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationRequestByString(
        const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result);
    
    /**
     * @brief Sets a js object by the number obejcts of specified NotificationRequest object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param request Indicates a NotificationRequest object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationRequestByNumber(
        const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result);
    
    /**
     * @brief Sets a js object by the bool obejcts of specified NotificationRequest object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param request Indicates a NotificationRequest object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationRequestByBool(
        const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result);
    
    /**
     * @brief Sets a js object by the WantAgent obejct of specified NotificationRequest object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param request Indicates a NotificationRequest object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationRequestByWantAgent(
        const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result);
    
    /**
     * @brief Sets a js object by the PixelMap obejct of specified NotificationRequest object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param request Indicates a NotificationRequest object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationRequestByPixelMap(
        const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result);
    
    /**
     * @brief Sets a js object by the custom obejcts of specified NotificationRequest object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param request Indicates a NotificationRequest object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationRequestByCustom(
        const napi_env &env, const OHOS::Notification::NotificationRequest *request, napi_value &result);

    /**
     * @brief Sets a js object by the Distributed Options object of specified Notification object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param notification Indicates a Notification object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationByDistributedOptions(
        const napi_env &env, const OHOS::Notification::Notification *notification, napi_value &result);

    /**
     * @brief Sets a js object by specified NotificationSortingMap object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param sortingMap Indicates a NotificationSortingMap object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationSortingMap(
        const napi_env &env, const std::shared_ptr<NotificationSortingMap> &sortingMap, napi_value &result);

    /**
     * @brief Sets a js object by specified NotificationSorting object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param sorting Indicates a NotificationSorting object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationSorting(
        const napi_env &env, const NotificationSorting &sorting, napi_value &result);

    /**
     * @brief Sets a js object by specified NotificationSlot object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param slot Indicates a NotificationSlot object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationSlot(const napi_env &env, const NotificationSlot &slot, napi_value &result);

    /**
     * @brief Sets a js object by specified NotificationContent object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param content Indicates a NotificationContent object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationContent(
        const napi_env &env, const std::shared_ptr<NotificationContent> &content, napi_value &result);
    
    /**
     * @brief Sets a js object by the object of specified type in specified NotificationContent object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param type Indicates the content type
     * @param content Indicates a NotificationContent object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationContentDetailed(const napi_env &env, const ContentType &type,
        const std::shared_ptr<NotificationContent> &content, napi_value &result);

    /**
     * @brief Sets a js NotificationBasicContent object by specified NotificationBasicContent object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param basicContent Indicates a NotificationBasicContent object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationBasicContent(
        const napi_env &env, const NotificationBasicContent *basicContent, napi_value &result);

    /**
     * @brief Sets a js NotificationLongTextContent object by specified NotificationBasicContent object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param basicContent Indicates a NotificationBasicContent object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationLongTextContent(
        const napi_env &env, NotificationBasicContent *basicContent, napi_value &result);

    /**
     * @brief Sets a js NotificationPictureContent object by specified NotificationBasicContent object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param basicContent Indicates a NotificationBasicContent object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationPictureContent(
        const napi_env &env, NotificationBasicContent *basicContent, napi_value &result);

    /**
     * @brief Sets a js NotificationConversationalContent object by specified NotificationBasicContent object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param basicContent Indicates a NotificationBasicContent object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationConversationalContent(const napi_env &env,
        NotificationBasicContent *basicContent, napi_value &result);

    /**
     * @brief Sets a js NotificationMultiLineContent object by specified NotificationBasicContent object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param basicContent Indicates a NotificationBasicContent object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationMultiLineContent(
        const napi_env &env, NotificationBasicContent *basicContent, napi_value &result);

    /**
     * @brief Sets a js object by specified MessageUser object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param messageUser Indicates a MessageUser object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetMessageUser(const napi_env &env, const MessageUser &messageUser, napi_value &result);

    /**
     * @brief Sets a js object by specified NotificationConversationalContent object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param conversationalContent Indicates a NotificationConversationalContent object to be converted
     * @param arr Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetConversationalMessages(const napi_env &env,
        const OHOS::Notification::NotificationConversationalContent *conversationalContent, napi_value &arr);

    /**
     * @brief Sets a js object by specified NotificationConversationalMessage object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param conversationalMessage Indicates a NotificationConversationalMessage object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetConversationalMessage(const napi_env &env,
        const std::shared_ptr<NotificationConversationalMessage> &conversationalMessage, napi_value &result);

    /**
     * @brief Sets a js object by specified NotificationActionButton object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param actionButton Indicates a NotificationActionButton object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationActionButton(
        const napi_env &env, const std::shared_ptr<NotificationActionButton> &actionButton, napi_value &result);
    
    /**
     * @brief Sets a js object by the extra objects of specified NotificationActionButton object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param actionButton Indicates a NotificationActionButton object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationActionButtonByExtras(
        const napi_env &env, const std::shared_ptr<NotificationActionButton> &actionButton, napi_value &result);
    
    /**
     * @brief Sets a js object by specified NotificationUserInput object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param userInput Indicates a NotificationUserInput object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationActionButtonByUserInput(
        const napi_env &env, const std::shared_ptr<NotificationUserInput> &userInput, napi_value &result);

    /**
     * @brief Sets a js object by specified NotificationDoNotDisturbDate object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param date Indicates a NotificationDoNotDisturbDate object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetDoNotDisturbDate(
        const napi_env &env, const NotificationDoNotDisturbDate &date, napi_value &result);

    /**
     * @brief Sets a js object by specified EnabledNotificationCallbackData object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param date Indicates a EnabledNotificationCallbackData object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetEnabledNotificationCallbackData(const napi_env &env,
        const EnabledNotificationCallbackData &data, napi_value &result);

    /**
     * @brief Gets a NotificationSubscribeInfo object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param result Indicates a NotificationSubscribeInfo object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationSubscriberInfo(
        const napi_env &env, const napi_value &value, NotificationSubscribeInfo &result);

    /**
     * @brief Gets a NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param result Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationRequest(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets a NotificationRequest object by number type from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationRequestByNumber(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets a NotificationRequest object by string type from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationRequestByString(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets a NotificationRequest object by bool type from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationRequestByBool(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets a NotificationRequest object by custom type from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationRequestByCustom(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the id of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationId(const napi_env &env, const napi_value &value, NotificationRequest &request);
    
    /**
     * @brief Gets the slot type of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationSlotType(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the isOngoing flag of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationIsOngoing(
        const napi_env &env, const napi_value &value, NotificationRequest &request);
    
    /**
     * @brief Gets the isUnremovable flag of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationIsUnremovable(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the delivery time of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationDeliveryTime(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the tapDismissed flag of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationtapDismissed(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the extra information of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationExtraInfo(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the group name of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationGroupName(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the removal WantAgent object of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationRemovalWantAgent(
        const napi_env &env, const napi_value &value, NotificationRequest &request);
    
    /**
     * @brief Gets the max screen WantAgent object of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationMaxScreenWantAgent(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the auto deleted time of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationAutoDeletedTime(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the classification of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationClassification(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the color of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationColor(const napi_env &env, const napi_value &value, NotificationRequest &request);
    
    /**
     * @brief Gets the colorEnabled flag of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationColorEnabled(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the isAlertOnce flag of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationIsAlertOnce(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the isStopwatch flag of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationIsStopwatch(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the isCountDown flag of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationIsCountDown(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the status bar text of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationStatusBarText(
        const napi_env &env, const napi_value &value, NotificationRequest &request);
    
    /**
     * @brief Gets the label of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationLabel(const napi_env &env, const napi_value &value, NotificationRequest &request);
    
    /**
     * @brief Gets the badge icon style of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationBadgeIconStyle(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the showDeliveryTime flag of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationShowDeliveryTime(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the content of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationContent(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the WantAgent object of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationWantAgent(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets a NotificationSlot object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param slot Indicates a NotificationSlot object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationSlot(
        const napi_env &env, const napi_value &value, NotificationSlot &slot);

    /**
     * @brief Gets the string objects of NotificationSlot object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param slot Indicates a NotificationSlot object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationSlotByString(
        const napi_env &env, const napi_value &value, NotificationSlot &slot);

    /**
     * @brief Gets the bool objects of NotificationSlot object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param slot Indicates a NotificationSlot object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationSlotByBool(
        const napi_env &env, const napi_value &value, NotificationSlot &slot);

    /**
     * @brief Gets the number objects of NotificationSlot object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param slot Indicates a NotificationSlot object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationSlotByNumber(
        const napi_env &env, const napi_value &value, NotificationSlot &slot);

    /**
     * @brief Gets the vibration of NotificationSlot object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param slot Indicates a NotificationSlot object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationSlotByVibration(
        const napi_env &env, const napi_value &value, NotificationSlot &slot);

    /**
     * @brief Gets the action buttons of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationActionButtons(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets a NotificationActionButton object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param actionButton Indicates a js object to be converted
     * @param pActionButton Indicates a NotificationActionButton object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationActionButtonsDetailed(
        const napi_env &env, const napi_value &actionButton, std::shared_ptr<NotificationActionButton> &pActionButton);
    
    /**
     * @brief Gets the basic information of NotificationActionButton object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param actionButton Indicates a js object to be converted
     * @param pActionButton Indicates a NotificationActionButton object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationActionButtonsDetailedBasicInfo(
        const napi_env &env, const napi_value &actionButton, std::shared_ptr<NotificationActionButton> &pActionButton);
    
    /**
     * @brief Gets the extras of NotificationActionButton object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param actionButton Indicates a js object to be converted
     * @param pActionButton Indicates a NotificationActionButton object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationActionButtonsDetailedByExtras(
        const napi_env &env, const napi_value &actionButton, std::shared_ptr<NotificationActionButton> &pActionButton);
    
    /**
     * @brief Gets the user input of NotificationActionButton object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param actionButton Indicates a js object to be converted
     * @param pActionButton Indicates a NotificationActionButton object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationUserInput(
        const napi_env &env, const napi_value &actionButton, std::shared_ptr<NotificationActionButton> &pActionButton);
    
    /**
     * @brief Gets the input key of NotificationUserInput object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param userInputResult Indicates a js object to be converted
     * @param userInput Indicates a NotificationUserInput object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationUserInputByInputKey(
        const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput);
    
    /**
     * @brief Gets the tag of NotificationUserInput object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param userInputResult Indicates a js object to be converted
     * @param userInput Indicates a NotificationUserInput object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationUserInputByTag(
        const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput);
    
    /**
     * @brief Gets the options of NotificationUserInput object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param userInputResult Indicates a js object to be converted
     * @param userInput Indicates a NotificationUserInput object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationUserInputByOptions(
        const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput);
    
    /**
     * @brief Gets the permit mime types of NotificationUserInput object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param userInputResult Indicates a js object to be converted
     * @param userInput Indicates a NotificationUserInput object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationUserInputByPermitMimeTypes(
        const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput);
    
    /**
     * @brief Gets the permit free from input of NotificationUserInput object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param userInputResult Indicates a js object to be converted
     * @param userInput Indicates a NotificationUserInput object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationUserInputByPermitFreeFormInput(
        const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput);
    
    /**
     * @brief Gets the edit type of NotificationUserInput object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param userInputResult Indicates a js object to be converted
     * @param userInput Indicates a NotificationUserInput object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationUserInputByEditType(
        const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput);
    
    /**
     * @brief Gets the additional data of NotificationUserInput object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param userInputResult Indicates a js object to be converted
     * @param userInput Indicates a NotificationUserInput object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationUserInputByAdditionalData(
        const napi_env &env, const napi_value &userInputResult, std::shared_ptr<NotificationUserInput> &userInput);

    /**
     * @brief Gets the small icon of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationSmallIcon(
        const napi_env &env, const napi_value &value, NotificationRequest &request);
    
    /**
     * @brief Gets the large icon of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationLargeIcon(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the distributed options of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationRequestDistributedOptions(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the isDistributed flag of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationIsDistributed(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the devices that support display of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationSupportDisplayDevices(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets the devices that support operation of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationSupportOperateDevices(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets a content type of notification from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param type Indicates a the content type of notification from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationContentType(const napi_env &env, const napi_value &result, int32_t &type);
    
    /**
     * @brief Gets a basic content of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationBasicContent(
        const napi_env &env, const napi_value &result, NotificationRequest &request);

    /**
     * @brief Gets a NotificationBasicContent object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param contentResult Indicates a js object to be converted
     * @param basicContent Indicates a NotificationBasicContent object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationBasicContentDetailed(
        const napi_env &env, const napi_value &contentResult, std::shared_ptr<NotificationBasicContent> basicContent);

    /**
     * @brief Gets a long-text content of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationLongTextContent(
        const napi_env &env, const napi_value &result, NotificationRequest &request);

    /**
     * @brief Gets a NotificationLongTextContent object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param contentResult Indicates a js object to be converted
     * @param longContent Indicates a NotificationLongTextContent object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationLongTextContentDetailed(
        const napi_env &env, const napi_value &contentResult,
        std::shared_ptr<OHOS::Notification::NotificationLongTextContent> &longContent);

    /**
     * @brief Gets a picture content of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param result Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationPictureContent(
        const napi_env &env, const napi_value &result, NotificationRequest &request);

    /**
     * @brief Gets a NotificationPictureContent object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param contentResult Indicates a js object to be converted
     * @param pictureContent Indicates a NotificationPictureContent object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationPictureContentDetailed(
        const napi_env &env, const napi_value &contentResult,
        std::shared_ptr<OHOS::Notification::NotificationPictureContent> &pictureContent);

    /**
     * @brief Gets a conversational content of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param result Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationConversationalContent(
        const napi_env &env, const napi_value &result, NotificationRequest &request);

    /**
     * @brief Gets the user of NotificationConversationalContent object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param contentResult Indicates a js object to be converted
     * @param user Indicates a MessageUser object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationConversationalContentByUser(
        const napi_env &env, const napi_value &contentResult, MessageUser &user);

    /**
     * @brief Gets the title of NotificationConversationalContent object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param contentResult Indicates a js object to be converted
     * @param conversationalContent Indicates a NotificationConversationalContent object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationConversationalContentTitle(
        const napi_env &env, const napi_value &contentResult,
        std::shared_ptr<OHOS::Notification::NotificationConversationalContent> &conversationalContent);
    
    /**
     * @brief Gets the group of NotificationConversationalContent object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param contentResult Indicates a js object to be converted
     * @param conversationalContent Indicates a NotificationConversationalContent object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationConversationalContentGroup(
        const napi_env &env, const napi_value &contentResult,
        std::shared_ptr<OHOS::Notification::NotificationConversationalContent> &conversationalContent);
    
    /**
     * @brief Gets the messages of NotificationConversationalContent object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param contentResult Indicates a js object to be converted
     * @param conversationalContent Indicates a NotificationConversationalContent object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationConversationalContentMessages(
        const napi_env &env, const napi_value &contentResult,
        std::shared_ptr<OHOS::Notification::NotificationConversationalContent> &conversationalContent);
    
    /**
     * @brief Gets a NotificationConversationalMessage object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param conversationalMessage Indicates a js object to be converted
     * @param message Indicates a NotificationConversationalMessage object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetConversationalMessage(
        const napi_env &env, const napi_value &conversationalMessage,
        std::shared_ptr<NotificationConversationalMessage> &message);
    
    /**
     * @brief Gets the basic information of NotificationConversationalMessage object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param conversationalMessage Indicates a js object to be converted
     * @param message Indicates a NotificationConversationalMessage object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetConversationalMessageBasicInfo(
        const napi_env &env, const napi_value &conversationalMessage,
        std::shared_ptr<NotificationConversationalMessage> &message);

    /**
     * @brief Gets the other information of NotificationConversationalMessage object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param conversationalMessage Indicates a js object to be converted
     * @param message Indicates a NotificationConversationalMessage object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetConversationalMessageOtherInfo(
        const napi_env &env, const napi_value &conversationalMessage,
        std::shared_ptr<NotificationConversationalMessage> &message);

    /**
     * @brief Gets a MessageUser object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param result Indicates a js object to be converted
     * @param messageUser Indicates a MessageUser object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetMessageUser(const napi_env &env, const napi_value &result, MessageUser &messageUser);
    
    /**
     * @brief Gets a MessageUser object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param result Indicates a js object to be converted
     * @param messageUser Indicates a MessageUser object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetMessageUserByString(const napi_env &env, const napi_value &result, MessageUser &messageUser);
    
    /**
     * @brief Gets the bool objects of MessageUser object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param result Indicates a js object to be converted
     * @param messageUser Indicates a MessageUser object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetMessageUserByBool(const napi_env &env, const napi_value &result, MessageUser &messageUser);
    
    /**
     * @brief Gets the custom objects of MessageUser object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param result Indicates a js object to be converted
     * @param messageUser Indicates a MessageUser object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetMessageUserByCustom(const napi_env &env, const napi_value &result, MessageUser &messageUser);

    /**
     * @brief Gets the multi-line content of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param result Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationMultiLineContent(
        const napi_env &env, const napi_value &result, NotificationRequest &request);

    /**
     * @brief Gets the lines of NotificationMultiLineContent object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param result Indicates a js object to be converted
     * @param multiLineContent Indicates a NotificationMultiLineContent object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationMultiLineContentLines(const napi_env &env, const napi_value &result,
        std::shared_ptr<OHOS::Notification::NotificationMultiLineContent> &multiLineContent);

    /**
     * @brief Gets a NotificationBundleOption object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param option Indicates a NotificationBundleOption object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetBundleOption(const napi_env &env, const napi_value &value, NotificationBundleOption &option);

    /**
     * @brief Gets a NotificationKey object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param key Indicates a NotificationKey object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationKey(const napi_env &env, const napi_value &value, NotificationKey &key);

    /**
     * @brief Converts content type from js to native
     *
     * @param inType Indicates a js ContentType object
     * @param outType Indicates a NotificationContent object
     * @return Returns true if success, returns false otherwise
     */
    static bool ContentTypeJSToC(const ContentType &inType, NotificationContent::Type &outType);

    /**
     * @brief Converts content type from native to js
     *
     * @param inType Indicates a NotificationContent object
     * @param outType Indicates a js ContentType object
     * @return Returns true if success, returns false otherwise
     */
    static bool ContentTypeCToJS(const NotificationContent::Type &inType, ContentType &outType);

    /**
     * @brief Converts slot type from js to native
     *
     * @param inType Indicates a native SlotType object
     * @param outType Indicates a js SlotType object
     * @return Returns true if success, returns false otherwise
     */
    static bool SlotTypeJSToC(const SlotType &inType, NotificationConstant::SlotType &outType);

    /**
     * @brief Converts slot type from native to js
     *
     * @param inType Indicates a js SlotType object
     * @param outType Indicates a native SlotType object
     * @return Returns true if success, returns false otherwise
     */
    static bool SlotTypeCToJS(const NotificationConstant::SlotType &inType, SlotType &outType);

    /**
     * @brief Converts slot level from js to native
     *
     * @param inType Indicates a native SlotLevel object
     * @param outType Indicates a js NotificationLevel object
     * @return Returns true if success, returns false otherwise
     */
    static bool SlotLevelJSToC(const SlotLevel &inLevel, NotificationSlot::NotificationLevel &outLevel);

    /**
     * @brief Converts slot level from native to js
     *
     * @param inType Indicates a js NotificationLevel object
     * @param outType Indicates a native SlotLevel object
     * @return Returns true if success, returns false otherwise
     */
    static bool SlotLevelCToJS(const NotificationSlot::NotificationLevel &inLevel, SlotLevel &outLevel);

    /**
     * @brief Converts reason type from native to js
     *
     * @param inType Indicates a native reason type
     * @param outType Indicates a js reason type
     * @return Returns true if success, returns false otherwise
     */
    static bool ReasonCToJS(const int32_t &inType, int32_t &outType);

    /**
     * @brief Converts do-not-disturb type from js to native
     *
     * @param inType Indicates a js DoNotDisturbType object
     * @param outType Indicates a native DoNotDisturbType object
     * @return Returns true if success, returns false otherwise
     */
    static bool DoNotDisturbTypeJSToC(const DoNotDisturbType &inType, NotificationConstant::DoNotDisturbType &outType);

    /**
     * @brief Converts do-not-disturb type from native to js
     *
     * @param inType Indicates a native DoNotDisturbType object
     * @param outType Indicates a js DoNotDisturbType object
     * @return Returns true if success, returns false otherwise
     */
    static bool DoNotDisturbTypeCToJS(const NotificationConstant::DoNotDisturbType &inType, DoNotDisturbType &outType);

    /**
     * @brief Converts remind type from native to js
     *
     * @param inType Indicates a native RemindType object
     * @param outType Indicates a js DeviceRemindType object
     * @return Returns true if success, returns false otherwise
     */
    static bool DeviceRemindTypeCToJS(const NotificationConstant::RemindType &inType, DeviceRemindType &outType);

    /**
     * @brief Converts source type from native to js
     *
     * @param inType Indicates a native SourceType object
     * @param outType Indicates a js SourceType object
     * @return Returns true if success, returns false otherwise
     */
    static bool SourceTypeCToJS(const NotificationConstant::SourceType &inType, SourceType &outType);

    /**
     * @brief Creates a js object from specified WantAgent object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param agent Indicates specified WantAgent object
     * @return Returns a js object from specified WantAgent object
     */
    static napi_value CreateWantAgentByJS(const napi_env &env,
        const std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> &agent);

    /**
     * @brief Gets the template of NotificationRequest object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param request Indicates a NotificationRequest object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationTemplate(
        const napi_env &env, const napi_value &value, NotificationRequest &request);

    /**
     * @brief Gets a NotificationTemplate object from specified js object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param value Indicates a js object to be converted
     * @param templ Indicates a NotificationTemplate object from specified js object
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value GetNotificationTemplateInfo(const napi_env &env, const napi_value &value,
        std::shared_ptr<NotificationTemplate> &templ);

    /**
     * @brief Sets a js object by specified NotificationTemplate object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param templ Indicates a NotificationTemplate object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationTemplateInfo(
        const napi_env &env, const std::shared_ptr<NotificationTemplate> &templ, napi_value &result);

    /**
     * @brief Sets a js object by specified NotificationFlags object
     *
     * @param env Indicates the environment that the API is invoked under
     * @param flags Indicates a NotificationFlags object to be converted
     * @param result Indicates a js object to be set
     * @return Returns the null object if success, returns the null value otherwise
     */
    static napi_value SetNotificationFlags(
        const napi_env &env, const std::shared_ptr<NotificationFlags> &flags, napi_value &result);

private:
    static const int32_t ARGS_ONE = 1;
    static const int32_t ARGS_TWO = 2;
    static const int32_t ONLY_CALLBACK_MAX_PARA = 1;
    static const int32_t ONLY_CALLBACK_MIN_PARA = 0;
    static std::set<std::shared_ptr<AbilityRuntime::WantAgent::WantAgent>> wantAgent_;
};
}  // namespace NotificationNapi
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_COMMON_H