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
#include "../include/notificationgetparam.h"

#include <cfloat>
#include <chrono>
#include <climits>
#include <functional>
#include <random>

#include "abs_shared_result_set.h"
#include "data_ability_predicates.h"
#include "values_bucket.h"
using namespace std;
using namespace OHOS::AppExecFwk;
using namespace OHOS::EventFwk;
using Uri = OHOS::Uri;

namespace {
static const int INDEX_ZERO = 0;
static const int INDEX_ONE = 1;
static const int INDEX_TWO = 2;
static const int INDEX_THREE = 3;
static const int INDEX_FOUR = 4;
static const int INDEX_FIVE = 5;
static const int INDEX_SIX = 6;
static const int INDEX_SEVEN = 7;
static const int INDEX_EIGHT = 8;
static const int INDEX_NINE = 9;
static const int INDEX_TEN = 10;
static const int CHAR_MINCOUNT = -128;
static const int CHAR_MAXCOUNT = 127;
static const int MAX_LENGTH = 255;
}  // namespace

namespace OHOS {
namespace Notification {
std::mutex TestAnsSubscriber::mutex = std::mutex();
bool GetBoolParam()
{
    bool param;
    if (GetIntParam() % INDEX_TWO == 0) {
        param = true;
    } else {
        param = false;
    }
    return param;
}

size_t GenRandom(size_t min, size_t max)
{
    std::random_device rd;
    static uniform_int_distribution<size_t> u(min, max);
    static default_random_engine e(rd());
    size_t param = u(e);
    return param;
}

int8_t GetS8Param()
{
    std::random_device rd;
    static uniform_int_distribution<int8_t> u(INT8_MIN, INT8_MAX);
    static default_random_engine e(rd());
    int8_t param = u(e);
    return param;
}
int16_t GetS16Param()
{
    std::random_device rd;
    static uniform_int_distribution<int16_t> u(INT16_MIN, INT16_MAX);
    static default_random_engine e(rd());
    int16_t param = u(e);
    return param;
}
int32_t GetS32Param()
{
    std::random_device rd;
    static uniform_int_distribution<int32_t> u(INT32_MIN, INT32_MAX);
    static default_random_engine e(rd());
    int32_t param = u(e);
    return param;
}

int64_t GetS64Param()
{
    std::random_device rd;
    static uniform_int_distribution<int64_t> u(INT64_MIN, INT64_MAX);
    static default_random_engine e(rd());
    int64_t param = u(e);
    return param;
}

template <class T>
T GetUnsignParam()
{
    std::random_device rd;
    static uniform_int_distribution<T> u;
    static default_random_engine e(rd());
    T param = u(e);
    return param;
}

size_t GetSizeTParam()
{
    size_t param = GetUnsignParam<size_t>();
    return param;
}

uint8_t GetU8Param()
{
    uint8_t param = GetUnsignParam<uint8_t>();
    return param;
}

unsigned int GetUIntParam()
{
    unsigned int param = GetUnsignParam<unsigned int>();
    return param;
}

uint16_t GetU16Param()
{
    uint16_t param = GetUnsignParam<uint16_t>();
    return param;
}

uint32_t GetU32Param()
{
    uint32_t param = GetUnsignParam<uint32_t>();
    return param;
}

uint64_t GetU64Param()
{
    uint64_t param = GetUnsignParam<uint64_t>();
    return param;
}

short GetShortParam()
{
    std::random_device rd;
    static uniform_int_distribution<short> u(SHRT_MIN, SHRT_MAX);
    static default_random_engine e(rd());
    short param = u(e);
    return param;
}

long GetLongParam()
{
    std::random_device rd;
    static uniform_int_distribution<long> u(LONG_MIN, LONG_MAX);
    static default_random_engine e(rd());
    long param = u(e);
    return param;
}

int GetIntParam()
{
    std::random_device rd;
    static uniform_int_distribution<> u(INT_MIN, INT_MAX);
    static default_random_engine e(rd());
    int param = u(e);
    return param;
}

double GetDoubleParam()
{
    double param = 0;
    std::random_device rd;
    static uniform_real_distribution<double> u(DBL_MIN, DBL_MAX);
    static default_random_engine e(rd());
    param = u(e);
    return param;
}

float GetFloatParam()
{
    float param = 0;
    std::random_device rd;
    static uniform_real_distribution<float> u(FLT_MIN, FLT_MAX);
    static default_random_engine e(rd());
    param = u(e);
    return param;
}

char GetCharParam()
{
    std::random_device rd;
    static uniform_int_distribution<> u(CHAR_MINCOUNT, CHAR_MAXCOUNT);
    static default_random_engine e(rd());
    char param = u(e);
    return param;
}

char32_t GetChar32Param()
{
    char32_t param = ' ';
    std::random_device rd;
    static uniform_int_distribution<char32_t> u;
    static default_random_engine e(rd());
    param = u(e);
    return param;
}

char *GetCharArryParam()
{
    static char param[256];
    size_t len = 0;
    string strparam = GetStringParam();
    if (!strparam.empty()) {
        len = strparam.size() + 1;
        if (len > sizeof(param)) {
            len = sizeof(param) - 1;
        }

        int ret = strcpy_s(param, len, strparam.c_str());
        if (ret == 0) {
            return param;
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

string GetStringParam()
{
    string param = "";
    char ch = GetCharParam();
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        ch = GetCharParam();
        param += ch;
    }
    return param;
}

template <class T>
vector<T> GetUnsignVectorParam()
{
    vector<T> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        T t = GetUnsignParam<T>();
        param.push_back(t);
    }
    return param;
}

template <class T>
T GetClassParam()
{
    T param;
    return param;
}

std::vector<bool> GetBoolVectorParam()
{
    vector<bool> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        int t = GetBoolParam();
        param.push_back(t);
    }
    return param;
}

std::vector<short> GetShortVectorParam()
{
    vector<short> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        short t = GetShortParam();
        param.push_back(t);
    }
    return param;
}

std::vector<long> GetLongVectorParam()
{
    vector<long> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        long t = GetLongParam();
        param.push_back(t);
    }
    return param;
}

