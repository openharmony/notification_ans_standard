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

#include "napi_want_agent.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <uv.h>

#include "hilog_wrapper.h"
#include "ohos/aafwk/base/base_interfaces.h"
#include "ohos/aafwk/base/array_wrapper.h"
#include "ohos/aafwk/base/base_object.h"
#include "ohos/aafwk/base/bool_wrapper.h"
#include "ohos/aafwk/base/byte_wrapper.h"
#include "ohos/aafwk/base/short_wrapper.h"
#include "ohos/aafwk/base/int_wrapper.h"
#include "ohos/aafwk/base/long_wrapper.h"
#include "ohos/aafwk/base/float_wrapper.h"
#include "ohos/aafwk/base/double_wrapper.h"
#include "ohos/aafwk/base/string_wrapper.h"
#include "ohos/aafwk/base/zchar_wrapper.h"

#include "want_wrapper.h"
#include "want_agent_helper.h"

namespace OHOS {

constexpr int32_t BUSINESS_ERROR_CODE_OK = 0;

TriggerCompleteCallBack::TriggerCompleteCallBack()
{}

TriggerCompleteCallBack::~TriggerCompleteCallBack()
{}

void TriggerCompleteCallBack::SetCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    triggerCompleteInfo_.env = env;
    triggerCompleteInfo_.ref = ref;
}

void TriggerCompleteCallBack::SetWantAgentInstance(const std::shared_ptr<Notification::WantAgent::WantAgent> &wantAgent)
{
    triggerCompleteInfo_.wantAgent = wantAgent;
}

void TriggerCompleteCallBack::OnSendFinished(
    const AAFwk::Want &want, int resultCode, const std::string &resultData, const AAFwk::WantParams &resultExtras)
{
    HILOG_INFO("TriggerCompleteCallBack::OnSendFinished start");
    if (triggerCompleteInfo_.ref == nullptr) {
        HILOG_INFO("triggerCompleteInfo_ CallBack is nullptr");
        return;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(triggerCompleteInfo_.env, &loop);
#endif  // NAPI_VERSION >= 2
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        HILOG_INFO("uv_work_t instance is nullptr");
        return;
    }
    TriggerReceiveDataWorker *dataWorker = new (std::nothrow) TriggerReceiveDataWorker();
    if (dataWorker == nullptr) {
        HILOG_INFO("TriggerReceiveDataWorker instance is nullptr");
        return;
    }
    dataWorker->want = want;
    dataWorker->resultCode = resultCode;
    dataWorker->resultData = resultData;
    dataWorker->resultExtras = resultExtras;
    dataWorker->env = triggerCompleteInfo_.env;
    dataWorker->ref = triggerCompleteInfo_.ref;
    dataWorker->wantAgent = triggerCompleteInfo_.wantAgent;
    work->data = (void *)dataWorker;
    uv_queue_work(loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            TriggerReceiveDataWorker *dataWorkerData = (TriggerReceiveDataWorker *)work->data;
            if (dataWorkerData == nullptr) {
                HILOG_INFO("TriggerReceiveDataWorker instance(uv_work_t) is nullptr");
                return;
            }
            napi_value result[2] = {0};
            napi_value callback;
            napi_value undefined;
            napi_value callResult = 0;

            result[0] = GetCallbackErrorResult(dataWorkerData->env, BUSINESS_ERROR_CODE_OK);
            napi_create_object(dataWorkerData->env, &result[1]);
            // wrap wantAgent
            napi_value jsWantAgent = nullptr;
            napi_wrap(dataWorkerData->env,
                jsWantAgent,
                (void *)dataWorkerData->wantAgent.get(),
                [](napi_env env, void *data, void *hint) {},
                nullptr,
                nullptr);
            napi_set_named_property(dataWorkerData->env, result[1], "wantAgent", jsWantAgent);
            //  wrap want
            napi_value jsWant;
            jsWant = WrapWant(dataWorkerData->want, dataWorkerData->env);
            napi_set_named_property(dataWorkerData->env, result[1], "want", jsWant);
            // wrap finalCode
            napi_value jsFinalCode;
            napi_create_int32(dataWorkerData->env, dataWorkerData->resultCode, &jsFinalCode);
            napi_set_named_property(dataWorkerData->env, result[1], "finalCode", jsFinalCode);
            // wrap finalData
            napi_value jsFinalData;
            napi_create_string_utf8(
                dataWorkerData->env, dataWorkerData->resultData.c_str(), NAPI_AUTO_LENGTH, &jsFinalData);
            napi_set_named_property(dataWorkerData->env, result[1], "finalData", jsFinalCode);
            // wrap extraInfo
            napi_value jsExtraInfo;
            jsExtraInfo = WrapWantParam(dataWorkerData->resultExtras, dataWorkerData->env);
            napi_set_named_property(dataWorkerData->env, result[1], "extraInfo", jsExtraInfo);

            napi_get_undefined(dataWorkerData->env, &undefined);
            napi_get_reference_value(dataWorkerData->env, dataWorkerData->ref, &callback);
            napi_call_function(dataWorkerData->env, undefined, callback, 2, &result[0], &callResult);

            delete dataWorkerData;
            dataWorkerData = nullptr;
            delete work;
            work = nullptr;
        });

    HILOG_INFO("TriggerCompleteCallBack::OnSendFinished end");
}
napi_value WantAgentInit(napi_env env, napi_value exports)
{
    HILOG_INFO("napi_moudule Init start...");
    napi_property_descriptor desc[] = {DECLARE_NAPI_FUNCTION("getBundleName", NAPI_GetBundleName),
        DECLARE_NAPI_FUNCTION("getUid", NAPI_GetUid),
        DECLARE_NAPI_FUNCTION("cancel", NAPI_Cancel),
        DECLARE_NAPI_FUNCTION("trigger", NAPI_Trigger),
        DECLARE_NAPI_FUNCTION("equal", NAPI_Equal),
        DECLARE_NAPI_FUNCTION("getWant", NAPI_GetWant),
        DECLARE_NAPI_FUNCTION("getWantAgent", NAPI_GetWantAgent)};

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    HILOG_INFO("napi_moudule Init end...");
    return exports;
}

void SetNamedPropertyByInteger(napi_env env, napi_value dstObj, int32_t objName, const char *propName)
{
    napi_value prop = nullptr;
    if (napi_create_int32(env, objName, &prop) == napi_ok) {
        napi_set_named_property(env, dstObj, propName, prop);
    }
}

