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

#include <uv.h>
#include "create_subscriber.h"

namespace OHOS {
namespace NotificationNapi {
SubscriberInstance::SubscriberInstance()
{}

SubscriberInstance::~SubscriberInstance()
{}

void SubscriberInstance::OnCanceled(const std::shared_ptr<OHOS::Notification::Notification> &request)
{
    ANS_LOGI("OnCanceled start");

    if (canceCallbackInfo_.ref == nullptr) {
        ANS_LOGI("cancel callback unset");
        return;
    }

    if (request == nullptr) {
        ANS_LOGE("request is null");
        return;
    }
    ANS_LOGI("OnCanceled NotificationId = %{public}d", request->GetNotificationRequest().GetNotificationId());

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(canceCallbackInfo_.env, &loop);
#endif  // NAPI_VERSION >= 2

    NotificationReceiveDataWorker *dataWorker = new NotificationReceiveDataWorker();
    dataWorker->request = request;
    dataWorker->env = canceCallbackInfo_.env;
    dataWorker->ref = canceCallbackInfo_.ref;

    uv_work_t *work = new uv_work_t;
    work->data = (void *)dataWorker;

    uv_queue_work(loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ANS_LOGI("OnCanceled uv_work_t start");
            NotificationReceiveDataWorker *dataWorkerData = (NotificationReceiveDataWorker *)work->data;
            if (dataWorkerData == nullptr) {
                ANS_LOGI("dataWorkerData is nullptr");
                return;
            }
            napi_value result;
            napi_create_object(dataWorkerData->env, &result);
            int error = 0;
            if (!SetSubscriberCallbackData(dataWorkerData->env, dataWorkerData->request, result)) {
                result = Common::NapiGetNull(dataWorkerData->env);
                error = ERROR;
            } else {
                error = NO_ERROR;
            }
            Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, error, result);

            delete dataWorkerData;
            dataWorkerData = nullptr;
            delete work;
            work = nullptr;
        });
}

void SubscriberInstance::OnCanceled(const std::shared_ptr<OHOS::Notification::Notification> &request,
    const std::shared_ptr<NotificationSortingMap> &sortingMap, int deleteReason)
{
    ANS_LOGI("OnCanceled2 start");

    if (canceCallbackInfo_.ref == nullptr) {
        ANS_LOGI("cancel callback unset");
        return;
    }

    if (request == nullptr) {
        ANS_LOGE("request is null");
        return;
    }

    if (sortingMap == nullptr) {
        ANS_LOGE("sortingMap is null");
        return;
    }
    ANS_LOGI("OnCanceled2 NotificationId = %{public}d", request->GetNotificationRequest().GetNotificationId());
    ANS_LOGI("OnCanceled2 sortingMap size = %{public}d", sortingMap->GetKey().size());
    ANS_LOGI("OnCanceled2 deleteReason  = %{public}d", deleteReason);

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(canceCallbackInfo_.env, &loop);
#endif  // NAPI_VERSION >= 2

    NotificationReceiveDataWorker *dataWorker = new NotificationReceiveDataWorker();
    dataWorker->request = request;
    dataWorker->sortingMap = sortingMap;
    dataWorker->deleteReason = deleteReason;
    dataWorker->env = canceCallbackInfo_.env;
    dataWorker->ref = canceCallbackInfo_.ref;

    uv_work_t *work = new uv_work_t;
    work->data = (void *)dataWorker;

    uv_queue_work(loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ANS_LOGI("OnCanceled2 uv_work_t start");
            NotificationReceiveDataWorker *dataWorkerData = (NotificationReceiveDataWorker *)work->data;
            if (dataWorkerData == nullptr) {
                ANS_LOGE("dataWorkerData is null");
                return;
            }
            napi_value result;
            napi_create_object(dataWorkerData->env, &result);
            int error = 0;
            if (!SetSubscriberCallbackData(dataWorkerData->env,
                    dataWorkerData->request,
                    dataWorkerData->sortingMap,
                    dataWorkerData->deleteReason,
                    result)) {
                result = Common::NapiGetNull(dataWorkerData->env);
                error = ERROR;
            } else {
                error = NO_ERROR;
            }
            Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, error, result);

            delete dataWorkerData;
            dataWorkerData = nullptr;
            delete work;
            work = nullptr;
        });
}

