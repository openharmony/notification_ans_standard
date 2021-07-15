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

#include "slot.h"

namespace OHOS {
namespace NotificationNapi {
napi_value ParseParametersByAddSlot(const napi_env &env, const napi_value (&argv)[ADD_SLOT_MAX_PARA],
    const size_t &argc, NotificationSlot &slot, napi_ref &callback)
{
    napi_valuetype valuetype;
    // argv[0]: NotificationSlot
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
    if (!Common::GetNotificationSlot(env, slot, argv[0])) {
        return nullptr;
    }
    // argv[1]:callback
    if (argc >= ADD_SLOT_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &callback);
    }
    return Common::NapiGetNull(env);
}

void PaddingAsyncCallbackInfoIsByAddSlot(
    const napi_env &env, AsyncCallbackInfoAddSlot *&asynccallbackinfo, const napi_ref &callback, napi_value &promise)
{
    ANS_LOGI("PaddingAsyncCallbackInfoIs start");

    if (callback) {
        asynccallbackinfo->callback = callback;
        asynccallbackinfo->isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        asynccallbackinfo->deferred = deferred;
        asynccallbackinfo->isCallback = false;
    }
}

napi_value ParseParametersByAddSlots(const napi_env &env, const napi_value (&argv)[ADD_SLOTS_MAX_PARA],
    const size_t &argc, std::vector<NotificationSlot> &slots, napi_ref &callback)
{
    napi_valuetype valuetype;
    // argv[0]: Array<NotificationSlot>
    bool isArray = false;
    napi_is_array(env, argv[0], &isArray);
    if (isArray) {
        uint32_t length = 0;
        napi_get_array_length(env, argv[0], &length);
        for (size_t i = 0; i < length; i++) {
            napi_value nSlot;
            napi_get_element(env, argv[0], i, &nSlot);
            NAPI_CALL(env, napi_typeof(env, nSlot, &valuetype));
            NAPI_ASSERT(env, valuetype == napi_object, "Wrong argument type. Object expected.");
            NotificationSlot slot;
            if (!Common::GetNotificationSlot(env, slot, argv[0])) {
                return nullptr;
            }
            slots.emplace_back(slot);
        }
        if (slots.size() == 0) {
            ANS_LOGI("slots size is empyt");
            return nullptr;
        }
    }
    // argv[1]:callback
    if (argc >= ADD_SLOTS_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &callback);
    }
    return Common::NapiGetNull(env);
}

void PaddingAsyncCallbackInfoIsByAddSlots(
    const napi_env &env, AsyncCallbackInfoAddSlots *&asynccallbackinfo, const napi_ref &callback, napi_value &promise)
{
    ANS_LOGI("PaddingAsyncCallbackInfoIs start");

    if (callback) {
        asynccallbackinfo->callback = callback;
        asynccallbackinfo->isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        asynccallbackinfo->deferred = deferred;
        asynccallbackinfo->isCallback = false;
    }
}

napi_value ParseParametersByGetSlot(const napi_env &env, const napi_value (&argv)[GET_SLOT_MAX_PARA],
    const size_t &argc, enum NotificationConstant::SlotType &outType, napi_ref &callback)
{
    napi_valuetype valuetype;
    // argv[0]: SlotType
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");

    int slotType = 0;
    napi_get_value_int32(env, argv[0], &slotType);

    if (!Common::SlotTypeJSToC(SlotType(slotType), outType)) {
        return nullptr;
    }

    // argv[1]:callback
    if (argc >= GET_SLOT_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &callback);
    }
    return Common::NapiGetNull(env);
}

void PaddingAsyncCallbackInfoIsByGetSlot(
    const napi_env &env, AsyncCallbackInfoGetSlot *&asynccallbackinfo, const napi_ref &callback, napi_value &promise)
{
    ANS_LOGI("PaddingAsyncCallbackInfoIs start");

    if (callback) {
        asynccallbackinfo->callback = callback;
        asynccallbackinfo->isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        asynccallbackinfo->deferred = deferred;
        asynccallbackinfo->isCallback = false;
    }
}

napi_value ParseParametersByGetSlots(
    const napi_env &env, const napi_value (&argv)[GET_SLOTS_MAX_PARA], const size_t &argc, napi_ref &callback)
{
    // argv[0]:callback
    napi_valuetype valuetype;
    if (argc >= GET_SLOTS_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[0], 1, &callback);
    }
    return Common::NapiGetNull(env);
}

void PaddingAsyncCallbackInfoIsByGetSlots(
    const napi_env &env, AsyncCallbackInfoGetSlots *&asynccallbackinfo, const napi_ref &callback, napi_value &promise)
{
    ANS_LOGI("PaddingAsyncCallbackInfoIs start");

    if (callback) {
        asynccallbackinfo->callback = callback;
        asynccallbackinfo->isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        asynccallbackinfo->deferred = deferred;
        asynccallbackinfo->isCallback = false;
    }
}