napi_value WantAgentFlagsInit(napi_env env, napi_value exports)
{
    HILOG_INFO("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, 0, "ONE_TIME_FLAG");
    SetNamedPropertyByInteger(env, obj, 1, "NO_BUILD_FLAG");
    SetNamedPropertyByInteger(env, obj, 2, "CANCEL_PRESENT_FLAG");
    SetNamedPropertyByInteger(env, obj, 3, "UPDATE_PRESENT_FLAG");
    SetNamedPropertyByInteger(env, obj, 4, "CONSTANT_FLAG");
    SetNamedPropertyByInteger(env, obj, 5, "REPLACE_ELEMENT");
    SetNamedPropertyByInteger(env, obj, 6, "REPLACE_ACTION");
    SetNamedPropertyByInteger(env, obj, 7, "REPLACE_URI");
    SetNamedPropertyByInteger(env, obj, 8, "REPLACE_ENTITIES");
    SetNamedPropertyByInteger(env, obj, 9, "REPLACE_BUNDLE");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("Flags", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value WantAgentOperationTypeInit(napi_env env, napi_value exports)
{
    HILOG_INFO("%{public}s, called", __func__);

    napi_value obj = nullptr;
    napi_create_object(env, &obj);

    SetNamedPropertyByInteger(env, obj, 0, "UNKNOWN_TYPE");
    SetNamedPropertyByInteger(env, obj, 1, "START_ABILITY");
    SetNamedPropertyByInteger(env, obj, 2, "START_ABILITIES");
    SetNamedPropertyByInteger(env, obj, 3, "START_SERVICE");
    SetNamedPropertyByInteger(env, obj, 4, "SEND_COMMON_EVENT");
    SetNamedPropertyByInteger(env, obj, 5, "START_FOREGROUND_SERVICE");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("OperationType", obj),
    };

    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    return exports;
}

napi_value NAPI_GetBundleNameWrap(
    napi_env env, napi_callback_info info, bool callBackMode, AsyncGetBundleNameCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("NAPI_GetBundleNameWrap called...");
    if (callBackMode) {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetBundleNameCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("GetBundleName called(CallBack Mode)...");
                AsyncGetBundleNameCallbackInfo *asyncCallbackInfo = (AsyncGetBundleNameCallbackInfo *)data;
                asyncCallbackInfo->bundleName = WantAgentHelper::GetBundleName(asyncCallbackInfo->wantAgent);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("GetBundleName compeleted(CallBack Mode)...");
                AsyncGetBundleNameCallbackInfo *asyncCallbackInfo = (AsyncGetBundleNameCallbackInfo *)data;
                napi_value result[2] = {0};
                napi_value callback;
                napi_value undefined;
                napi_value callResult = 0;

                result[0] = GetCallbackErrorResult(asyncCallbackInfo->env, BUSINESS_ERROR_CODE_OK);
                napi_create_string_utf8(env, asyncCallbackInfo->bundleName.c_str(), NAPI_AUTO_LENGTH, &result[1]);
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_call_function(env, undefined, callback, 2, &result[0], &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        // create reutrn
        napi_value ret = 0;
        NAPI_CALL(env, napi_create_int32(env, 0, &ret));
        return ret;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetBundleNamePromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("GetBundleName called(Promise Mode)...");
                AsyncGetBundleNameCallbackInfo *asyncCallbackInfo = (AsyncGetBundleNameCallbackInfo *)data;
                asyncCallbackInfo->bundleName = WantAgentHelper::GetBundleName(asyncCallbackInfo->wantAgent);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("GetBundleName compeleted(Promise Mode)...");
                AsyncGetBundleNameCallbackInfo *asyncCallbackInfo = (AsyncGetBundleNameCallbackInfo *)data;
                napi_value result;
                napi_create_string_utf8(env, asyncCallbackInfo->bundleName.c_str(), NAPI_AUTO_LENGTH, &result);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

napi_value NAPI_GetBundleName(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);
    Notification::WantAgent::WantAgent *pWantAgent = nullptr;
    if (argv[0] == nullptr) {
        HILOG_INFO("argv[0] is nullptr");
    }
    napi_unwrap(env, argv[0], (void **)&(pWantAgent));
    if (pWantAgent == nullptr) {
        HILOG_INFO("Notification::WantAgent::WantAgent napi_unwrap error");
        return NapiGetNull(env);
    }
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent(pWantAgent);

    bool callBackMode = false;
    if (argc >= 2) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        callBackMode = true;
    }
    AsyncGetBundleNameCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) AsyncGetBundleNameCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};
    if (asyncCallbackInfo == nullptr) {
        return NapiGetNull(env);
    }
    asyncCallbackInfo->wantAgent = wantAgent;

    if (callBackMode) {
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);
    }
    napi_value ret = NAPI_GetBundleNameWrap(env, info, callBackMode, asyncCallbackInfo);
    if (ret == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
    }
    return ((callBackMode) ? (NapiGetNull(env)) : (ret));
}

napi_value NAPI_GetUidWrap(
    napi_env env, napi_callback_info info, bool callBackMode, AsyncGetUidCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("NAPI_GetUidWrap called...");
    if (callBackMode) {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetUidCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("GetUid called(CallBack Mode)...");
                AsyncGetUidCallbackInfo *asyncCallbackInfo = (AsyncGetUidCallbackInfo *)data;
                asyncCallbackInfo->uid = WantAgentHelper::GetUid(asyncCallbackInfo->wantAgent);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("GetUid compeleted(CallBack Mode)...");
                AsyncGetUidCallbackInfo *asyncCallbackInfo = (AsyncGetUidCallbackInfo *)data;
                napi_value result[2] = {0};
                napi_value callback;
                napi_value undefined;
                napi_value callResult = 0;

                result[0] = GetCallbackErrorResult(asyncCallbackInfo->env, BUSINESS_ERROR_CODE_OK);
                napi_create_int32(env, asyncCallbackInfo->uid, &result[1]);
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_call_function(env, undefined, callback, 2, &result[0], &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        // create reutrn
        napi_value ret = 0;
        NAPI_CALL(env, napi_create_int32(env, 0, &ret));
        return ret;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetUidPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("GetUid called(Promise Mode)...");
                AsyncGetUidCallbackInfo *asyncCallbackInfo = (AsyncGetUidCallbackInfo *)data;
                asyncCallbackInfo->uid = WantAgentHelper::GetUid(asyncCallbackInfo->wantAgent);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("GetUid compeleted(Promise Mode)...");
                AsyncGetUidCallbackInfo *asyncCallbackInfo = (AsyncGetUidCallbackInfo *)data;
                napi_value result;
                napi_create_int32(env, asyncCallbackInfo->uid, &result);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

napi_value NAPI_GetUid(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    Notification::WantAgent::WantAgent *pWantAgent = nullptr;
    napi_unwrap(env, argv[0], (void **)&(pWantAgent));
    if (pWantAgent == nullptr) {
        return NapiGetNull(env);
    }
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent(pWantAgent);

    bool callBackMode = false;
    if (argc >= 2) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        callBackMode = true;
    }
    AsyncGetUidCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) AsyncGetUidCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};
    if (asyncCallbackInfo == nullptr) {
        return NapiGetNull(env);
    }
    asyncCallbackInfo->wantAgent = wantAgent;

    if (callBackMode) {
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);
    }
    napi_value ret = NAPI_GetUidWrap(env, info, callBackMode, asyncCallbackInfo);
    if (ret == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
    }
    return ((callBackMode) ? (NapiGetNull(env)) : (ret));
}

