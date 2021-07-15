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
#ifndef BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_SLOT_H
#define BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_SLOT_H

#include "common.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;

const int ADD_SLOT_MAX_PARA = 2;
const int ADD_SLOTS_MAX_PARA = 2;
const int GET_SLOT_MAX_PARA = 2;
const int GET_SLOTS_MAX_PARA = 1;
const int REMOVE_SLOT_MAX_PARA = 2;

struct AsyncCallbackInfoAddSlot {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = nullptr;
    napi_deferred deferred;
    NotificationSlot slot;
    bool isCallback = false;
    int errorCode = 0;
};

struct AsyncCallbackInfoAddSlots {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = nullptr;
    napi_deferred deferred;
    std::vector<NotificationSlot> slots;
    bool isCallback = false;
    int errorCode = 0;
};

struct AsyncCallbackInfoGetSlot {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = nullptr;
    napi_deferred deferred;
    enum NotificationConstant::SlotType outType;
    bool isCallback = false;
    napi_value result = nullptr;
    int errorCode = 0;
};

struct AsyncCallbackInfoGetSlots {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = nullptr;
    napi_deferred deferred;
    bool isCallback = false;
    napi_value result = nullptr;
    int errorCode = 0;
};

struct AsyncCallbackInfoRemoveSlot {
    napi_env env;
    napi_async_work asyncWork;
    napi_ref callback = nullptr;
    napi_deferred deferred;
    enum NotificationConstant::SlotType outType;
    bool isCallback = false;
    int errorCode = 0;
};

napi_value AddSlot(napi_env env, napi_callback_info info);
napi_value AddSlots(napi_env env, napi_callback_info info);
napi_value GetSlot(napi_env env, napi_callback_info info);
napi_value GetSlots(napi_env env, napi_callback_info info);
napi_value RemoveSlot(napi_env env, napi_callback_info info);

napi_value ParseParametersByAddSlot(const napi_env &env, const napi_value (&argv)[ADD_SLOT_MAX_PARA],
    const size_t &argc, NotificationSlot &slot, napi_ref &callback);

void PaddingAsyncCallbackInfoIsByAddSlot(
    const napi_env &env, AsyncCallbackInfoAddSlot *&asynccallbackinfo, const napi_ref &callback, napi_value &promise);

napi_value ParseParametersByAddSlots(const napi_env &env, const napi_value (&argv)[ADD_SLOTS_MAX_PARA],
    const size_t &argc, std::vector<NotificationSlot> &slots, napi_ref &callback);

void PaddingAsyncCallbackInfoIsByAddSlots(
    const napi_env &env, AsyncCallbackInfoAddSlots *&asynccallbackinfo, const napi_ref &callback, napi_value &promise);

napi_value ParseParametersByGetSlot(const napi_env &env, const napi_value (&argv)[GET_SLOT_MAX_PARA],
    const size_t &argc, enum NotificationConstant::SlotType &outType, napi_ref &callback);

void PaddingAsyncCallbackInfoIsByGetSlot(
    const napi_env &env, AsyncCallbackInfoGetSlot *&asynccallbackinfo, const napi_ref &callback, napi_value &promise);

napi_value ParseParametersByGetSlots(
    const napi_env &env, const napi_value (&argv)[GET_SLOTS_MAX_PARA], const size_t &argc, napi_ref &callback);

void PaddingAsyncCallbackInfoIsByGetSlots(
    const napi_env &env, AsyncCallbackInfoGetSlots *&asynccallbackinfo, const napi_ref &callback, napi_value &promise);

napi_value ParseParametersByRemoveSlot(const napi_env &env, const napi_value (&argv)[REMOVE_SLOT_MAX_PARA],
    const size_t &argc, enum NotificationConstant::SlotType &outType, napi_ref &callback);

void PaddingAsyncCallbackInfoIsByRemoveSlot(const napi_env &env, AsyncCallbackInfoRemoveSlot *&asynccallbackinfo,
    const napi_ref &callback, napi_value &promise);

}  // namespace NotificationNapi
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_KITS_NAPI_INCLUDE_SLOT_H