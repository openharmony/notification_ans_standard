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
#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_FRAMEWORKS_JS_NAPI_INCLUDE_REMINDER_NATIVE_MODULE_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_FRAMEWORKS_JS_NAPI_INCLUDE_REMINDER_NATIVE_MODULE_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace ReminderAgentNapi {
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((constructor)) void RegisterModule(void);
napi_value ConstantInit(napi_env env, napi_value exports);
napi_value ReminderAgentInit(napi_env env, napi_value exports);

static napi_value Init(napi_env env, napi_value exports);

#ifdef __cplusplus
}
#endif
}  // namespace OHOS
}  // namespace ReminderAgentNapi

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_FRAMEWORKS_JS_NAPI_INCLUDE_REMINDER_NATIVE_MODULE_H