void SubscriberInstance::OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &request)
{
    ANS_LOGI("OnConsumed start");

    if (consumeCallbackInfo_.ref == nullptr) {
        ANS_LOGI("consume callback unset");
        return;
    }

    if (request == nullptr) {
        ANS_LOGE("request is null");
        return;
    }
    ANS_LOGI("OnConsumed NotificationId = %{public}d", request->GetNotificationRequest().GetNotificationId());

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(consumeCallbackInfo_.env, &loop);
#endif  // NAPI_VERSION >= 2

    NotificationReceiveDataWorker *dataWorker = new NotificationReceiveDataWorker();
    dataWorker->request = request;
    dataWorker->env = consumeCallbackInfo_.env;
    dataWorker->ref = consumeCallbackInfo_.ref;

    uv_work_t *work = new uv_work_t;
    work->data = (void *)dataWorker;

    uv_queue_work(loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ANS_LOGI("OnConsumed uv_work_t start");
            NotificationReceiveDataWorker *dataWorkerData = (NotificationReceiveDataWorker *)work->data;
            if (dataWorkerData == nullptr) {
                ANS_LOGE("dataWorkerData is null");
                return;
            }
            napi_value result;
            napi_create_object(dataWorkerData->env, &result);
            int error = 0;
            if (!SetSubscriberCallbackData(dataWorkerData->env, dataWorkerData->request, result)) {
                result = Common::NapiGetNull(dataWorkerData->env);
                error = ERROR;
            } else {
                error = NO_ERROR;
            }
            Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, error, result);

            delete dataWorkerData;
            dataWorkerData = nullptr;
            delete work;
            work = nullptr;
        });
}

void SubscriberInstance::OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &request,
    const std::shared_ptr<NotificationSortingMap> &sortingMap)
{
    ANS_LOGI("OnConsumed2 start");

    if (consumeCallbackInfo_.ref == nullptr) {
        ANS_LOGI("consume callback unset");
        return;
    }

    if (request == nullptr) {
        ANS_LOGE("request is null");
        return;
    }

    if (sortingMap == nullptr) {
        ANS_LOGE("sortingMap is null");
        return;
    }
    ANS_LOGI("OnConsumed2 NotificationId = %{public}d", request->GetNotificationRequest().GetNotificationId());
    ANS_LOGI("OnConsumed2 sortingMap size = %{public}d", sortingMap->GetKey().size());

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(consumeCallbackInfo_.env, &loop);
#endif  // NAPI_VERSION >= 2

    NotificationReceiveDataWorker *dataWorker = new NotificationReceiveDataWorker();
    dataWorker->request = request;
    dataWorker->sortingMap = sortingMap;
    dataWorker->env = consumeCallbackInfo_.env;
    dataWorker->ref = consumeCallbackInfo_.ref;

    uv_work_t *work = new uv_work_t;
    work->data = (void *)dataWorker;

    uv_queue_work(loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ANS_LOGI("OnConsumed2 uv_work_t start");
            NotificationReceiveDataWorker *dataWorkerData = (NotificationReceiveDataWorker *)work->data;
            if (dataWorkerData == nullptr) {
                ANS_LOGE("dataWorkerData is null");
                return;
            }
            napi_value result;
            napi_create_object(dataWorkerData->env, &result);
            int error = 0;
            if (!SetSubscriberCallbackData(dataWorkerData->env,
                    dataWorkerData->request,
                    dataWorkerData->sortingMap,
                    dataWorkerData->deleteReason,
                    result)) {
                result = Common::NapiGetNull(dataWorkerData->env);
                error = ERROR;
            } else {
                error = NO_ERROR;
            }
            Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, error, result);

            delete dataWorkerData;
            dataWorkerData = nullptr;
            delete work;
            work = nullptr;
        });
}