vector<int> GetIntVectorParam()
{
    vector<int> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        int t = GetIntParam();
        param.push_back(t);
    }
    return param;
}

std::vector<float> GetFloatVectorParam()
{
    vector<float> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        float t = GetIntParam();
        param.push_back(t);
    }
    return param;
}

std::vector<double> GetDoubleVectorParam()
{
    vector<double> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        double t = GetIntParam();
        param.push_back(t);
    }
    return param;
}

vector<char> GetCharVectorParam()
{
    vector<char> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        char t = GetCharParam();
        param.push_back(t);
    }
    return param;
}

vector<char32_t> GetChar32VectorParam()
{
    vector<char32_t> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        char32_t t = GetChar32Param();
        param.push_back(t);
    }
    return param;
}

vector<string> GetStringVectorParam()
{
    vector<string> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        string t = GetStringParam();
        param.push_back(t);
    }
    return param;
}

vector<int8_t> GetS8VectorParam()
{
    vector<int8_t> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        int8_t temp = GetS8Param();
        param.push_back(temp);
    }
    return param;
}

vector<int16_t> GetS16VectorParam()
{
    vector<int16_t> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        int16_t temp = GetS16Param();
        param.push_back(temp);
    }
    return param;
}

vector<int32_t> GetS32VectorParam()
{
    vector<int32_t> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        int32_t temp = GetS32Param();
        param.push_back(temp);
    }
    return param;
}

vector<int64_t> GetS64VectorParam()
{
    vector<int64_t> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        int64_t temp = GetS64Param();
        param.push_back(temp);
    }
    return param;
}