napi_value ParseParametersByRemoveSlot(const napi_env &env, const napi_value (&argv)[REMOVE_SLOT_MAX_PARA],
    const size_t &argc, enum NotificationConstant::SlotType &outType, napi_ref &callback)
{
    napi_valuetype valuetype;
    // argv[0]: SlotType
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number expected.");

    int slotType = 0;
    napi_get_value_int32(env, argv[0], &slotType);

    if (!Common::SlotTypeJSToC(SlotType(slotType), outType)) {
        return nullptr;
    }

    // argv[1]:callback
    if (argc >= REMOVE_SLOT_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &callback);
    }
    return Common::NapiGetNull(env);
}

void PaddingAsyncCallbackInfoIsByRemoveSlot(
    const napi_env &env, AsyncCallbackInfoRemoveSlot *&asynccallbackinfo, const napi_ref &callback, napi_value &promise)
{
    ANS_LOGI("PaddingAsyncCallbackInfoIs start");

    if (callback) {
        asynccallbackinfo->callback = callback;
        asynccallbackinfo->isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        asynccallbackinfo->deferred = deferred;
        asynccallbackinfo->isCallback = false;
    }
}

napi_value AddSlot(napi_env env, napi_callback_info info)
{
    ANS_LOGI("AddSlot start");

    size_t argc = ADD_SLOT_MAX_PARA;
    napi_value argv[ADD_SLOT_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
    napi_ref callback = nullptr;

    NotificationSlot slot;
    if (ParseParametersByAddSlot(env, argv, argc, slot, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }

    AsyncCallbackInfoAddSlot *asynccallbackinfo =
        new AsyncCallbackInfoAddSlot{.env = env, .asyncWork = nullptr, .slot = slot};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIsByAddSlot(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "addSlot", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("AddSlot napi_create_async_work start");
            AsyncCallbackInfoAddSlot *asynccallbackinfo = (AsyncCallbackInfoAddSlot *)data;
            asynccallbackinfo->errorCode = NotificationHelper::AddNotificationSlot(asynccallbackinfo->slot);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("AddSlot napi_create_async_work end");
            AsyncCallbackInfoAddSlot *asynccallbackinfo = (AsyncCallbackInfoAddSlot *)data;

            CallbackPromiseInfo info;
            info.isCallback = asynccallbackinfo->isCallback;
            info.callback = asynccallbackinfo->callback;
            info.deferred = asynccallbackinfo->deferred;
            info.errorCode = asynccallbackinfo->errorCode;
            Common::ReturnCallbackPromise(env, info, Common::NapiGetNull(env));

            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}
napi_value AddSlots(napi_env env, napi_callback_info info)
{
    ANS_LOGI("AddSlots start");

    size_t argc = ADD_SLOTS_MAX_PARA;
    napi_value argv[ADD_SLOTS_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
    napi_ref callback = nullptr;

    std::vector<NotificationSlot> slots;
    if (ParseParametersByAddSlots(env, argv, argc, slots, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }

    AsyncCallbackInfoAddSlots *asynccallbackinfo =
        new AsyncCallbackInfoAddSlots{.env = env, .asyncWork = nullptr, .slots = slots};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIsByAddSlots(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "addSlots", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("AddSlots napi_create_async_work start");
            AsyncCallbackInfoAddSlots *asynccallbackinfo = (AsyncCallbackInfoAddSlots *)data;
            asynccallbackinfo->errorCode = NotificationHelper::AddNotificationSlots(asynccallbackinfo->slots);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("AddSlots napi_create_async_work end");
            AsyncCallbackInfoAddSlots *asynccallbackinfo = (AsyncCallbackInfoAddSlots *)data;

            CallbackPromiseInfo info;
            info.isCallback = asynccallbackinfo->isCallback;
            info.callback = asynccallbackinfo->callback;
            info.deferred = asynccallbackinfo->deferred;
            info.errorCode = asynccallbackinfo->errorCode;
            Common::ReturnCallbackPromise(env, info, Common::NapiGetNull(env));

            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}
napi_value GetSlot(napi_env env, napi_callback_info info)
{
    ANS_LOGI("GetSlot start");

    size_t argc = GET_SLOT_MAX_PARA;
    napi_value argv[GET_SLOT_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
    napi_ref callback = nullptr;

    enum NotificationConstant::SlotType outType;
    if (ParseParametersByGetSlot(env, argv, argc, outType, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }

    AsyncCallbackInfoGetSlot *asynccallbackinfo =
        new AsyncCallbackInfoGetSlot{.env = env, .asyncWork = nullptr, .outType = outType};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIsByGetSlot(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "getSlot", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetSlot napi_create_async_work start");
            AsyncCallbackInfoGetSlot *asynccallbackinfo = (AsyncCallbackInfoGetSlot *)data;

            sptr<NotificationSlot> slot;
            asynccallbackinfo->errorCode = NotificationHelper::GetNotificationSlot(asynccallbackinfo->outType, slot);
            if (slot) {
                napi_create_object(env, &asynccallbackinfo->result);
                if (!Common::SetNotificationSlot(env, *slot, asynccallbackinfo->result)) {
                    asynccallbackinfo->result = Common::NapiGetNull(env);
                }
            } else {
                asynccallbackinfo->result = Common::NapiGetNull(env);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("GetSlot napi_create_async_work end");
            AsyncCallbackInfoGetSlot *asynccallbackinfo = (AsyncCallbackInfoGetSlot *)data;

            CallbackPromiseInfo info;
            info.isCallback = asynccallbackinfo->isCallback;
            info.callback = asynccallbackinfo->callback;
            info.deferred = asynccallbackinfo->deferred;
            info.errorCode = asynccallbackinfo->errorCode;
            Common::ReturnCallbackPromise(env, info, asynccallbackinfo->result);

            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value GetSlots(napi_env env, napi_callback_info info)
{
    ANS_LOGI("GetSlots start");

    size_t argc = GET_SLOTS_MAX_PARA;
    napi_value argv[GET_SLOTS_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 0, "Wrong number of arguments");
    napi_ref callback = nullptr;

    if (ParseParametersByGetSlots(env, argv, argc, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }

    AsyncCallbackInfoGetSlots *asynccallbackinfo = new AsyncCallbackInfoGetSlots{.env = env, .asyncWork = nullptr};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIsByGetSlots(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "getSlots", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("GetSlots napi_create_async_work start");
            AsyncCallbackInfoGetSlots *asynccallbackinfo = (AsyncCallbackInfoGetSlots *)data;

            std::vector<sptr<NotificationSlot>> slots;
            asynccallbackinfo->errorCode = NotificationHelper::GetNotificationSlots(slots);

            napi_value arr;
            napi_create_array(env, &arr);
            size_t count = 0;
            for (auto vec : slots) {
                if (vec) {
                    napi_value nSlot;
                    napi_create_object(env, &nSlot);
                    if (Common::SetNotificationSlot(env, *vec, nSlot)) {
                        napi_set_element(env, arr, count, nSlot);
                        count++;
                    }
                }
            }
            ANS_LOGI("getSlots count = %{public}d", count);
            if (count == 0) {
                asynccallbackinfo->result = Common::NapiGetNull(env);
            }
            asynccallbackinfo->result = arr;
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("GetSlots napi_create_async_work end");
            AsyncCallbackInfoGetSlots *asynccallbackinfo = (AsyncCallbackInfoGetSlots *)data;

            CallbackPromiseInfo info;
            info.isCallback = asynccallbackinfo->isCallback;
            info.callback = asynccallbackinfo->callback;
            info.deferred = asynccallbackinfo->deferred;
            info.errorCode = asynccallbackinfo->errorCode;
            Common::ReturnCallbackPromise(env, info, asynccallbackinfo->result);

            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}
napi_value RemoveSlot(napi_env env, napi_callback_info info)
{
    ANS_LOGI("RemoveSlot start");

    size_t argc = REMOVE_SLOT_MAX_PARA;
    napi_value argv[REMOVE_SLOT_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
    napi_ref callback = nullptr;

    enum NotificationConstant::SlotType outType;
    if (ParseParametersByRemoveSlot(env, argv, argc, outType, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }

    AsyncCallbackInfoRemoveSlot *asynccallbackinfo =
        new AsyncCallbackInfoRemoveSlot{.env = env, .asyncWork = nullptr, .outType = outType};

    napi_value promise = 0;
    PaddingAsyncCallbackInfoIsByRemoveSlot(env, asynccallbackinfo, callback, promise);

    napi_value resourceName;
    napi_create_string_latin1(env, "removeSlot", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("removeSlot napi_create_async_work start");
            AsyncCallbackInfoRemoveSlot *asynccallbackinfo = (AsyncCallbackInfoRemoveSlot *)data;
            asynccallbackinfo->errorCode = NotificationHelper::RemoveNotificationSlot(asynccallbackinfo->outType);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("removeSlot napi_create_async_work end");
            AsyncCallbackInfoRemoveSlot *asynccallbackinfo = (AsyncCallbackInfoRemoveSlot *)data;

            CallbackPromiseInfo info;
            info.isCallback = asynccallbackinfo->isCallback;
            info.callback = asynccallbackinfo->callback;
            info.deferred = asynccallbackinfo->deferred;
            info.errorCode = asynccallbackinfo->errorCode;
            Common::ReturnCallbackPromise(env, info, Common::NapiGetNull(env));

            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    if (asynccallbackinfo->isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

}  // namespace NotificationNapi
}  // namespace OHOS