void SubscriberInstance::OnUpdate(const std::shared_ptr<NotificationSortingMap> &sortingMap)
{
    ANS_LOGI("OnUpdate start");

    if (updateCallbackInfo_.ref == nullptr) {
        ANS_LOGI("update callback unset");
        return;
    }

    if (sortingMap == nullptr) {
        ANS_LOGE("sortingMap is null");
        return;
    }
    ANS_LOGI("OnUpdate sortingMap size = %{public}d", sortingMap->GetKey().size());

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(updateCallbackInfo_.env, &loop);
#endif  // NAPI_VERSION >= 2

    NotificationReceiveDataWorker *dataWorker = new NotificationReceiveDataWorker();
    dataWorker->sortingMap = sortingMap;
    dataWorker->env = updateCallbackInfo_.env;
    dataWorker->ref = updateCallbackInfo_.ref;

    uv_work_t *work = new uv_work_t;
    work->data = (void *)dataWorker;

    uv_queue_work(loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ANS_LOGI("OnUpdate uv_work_t start");
            NotificationReceiveDataWorker *dataWorkerData = (NotificationReceiveDataWorker *)work->data;
            if (dataWorkerData == nullptr) {
                ANS_LOGE("dataWorkerData is null");
                return;
            }
            napi_value result;
            napi_create_object(dataWorkerData->env, &result);
            int error = 0;
            if (!Common::SetNotificationSortingMap(dataWorkerData->env, dataWorkerData->sortingMap, result)) {
                result = Common::NapiGetNull(dataWorkerData->env);
                error = ERROR;
            } else {
                error = NO_ERROR;
            }
            Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, error, result);

            delete dataWorkerData;
            dataWorkerData = nullptr;
            delete work;
            work = nullptr;
        });
}

void SubscriberInstance::OnSubscribeResult(NotificationConstant::SubscribeResult result)
{
    ANS_LOGI("OnSubscribeResult start");

    if (subscribeCallbackInfo_.ref == nullptr) {
        ANS_LOGI("subscribe callback unset");
        return;
    }
    ANS_LOGI("OnSubscribeResult result = %{public}d", result);

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(subscribeCallbackInfo_.env, &loop);
#endif  // NAPI_VERSION >= 2

    NotificationReceiveDataWorker *dataWorker = new NotificationReceiveDataWorker();
    dataWorker->result = (int)result;
    dataWorker->env = subscribeCallbackInfo_.env;
    dataWorker->ref = subscribeCallbackInfo_.ref;

    uv_work_t *work = new uv_work_t;
    work->data = (void *)dataWorker;

    uv_queue_work(loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ANS_LOGI("OnSubscribeResult uv_work_t start");
            NotificationReceiveDataWorker *dataWorkerData = (NotificationReceiveDataWorker *)work->data;
            if (dataWorkerData == nullptr) {
                ANS_LOGE("dataWorkerData is null");
                return;
            }
            // result: number
            napi_value result;
            napi_create_int32(dataWorkerData->env, dataWorkerData->result, &result);

            Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, NO_ERROR, result);

            delete dataWorkerData;
            dataWorkerData = nullptr;
            delete work;
            work = nullptr;
        });
}

void SubscriberInstance::OnUnsubscribeResult(NotificationConstant::SubscribeResult result)
{
    ANS_LOGI("OnUnsubscribeResult start");

    if (unsubscribeCallbackInfo_.ref == nullptr) {
        ANS_LOGI("unsubscribe callback unset");
        return;
    }
    ANS_LOGI("OnUnsubscribeResult result = %{public}d", result);

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(unsubscribeCallbackInfo_.env, &loop);
#endif  // NAPI_VERSION >= 2

    NotificationReceiveDataWorker *dataWorker = new NotificationReceiveDataWorker();
    dataWorker->result = (int)result;
    dataWorker->env = unsubscribeCallbackInfo_.env;
    dataWorker->ref = unsubscribeCallbackInfo_.ref;

    uv_work_t *work = new uv_work_t;
    work->data = (void *)dataWorker;

    uv_queue_work(loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ANS_LOGI("OnUnsubscribeResult uv_work_t start");
            NotificationReceiveDataWorker *dataWorkerData = (NotificationReceiveDataWorker *)work->data;
            if (dataWorkerData == nullptr) {
                ANS_LOGE("dataWorkerData is null");
                return;
            }
            // result: number
            napi_value result;
            napi_create_int32(dataWorkerData->env, dataWorkerData->result, &result);

            Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, NO_ERROR, result);

            delete dataWorkerData;
            dataWorkerData = nullptr;
            delete work;
            work = nullptr;
        });
}