std::shared_ptr<Parcel> GetParamParcel()
{
    return make_shared<Parcel>();
}
std::shared_ptr<Want> GetParamWant()
{
    return make_shared<Want>();
}

std::vector<std::shared_ptr<Want>> GetParamWantVector()
{
    vector<std::shared_ptr<Want>> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        std::shared_ptr<Want> t = GetParamWant();
        param.push_back(t);
    }
    return param;
}

OHOS::AAFwk::Operation GetParamOperation()
{
    return OHOS::AAFwk::Operation();
}

sptr<IRemoteObject> GetParamSptrRemote()
{
    return sptr<TestRemoteObject>();
}

std::shared_ptr<EventRunner> GetParamEventRunner()
{
    return EventRunner::Create(GetCharArryParam());
}
std::shared_ptr<OHOS::AppExecFwk::Ability> GetParamAbility()
{
    return make_shared<OHOS::AppExecFwk::Ability>();
}

std::shared_ptr<OHOS::AAFwk::WantParams> GetParamWantParams()
{
    return make_shared<OHOS::AAFwk::WantParams>();
}

OHOS::AppExecFwk::ApplicationFlag GetParamApplicationFlag()
{
    if (GetBoolParam()) {
        return OHOS::AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO;
    } else {
        return OHOS::AppExecFwk::ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMS;
    }
}

OHOS::AppExecFwk::ApplicationInfo GetParamApplicationInfo()
{
    return OHOS::AppExecFwk::ApplicationInfo();
}
OHOS::AppExecFwk::AbilityInfo GetParamAbilityInfo()
{
    return OHOS::AppExecFwk::AbilityInfo();
}

sptr<OHOS::AppExecFwk::IBundleStatusCallback> GetParamIBundleStatusCallback()
{
    return sptr<TestIBundleStatusCallback>();
}

std::shared_ptr<NotificationSorting> GetParamNotificationSorting()
{
    return std::make_shared<NotificationSorting>();
}

std::vector<NotificationSorting> GetParamNotificationSortingVector()
{
    vector<NotificationSorting> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        NotificationSorting t;
        param.push_back(t);
    }
    return param;
}

std::shared_ptr<NotificationSortingMap> GetParamNotificationSortingMap()
{
    if (GetBoolParam()) {
        return std::make_shared<NotificationSortingMap>();
    } else {
        std::vector<NotificationSorting> sortingList = GetParamNotificationSortingVector();
        return std::make_shared<NotificationSortingMap>(sortingList);
    }
}

std::shared_ptr<OHOS::Notification::NotificationSlot> GetParamNotificationSlot()
{
    return std::make_shared<OHOS::Notification::NotificationSlot>(GetParamSlotType());
}

sptr<OHOS::Notification::NotificationSlot> GetParamNotificationSlotSptr()
{
    sptr<OHOS::Notification::NotificationSlot> param = new OHOS::Notification::NotificationSlot(GetParamSlotType());
    return param;
}

OHOS::Notification::NotificationConstant::SlotType GetParamSlotType()
{
    switch (GetIntParam() % INDEX_FIVE) {
        case INDEX_ZERO:
            return OHOS::Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION;
            break;
        case INDEX_ONE:
            return OHOS::Notification::NotificationConstant::SlotType::SERVICE_REMINDER;
            break;
        case INDEX_TWO:
            return OHOS::Notification::NotificationConstant::SlotType::CONTENT_INFORMATION;
            break;
        case INDEX_THREE:
            return OHOS::Notification::NotificationConstant::SlotType::CUSTOM;
            break;
        case INDEX_FOUR:
            return OHOS::Notification::NotificationConstant::SlotType::OTHER;
            break;
        default:
            return OHOS::Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION;
            break;
    }
}
std::vector<sptr<OHOS::Notification::NotificationSlot>> GetParamNotificationSlotSptrVector()
{
    vector<sptr<NotificationSlot>> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        sptr<NotificationSlot> t = GetParamNotificationSlotSptr();
        param.push_back(t);
    }
    return param;
}