napi_value NAPI_GetWantWrap(
    napi_env env, napi_callback_info info, bool callBackMode, AsyncGetWantCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("NAPI_GetWantWrap called...");
    if (callBackMode) {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetWantCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("GetWant called(CallBack Mode)...");
                AsyncGetWantCallbackInfo *asyncCallbackInfo = (AsyncGetWantCallbackInfo *)data;
                asyncCallbackInfo->want = WantAgentHelper::GetWant(asyncCallbackInfo->wantAgent);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("GetWant compeleted(CallBack Mode)...");
                AsyncGetWantCallbackInfo *asyncCallbackInfo = (AsyncGetWantCallbackInfo *)data;
                napi_value result[2] = {0};
                napi_value callback;
                napi_value undefined;
                napi_value callResult = 0;

                result[0] = GetCallbackErrorResult(asyncCallbackInfo->env, BUSINESS_ERROR_CODE_OK);
                result[1] = WrapWant(*(asyncCallbackInfo->want), env);
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_call_function(env, undefined, callback, 2, &result[0], &callResult);
                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        // create reutrn
        napi_value ret = 0;
        NAPI_CALL(env, napi_create_int32(env, 0, &ret));
        return ret;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetWantPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("GetWant called(Promise Mode)...");
                AsyncGetWantCallbackInfo *asyncCallbackInfo = (AsyncGetWantCallbackInfo *)data;
                asyncCallbackInfo->want = WantAgentHelper::GetWant(asyncCallbackInfo->wantAgent);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("GetWant compeleted(Promise Mode)...");
                AsyncGetWantCallbackInfo *asyncCallbackInfo = (AsyncGetWantCallbackInfo *)data;
                napi_value result;
                result = WrapWant(*(asyncCallbackInfo->want), env);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

napi_value NAPI_GetWant(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    Notification::WantAgent::WantAgent *pWantAgent = nullptr;
    napi_unwrap(env, argv[0], (void **)&(pWantAgent));
    if (pWantAgent == nullptr) {
        return NapiGetNull(env);
    }
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent(pWantAgent);

    bool callBackMode = false;
    if (argc >= 2) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        callBackMode = true;
    }
    AsyncGetWantCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) AsyncGetWantCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};
    if (asyncCallbackInfo == nullptr) {
        return NapiGetNull(env);
    }
    asyncCallbackInfo->wantAgent = wantAgent;

    if (callBackMode) {
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);
    }
    napi_value ret = NAPI_GetWantWrap(env, info, callBackMode, asyncCallbackInfo);
    if (ret == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
    }
    return ((callBackMode) ? (NapiGetNull(env)) : (ret));
}

napi_value NAPI_CancelWrap(
    napi_env env, napi_callback_info info, bool callBackMode, AsyncCancelCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("NAPI_CancelWrap called...");
    if (callBackMode) {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_CancelCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("Cancel called(CallBack Mode)...");
                AsyncCancelCallbackInfo *asyncCallbackInfo = (AsyncCancelCallbackInfo *)data;
                WantAgentHelper::Cancel(asyncCallbackInfo->wantAgent);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("Cancel compeleted(CallBack Mode)...");
                AsyncCancelCallbackInfo *asyncCallbackInfo = (AsyncCancelCallbackInfo *)data;
                napi_value result[2] = {0};
                napi_value callback;
                napi_value undefined;
                napi_value callResult = 0;

                result[0] = GetCallbackErrorResult(asyncCallbackInfo->env, BUSINESS_ERROR_CODE_OK);
                napi_get_null(env, &result[1]);
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_call_function(env, undefined, callback, 2, &result[0], &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        // create reutrn
        napi_value ret = 0;
        NAPI_CALL(env, napi_create_int32(env, 0, &ret));
        return ret;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_CancelPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("Cancel called(Promise Mode)...");
                AsyncCancelCallbackInfo *asyncCallbackInfo = (AsyncCancelCallbackInfo *)data;
                WantAgentHelper::Cancel(asyncCallbackInfo->wantAgent);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("Cancel compeleted(Promise Mode)...");
                AsyncCancelCallbackInfo *asyncCallbackInfo = (AsyncCancelCallbackInfo *)data;
                napi_value result;
                napi_get_null(env, &result);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

napi_value NAPI_Cancel(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    Notification::WantAgent::WantAgent *pWantAgent = nullptr;
    napi_unwrap(env, argv[0], (void **)&(pWantAgent));
    if (pWantAgent == nullptr) {
        return NapiGetNull(env);
    }
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent(pWantAgent);

    bool callBackMode = false;
    if (argc >= 2) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        callBackMode = true;
    }
    AsyncCancelCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) AsyncCancelCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};
    if (asyncCallbackInfo == nullptr) {
        return NapiGetNull(env);
    }
    asyncCallbackInfo->wantAgent = wantAgent;

    if (callBackMode) {
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);
    }
    napi_value ret = NAPI_CancelWrap(env, info, callBackMode, asyncCallbackInfo);
    if (ret == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
    }
    return ((callBackMode) ? (NapiGetNull(env)) : (ret));
}

napi_value NAPI_TriggerWrap(napi_env env, napi_callback_info info, AsyncTriggerCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("NAPI_TriggerWrap called...");
    napi_value resourceName;
    napi_create_string_latin1(env, "NAPI_TriggerWrap", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            HILOG_INFO("Trigger called ...");
            AsyncTriggerCallbackInfo *asyncCallbackInfo = (AsyncTriggerCallbackInfo *)data;
            asyncCallbackInfo->triggerObj->SetCallbackInfo(env, asyncCallbackInfo->callback[0]);
            asyncCallbackInfo->triggerObj->SetWantAgentInstance(asyncCallbackInfo->wantAgent);
            WantAgentHelper::TriggerWantAgent(asyncCallbackInfo->context,
                asyncCallbackInfo->wantAgent,
                asyncCallbackInfo->triggerObj,
                asyncCallbackInfo->triggerInfo);
        },
        [](napi_env env, napi_status status, void *data) {
            HILOG_INFO("Trigger compeleted ...");
            AsyncTriggerCallbackInfo *asyncCallbackInfo = (AsyncTriggerCallbackInfo *)data;
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    // create reutrn
    napi_value ret = 0;
    NAPI_CALL(env, napi_create_int32(env, 0, &ret));
    return ret;
}

napi_value NAPI_Trigger(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    Notification::WantAgent::WantAgent *pWantAgent = nullptr;
    napi_unwrap(env, argv[0], (void **)&(pWantAgent));
    if (pWantAgent == nullptr) {
        return NapiGetNull(env);
    }
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent(pWantAgent);

    // Get triggerInfo
    napi_value jsTriggerInfo = argv[1];
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, jsTriggerInfo, &valueType));
    NAPI_ASSERT(env, valueType == napi_object, "param type mismatch!");

    // Get triggerInfo code
    int32_t code = -1;
    if (!GetInt32ByPropertyName(env, jsTriggerInfo, "code", code)) {
        return NapiGetNull(env);
    }
    // Get triggerInfo want
    napi_value jsWant = nullptr;
    jsWant = GetObjectByPropertyName(env, jsTriggerInfo, "want");
    std::shared_ptr<AAFwk::Want> want = nullptr;
    if (jsWant != nullptr) {
        want = std::make_shared<AAFwk::Want>();
        WantAgentUnwrapWant(*want, env, jsWant);
    }
    // Get triggerInfo permission
    std::string permission;
    GetStringByPropertyName(env, jsTriggerInfo, "permission", permission);
    // Get triggerInfo extraInfo
    napi_value jsExtraInfo = nullptr;
    jsExtraInfo = GetObjectByPropertyName(env, jsTriggerInfo, "extraInfo");
    std::shared_ptr<AAFwk::WantParams> extraInfo = nullptr;
    if (jsExtraInfo != nullptr) {
        extraInfo = std::make_shared<AAFwk::WantParams>();
        UnwrapWantParam(*extraInfo, env, jsExtraInfo);
    }
    // Get context
    napi_value global = 0;
    NAPI_CALL(env, napi_get_global(env, &global));
    napi_value abilityObj = 0;
    NAPI_CALL(env, napi_get_named_property(env, global, "ability", &abilityObj));
    Ability *ability = nullptr;
    NAPI_CALL(env, napi_get_value_external(env, abilityObj, (void **)&ability));
    std::shared_ptr<OHOS::AppExecFwk::Context> context = nullptr;
    context = ability->GetContext();

    bool callBackMode = false;
    if (argc >= 3) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[2], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        callBackMode = true;
    }
    AsyncTriggerCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) AsyncTriggerCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};
    if (asyncCallbackInfo == nullptr) {
        return NapiGetNull(env);
    }
    asyncCallbackInfo->wantAgent = wantAgent;
    asyncCallbackInfo->context = context;
    Notification::WantAgent::TriggerInfo triggerInfo(permission, extraInfo, want, code);
    asyncCallbackInfo->triggerInfo = triggerInfo;
    asyncCallbackInfo->triggerObj = nullptr;
    if (callBackMode) {
        asyncCallbackInfo->callBackMode = callBackMode;
        asyncCallbackInfo->triggerObj = std::make_shared<TriggerCompleteCallBack>();
        napi_create_reference(env, argv[2], 1, &asyncCallbackInfo->callback[0]);
    }

    napi_value ret = NAPI_TriggerWrap(env, info, asyncCallbackInfo);
    if (ret == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
    }
    return NapiGetNull(env);
}