void SubscriberInstance::OnDied()
{
    ANS_LOGI("OnDied start");

    if (dieCallbackInfo_.ref == nullptr) {
        ANS_LOGE("die callback unset");
        return;
    }

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(dieCallbackInfo_.env, &loop);
#endif  // NAPI_VERSION >= 2

    NotificationReceiveDataWorker *dataWorker = new NotificationReceiveDataWorker();
    dataWorker->env = dieCallbackInfo_.env;
    dataWorker->ref = dieCallbackInfo_.ref;

    uv_work_t *work = new uv_work_t;
    work->data = (void *)dataWorker;

    uv_queue_work(loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ANS_LOGI("OnDied uv_work_t start");
            NotificationReceiveDataWorker *dataWorkerData = (NotificationReceiveDataWorker *)work->data;
            if (dataWorkerData == nullptr) {
                ANS_LOGE("dataWorkerData is null");
                return;
            }

            Common::SetCallback(
                dataWorkerData->env, dataWorkerData->ref, NO_ERROR, Common::NapiGetNull(dataWorkerData->env));

            delete dataWorkerData;
            dataWorkerData = nullptr;
            delete work;
            work = nullptr;
        });
}

void SubscriberInstance::OnDisturbModeChanged(int disturbMode)
{
    ANS_LOGI("OnDisturbModeChanged start");

    if (disturbModeCallbackInfo_.ref == nullptr) {
        ANS_LOGI("disturbModeChange callback unset");
        return;
    }
    ANS_LOGI("OnDisturbModeChanged disturbMode = %{public}d", disturbMode);

    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(disturbModeCallbackInfo_.env, &loop);
#endif  // NAPI_VERSION >= 2

    NotificationReceiveDataWorker *dataWorker = new NotificationReceiveDataWorker();
    dataWorker->disturbMode = disturbMode;
    dataWorker->env = disturbModeCallbackInfo_.env;
    dataWorker->ref = disturbModeCallbackInfo_.ref;

    uv_work_t *work = new uv_work_t;
    work->data = (void *)dataWorker;

    uv_queue_work(loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ANS_LOGI("OnDisturbModeChanged uv_work_t start");
            NotificationReceiveDataWorker *dataWorkerData = (NotificationReceiveDataWorker *)work->data;
            if (dataWorkerData == nullptr) {
                ANS_LOGE("dataWorkerData is null");
                return;
            }
            // disturbMode: number
            napi_value result;
            napi_create_int32(dataWorkerData->env, dataWorkerData->disturbMode, &result);

            Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, NO_ERROR, result);

            delete dataWorkerData;
            dataWorkerData = nullptr;
            delete work;
            work = nullptr;
        });
}

void SubscriberInstance::SetCancelCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    canceCallbackInfo_.env = env;
    canceCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetConsumeCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    consumeCallbackInfo_.env = env;
    consumeCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetUpdateCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    updateCallbackInfo_.env = env;
    updateCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetSubscribeCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    subscribeCallbackInfo_.env = env;
    subscribeCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetUnsubscribeCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    unsubscribeCallbackInfo_.env = env;
    unsubscribeCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetDieCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    dieCallbackInfo_.env = env;
    dieCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetDisturbModeCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    disturbModeCallbackInfo_.env = env;
    disturbModeCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetCallbackInfo(const napi_env &env, const std::string &type, const napi_ref &ref)
{
    if (type == CONSUME) {
        SetConsumeCallbackInfo(env, ref);
    } else if (type == CANCEL) {
        SetCancelCallbackInfo(env, ref);
    } else if (type == UPDATE) {
        SetUpdateCallbackInfo(env, ref);
    } else if (type == SUBSCRIBE) {
        SetSubscribeCallbackInfo(env, ref);
    } else if (type == UNSUBSCRIBE) {
        SetUnsubscribeCallbackInfo(env, ref);
    } else if (type == DIE) {
        SetDieCallbackInfo(env, ref);
    } else if (type == DISTURB_MODE_CHANGE) {
        SetDisturbModeCallbackInfo(env, ref);
    } else {
        ANS_LOGI("type is error");
    }
}

napi_value ParseParametersByOn(const napi_env &env, const size_t &argc, const napi_value (&argv)[ON_MAX_PARA],
    std::string &type, napi_ref &callback)
{
    ANS_LOGI("napi_value ParseParametersByOn start");
    napi_valuetype valuetype;
    size_t strLen = 0;
    char str[STR_MAX_SIZE] = {0};

    if (argc >= ON_MAX_PARA) {
        // argv[0]:type
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], str, STR_MAX_SIZE - 1, &strLen));
        type = str;
        if (!(type == CONSUME || type == CANCEL || type == UPDATE || type == SUBSCRIBE || type == UNSUBSCRIBE ||
                type == DIE || type == DISTURB_MODE_CHANGE)) {
            ANS_LOGI("type parameter is error");
            return nullptr;
        }

        // argv[1]:callback
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[1], 1, &callback);
    }
    return Common::NapiGetNull(env);
}