std::vector<OHOS::Notification::NotificationSlot> GetParamNotificationSlotVector()
{
    vector<NotificationSlot> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        NotificationSlot t = *GetParamNotificationSlot();
        param.push_back(t);
    }
    return param;
}

std::vector<OHOS::Notification::NotificationSlotGroup> GetParamNotificationSlotGroupVector()
{
    vector<OHOS::Notification::NotificationSlotGroup> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        OHOS::Notification::NotificationSlotGroup t = *GetParamNotificationSlotGroup();
        param.push_back(t);
    }
    return param;
}
sptr<OHOS::Notification::NotificationSlotGroup> GetParamNotificationSlotGroup()
{
    sptr<OHOS::Notification::NotificationSlotGroup> param =
        new OHOS::Notification::NotificationSlotGroup(GetStringParam(), GetStringParam());
    return param;
}
std::vector<sptr<OHOS::Notification::NotificationSlotGroup>> GetParamNotificationSlotGroupSptrVector()
{
    vector<sptr<OHOS::Notification::NotificationSlotGroup>> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        sptr<OHOS::Notification::NotificationSlotGroup> t = GetParamNotificationSlotGroup();
        param.push_back(t);
    }
    return param;
}
std::shared_ptr<OHOS::Notification::NotificationBundleOption> GetParamNotificationBundleOption()
{
    if (GetBoolParam()) {
        return std::make_shared<NotificationBundleOption>();
    } else {
        return std::make_shared<NotificationBundleOption>(GetStringParam(), GetS32Param());
    }
}
std::shared_ptr<OHOS::Notification::NotificationRequest> GetParamNotificationRequest()
{
    switch (GetIntParam() % INDEX_THREE) {
        case INDEX_ZERO:
            return std::make_shared<OHOS::Notification::NotificationRequest>();
            break;
        case INDEX_ONE:
            return std::make_shared<OHOS::Notification::NotificationRequest>(GetS32Param());
            break;
        case INDEX_TWO:
            return std::make_shared<OHOS::Notification::NotificationRequest>(GetParamContext(), GetS32Param());
            break;
        default:
            return std::make_shared<OHOS::Notification::NotificationRequest>();
            break;
    }
}

sptr<OHOS::Notification::NotificationRequest> GetParamNotificationRequestSptr()
{
    switch (GetIntParam() % INDEX_THREE) {
        case INDEX_ZERO: {
            sptr<OHOS::Notification::NotificationRequest> param = new OHOS::Notification::NotificationRequest();
            return param;
        }    break;
        case INDEX_ONE: {
            sptr<OHOS::Notification::NotificationRequest> param =
                new OHOS::Notification::NotificationRequest(GetS32Param());
            return param;
        }    break;
        case INDEX_TWO: {
            sptr<OHOS::Notification::NotificationRequest> param =
                new OHOS::Notification::NotificationRequest(GetParamContext(), GetS32Param());
            return param;
        }    break;
        default: {
            sptr<OHOS::Notification::NotificationRequest> param = new OHOS::Notification::NotificationRequest();
            return param;
        }    break;
    }
}