napi_value NAPI_EqualWrap(
    napi_env env, napi_callback_info info, bool callBackMode, AsyncEqualCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("NAPI_EqualWrap called...");
    if (callBackMode) {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_EqualWrapCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("Equal called(CallBack Mode)...");
                AsyncEqualCallbackInfo *asyncCallbackInfo = (AsyncEqualCallbackInfo *)data;
                asyncCallbackInfo->result = WantAgentHelper::JudgeEquality(
                    asyncCallbackInfo->wantAgentFirst, asyncCallbackInfo->wantAgentSecond);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("Equal compeleted(CallBack Mode)...");
                AsyncEqualCallbackInfo *asyncCallbackInfo = (AsyncEqualCallbackInfo *)data;
                napi_value result[2] = {0};
                napi_value callback;
                napi_value undefined;
                napi_value callResult = 0;

                result[0] = GetCallbackErrorResult(asyncCallbackInfo->env, BUSINESS_ERROR_CODE_OK);
                napi_get_boolean(env, asyncCallbackInfo->result, &result[1]);
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_call_function(env, undefined, callback, 2, &result[0], &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        // create reutrn
        napi_value ret = 0;
        NAPI_CALL(env, napi_create_int32(env, 0, &ret));
        return ret;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_EqualPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("Equal called(Promise Mode)...");
                AsyncEqualCallbackInfo *asyncCallbackInfo = (AsyncEqualCallbackInfo *)data;
                asyncCallbackInfo->result = WantAgentHelper::JudgeEquality(
                    asyncCallbackInfo->wantAgentFirst, asyncCallbackInfo->wantAgentSecond);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("Equal compeleted(Promise Mode)...");
                AsyncEqualCallbackInfo *asyncCallbackInfo = (AsyncEqualCallbackInfo *)data;
                napi_value result;
                napi_get_boolean(env, asyncCallbackInfo->result, &result);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

napi_value NAPI_Equal(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    Notification::WantAgent::WantAgent *pWantAgentFirst = nullptr;
    napi_unwrap(env, argv[0], (void **)&(pWantAgentFirst));
    if (pWantAgentFirst == nullptr) {
        return NapiGetNull(env);
    }
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgentFirst(pWantAgentFirst);

    Notification::WantAgent::WantAgent *pWantAgentSecond = nullptr;
    napi_unwrap(env, argv[1], (void **)&(pWantAgentSecond));
    if (pWantAgentSecond == nullptr) {
        return NapiGetNull(env);
    }
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgentSecond(pWantAgentSecond);

    bool callBackMode = false;
    if (argc >= 3) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[2], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        callBackMode = true;
    }
    AsyncEqualCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) AsyncEqualCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};
    if (asyncCallbackInfo == nullptr) {
        return NapiGetNull(env);
    }
    asyncCallbackInfo->wantAgentFirst = wantAgentFirst;
    asyncCallbackInfo->wantAgentSecond = wantAgentSecond;

    if (callBackMode) {
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);
    }
    napi_value ret = NAPI_EqualWrap(env, info, callBackMode, asyncCallbackInfo);
    if (ret == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
    }
    return ((callBackMode) ? (NapiGetNull(env)) : (ret));
}

std::string NapiGetStringParam(const AAFwk::WantParams &extraInfo, const std::string &key)
{
    auto value = extraInfo.GetParam(key);
    AAFwk::IString *ao = AAFwk::IString::Query(value);
    if (ao != nullptr) {
        return AAFwk::String::Unbox(ao);
    }
    return std::string();
}

bool NapiGetBoolParam(const AAFwk::WantParams &extraInfo, const std::string &key, bool defaultValue)
{
    auto value = extraInfo.GetParam(key);
    AAFwk::IBoolean *bo = AAFwk::IBoolean::Query(value);
    if (bo != nullptr) {
        return AAFwk::Boolean::Unbox(bo);
    }
    return defaultValue;
}

short NapiGetShortParam(const AAFwk::WantParams &extraInfo, const std::string &key, short defaultValue)
{
    auto value = extraInfo.GetParam(key);
    AAFwk::IShort *ao = AAFwk::IShort::Query(value);
    if (ao != nullptr) {
        return AAFwk::Short::Unbox(ao);
    }
    return defaultValue;
}

int NapiGetIntParam(const AAFwk::WantParams &extraInfo, const std::string &key, const int defaultValue)
{
    auto value = extraInfo.GetParam(key);
    AAFwk::IInteger *ao = AAFwk::IInteger::Query(value);
    if (ao != nullptr) {
        return AAFwk::Integer::Unbox(ao);
    }
    return defaultValue;
}

long NapiGetLongParam(const AAFwk::WantParams &extraInfo, const std::string &key, long defaultValue)
{
    auto value = extraInfo.GetParam(key);
    AAFwk::ILong *ao = AAFwk::ILong::Query(value);
    if (ao != nullptr) {
        return AAFwk::Long::Unbox(ao);
    }
    return defaultValue;
}

float NapiGetFloatParam(const AAFwk::WantParams &extraInfo, const std::string &key, float defaultValue)
{
    auto value = extraInfo.GetParam(key);
    AAFwk::IFloat *ao = AAFwk::IFloat::Query(value);
    if (ao != nullptr) {
        return AAFwk::Float::Unbox(ao);
    }
    return defaultValue;
}

double NapiGetDoubleParam(const AAFwk::WantParams &extraInfo, const std::string &key, double defaultValue)
{
    auto value = extraInfo.GetParam(key);
    AAFwk::IDouble *ao = AAFwk::IDouble::Query(value);
    if (ao != nullptr) {
        return AAFwk::Double::Unbox(ao);
    }
    return defaultValue;
}