napi_value On(napi_env env, napi_callback_info info)
{
    ANS_LOGI("On start");

    size_t argc = ON_MAX_PARA;
    napi_value argv[ON_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= ON_MAX_PARA, "Wrong number of arguments");

    std::string type;
    napi_ref callback = 0;
    if (ParseParametersByOn(env, argc, argv, type, callback) == nullptr) {
        return Common::NapiGetNull(env);
    }

    SubscriberInstance *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    ANS_LOGI("On objectInfo = %{public}p", objectInfo);

    AsyncCallbackInfoOn *asynccallbackinfo = new AsyncCallbackInfoOn{
        .env = env, .asyncWork = nullptr, .objectInfo = objectInfo, .type = type, .callback = callback};

    if (!AddAsyncCallbackInfoOn(objectInfo, asynccallbackinfo)) {
        return Common::NapiGetNull(env);
    }

    napi_value resourceName;
    napi_create_string_latin1(env, "on", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("On napi_create_async_work start");
            AsyncCallbackInfoOn *asynccallbackinfo = (AsyncCallbackInfoOn *)data;
            ANS_LOGI("On asynccallbackinfo->type = %{public}s", asynccallbackinfo->type.c_str());
            asynccallbackinfo->objectInfo->SetCallbackInfo(env, asynccallbackinfo->type, asynccallbackinfo->callback);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("On napi_create_async_work end");
            AsyncCallbackInfoOn *asynccallbackinfo = (AsyncCallbackInfoOn *)data;
            napi_delete_async_work(env, asynccallbackinfo->asyncWork);
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    return Common::NapiGetNull(env);
}

napi_value ParseParametersByOff(
    const napi_env &env, const size_t &argc, const napi_value (&argv)[OFF_MAX_PARA], std::string &type)
{
    ANS_LOGI("napi_value ParseParametersByOff start");
    napi_valuetype valuetype;
    size_t strLen = 0;
    char str[STR_MAX_SIZE] = {0};

    if (argc >= OFF_MAX_PARA) {
        // argv[0]:type
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
        NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], str, STR_MAX_SIZE - 1, &strLen));
        type = str;
        if (!(type == CONSUME || type == CANCEL || type == UPDATE || type == SUBSCRIBE || type == UNSUBSCRIBE ||
                type == DIE || type == DISTURB_MODE_CHANGE)) {
            ANS_LOGI("type parameter is error");
            return nullptr;
        }
    }
    return Common::NapiGetNull(env);
}

napi_value Off(napi_env env, napi_callback_info info)
{
    ANS_LOGI("Off start");

    size_t argc = OFF_MAX_PARA;
    napi_value argv[OFF_MAX_PARA];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    NAPI_ASSERT(env, argc >= OFF_MAX_PARA, "Wrong number of arguments");

    std::string type;
    if (ParseParametersByOff(env, argc, argv, type) == nullptr) {
        return Common::NapiGetNull(env);
    }

    SubscriberInstance *objectInfo = nullptr;
    napi_unwrap(env, thisVar, (void **)&objectInfo);
    ANS_LOGI("Off objectInfo = %{public}p", objectInfo);

    AsyncCallbackInfoOff *asynccallbackinfo = new AsyncCallbackInfoOff{
        .env = env, .asyncWork = nullptr, .objectInfo = objectInfo, .type = type, .callback = nullptr};

    napi_value resourceName;
    napi_create_string_latin1(env, "off", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Off napi_create_async_work start");
            AsyncCallbackInfoOff *asynccallbackinfo = (AsyncCallbackInfoOff *)data;
            ANS_LOGI("Off asynccallbackinfo->type = %{public}s", asynccallbackinfo->type.c_str());
            asynccallbackinfo->objectInfo->SetCallbackInfo(env, asynccallbackinfo->type, asynccallbackinfo->callback);
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Off napi_create_async_work end");
            AsyncCallbackInfoOff *asynccallbackinfo = (AsyncCallbackInfoOff *)data;
            napi_delete_async_work(env, asynccallbackinfo->asyncWork);

            DelAsyncCallbackInfoOn(asynccallbackinfo->objectInfo, asynccallbackinfo->type);

            if (asynccallbackinfo) {
                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));

    return Common::NapiGetNull(env);
}

