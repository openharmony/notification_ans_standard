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

#ifndef NAPI_WANT_AGENT_H
#define NAPI_WANT_AGENT_H

#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "want_agent_constant.h"
#include "completed_callback.h"
#include "context.h"
#include "want.h"
#include "want_params.h"
#include "ability.h"
#include "want_agent.h"
#include "trigger_info.h"

namespace OHOS {

using namespace OHOS::AppExecFwk;
using namespace OHOS::Notification;
using namespace OHOS::Notification::WantAgent;

class TriggerCompleteCallBack;

struct AsyncGetWantAgentCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    Notification::WantAgent::WantAgentConstant::OperationType operationType;
    int32_t requestCode = -1;
    std::vector<Notification::WantAgent::WantAgentConstant::Flags> wantAgentFlags;
    std::shared_ptr<AAFwk::WantParams> extraInfo;
    std::shared_ptr<AppExecFwk::Context> context;
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent;
};

struct AsyncGetBundleNameCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent;
    std::string bundleName;
};

struct AsyncGetUidCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent;
    int32_t uid;
};

struct AsyncGetWantCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent;
    std::shared_ptr<AAFwk::Want> want;
};

struct AsyncEqualCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgentFirst;
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgentSecond;
    bool result;
};

struct AsyncCancelCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent;
};

struct AsyncTriggerCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    bool callBackMode = false;
    std::shared_ptr<TriggerCompleteCallBack> triggerObj;
    std::shared_ptr<AppExecFwk::Context> context;
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent;
    Notification::WantAgent::TriggerInfo triggerInfo;
};

struct CallbackInfo {
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent;
    napi_env env;
    napi_ref ref;
};

struct TriggerReceiveDataWorker {
    napi_env env;
    napi_ref ref = 0;
    std::shared_ptr<Notification::WantAgent::WantAgent> wantAgent;
    AAFwk::Want want;
    int resultCode;
    std::string resultData;
    AAFwk::WantParams resultExtras;
};

class TriggerCompleteCallBack : public CompletedCallback {
public:
    TriggerCompleteCallBack();
    virtual ~TriggerCompleteCallBack();

public:
    virtual void OnSendFinished(const AAFwk::Want &want, int resultCode, const std::string &resultData,
        const AAFwk::WantParams &resultExtras) override;
    void SetCallbackInfo(const napi_env &env, const napi_ref &ref);
    void SetWantAgentInstance(const std::shared_ptr<Notification::WantAgent::WantAgent> &wantAgent);

private:
    CallbackInfo triggerCompleteInfo_;
};

napi_value WantAgentInit(napi_env env, napi_value exports);

void SetNamedPropertyByInteger(napi_env env, napi_value dstObj, int32_t objName, const char *propName);
napi_value WantAgentFlagsInit(napi_env env, napi_value exports);
napi_value WantAgentOperationTypeInit(napi_env env, napi_value exports);

napi_value NAPI_GetBundleName(napi_env env, napi_callback_info info);
napi_value NAPI_GetUid(napi_env env, napi_callback_info info);
napi_value NAPI_GetWant(napi_env env, napi_callback_info info);
napi_value NAPI_Cancel(napi_env env, napi_callback_info info);
napi_value NAPI_Trigger(napi_env env, napi_callback_info info);
napi_value NAPI_Equal(napi_env env, napi_callback_info info);
napi_value NAPI_GetWantAgent(napi_env env, napi_callback_info info);

napi_value GetCallbackErrorResult(napi_env env, int errCode);
napi_value WrapWantParam(const AAFwk::WantParams &extraInfo, napi_env env);
napi_value UnwrapWantParamArray(
    AAFwk::WantParams &extraInfo, napi_env env, std::string strProName, napi_value proValue);
napi_value UnwrapWantParam(AAFwk::WantParams &extraInfo, napi_env env, napi_value JsExtraInfo);
napi_value NapiGetNull(napi_env env);
napi_value WantAgentUnwrapWant(Want &param, napi_env env, napi_value jsWant);

std::string NapiGetStringParam(const AAFwk::WantParams &extraInfo, const std::string &key);
bool NapiGetBoolParam(const AAFwk::WantParams &extraInfo, const std::string &key, bool defaultValue);
short NapiGetShortParam(const AAFwk::WantParams &extraInfo, const std::string &key, short defaultValue);
int NapiGetIntParam(const AAFwk::WantParams &extraInfo, const std::string &key, const int defaultValue);
long NapiGetLongParam(const AAFwk::WantParams &extraInfo, const std::string &key, long defaultValue);
float NapiGetFloatParam(const AAFwk::WantParams &extraInfo, const std::string &key, float defaultValue);
double NapiGetDoubleParam(const AAFwk::WantParams &extraInfo, const std::string &key, double defaultValue);

void SetParam(AAFwk::WantParams &extraInfo, const std::string &key, const std::vector<std::string> &value);
void SetParam(AAFwk::WantParams &extraInfo, const std::string &key, const std::vector<int> &value);
void SetParam(AAFwk::WantParams &extraInfo, const std::string &key, const std::vector<long> &value);
void SetParam(AAFwk::WantParams &extraInfo, const std::string &key, const std::vector<bool> &value);
void SetParam(AAFwk::WantParams &extraInfo, const std::string &key, const std::vector<double> &value);
void WrapWantParamArray(const AAFwk::WantParams &extraInfo, napi_env env, const std::string &key,
    sptr<AAFwk::IArray> &ao, napi_value jsObject);
std::vector<std::string> GetStringArrayParam(const AAFwk::WantParams &extraInfo, const std::string &key);
std::vector<bool> GetBoolArrayParam(const AAFwk::WantParams &extraInfo, const std::string &key);
std::vector<int> GetIntArrayParam(const AAFwk::WantParams &extraInfo, const std::string &key);
std::vector<long> GetLongArrayParam(const AAFwk::WantParams &extraInfo, const std::string &key);
std::vector<double> GetDoubleArrayParam(const AAFwk::WantParams &extraInfo, const std::string &key);

}  // namespace OHOS
#endif  // NAPI_WANT_AGENT_H