napi_value WrapWantParam(const AAFwk::WantParams &extraInfo, napi_env env)
{
    HILOG_INFO("%{public}s,called", __func__);
    napi_value jsParam = nullptr;
    napi_value proValue = nullptr;
    NAPI_CALL(env, napi_create_object(env, &jsParam));
    const std::map<std::string, sptr<AAFwk::IInterface>> paramList = extraInfo.GetParams();
    for (auto iter = paramList.begin(); iter != paramList.end(); iter++) {
        proValue = nullptr;
        if (AAFwk::IString::Query(iter->second) != nullptr) {
            std::string nativeValue = NapiGetStringParam(extraInfo, iter->first);
            HILOG_INFO("%{public}s, %{public}s=%{public}s", __func__, iter->first.c_str(), nativeValue.c_str());
            if (napi_create_string_utf8(env, nativeValue.c_str(), NAPI_AUTO_LENGTH, &proValue) == napi_ok) {
                napi_set_named_property(env, jsParam, iter->first.c_str(), proValue);
            }
        } else if (AAFwk::IBoolean::Query(iter->second) != nullptr) {
            bool natvieValue = NapiGetBoolParam(extraInfo, iter->first, false);
            if (napi_get_boolean(env, natvieValue, &proValue) == napi_ok) {
                napi_set_named_property(env, jsParam, iter->first.c_str(), proValue);
            }
        } else if (AAFwk::IShort::Query(iter->second) != nullptr) {
            short natvieValue = NapiGetShortParam(extraInfo, iter->first, 0);
            if (napi_create_int32(env, natvieValue, &proValue) == napi_ok) {
                napi_set_named_property(env, jsParam, iter->first.c_str(), proValue);
            }
        } else if (AAFwk::IInteger::Query(iter->second) != nullptr) {
            int natvieValue = NapiGetIntParam(extraInfo, iter->first, 0);
            if (napi_create_int32(env, natvieValue, &proValue) == napi_ok) {
                napi_set_named_property(env, jsParam, iter->first.c_str(), proValue);
            }
        } else if (AAFwk::ILong::Query(iter->second) != nullptr) {
            long natvieValue = NapiGetLongParam(extraInfo, iter->first, 0);
            if (napi_create_int64(env, natvieValue, &proValue) == napi_ok) {
                napi_set_named_property(env, jsParam, iter->first.c_str(), proValue);
            }
        } else if (AAFwk::IFloat::Query(iter->second) != nullptr) {
            float natvieValue = NapiGetFloatParam(extraInfo, iter->first, 0.0);
            if (napi_create_double(env, natvieValue, &proValue) == napi_ok) {
                napi_set_named_property(env, jsParam, iter->first.c_str(), proValue);
            }
        } else if (AAFwk::IDouble::Query(iter->second) != nullptr) {
            double natvieValue = NapiGetDoubleParam(extraInfo, iter->first, 0.0);
            if (napi_create_double(env, natvieValue, &proValue) == napi_ok) {
                napi_set_named_property(env, jsParam, iter->first.c_str(), proValue);
            }
        } else {
            AAFwk::IArray *ao = AAFwk::IArray::Query(iter->second);
            if (ao != nullptr) {
                sptr<AAFwk::IArray> array(ao);
                WrapWantParamArray(extraInfo, env, iter->first, array, jsParam);
            }
        }
    }
    return jsParam;
}

napi_value UnwrapWantParam(AAFwk::WantParams &extraInfo, napi_env env, napi_value JsExtraInfo)
{
    HILOG_INFO("%{public}s,called", __func__);
    napi_valuetype valueType = napi_undefined;
    napi_value proNameList = 0;
    uint32_t proCount = 0;

    NAPI_CALL(env, napi_get_property_names(env, JsExtraInfo, &proNameList));
    NAPI_CALL(env, napi_get_array_length(env, proNameList, &proCount));
    HILOG_INFO("UnwrapWantParam property size=%{public}d", proCount);

    napi_value proName = 0;
    napi_value proValue = 0;
    for (uint32_t index = 0; index < proCount; index++) {
        NAPI_CALL(env, napi_get_element(env, proNameList, index, &proName));
        std::string strProName = NapiValueToStringUtf8(env, proName);
        HILOG_INFO("UnwrapWantParam proName=%{public}s", strProName.c_str());
        NAPI_CALL(env, napi_get_named_property(env, JsExtraInfo, strProName.c_str(), &proValue));
        NAPI_CALL(env, napi_typeof(env, proValue, &valueType));
        switch (valueType) {
            case napi_string: {
                std::string str_pro_value = NapiValueToStringUtf8(env, proValue);
                HILOG_INFO("UnwrapWantParam proValue=%{public}s", str_pro_value.c_str());
                extraInfo.SetParam(strProName, AAFwk::String::Box(str_pro_value));
                break;
            }
            case napi_boolean: {
                bool c_pro_value = false;
                NAPI_CALL(env, napi_get_value_bool(env, proValue, &c_pro_value));
                HILOG_INFO("UnwrapWantParam proValue=%{public}d", c_pro_value);
                extraInfo.SetParam(strProName, AAFwk::Boolean::Box(c_pro_value));
                break;
            }
            case napi_number: {
                int32_t c_pro_value32 = 0;
                double c_pro_value_double = 0.0;

                if (napi_get_value_int32(env, proValue, &c_pro_value32) == napi_ok) {
                    HILOG_INFO("UnwrapWantParam proValue=%{public}d", c_pro_value32);
                    extraInfo.SetParam(strProName, AAFwk::Integer::Box(c_pro_value32));
                    break;
                }
                if (napi_get_value_double(env, proValue, &c_pro_value_double) == napi_ok) {
                    HILOG_INFO("UnwrapWantParam proValue=%{public}lf", c_pro_value_double);
                    extraInfo.SetParam(strProName, AAFwk::Double::Box(c_pro_value_double));
                    break;
                }
                HILOG_INFO("UnwrapWantParam unknown proValue of Number");
                break;
            }
            default: {
                if (UnwrapWantParamArray(extraInfo, env, strProName, proValue) == nullptr) {
                    return nullptr;
                }
            }
        }
    }
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

void SetParam(AAFwk::WantParams &extraInfo, const std::string &key, const std::vector<std::string> &value)
{
    long size = value.size();
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_IString);
    if (ao == nullptr) {
        return;
    }
    for (long i = 0; i < size; i++) {
        ao->Set(i, AAFwk::String::Box(value[i]));
    }
    extraInfo.SetParam(key, ao);
}

void SetParam(AAFwk::WantParams &extraInfo, const std::string &key, const std::vector<int> &value)
{
    long size = value.size();
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_IInteger);
    if (ao == nullptr) {
        return;
    }
    for (long i = 0; i < size; i++) {
        ao->Set(i, AAFwk::Integer::Box(value[i]));
    }
    extraInfo.SetParam(key, ao);
}

void SetParam(AAFwk::WantParams &extraInfo, const std::string &key, const std::vector<long> &value)
{
    long size = value.size();
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_ILong);
    if (ao == nullptr) {
        return;
    }
    for (long i = 0; i < size; i++) {
        ao->Set(i, AAFwk::Long::Box(value[i]));
    }
    extraInfo.SetParam(key, ao);
}

void SetParam(AAFwk::WantParams &extraInfo, const std::string &key, const std::vector<bool> &value)
{
    long size = value.size();
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_IBoolean);
    if (ao != nullptr) {
        for (long i = 0; i < size; i++) {
            ao->Set(i, AAFwk::Boolean::Box(value[i]));
        }
        extraInfo.SetParam(key, ao);
    }
}