napi_value CreateSubscriber(napi_env env, napi_callback_info info)
{
    ANS_LOGI("CreateSubscriber start");

    size_t argc = 0;
    napi_value argv[1];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == 0, "Wrong number of arguments");

    napi_value result = 0;
    napi_new_instance(env, g_NotificationSubscriber, 0, nullptr, &result);

    return result;
}

napi_value SubscriberConstructor(napi_env env, napi_callback_info info)
{
    ANS_LOGI("SubscriberConstructor start");

    size_t argc = 0;
    napi_value argv[1];
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    SubscriberInstance *objectInfo = nullptr;
    if (!CreateSubscriberInstanceRecord(&objectInfo)) {
        return Common::NapiGetNull(env);
    }
    ANS_LOGI("SubscriberConstructor  objectInfo = %{public}p", objectInfo);

    napi_wrap(env,
        thisVar,
        objectInfo,
        [](napi_env env, void *data, void *hint) {
            SubscriberInstance *objectInfo = (SubscriberInstance *)data;
            ANS_LOGI("SubscriberConstructor this = %{public}p, destruct", objectInfo);

            DelAsyncCallbackInfoOn(objectInfo);

            if (objectInfo) {
                delete objectInfo;
                objectInfo = nullptr;
            }
        },
        nullptr,
        nullptr);

    ANS_LOGI("SubscriberConstructor end");
    return thisVar;
}

bool CreateSubscriberInstanceRecord(SubscriberInstance **subscriber)
{
    ANS_LOGI("CreateSubscriberInstanceRecord start");

    *subscriber = new SubscriberInstance();

    if (*subscriber == nullptr) {
        ANS_LOGE("subscriber is null");
        return false;
    }

    std::vector<AsyncCallbackInfoOn *> info;
    g_SubscriberInstances[*subscriber] = info;

    return true;
}

