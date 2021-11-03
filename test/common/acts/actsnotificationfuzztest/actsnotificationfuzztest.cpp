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
#include "../../../resource/notificationfuzztest/include/notificationfuzztestmanager.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace testing::ext;
namespace OHOS {
namespace Notification {
class ActsNotificationFuzzTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ActsNotificationFuzzTest::SetUpTestCase()
{}

void ActsNotificationFuzzTest::TearDownTestCase()
{}

void ActsNotificationFuzzTest::SetUp()
{}

void ActsNotificationFuzzTest::TearDown()
{}

HWTEST_F(ActsNotificationFuzzTest, ACTS_FuzzTest_0100, Function | MediumTest | Level1)
{
    std::cout << "fuzztest start" << std::endl;
    NotificationFuzzTestManager::GetInstance()->StartFuzzTest();
    std::cout << "fuzztest end" << std::endl;
}
}  // namespace Notification
}  // namespace OHOS