void SetParam(AAFwk::WantParams &extraInfo, const std::string &key, const std::vector<double> &value)
{
    long size = value.size();
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_IDouble);
    if (ao == nullptr) {
        return;
    }
    for (long i = 0; i < size; i++) {
        ao->Set(i, AAFwk::Double::Box(value[i]));
    }
    extraInfo.SetParam(key, ao);
}

napi_value UnwrapWantParamArray(AAFwk::WantParams &extraInfo, napi_env env, std::string strProName, napi_value proValue)
{
    HILOG_INFO("%{public}s,called", __func__);
    bool isArray = false;
    uint32_t arrayLength = 0;
    napi_value valueAry = 0;
    napi_valuetype valueAryType = napi_undefined;

    NAPI_CALL(env, napi_is_array(env, proValue, &isArray));
    NAPI_CALL(env, napi_get_array_length(env, proValue, &arrayLength));
    HILOG_INFO("UnwrapWantParam proValue is array, length=%{public}d", arrayLength);

    std::vector<std::string> stringList;
    std::vector<int> intList;
    std::vector<long> longList;
    std::vector<bool> boolList;
    std::vector<double> doubleList;
    bool isDouble = false;
    for (uint32_t j = 0; j < arrayLength; j++) {
        NAPI_CALL(env, napi_get_element(env, proValue, j, &valueAry));
        NAPI_CALL(env, napi_typeof(env, valueAry, &valueAryType));
        switch (valueAryType) {
            case napi_string: {
                std::string str_ary_value = NapiValueToStringUtf8(env, valueAry);
                HILOG_INFO("UnwrapWantParam string array proValue=%{public}s", str_ary_value.c_str());
                stringList.push_back(str_ary_value);
                break;
            }
            case napi_boolean: {
                bool c_ary_value = false;
                NAPI_CALL(env, napi_get_value_bool(env, valueAry, &c_ary_value));
                HILOG_INFO("UnwrapWantParam bool array proValue=%{public}d", c_ary_value);
                boolList.push_back(c_ary_value);
                break;
            }
            case napi_number: {
                int32_t c_ary_value32 = 0;
                int64_t c_ary_value64 = 0;
                double c_ary_value_double = 0.0;
                if (isDouble) {
                    if (napi_get_value_double(env, valueAry, &c_ary_value_double) == napi_ok) {
                        HILOG_INFO("UnwrapWantParam double array proValue=%{public}lf", c_ary_value_double);
                        doubleList.push_back(c_ary_value_double);
                    }
                    break;
                } else {
                    if (napi_get_value_int32(env, valueAry, &c_ary_value32) == napi_ok) {
                        HILOG_INFO("UnwrapWantParam int array proValue=%{public}d", c_ary_value32);
                        intList.push_back(c_ary_value32);
                        break;
                    }
                }

                if (napi_get_value_int64(env, valueAry, &c_ary_value64) == napi_ok) {
                    HILOG_INFO("UnwrapWantParam int64 array proValue=%{public}lld", c_ary_value64);
                    longList.push_back(c_ary_value64);
                    break;
                }
                if (napi_get_value_double(env, valueAry, &c_ary_value_double) == napi_ok) {
                    HILOG_INFO("UnwrapWantParam double array proValue=%{public}lf", c_ary_value_double);
                    isDouble = true;
                    if (intList.size() > 0) {
                        for (int k = 0; k < (int)intList.size(); k++) {
                            doubleList.push_back(intList[k]);
                        }
                        intList.clear();
                    }
                    doubleList.push_back(c_ary_value_double);
                    break;
                }
                HILOG_INFO("UnwrapWantParam array unkown Number");
                break;
            }
            default:
                HILOG_INFO("UnwrapWantParam array unkown");
                break;
        }
    }
    if (stringList.size() > 0) {
        SetParam(extraInfo, strProName, stringList);
    }
    if (intList.size() > 0) {
        SetParam(extraInfo, strProName, intList);
    }
    if (longList.size() > 0) {
        SetParam(extraInfo, strProName, longList);
    }
    if (boolList.size() > 0) {
        SetParam(extraInfo, strProName, boolList);
    }
    if (doubleList.size() > 0) {
        SetParam(extraInfo, strProName, doubleList);
    }
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

napi_value NAPI_GetWantAgentWrap(
    napi_env env, napi_callback_info info, bool callBackMode, AsyncGetWantAgentCallbackInfo *asyncCallbackInfo)
{
    HILOG_INFO("NAPI_GetWantAgentWrap called...");
    if (callBackMode) {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetWantAgentCallBack", NAPI_AUTO_LENGTH, &resourceName);

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("GetWantAgent called(CallBack Mode)...");
                AsyncGetWantAgentCallbackInfo *asyncCallbackInfo = (AsyncGetWantAgentCallbackInfo *)data;
                Notification::WantAgent::WantAgentInfo wantAgentInfo(asyncCallbackInfo->requestCode,
                    asyncCallbackInfo->operationType,
                    asyncCallbackInfo->wantAgentFlags,
                    asyncCallbackInfo->wants,
                    asyncCallbackInfo->extraInfo);
                asyncCallbackInfo->wantAgent =
                    Notification::WantAgent::WantAgentHelper::GetWantAgent(asyncCallbackInfo->context, wantAgentInfo);
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("GetWantAgent compeleted(CallBack Mode)...");
                AsyncGetWantAgentCallbackInfo *asyncCallbackInfo = (AsyncGetWantAgentCallbackInfo *)data;
                napi_value result[2] = {0};
                napi_value callback;
                napi_value undefined;
                napi_value callResult = 0;

                result[0] = GetCallbackErrorResult(asyncCallbackInfo->env, BUSINESS_ERROR_CODE_OK);

                napi_value wantAgentClass = nullptr;
                napi_define_class(env,
                    "WantAgentClass",
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
                napi_new_instance(env, wantAgentClass, 0, nullptr, &result[1]);
                napi_wrap(env,
                    result[1],
                    (void *)asyncCallbackInfo->wantAgent.get(),
                    [](napi_env env, void *data, void *hint) {},
                    nullptr,
                    nullptr);
                napi_get_undefined(env, &undefined);
                napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
                napi_call_function(env, undefined, callback, 2, &result[0], &callResult);

                if (asyncCallbackInfo->callback[0] != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);

        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        // create reutrn
        napi_value ret = 0;
        NAPI_CALL(env, napi_create_int32(env, 0, &ret));
        return ret;
    } else {
        napi_value resourceName;
        napi_create_string_latin1(env, "NAPI_GetWantAgentPromise", NAPI_AUTO_LENGTH, &resourceName);

        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_create_async_work(env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                HILOG_INFO("GetWantAgent called(Promise Mode)...");
                AsyncGetWantAgentCallbackInfo *asyncCallbackInfo = (AsyncGetWantAgentCallbackInfo *)data;
                HILOG_INFO("GetWantAgent wants.size = [%{public}d], wantAgentFlags.size = [%{public}d]",
                    asyncCallbackInfo->wants.size(),
                    asyncCallbackInfo->wantAgentFlags.size());

                Notification::WantAgent::WantAgentInfo wantAgentInfo(asyncCallbackInfo->requestCode,
                    asyncCallbackInfo->operationType,
                    asyncCallbackInfo->wantAgentFlags,
                    asyncCallbackInfo->wants,
                    asyncCallbackInfo->extraInfo);
                asyncCallbackInfo->wantAgent =
                    Notification::WantAgent::WantAgentHelper::GetWantAgent(asyncCallbackInfo->context, wantAgentInfo);
                if (asyncCallbackInfo->wantAgent == nullptr) {
                    HILOG_INFO("GetWantAgent instance is nullptr...");
                } else {
                    HILOG_INFO("GetWantAgent instance is not nullptr...");
                }
            },
            [](napi_env env, napi_status status, void *data) {
                HILOG_INFO("GetWantAgent compeleted(Promise Mode)...");
                AsyncGetWantAgentCallbackInfo *asyncCallbackInfo = (AsyncGetWantAgentCallbackInfo *)data;
                napi_value wantAgentClass = nullptr;
                napi_define_class(env,
                    "WantAgentClass",
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
                napi_value result = nullptr;
                napi_new_instance(env, wantAgentClass, 0, nullptr, &result);
                napi_wrap(env,
                    result,
                    (void *)asyncCallbackInfo->wantAgent.get(),
                    [](napi_env env, void *data, void *hint) {},
                    nullptr,
                    nullptr);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork);
        napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
        return promise;
    }
}

napi_value WantAgentUnwrapWant(Want &param, napi_env env, napi_value jsWant)
{
    HILOG_INFO("%{public}s,called", __func__);
    // unwrap the param
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, jsWant, &valueType));
    if (valueType != napi_object) {
        return nullptr;
    }
    std::string strValue;
    // get want action property
    if (GetStringByPropertyName(env, jsWant, "action", strValue)) {
        param.SetAction(strValue);
    }
    // get want entities property
    std::vector<std::string> nativeStringList;
    GetStringArrayByPropertyName(env, jsWant, "entities", nativeStringList);
    for (size_t i = 0; i < nativeStringList.size(); i++) {
        param.AddEntity(nativeStringList[i]);
    }
    // get want type property
    if (GetStringByPropertyName(env, jsWant, "type", strValue)) {
        param.SetType(strValue);
    }
    // get want flags property(WantOptions in want.d.ts)
    GetWantOptions(env, jsWant, "options", param);
    // get want uri property
    if (GetStringByPropertyName(env, jsWant, "uri", strValue)) {
        param.SetUri(strValue);
    }
    // get elementName property
    UnwrapElementName(param, env, jsWant);
    // get want param (optional)
    UnwrapWantParam(param, env, jsWant);
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

napi_value NAPI_GetWantAgent(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[argc];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    HILOG_INFO("argc = [%{public}d]", argc);

    napi_valuetype jsWantAgentInfoType;
    napi_value jsWantAgentInfo = argv[0];
    NAPI_CALL(env, napi_typeof(env, jsWantAgentInfo, &jsWantAgentInfoType));
    NAPI_ASSERT(env, jsWantAgentInfoType == napi_object, "param type mismatch!");

    napi_value jsWants = nullptr;
    jsWants = GetObjectByPropertyName(env, jsWantAgentInfo, "wants");
    if (jsWants == nullptr) {
        return NapiGetNull(env);
    }
    bool isArray = false;
    if (napi_is_array(env, jsWants, &isArray) != napi_ok) {
        return NapiGetNull(env);
    }
    if (isArray == false) {
        return NapiGetNull(env);
    }
    uint32_t wantsLen = 0;
    napi_get_array_length(env, jsWants, &wantsLen);
    if (wantsLen < 0) {
        return NapiGetNull(env);
    }
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    for (uint32_t i = 0; i < wantsLen; i++) {
        std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
        napi_value jsWant;
        napi_get_element(env, jsWants, i, &jsWant);
        WantAgentUnwrapWant(*want, env, jsWant);
        HILOG_INFO("want type is [%{public}s]", want->GetType().c_str());
        wants.emplace_back(want);
    }
    // Get operationType
    int32_t operationType = -1;
    if (!GetInt32ByPropertyName(env, jsWantAgentInfo, "operationType", operationType)) {
        return NapiGetNull(env);
    }
    // Get requestCode
    int32_t requestCode = -1;
    if (!GetInt32ByPropertyName(env, jsWantAgentInfo, "requestCode", requestCode)) {
        return NapiGetNull(env);
    }
    // Get wantAgentFlags
    napi_value JsWantAgentFlags = nullptr;
    std::vector<Notification::WantAgent::WantAgentConstant::Flags> wantAgentFlags;
    JsWantAgentFlags = GetObjectByPropertyName(env, jsWantAgentInfo, "wantAgentFlags");
    if (JsWantAgentFlags != nullptr) {
        uint32_t arrayLength = 0;
        NAPI_CALL(env, napi_get_array_length(env, JsWantAgentFlags, &arrayLength));
        HILOG_INFO("property is array, length=%{public}d", arrayLength);
        for (uint32_t i = 0; i < arrayLength; i++) {
            napi_value napiWantAgentFlags;
            napi_get_element(env, JsWantAgentFlags, i, &napiWantAgentFlags);
            napi_valuetype valuetype0;
            NAPI_CALL(env, napi_typeof(env, napiWantAgentFlags, &valuetype0));
            NAPI_ASSERT(env, valuetype0 == napi_number, "Wrong argument type. Numbers expected.");
            int32_t value0 = 0;
            NAPI_CALL(env, napi_get_value_int32(env, napiWantAgentFlags, &value0));
            wantAgentFlags.emplace_back(static_cast<Notification::WantAgent::WantAgentConstant::Flags>(value0));
        }
    }
    // Get extraInfo
    napi_value JsExtraInfo = nullptr;
    JsExtraInfo = GetObjectByPropertyName(env, jsWantAgentInfo, "extraInfo");
    AAFwk::WantParams extraInfo;
    if (JsExtraInfo != nullptr) {
        UnwrapWantParam(extraInfo, env, JsExtraInfo);
    }
    // Get context
    napi_value global = 0;
    NAPI_CALL(env, napi_get_global(env, &global));
    napi_value abilityObj = 0;
    NAPI_CALL(env, napi_get_named_property(env, global, "ability", &abilityObj));
    Ability *ability = nullptr;
    NAPI_CALL(env, napi_get_value_external(env, abilityObj, (void **)&ability));
    std::shared_ptr<OHOS::AppExecFwk::Context> context = nullptr;
    context = ability->GetContext();

    bool callBackMode = false;
    if (argc >= 2) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        callBackMode = true;
    }

    AsyncGetWantAgentCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) AsyncGetWantAgentCallbackInfo{.env = env, .asyncWork = nullptr, .deferred = nullptr};
    if (asyncCallbackInfo == nullptr) {
        return NapiGetNull(env);
    }
    asyncCallbackInfo->wants = wants;
    asyncCallbackInfo->operationType =
        static_cast<Notification::WantAgent::WantAgentConstant::OperationType>(operationType);
    asyncCallbackInfo->requestCode = requestCode;
    asyncCallbackInfo->wantAgentFlags = wantAgentFlags;
    asyncCallbackInfo->extraInfo.reset(new (std::nothrow) AAFwk::WantParams(extraInfo));
    asyncCallbackInfo->context = context;

    if (callBackMode) {
        napi_create_reference(env, argv[1], 1, &asyncCallbackInfo->callback[0]);
    }
    napi_value ret = NAPI_GetWantAgentWrap(env, info, callBackMode, asyncCallbackInfo);
    if (ret == nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
    }

    return ((callBackMode) ? (NapiGetNull(env)) : (ret));
}