std::vector<sptr<OHOS::Notification::NotificationRequest>> GetParamNotificationRequestVector()
{
    vector<sptr<OHOS::Notification::NotificationRequest>> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        sptr<OHOS::Notification::NotificationRequest> t = GetParamNotificationRequestSptr();
        param.push_back(t);
    }
    return param;
}
sptr<OHOS::Notification::NotificationSortingMap> GetParamNotificationSortingMapSptr()
{
    sptr<OHOS::Notification::NotificationSortingMap> param;
    if (GetBoolParam()) {
        param = new OHOS::Notification::NotificationSortingMap();
    } else {
        std::vector<NotificationSorting> sortingList = GetParamNotificationSortingVector();
        param = new OHOS::Notification::NotificationSortingMap(sortingList);
    }
    return param;
}
OHOS::Notification::NotificationSlot::NotificationLevel GetParamNotificationLevel()
{
    switch (GetIntParam() % INDEX_SIX) {
        case INDEX_ZERO:
            return OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_NONE;
            break;
        case INDEX_ONE:
            return OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_MIN;
            break;
        case INDEX_TWO:
            return OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_LOW;
            break;
        case INDEX_THREE:
            return OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_DEFAULT;
            break;
        case INDEX_FOUR:
            return OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_HIGH;
            break;
        case INDEX_FIVE:
            return OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_UNDEFINED;
            break;
        default:
            return OHOS::Notification::NotificationSlot::NotificationLevel::LEVEL_NONE;
            break;
    }
}

std::shared_ptr<OHOS::Notification::NotificationSubscriber> GetParamNotificationSubscriber()
{
    return std::make_shared<TestAnsSubscriber>();
}

std::shared_ptr<OHOS::Notification::NotificationSubscribeInfo> GetParamNotificationSubscribeInfo()
{
    return std::make_shared<OHOS::Notification::NotificationSubscribeInfo>();
}

std::shared_ptr<OHOS::Notification::WantAgent::WantAgentInfo> GetParamWantAgentInfo()
{
    switch (GetIntParam() % INDEX_THREE) {
        case INDEX_ZERO:
            return std::make_shared<OHOS::Notification::WantAgent::WantAgentInfo>();
            break;
        case INDEX_ONE: {
            std::vector<std::shared_ptr<Want>> param = GetParamWantVector();
            return std::make_shared<OHOS::Notification::WantAgent::WantAgentInfo>(
                GetIntParam(), GetParamOperationType(), GetParamFlags(), param, GetParamWantParams());
        }    break;
        case INDEX_TWO: {
            std::vector<std::shared_ptr<Want>> param = GetParamWantVector();
            return std::make_shared<OHOS::Notification::WantAgent::WantAgentInfo>(
                GetIntParam(), GetParamOperationType(), GetParamFlagsVector(), param, GetParamWantParams());
        }    break;
        default:
            return std::make_shared<OHOS::Notification::WantAgent::WantAgentInfo>();
            break;
    }
}

OHOS::Notification::WantAgent::WantAgentConstant::OperationType GetParamOperationType()
{
    switch (GetIntParam() % INDEX_SIX) {
        case INDEX_ZERO:
            return OHOS::Notification::WantAgent::WantAgentConstant::OperationType::UNKNOWN_TYPE;
            break;
        case INDEX_ONE:
            return OHOS::Notification::WantAgent::WantAgentConstant::OperationType::START_ABILITY;
            break;
        case INDEX_TWO:
            return OHOS::Notification::WantAgent::WantAgentConstant::OperationType::START_ABILITIES;
            break;
        case INDEX_THREE:
            return OHOS::Notification::WantAgent::WantAgentConstant::OperationType::START_SERVICE;
            break;
        case INDEX_FOUR:
            return OHOS::Notification::WantAgent::WantAgentConstant::OperationType::SEND_COMMON_EVENT;
            break;
        case INDEX_FIVE:
            return OHOS::Notification::WantAgent::WantAgentConstant::OperationType::START_FOREGROUND_SERVICE;
            break;
        default:
            return OHOS::Notification::WantAgent::WantAgentConstant::OperationType::UNKNOWN_TYPE;
            break;
    }
}
OHOS::Notification::WantAgent::WantAgentConstant::Flags GetParamFlags()
{
    switch (GetIntParam() % INDEX_TEN) {
        case INDEX_ZERO:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::ONE_TIME_FLAG;
            break;
        case INDEX_ONE:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::NO_BUILD_FLAG;
            break;
        case INDEX_TWO:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::CANCEL_PRESENT_FLAG;
            break;
        case INDEX_THREE:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::UPDATE_PRESENT_FLAG;
            break;
        case INDEX_FOUR:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::CONSTANT_FLAG;
            break;
        case INDEX_FIVE:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::REPLACE_ELEMENT;
            break;
        case INDEX_SIX:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::REPLACE_ACTION;
            break;
        case INDEX_SEVEN:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::REPLACE_URI;
            break;
        case INDEX_EIGHT:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::REPLACE_ENTITIES;
            break;
        case INDEX_NINE:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::REPLACE_BUNDLE;
            break;
        default:
            return OHOS::Notification::WantAgent::WantAgentConstant::Flags::ONE_TIME_FLAG;
            break;
    }
}
std::vector<OHOS::Notification::WantAgent::WantAgentConstant::Flags> GetParamFlagsVector()
{
    vector<OHOS::Notification::WantAgent::WantAgentConstant::Flags> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        OHOS::Notification::WantAgent::WantAgentConstant::Flags t = GetParamFlags();
        param.push_back(t);
    }
    return param;
}

