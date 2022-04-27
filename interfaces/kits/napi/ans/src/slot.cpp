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

#include "slot.h"

namespace OHOS {
namespace NotificationNapi {
const int ADD_SLOT_MAX_PARA = 2;
const int ADD_SLOTS_MAX_PARA = 2;
const int SET_SLOT_AS_BUNDLE_MAX_PARA = 3;
const int GET_SLOT_MAX_PARA = 2;
const int GET_SLOT_NUM_AS_BUNDLE_MAX_PARA = 2;
const int GET_SLOTS_AS_BUNDLE_MAX_PARA = 2;
const int REMOVE_SLOT_MAX_PARA = 2;
const int32_t GET_ENABLE_SLOT_MAX_PARA = 3;
const int32_t SET_ENABLE_SLOT_MAX_PARA = 4;

struct ParametersInfoAddSlot {
    NotificationSlot slot;
    enum NotificationConstant::SlotType inType = NotificationConstant::SlotType::OTHER;
    bool isAddSlotByType = false;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoAddSlot {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    NotificationSlot slot;
    enum NotificationConstant::SlotType inType = NotificationConstant::SlotType::OTHER;
    bool isAddSlotByType = false;
    CallbackPromiseInfo info;
};

struct ParametersInfoAddSlots {
    std::vector<NotificationSlot> slots;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoAddSlots {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    std::vector<NotificationSlot> slots;
    CallbackPromiseInfo info;
};

struct ParametersInfoSetSlotByBundle {
    NotificationBundleOption option;
    std::vector<sptr<NotificationSlot>> slots;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoSetSlotByBundle {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    ParametersInfoSetSlotByBundle params;
    CallbackPromiseInfo info;
};

struct ParametersInfoGetSlot {
    enum NotificationConstant::SlotType outType = NotificationConstant::SlotType::OTHER;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoGetSlot {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    enum NotificationConstant::SlotType outType = NotificationConstant::SlotType::OTHER;
    CallbackPromiseInfo info;
    sptr<NotificationSlot> slot = nullptr;
};

struct ParametersInfoGetSlotNumByBundle {
    NotificationBundleOption option;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoGetSlotNumByBundle {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    ParametersInfoGetSlotNumByBundle params;
    CallbackPromiseInfo info;
    int num = 0;
};

struct AsyncCallbackInfoGetSlots {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    CallbackPromiseInfo info;
    std::vector<sptr<NotificationSlot>> slots;
};

struct ParametersInfoGetSlotsByBundle {
    NotificationBundleOption option;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoGetSlotsByBundle {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    ParametersInfoGetSlotsByBundle params;
    CallbackPromiseInfo info;
    std::vector<sptr<NotificationSlot>> slots;
};

struct ParametersInfoRemoveSlot {
    enum NotificationConstant::SlotType outType = NotificationConstant::SlotType::OTHER;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoRemoveSlot {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    enum NotificationConstant::SlotType outType = NotificationConstant::SlotType::OTHER;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoRemoveAllSlots {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    CallbackPromiseInfo info;
};

struct ParametersInfoEnableSlot {
    NotificationBundleOption option;
    enum NotificationConstant::SlotType outType = NotificationConstant::SlotType::OTHER;
    bool enable = false;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoInfoEnableSlot {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    ParametersInfoEnableSlot params;
    CallbackPromiseInfo info;
};

struct ParametersInfoIsEnableSlot {
    NotificationBundleOption option;
    enum NotificationConstant::SlotType outType = NotificationConstant::SlotType::OTHER;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoInfoIsEnableSlot {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    ParametersInfoIsEnableSlot params;
    bool isEnable = false;
    CallbackPromiseInfo info;
};

napi_value ParseParametersByAddSlot(const napi_env &env, const napi_callback_info &info, ParametersInfoAddSlot &paras)
{
    ANS_LOGI("enter");
    size_t argc = ADD_SLOT_MAX_PARA;
    napi_value argv[ADD_SLOT_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");

    napi_valuetype valuetype = napi_undefined;
    // argv[0]: NotificationSlot
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(
        env, (valuetype == napi_object || valuetype == napi_number), "Wrong argument type. Object or number expected.");
    if (valuetype == napi_number) {
        paras.isAddSlotByType = true;
        int slotType = 0;
        napi_get_value_int32(env, argv[PARAM0], &slotType);
        if (!Common::SlotTypeJSToC(SlotType(slotType), paras.inType)) {
            return nullptr;
        }
    } else {
        paras.isAddSlotByType = false;
        if (!Common::GetNotificationSlot(env, argv[PARAM0], paras.slot)) {
            return nullptr;
        }
    }

    // argv[1]:callback
    if (argc >= ADD_SLOT_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &paras.callback);
    }
    return Common::NapiGetNull(env);
}

napi_value ParseParametersByAddSlots(const napi_env &env, const napi_callback_info &info, ParametersInfoAddSlots &paras)
{
    ANS_LOGI("enter");
    size_t argc = ADD_SLOTS_MAX_PARA;
    napi_value argv[ADD_SLOTS_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");

    napi_valuetype valuetype = napi_undefined;
    // argv[0]: Array<NotificationSlot>
    bool isArray = false;
    napi_is_array(env, argv[PARAM0], &isArray);
    NAPI_ASSERT(env, isArray, "Wrong argument type. Array expected.");
    uint32_t length = 0;
    napi_get_array_length(env, argv[PARAM0], &length);
    NAPI_ASSERT(env, length > 0, "The array is empty.");
    for (size_t i = 0; i < length; i++) {
        napi_value nSlot = nullptr;
        napi_get_element(env, argv[PARAM0], i, &nSlot);
        NAPI_CALL(env, napi_typeof(env, nSlot, &valuetype));
        NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
        NotificationSlot slot;
        if (!Common::GetNotificationSlot(env, nSlot, slot)) {
            return nullptr;
        }
        paras.slots.emplace_back(slot);
    }

    // argv[1]:callback
    if (argc >= ADD_SLOTS_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &paras.callback);
    }
    return Common::NapiGetNull(env);
}

napi_value ParseParametersSetSlotByBundle(
    const napi_env &env, const napi_callback_info &info, ParametersInfoSetSlotByBundle &params)
{
    ANS_LOGI("enter");

    size_t argc = SET_SLOT_AS_BUNDLE_MAX_PARA;
    napi_value argv[SET_SLOT_AS_BUNDLE_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= SET_SLOT_AS_BUNDLE_MAX_PARA - 1, "Wrong number of arguments");

    // argv[0]: bundle
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    auto retValue = Common::GetBundleOption(env, argv[PARAM0], params.option);
    if (retValue == nullptr) {
        ANS_LOGE("GetBundleOption failed.");
        return nullptr;
    }

    // argv[1]: slot
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    NotificationSlot slot;
    if (!Common::GetNotificationSlot(env, argv[PARAM1], slot)) {
        return nullptr;
    }
    std::vector<NotificationSlot> slots;
    slots.emplace_back(slot);

    for (auto vec : slots) {
        sptr<NotificationSlot> slotPtr = new (std::nothrow) NotificationSlot(vec);
        if (slotPtr == nullptr) {
            ANS_LOGE("Failed to create NotificationSlot ptr");
            return nullptr;
        }
        params.slots.emplace_back(slotPtr);
    }

    // argv[2]:callback
    if (argc >= SET_SLOT_AS_BUNDLE_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM2], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value ParseParametersByGetSlot(const napi_env &env, const napi_callback_info &info, ParametersInfoGetSlot &paras)
{
    ANS_LOGI("enter");
    size_t argc = GET_SLOT_MAX_PARA;
    napi_value argv[GET_SLOT_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");

    napi_valuetype valuetype = napi_undefined;
    // argv[0]: SlotType
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
    int slotType = 0;
    napi_get_value_int32(env, argv[PARAM0], &slotType);
    if (!Common::SlotTypeJSToC(SlotType(slotType), paras.outType)) {
        return nullptr;
    }

    // argv[1]:callback
    if (argc >= GET_SLOT_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &paras.callback);
    }
    return Common::NapiGetNull(env);
}

napi_value ParseParametersGetSlotNumByBundle(
    const napi_env &env, const napi_callback_info &info, ParametersInfoGetSlotNumByBundle &params)
{
    ANS_LOGI("enter");

    size_t argc = GET_SLOT_NUM_AS_BUNDLE_MAX_PARA;
    napi_value argv[GET_SLOT_NUM_AS_BUNDLE_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= GET_SLOT_NUM_AS_BUNDLE_MAX_PARA - 1, "Wrong number of arguments");

    // argv[0]: bundle
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    auto retValue = Common::GetBundleOption(env, argv[PARAM0], params.option);
    if (retValue == nullptr) {
        ANS_LOGE("GetBundleOption failed.");
        return nullptr;
    }

    // argv[1]:callback
    if (argc >= GET_SLOT_NUM_AS_BUNDLE_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value ParseParametersGetSlotsByBundle(
    const napi_env &env, const napi_callback_info &info, ParametersInfoGetSlotsByBundle &params)
{
    ANS_LOGI("enter");

    size_t argc = GET_SLOTS_AS_BUNDLE_MAX_PARA;
    napi_value argv[GET_SLOTS_AS_BUNDLE_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= GET_SLOTS_AS_BUNDLE_MAX_PARA - 1, "Wrong number of arguments");

    // argv[0]: bundle
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    auto retValue = Common::GetBundleOption(env, argv[PARAM0], params.option);
    if (retValue == nullptr) {
        ANS_LOGE("GetBundleOption failed.");
        return nullptr;
    }

    // argv[1]:callback
    if (argc >= GET_SLOTS_AS_BUNDLE_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value ParseParametersByRemoveSlot(
    const napi_env &env, const napi_callback_info &info, ParametersInfoRemoveSlot &paras)
{
    ANS_LOGI("enter");
    size_t argc = REMOVE_SLOT_MAX_PARA;
    napi_value argv[REMOVE_SLOT_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");

    napi_valuetype valuetype = napi_undefined;
    // argv[0]: SlotType
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");
    int slotType = 0;
    napi_get_value_int32(env, argv[PARAM0], &slotType);
    if (!Common::SlotTypeJSToC(SlotType(slotType), paras.outType)) {
        return nullptr;
    }

    // argv[1]:callback
    if (argc >= REMOVE_SLOT_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[PARAM1], 1, &paras.callback);
    }
    return Common::NapiGetNull(env);
}

napi_value AddSlot(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoAddSlot paras;
    if (ParseParametersByAddSlot(env, info, paras) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoAddSlot *asynccallbackinfo = new (std::nothrow) AsyncCallbackInfoAddSlot {
        .env = env,
        .asyncWork = nullptr,
        .slot = paras.slot,
        .inType = paras.inType,
        .isAddSlotByType = paras.isAddSlotByType
    };
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, paras.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, paras.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "addSlot", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("AddSlot napi_create_async_work start");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoAddSlot *>(data);
            if (asynccallbackinfo->isAddSlotByType) {
                asynccallbackinfo->info.errorCode = NotificationHelper::AddSlotByType(asynccallbackinfo->inType);
            } else {
                asynccallbackinfo->info.errorCode = NotificationHelper::AddNotificationSlot(asynccallbackinfo->slot);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("AddSlot napi_create_async_work end");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoAddSlot *>(data);
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value AddSlots(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoAddSlots paras;
    if (ParseParametersByAddSlots(env, info, paras) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoAddSlots *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoAddSlots {.env = env, .asyncWork = nullptr, .slots = paras.slots};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, paras.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, paras.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "addSlots", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("AddSlots napi_create_async_work start");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoAddSlots *>(data);
            asynccallbackinfo->info.errorCode = NotificationHelper::AddNotificationSlots(asynccallbackinfo->slots);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("AddSlots napi_create_async_work end");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoAddSlots *>(data);
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value SetSlotByBundle(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoSetSlotByBundle params {};
    if (ParseParametersSetSlotByBundle(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoSetSlotByBundle *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoSetSlotByBundle {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "setSlotByBundle", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("SetSlotByBundle napi_create_async_work start");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoSetSlotByBundle *>(data);

            asynccallbackinfo->info.errorCode = NotificationHelper::UpdateNotificationSlots(
                asynccallbackinfo->params.option, asynccallbackinfo->params.slots);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("SetSlotByBundle napi_create_async_work end");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoSetSlotByBundle *>(data);
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}


void AsyncCompleteCallbackGetSlot(napi_env env, napi_status status, void *data)
{
    ANS_LOGI("GetSlot napi_create_async_work end");

    if (!data) {
        ANS_LOGE("Invalid async callback data");
        return;
    }

    auto asynccallbackinfo = static_cast<AsyncCallbackInfoGetSlot *>(data);
    napi_value result = Common::NapiGetNull(env);
    if (asynccallbackinfo->info.errorCode == ERR_OK) {
        if (asynccallbackinfo->slot == nullptr) {
            asynccallbackinfo->info.errorCode = ERROR;
        } else {
            napi_create_object(env, &result);
            if (!Common::SetNotificationSlot(env, *asynccallbackinfo->slot, result)) {
                asynccallbackinfo->info.errorCode = ERROR;
                result = Common::NapiGetNull(env);
            }
        }
    }
    Common::ReturnCallbackPromise(env, asynccallbackinfo->info, result);
    if (asynccallbackinfo->info.callback != nullptr) {
        napi_delete_reference(env, asynccallbackinfo->info.callback);
    }
    napi_delete_async_work(env, asynccallbackinfo->asyncWork);
    delete asynccallbackinfo;
    asynccallbackinfo = nullptr;
}

napi_value GetSlot(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoGetSlot paras;
    if (ParseParametersByGetSlot(env, info, paras) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoGetSlot *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoGetSlot {.env = env, .asyncWork = nullptr, .outType = paras.outType};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, paras.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, paras.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getSlot", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetSlot napi_create_async_work start");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoGetSlot *>(data);

            asynccallbackinfo->info.errorCode =
                NotificationHelper::GetNotificationSlot(asynccallbackinfo->outType, asynccallbackinfo->slot);
        },
        AsyncCompleteCallbackGetSlot,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value GetSlotNumByBundle(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoGetSlotNumByBundle params {};
    if (ParseParametersGetSlotNumByBundle(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoGetSlotNumByBundle *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoGetSlotNumByBundle {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getSlotNumByBundle", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetSlotNumByBundle napi_create_async_work start");
            auto asynccallbackinfo = (AsyncCallbackInfoGetSlotNumByBundle *)data;

            asynccallbackinfo->info.errorCode = NotificationHelper::GetNotificationSlotNumAsBundle(
                asynccallbackinfo->params.option, asynccallbackinfo->num);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("GetSlotNumByBundle napi_create_async_work end");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoGetSlotNumByBundle *>(data);
            if (asynccallbackinfo) {
                napi_value result = nullptr;
                napi_create_int32(env, asynccallbackinfo->num, &result);
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, result);
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

void AsyncCompleteCallbackGetSlots(napi_env env, napi_status status, void *data)
{
    ANS_LOGI("enter");
    if (!data) {
        ANS_LOGE("Invalid async callback data");
        return;
    }
    napi_value result = nullptr;
    auto asynccallbackinfo = (AsyncCallbackInfoGetSlots *)data;
    if (asynccallbackinfo->info.errorCode != ERR_OK) {
        result = Common::NapiGetNull(env);
    } else {
        napi_value arr = nullptr;
        napi_create_array(env, &arr);
        size_t count = 0;
        for (auto vec : asynccallbackinfo->slots) {
            if (!vec) {
                ANS_LOGW("Invalid NotificationSlot object ptr");
                continue;
            }
            napi_value nSlot = nullptr;
            napi_create_object(env, &nSlot);
            if (!Common::SetNotificationSlot(env, *vec, nSlot)) {
                continue;
            }
            napi_set_element(env, arr, count, nSlot);
            count++;
        }
        ANS_LOGI("getSlots count = %{public}zu", count);
        result = arr;
        if ((count == 0) && (asynccallbackinfo->slots.size() > 0)) {
            asynccallbackinfo->info.errorCode = ERROR;
            result = Common::NapiGetNull(env);
        }
    }
    Common::ReturnCallbackPromise(env, asynccallbackinfo->info, result);
    if (asynccallbackinfo->info.callback != nullptr) {
        napi_delete_reference(env, asynccallbackinfo->info.callback);
    }
    napi_delete_async_work(env, asynccallbackinfo->asyncWork);
    delete asynccallbackinfo;
    asynccallbackinfo = nullptr;
}

napi_value GetSlots(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    napi_ref callback = nullptr;
    if (Common::ParseParaOnlyCallback(env, info, callback) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoGetSlots *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoGetSlots {.env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getSlots", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetSlots napi_create_async_work start");
            auto asynccallbackinfo = (AsyncCallbackInfoGetSlots *)data;
            asynccallbackinfo->info.errorCode = NotificationHelper::GetNotificationSlots(asynccallbackinfo->slots);
        },
        AsyncCompleteCallbackGetSlots,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

void AsyncCompleteCallbackGetSlotsByBundle(napi_env env, napi_status status, void *data)
{
    ANS_LOGI("enter");
    if (!data) {
        ANS_LOGE("Invalid async callback data");
        return;
    }
    napi_value result = nullptr;
    auto asynccallbackinfo = (AsyncCallbackInfoGetSlotsByBundle *)data;
    if (asynccallbackinfo->info.errorCode != ERR_OK) {
        result = Common::NapiGetNull(env);
    } else {
        napi_value arr = nullptr;
        napi_create_array(env, &arr);
        size_t count = 0;
        for (auto vec : asynccallbackinfo->slots) {
            if (!vec) {
                ANS_LOGW("Invalid NotificationSlot object ptr");
                continue;
            }
            napi_value nSlot = nullptr;
            napi_create_object(env, &nSlot);
            if (!Common::SetNotificationSlot(env, *vec, nSlot)) {
                continue;
            }
            napi_set_element(env, arr, count, nSlot);
            count++;
        }
        ANS_LOGI("getSlots count = %{public}zu", count);
        result = arr;
        if ((count == 0) && (asynccallbackinfo->slots.size() > 0)) {
            asynccallbackinfo->info.errorCode = ERROR;
            result = Common::NapiGetNull(env);
        }
    }
    Common::ReturnCallbackPromise(env, asynccallbackinfo->info, result);
    if (asynccallbackinfo->info.callback != nullptr) {
        napi_delete_reference(env, asynccallbackinfo->info.callback);
    }
    napi_delete_async_work(env, asynccallbackinfo->asyncWork);
    delete asynccallbackinfo;
    asynccallbackinfo = nullptr;
}

napi_value GetSlotsByBundle(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoGetSlotsByBundle params {};
    if (ParseParametersGetSlotsByBundle(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoGetSlotsByBundle *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoGetSlotsByBundle {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getSlotsByBundle", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetSlotsByBundle napi_create_async_work start");
            auto asynccallbackinfo = (AsyncCallbackInfoGetSlotsByBundle *)data;

            asynccallbackinfo->info.errorCode = NotificationHelper::GetNotificationSlotsForBundle(
                asynccallbackinfo->params.option, asynccallbackinfo->slots);
        },
        AsyncCompleteCallbackGetSlotsByBundle,
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value RemoveSlot(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoRemoveSlot paras;
    if (ParseParametersByRemoveSlot(env, info, paras) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoRemoveSlot *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoRemoveSlot {.env = env, .asyncWork = nullptr, .outType = paras.outType};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, paras.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, paras.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "removeSlot", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("removeSlot napi_create_async_work start");
            auto asynccallbackinfo = (AsyncCallbackInfoRemoveSlot *)data;
            asynccallbackinfo->info.errorCode = NotificationHelper::RemoveNotificationSlot(asynccallbackinfo->outType);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("removeSlot napi_create_async_work end");
            auto asynccallbackinfo = (AsyncCallbackInfoRemoveSlot *)data;
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value RemoveAllSlots(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    napi_ref callback = nullptr;
    if (Common::ParseParaOnlyCallback(env, info, callback) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoRemoveAllSlots *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoRemoveAllSlots {.env = env, .asyncWork = nullptr};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "removeAll", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("RemoveAllSlots napi_create_async_work start");
            auto asynccallbackinfo = (AsyncCallbackInfoRemoveAllSlots *)data;
            asynccallbackinfo->info.errorCode = NotificationHelper::RemoveAllSlots();
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("RemoveAllSlots napi_create_async_work end");
            auto asynccallbackinfo = (AsyncCallbackInfoRemoveAllSlots *)data;
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value ParseParametersEnableSlot(
    const napi_env &env, const napi_callback_info &info, ParametersInfoEnableSlot &params)
{
    ANS_LOGI("enter");

    size_t argc = SET_ENABLE_SLOT_MAX_PARA;
    napi_value argv[SET_ENABLE_SLOT_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc < SET_ENABLE_SLOT_MAX_PARA - 1) {
        ANS_LOGW("Wrong number of arguments.");
        return nullptr;
    }

    // argv[0]: bundle
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_object) {
        ANS_LOGW("Wrong argument type. Object expected.");
        return nullptr;
    }
    auto retValue = Common::GetBundleOption(env, argv[PARAM0], params.option);
    if (retValue == nullptr) {
        ANS_LOGE("GetBundleOption failed.");
        return nullptr;
    }

    // argv[1]: SlotType
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
    if (valuetype != napi_number) {
        ANS_LOGW("Wrong argument type. Number expected.");
        return nullptr;
    }
    int slotType = 0;
    napi_get_value_int32(env, argv[PARAM1], &slotType);
    if (!Common::SlotTypeJSToC(SlotType(slotType), params.outType)) {
        return nullptr;
    }

    // argv[2]: enable
    NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valuetype));
    if (valuetype != napi_boolean) {
        ANS_LOGW("Wrong argument type. Bool expected.");
        return nullptr;
    }
    napi_get_value_bool(env, argv[PARAM2], &params.enable);

    // argv[3]:callback
    if (argc >= SET_ENABLE_SLOT_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM3], &valuetype));
        if (valuetype == napi_function) {
            ANS_LOGW("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, argv[PARAM3], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value EnableNotificationSlot(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoEnableSlot params {};
    if (ParseParametersEnableSlot(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoInfoEnableSlot *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoInfoEnableSlot {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "EnableNotificationSlot", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("EnableNotificationSlot napi_create_async_work start");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoInfoEnableSlot *>(data);

            asynccallbackinfo->info.errorCode = NotificationHelper::SetEnabledForBundleSlot(
                asynccallbackinfo->params.option, asynccallbackinfo->params.outType, asynccallbackinfo->params.enable);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("EnableNotificationSlot napi_create_async_work end");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoInfoEnableSlot *>(data);
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value ParseParametersIsEnableSlot(
    const napi_env &env, const napi_callback_info &info, ParametersInfoIsEnableSlot &params)
{
    ANS_LOGI("enter");

    size_t argc = GET_ENABLE_SLOT_MAX_PARA;
    napi_value argv[GET_ENABLE_SLOT_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc < GET_ENABLE_SLOT_MAX_PARA - 1) {
        ANS_LOGW("Wrong number of arguments.");
        return nullptr;
    }

    // argv[0]: bundle
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_object) {
        ANS_LOGW("Wrong argument type. Object expected.");
        return nullptr;
    }
    auto retValue = Common::GetBundleOption(env, argv[PARAM0], params.option);
    if (retValue == nullptr) {
        ANS_LOGE("GetBundleOption failed.");
        return nullptr;
    }

    // argv[1]: SlotType
    NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
    if (valuetype != napi_number) {
        ANS_LOGW("Wrong argument type. Number expected.");
        return nullptr;
    }
    int slotType = 0;
    napi_get_value_int32(env, argv[PARAM1], &slotType);
    if (!Common::SlotTypeJSToC(SlotType(slotType), params.outType)) {
        return nullptr;
    }

    // argv[2]:callback
    if (argc >= GET_ENABLE_SLOT_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valuetype));
        if (valuetype == napi_function) {
            ANS_LOGW("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, argv[PARAM2], 1, &params.callback);
    }

    return Common::NapiGetNull(env);
}

napi_value IsEnableNotificationSlot(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    ParametersInfoIsEnableSlot params {};
    if (ParseParametersIsEnableSlot(env, info, params) == nullptr) {
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoInfoIsEnableSlot *asynccallbackinfo =
        new (std::nothrow) AsyncCallbackInfoInfoIsEnableSlot {.env = env, .asyncWork = nullptr, .params = params};
    if (!asynccallbackinfo) {
        return Common::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, params.callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "IsEnableNotificationSlot", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("IsEnableNotificationSlot napi_create_async_work start");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoInfoIsEnableSlot *>(data);

            asynccallbackinfo->info.errorCode = NotificationHelper::GetEnabledForBundleSlot(
                asynccallbackinfo->params.option, asynccallbackinfo->params.outType, asynccallbackinfo->isEnable);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("IsEnableNotificationSlot napi_create_async_work end");
            auto asynccallbackinfo = static_cast<AsyncCallbackInfoInfoIsEnableSlot *>(data);
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));
                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}
}  // namespace NotificationNapi
}  // namespace OHOS