napi_value GetCallbackErrorResult(napi_env env, int errCode)
{
    napi_value result = nullptr;
    napi_value eCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &eCode));
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_set_named_property(env, result, "code", eCode));
    return result;
}

napi_value NapiGetNull(napi_env env)
{
    napi_value result = 0;
    napi_get_null(env, &result);
    return result;
}

std::vector<std::string> GetStringArrayParam(const AAFwk::WantParams &extraInfo, const std::string &key)
{
    std::vector<std::string> array;
    auto value = extraInfo.GetParam(key);
    AAFwk::IArray *ao = AAFwk::IArray::Query(value);
    if (ao != nullptr && AAFwk::Array::IsStringArray(ao)) {
        auto func = [&](AAFwk::IInterface *object) {
            if (object != nullptr) {
                AAFwk::IString *value = AAFwk::IString::Query(object);
                if (value != nullptr) {
                    array.push_back(AAFwk::String::Unbox(value));
                }
            }
        };
        AAFwk::Array::ForEach(ao, func);
    }
    return array;
}

std::vector<bool> GetBoolArrayParam(const AAFwk::WantParams &extraInfo, const std::string &key)
{
    std::vector<bool> array;
    auto value = extraInfo.GetParam(key);
    AAFwk::IArray *ao = AAFwk::IArray::Query(value);
    if (ao != nullptr && AAFwk::Array::IsBooleanArray(ao)) {
        auto func = [&](AAFwk::IInterface *object) {
            if (object != nullptr) {
                AAFwk::IBoolean *value = AAFwk::IBoolean::Query(object);
                if (value != nullptr) {
                    array.push_back(AAFwk::Boolean::Unbox(value));
                }
            }
        };

        AAFwk::Array::ForEach(ao, func);
    }
    return array;
}