std::shared_ptr<OHOS::Notification::Notification> GetParamNotification()
{
    sptr<OHOS::Notification::NotificationRequest> param = GetParamNotificationRequestSptr();
    return std::make_shared<OHOS::Notification::Notification>(param);
}

sptr<OHOS::Notification::Notification> GetParamNotificationSptr()
{
    sptr<OHOS::Notification::NotificationRequest> t = GetParamNotificationRequestSptr();
    sptr<OHOS::Notification::Notification> param = new OHOS::Notification::Notification(t);
    return param;
}

std::vector<sptr<OHOS::Notification::Notification>> GetParamNotificationSptrVector()
{
    std::vector<sptr<OHOS::Notification::Notification>> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        sptr<OHOS::Notification::Notification> t = GetParamNotificationSptr();
        param.push_back(t);
    }
    return param;
}
std::shared_ptr<OHOS::Notification::WantAgent::PendingWant> GetParamPendingWant()
{
    sptr<AAFwk::IWantSender> target = new WantSender();
    sptr<IRemoteObject> whitelistToken = GetParamSptrRemote();
    if (GetBoolParam()) {
        return std::make_shared<OHOS::Notification::WantAgent::PendingWant>(target);
    } else {
        return std::make_shared<OHOS::Notification::WantAgent::PendingWant>(target, whitelistToken);
    }
}
std::shared_ptr<OHOS::Notification::WantAgent::WantAgent> GetParamWantAgent()
{
    if (GetBoolParam()) {
        return std::make_shared<OHOS::Notification::WantAgent::WantAgent>();
    } else {
        return std::make_shared<OHOS::Notification::WantAgent::WantAgent>(GetParamPendingWant());
    }
}
std::shared_ptr<OHOS::Notification::WantAgent::CompletedCallback> GetParamCompletedCallback()
{
    return std::make_shared<TestCompletedCallback>();
}
std::shared_ptr<OHOS::Notification::WantAgent::TriggerInfo> GetParamTriggerInfo()
{
    if (GetBoolParam()) {
        return std::make_shared<OHOS::Notification::WantAgent::TriggerInfo>();
    } else {
        return std::make_shared<OHOS::Notification::WantAgent::TriggerInfo>(
            GetStringParam(), GetParamWantParams(), GetParamWant(), GetIntParam());
    }
}
std::shared_ptr<OHOS::Notification::WantAgent::CancelListener> GetParamCancelListener()
{
    return std::make_shared<TestCancelListener>();
}

