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

#include <gtest/gtest.h>

#include "ans_log_wrapper.h"
#include "reminder_request_timer.h"
#include "reminder_helper.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {
class ReminderRequestTimerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        ReminderHelper::CancelAllReminders();
    }
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown()
    {
        ReminderHelper::CancelAllReminders();
    }
};

/**
 * @tc.name: initCountDownTime_00100
 * @tc.desc: set countDownTime = 0 should throw exception.
 * @tc.type: FUNC
 * @tc.require: SR000GGTRC AR000GH8E8
 */
HWTEST_F(ReminderRequestTimerTest, initCountDownTime_00100, Function | SmallTest | Level1)
{
    try {
        uint64_t countDownTimeInSeconds = 0;
        auto rrc = std::make_shared<ReminderRequestTimer>(countDownTimeInSeconds);
        EXPECT_TRUE(false) << "countDownTime < 0 should throw exception.";
    } catch (const std::invalid_argument &e) {
        ANSR_LOGI("countDownTime < 0 throw exception.");
    }
}

/**
 * @tc.name: initCountDownTime_00200
 * @tc.desc: set countDownTime > (UINT64_MAX / 1000) should throw exception.
 * @tc.type: FUNC
 * @tc.require: SR000GGTRC AR000GH8E8
 */
HWTEST_F(ReminderRequestTimerTest, initCountDownTime_00200, Function | SmallTest | Level1)
{
    try {
        auto rrc = std::make_shared<ReminderRequestTimer>(UINT64_MAX / 1000);
        EXPECT_TRUE(false) << "countDownTime >= (UINT64_MAX / 1000) should throw exception.";
    } catch (const std::invalid_argument &e) {
        ANSR_LOGI("countDownTime > (UINT64_MAX / 1000) throw exception.");
    }
}


/**
 * @tc.name: initCountDownTime_00300
 * @tc.desc: set countDownTime with normal value.
 * @tc.type: FUNC
 * @tc.require: SR000GGTRC AR000GH8E8
 */
HWTEST_F(ReminderRequestTimerTest, initCountDownTime_00300, Function | SmallTest | Level1)
{
    uint64_t countDownTimeInSeconds = 1;
    auto rrc = std::make_shared<ReminderRequestTimer>(countDownTimeInSeconds);
    EXPECT_TRUE(rrc->GetInitInfo() == 1) << "countDownTime is not 1";

    countDownTimeInSeconds = 10;
    auto rrc2 = std::make_shared<ReminderRequestTimer>(countDownTimeInSeconds);
    EXPECT_TRUE(rrc2->GetInitInfo() == 10) << "countDownTime is not 10";

    countDownTimeInSeconds = 100;
    auto rrc3 = std::make_shared<ReminderRequestTimer>(countDownTimeInSeconds);
    EXPECT_TRUE(rrc3->GetInitInfo() == 100) << "countDownTime is not 1";
}
}
}