std::vector<int> GetIntArrayParam(const AAFwk::WantParams &extraInfo, const std::string &key)
{
    std::vector<int> array;
    auto value = extraInfo.GetParam(key);
    AAFwk::IArray *ao = AAFwk::IArray::Query(value);
    if (ao != nullptr && AAFwk::Array::IsIntegerArray(ao)) {
        auto func = [&](AAFwk::IInterface *object) {
            if (object != nullptr) {
                AAFwk::IInteger *value = AAFwk::IInteger::Query(object);
                if (value != nullptr) {
                    array.push_back(AAFwk::Integer::Unbox(value));
                }
            }
        };
        AAFwk::Array::ForEach(ao, func);
    }
    return array;
}

std::vector<long> GetLongArrayParam(const AAFwk::WantParams &extraInfo, const std::string &key)
{
    std::vector<long> array;
    auto value = extraInfo.GetParam(key);
    AAFwk::IArray *ao = AAFwk::IArray::Query(value);
    if (ao != nullptr && AAFwk::Array::IsLongArray(ao)) {
        auto func = [&](AAFwk::IInterface *object) {
            if (object != nullptr) {
                AAFwk::ILong *value = AAFwk::ILong::Query(object);
                if (value != nullptr) {
                    array.push_back(AAFwk::Long::Unbox(value));
                }
            }
        };
        AAFwk::Array::ForEach(ao, func);
    }
    return array;
}

std::vector<double> GetDoubleArrayParam(const AAFwk::WantParams &extraInfo, const std::string &key)
{
    std::vector<double> array;
    auto value = extraInfo.GetParam(key);
    AAFwk::IArray *ao = AAFwk::IArray::Query(value);
    if (ao != nullptr && AAFwk::Array::IsDoubleArray(ao)) {
        auto func = [&](AAFwk::IInterface *object) {
            if (object != nullptr) {
                AAFwk::IDouble *value = AAFwk::IDouble::Query(object);
                if (value != nullptr) {
                    array.push_back(AAFwk::Double::Unbox(value));
                }
            }
        };
        AAFwk::Array::ForEach(ao, func);
    }
    return array;
}

void WrapWantParamArray(const AAFwk::WantParams &extraInfo, napi_env env, const std::string &key,
    sptr<AAFwk::IArray> &ao, napi_value jsObject)
{
    napi_value jsArray = nullptr;
    napi_value proValue = nullptr;
    if (AAFwk::Array::IsStringArray(ao)) {
        std::vector<std::string> nativeValue = GetStringArrayParam(extraInfo, key);
        if (napi_create_array(env, &jsArray) != napi_ok) {
            return;
        }
        for (uint32_t i = 0; i < nativeValue.size(); i++) {
            proValue = nullptr;
            if (napi_create_string_utf8(env, nativeValue[i].c_str(), NAPI_AUTO_LENGTH, &proValue) == napi_ok) {
                napi_set_element(env, jsArray, i, proValue);
            }
        }
        napi_set_named_property(env, jsObject, key.c_str(), jsArray);
    } else if (AAFwk::Array::IsBooleanArray(ao)) {
        std::vector<bool> nativeValue = GetBoolArrayParam(extraInfo, key);
        if (napi_create_array(env, &jsArray) != napi_ok) {
            return;
        }
        for (uint32_t i = 0; i < nativeValue.size(); i++) {
            proValue = nullptr;
            if (napi_get_boolean(env, nativeValue[i], &proValue) == napi_ok) {
                napi_set_element(env, jsArray, i, proValue);
            }
        }
        napi_set_named_property(env, jsObject, key.c_str(), jsArray);
    } else if (AAFwk::Array::IsIntegerArray(ao)) {
        std::vector<int> nativeValue = GetIntArrayParam(extraInfo, key);
        if (napi_create_array(env, &jsArray) != napi_ok) {
            return;
        }
        for (uint32_t i = 0; i < nativeValue.size(); i++) {
            proValue = nullptr;
            if (napi_create_int32(env, nativeValue[i], &proValue) == napi_ok) {
                napi_set_element(env, jsArray, i, proValue);
            }
        }
        napi_set_named_property(env, jsObject, key.c_str(), jsArray);
    } else if (AAFwk::Array::IsLongArray(ao)) {
        std::vector<long> nativeValue = GetLongArrayParam(extraInfo, key);
        if (napi_create_array(env, &jsArray) != napi_ok) {
            return;
        }
        for (uint32_t i = 0; i < nativeValue.size(); i++) {
            proValue = nullptr;
            if (napi_create_int64(env, nativeValue[i], &proValue) == napi_ok) {
                napi_set_element(env, jsArray, i, proValue);
            }
        }
        napi_set_named_property(env, jsObject, key.c_str(), jsArray);
    } else if (AAFwk::Array::IsDoubleArray(ao)) {
        std::vector<double> nativeValue = GetDoubleArrayParam(extraInfo, key);
        if (napi_create_array(env, &jsArray) != napi_ok) {
            return;
        }
        for (uint32_t i = 0; i < nativeValue.size(); i++) {
            proValue = nullptr;
            if (napi_create_double(env, nativeValue[i], &proValue) == napi_ok) {
                napi_set_element(env, jsArray, i, proValue);
            }
        }
        napi_set_named_property(env, jsObject, key.c_str(), jsArray);
    }
}

}  // namespace OHOS