std::shared_ptr<OHOS::AppExecFwk::LauncherService> GetParamLauncherService()
{
    return make_shared<OHOS::AppExecFwk::LauncherService>();
}
std::vector<OHOS::AppExecFwk::LauncherAbilityInfo> GetParamLauncherAbilityInfoVector()
{
    std::vector<OHOS::AppExecFwk::LauncherAbilityInfo> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        OHOS::AppExecFwk::LauncherAbilityInfo t;
        param.push_back(t);
    }
    return param;
}
std::shared_ptr<OHOS::AppExecFwk::LauncherAbilityInfo> GetParamLauncherAbilityInfo()
{
    return make_shared<OHOS::AppExecFwk::LauncherAbilityInfo>();
}
std::vector<OHOS::AppExecFwk::LauncherShortcutInfo> GetParamLauncherShortcutInfoVector()
{
    std::vector<OHOS::AppExecFwk::LauncherShortcutInfo> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        OHOS::AppExecFwk::LauncherShortcutInfo t;
        param.push_back(t);
    }
    return param;
}
std::vector<OHOS::AppExecFwk::FormInfo> GetParamFormInfoVector()
{
    std::vector<OHOS::AppExecFwk::FormInfo> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        OHOS::AppExecFwk::FormInfo t;
        param.push_back(t);
    }
    return param;
}
std::vector<OHOS::AppExecFwk::ShortcutInfo> GetParamShortcutInfoVector()
{
    std::vector<OHOS::AppExecFwk::ShortcutInfo> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        OHOS::AppExecFwk::ShortcutInfo t;
        param.push_back(t);
    }
    return param;
}
std::vector<OHOS::AppExecFwk::ModuleUsageRecord> GetParamModuleUsageRecordVector()
{
    std::vector<OHOS::AppExecFwk::ModuleUsageRecord> param;
    size_t len = GenRandom(0, MAX_LENGTH);
    while (len--) {
        OHOS::AppExecFwk::ModuleUsageRecord t;
        param.push_back(t);
    }
    return param;
}
sptr<OHOS::AppExecFwk::OnPermissionChangedCallback> GetParamOnPermissionChangedCallback()
{
    return sptr<TestOnPermissionChangedCallback>();
}

std::shared_ptr<OHOS::AppExecFwk::TaskDispatcher> GetParamTaskDispatcher()
{
    switch (GetIntParam() % INDEX_FOUR) {
        case INDEX_ZERO:
            return GetParamContext()->GetMainTaskDispatcher();
            break;
        case INDEX_ONE:
            return GetParamContext()->CreateParallelTaskDispatcher(GetStringParam(), GetParamTaskPriority());
            break;
        case INDEX_TWO:
            return GetParamContext()->CreateSerialTaskDispatcher(GetStringParam(), GetParamTaskPriority());
            break;
        case INDEX_THREE:
            return GetParamContext()->GetGlobalTaskDispatcher(GetParamTaskPriority());
            break;
        default:
            return GetParamContext()->GetMainTaskDispatcher();
            break;
    }
}

OHOS::AppExecFwk::TaskPriority GetParamTaskPriority()
{
    switch (GetIntParam() % INDEX_THREE) {
        case INDEX_ZERO:
            return OHOS::AppExecFwk::TaskPriority::HIGH;
            break;
        case INDEX_ONE:
            return OHOS::AppExecFwk::TaskPriority::DEFAULT;
            break;
        case INDEX_TWO:
            return OHOS::AppExecFwk::TaskPriority::LOW;
            break;
        default:
            return OHOS::AppExecFwk::TaskPriority::HIGH;
            break;
    }
}

std::shared_ptr<OHOS::AppExecFwk::Runnable> GetParamRunnable()
{
    return std::make_shared<Runnable>([]() { std::cout << "Start task runnable."; });
}

std::shared_ptr<OHOS::AppExecFwk::Group> GetParamGroup()
{
    return std::make_shared<OHOS::AppExecFwk::Group>();
}

std::shared_ptr<IteratableTask<long>> GetParamIteratableTask()
{
    return std::make_shared<IteratableTask<long>>([](long data) { std::cout << "Start IteratableTask."; });
}