bool AddAsyncCallbackInfoOn(SubscriberInstance *subscriber, AsyncCallbackInfoOn *asynccallbackinfo)
{
    ANS_LOGI("AddAsyncCallbackInfoOn start");

    if (subscriber == nullptr) {
        ANS_LOGE("subscriber is null");
        return false;
    }

    if (asynccallbackinfo == nullptr) {
        ANS_LOGE("asynccallbackinfo is null");
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto infoItem = g_SubscriberInstances.find(subscriber);
    if (infoItem != g_SubscriberInstances.end()) {
        ANS_LOGI("AddAsyncCallbackInfoOn AsyncCallbackInfoOn size = %{public}d", infoItem->second.size());
        for (auto asyncCallbackInfoOn : infoItem->second) {
            ANS_LOGI("AddAsyncCallbackInfoOn AsyncCallbackInfoOn ptr = %{public}p", asyncCallbackInfoOn);
            ANS_LOGI("AsyncCallbackInfoOn->type  = %{public}s", asyncCallbackInfoOn->type.c_str());
            if (asyncCallbackInfoOn->type == asynccallbackinfo->type) {
                asyncCallbackInfoOn = asynccallbackinfo;
                return true;
            }
        }
        g_SubscriberInstances[subscriber].emplace_back(asynccallbackinfo);
    } else {
        ANS_LOGE("the current subscriber does not exist");
        return false;
    }

    return true;
}

bool DelAsyncCallbackInfoOn(SubscriberInstance *subscriber, const std::string &type)
{
    ANS_LOGI("DelSubscriberInstance start");

    if (subscriber == nullptr) {
        ANS_LOGE("subscriber is null");
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    auto infoItem = g_SubscriberInstances.find(subscriber);
    if (infoItem != g_SubscriberInstances.end()) {
        ANS_LOGI("DelSubscriberInstance AsyncCallbackInfoOn size = %{public}d", infoItem->second.size());
        if (type.empty()) {
            for (auto asyncCallbackInfoOn : infoItem->second) {
                ANS_LOGI("DelSubscriberInstance AsyncCallbackInfoOn ptr = %{public}p", asyncCallbackInfoOn);
                delete asyncCallbackInfoOn;
                asyncCallbackInfoOn = nullptr;
            }
            g_SubscriberInstances.erase(subscriber);
        } else {
            for (auto it = infoItem->second.begin(); it != infoItem->second.end(); ++it) {
                ANS_LOGI("DelSubscriberInstance AsyncCallbackInfoOn ptr = %{public}p", *it);
                ANS_LOGI("DelSubscriberInstance  type = %{public}s", (*it)->type.c_str());
                if ((*it)->type == type) {
                    delete *it;
                    *it = nullptr;
                    infoItem->second.erase(it);
                    break;
                }
            }
            if (infoItem->second.size() == 0) {
                g_SubscriberInstances.erase(subscriber);
            }
        }
    } else {
        return false;
    }

    return true;

    ANS_LOGI("DelSubscriberInstance end");
}

bool HasNotificationSubscriber(SubscriberInstance *subscriber)
{
    ANS_LOGI("HasNotificationSubscriber start");

    if (subscriber == nullptr) {
        ANS_LOGE("subscriber is null");
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    auto infoItem = g_SubscriberInstances.find(subscriber);
    if (infoItem != g_SubscriberInstances.end()) {
        return true;
    } else {
        ANS_LOGE("HasNotificationSubscriber The curren subscriber does not exist");
        return false;
    }
}

napi_value SetSubscriberCallbackData(
    const napi_env &env, const std::shared_ptr<OHOS::Notification::Notification> &request, napi_value &result)
{
    ANS_LOGI("SetSubscriberCallbackData start");
    if (request == nullptr) {
        ANS_LOGE("request is null");
        return Common::NapiGetboolean(env, false);
    }

    // request: NotificationRequest
    napi_value requestResult;
    napi_create_object(env, &requestResult);
    if (!Common::SetNotificationRequest(env, request, requestResult)) {
        ANS_LOGE("SetNotificationRequest call failed");
        return Common::NapiGetboolean(env, false);
    }
    napi_set_named_property(env, result, "request", requestResult);
    return Common::NapiGetboolean(env, true);
}

napi_value SetSubscriberCallbackData(const napi_env &env,
    const std::shared_ptr<OHOS::Notification::Notification> &request,
    const std::shared_ptr<NotificationSortingMap> &sortingMap, int deleteReason, napi_value &result)
{
    ANS_LOGI("SetSubscriberCallbackData2 start");
    if (request == nullptr) {
        ANS_LOGE("request is null");
        return Common::NapiGetboolean(env, false);
    }

    if (sortingMap == nullptr) {
        ANS_LOGE("sortingMap is null");
        return Common::NapiGetboolean(env, false);
    }

    // request: NotificationRequest
    napi_value requestResult;
    napi_create_object(env, &requestResult);
    if (!Common::SetNotificationRequest(env, request, requestResult)) {
        ANS_LOGE("SetNotificationRequest call failed");
        return Common::NapiGetboolean(env, false);
    }
    napi_set_named_property(env, result, "request", requestResult);

    // sortingMap?: NotificationSortingMap
    napi_value sortingMapResult;
    napi_create_object(env, &sortingMapResult);
    if (!Common::SetNotificationSortingMap(env, sortingMap, sortingMapResult)) {
        ANS_LOGE("SetNotificationSortingMap call failed");
        return Common::NapiGetboolean(env, false);
    }
    napi_set_named_property(env, result, "sortingMap", sortingMapResult);

    // reason?: number
    napi_value value = 0;
    int outReason = 0;
    if (!Common::ReasonCToJS(deleteReason, outReason)) {
        return Common::NapiGetboolean(env, false);
    }
    napi_create_int32(env, outReason, &value);
    napi_set_named_property(env, result, "reason", value);

    return Common::NapiGetboolean(env, true);
}

napi_value NotificationSubscriberInit(napi_env env, napi_value exports)
{
    ANS_LOGI("NotificationSubscriberInit start");

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("off", Off),
    };

    NAPI_CALL(env,
        napi_define_class(env,
            "notificationSubscriber",
            NAPI_AUTO_LENGTH,
            SubscriberConstructor,
            nullptr,
            sizeof(properties) / sizeof(*properties),
            properties,
            &g_NotificationSubscriber));

    return exports;
}
}  // namespace NotificationNapi
}  // namespace OHOS