OHOS::Notification::NotificationConstant::SubscribeResult GetParamSubscribeResult()
{
    switch (GetIntParam() % INDEX_THREE) {
        case INDEX_ZERO:
            return OHOS::Notification::NotificationConstant::SubscribeResult::SUCCESS;
            break;
        case INDEX_ONE:
            return OHOS::Notification::NotificationConstant::SubscribeResult::PREMISSION_FAIL;
            break;
        case INDEX_TWO:
            return OHOS::Notification::NotificationConstant::SubscribeResult::RESOURCES_FAIL;
            break;
        default:
            return OHOS::Notification::NotificationConstant::SubscribeResult::SUCCESS;
            break;
    }
}

OHOS::AppExecFwk::MissionInformation GetParamMissionInformation()
{
    return OHOS::AppExecFwk::MissionInformation();
}

std::shared_ptr<OHOS::AppExecFwk::AbilityLifecycleCallbacks> GetParamAbilityLifecycleCallbacks()
{
    return std::make_shared<TestAbilityLifecycleCallbacks>();
}

std::shared_ptr<OHOS::AppExecFwk::BaseTaskDispatcher> GetParamBaseTaskDispatcher()
{
    std::shared_ptr<OHOS::AppExecFwk::TaskDispatcherContext> context =
        std::make_shared<OHOS::AppExecFwk::TaskDispatcherContext>();
    std::shared_ptr<OHOS::AppExecFwk::SpecDispatcherConfig> config =
        std::make_shared<SpecDispatcherConfig>(GetStringParam(), GetParamTaskPriority());
    std::shared_ptr<OHOS::AppExecFwk::SpecTaskDispatcher> specTaskDispatcher =
        std::make_shared<SpecTaskDispatcher>(config, GetParamEventRunner());
    switch (GetIntParam() % INDEX_THREE) {
        case INDEX_ZERO:
            return context->CreateSerialDispatcher(GetStringParam(), GetParamTaskPriority());
            break;
        case INDEX_ONE:
            return context->CreateParallelDispatcher(GetStringParam(), GetParamTaskPriority());
            break;
        case INDEX_TWO:
            return specTaskDispatcher;
            break;
        default:
            return context->CreateSerialDispatcher(GetStringParam(), GetParamTaskPriority());
            break;
    }
}

std::shared_ptr<OHOS::AAFwk::IAbilityManager> GetParamIAbilityManager()
{
    std::shared_ptr<OHOS::AAFwk::IAbilityManager> param =
        DelayedSingleton<OHOS::AAFwk::AbilityManagerService>::GetInstance();
    return param;
}

std::shared_ptr<OHOS::Notification::NotificationDoNotDisturbDate> GetParamNotificationDoNotDisturbDate()
{
    if (GetBoolParam()) {
        return std::make_shared<OHOS::Notification::NotificationDoNotDisturbDate>();
    } else {
        return std::make_shared<OHOS::Notification::NotificationDoNotDisturbDate>(
            GetParamDoNotDisturbType(), GetS64Param(), GetS64Param());
    }
}

OHOS::Notification::NotificationConstant::DoNotDisturbType GetParamDoNotDisturbType()
{
    switch (GetIntParam() % INDEX_FOUR) {
        case INDEX_ZERO:
            return OHOS::Notification::NotificationConstant::DoNotDisturbType::NONE;
            break;
        case INDEX_ONE:
            return OHOS::Notification::NotificationConstant::DoNotDisturbType::ONCE;
            break;
        case INDEX_TWO:
            return OHOS::Notification::NotificationConstant::DoNotDisturbType::DAILY;
            break;
        case INDEX_THREE:
            return OHOS::Notification::NotificationConstant::DoNotDisturbType::CLEARLY;
            break;
        default:
            return OHOS::Notification::NotificationConstant::DoNotDisturbType::NONE;
            break;
    }
}
}  // namespace Notification
}  // namespace OHOS