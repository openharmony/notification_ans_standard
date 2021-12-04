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
#define private public
#define protected public

#include "ans_notification.h"
#include "iservice_registry.h"
#include "singleton.h"
#include "system_ability_definition.h"

#undef private
#undef protected

#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
namespace Notification {
class AnsDumpTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    std::shared_ptr<AnsNotification> ans_;
};

void AnsDumpTest::SetUpTestCase()
{}

void AnsDumpTest::TearDownTestCase()
{}

void AnsDumpTest::SetUp()
{
    if (!ans_) {
        ans_ = DelayedSingleton<AnsNotification>::GetInstance();
    }
}

void AnsDumpTest::TearDown()
{}

/*
 * @tc.number: AnsDumpTest_GetServiceObject_0100
 * @tc.name: Get Service Object
 * @tc.desc:
 */
HWTEST_F(AnsDumpTest, AnsDumpTest_GetServiceObject_0100, Function | MediumTest | Level0)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);

    sptr<IRemoteObject> remoteObject =
        systemAbilityManager->GetSystemAbility(ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID);
    EXPECT_NE(remoteObject, nullptr);
}

/*
 * @tc.number: AnsDumpTest_ShellDump_0100
 * @tc.name: Shell Dump
 * @tc.desc:
 */
HWTEST_F(AnsDumpTest, AnsDumpTest_ShellDump_0100, Function | MediumTest | Level0)
{
    std::vector<std::string> infos;
    if (ans_) {
        auto ret = ans_->ShellDump("active", infos);
        EXPECT_EQ(ret, 0);
    }
}
}  // namespace Notification
}  // namespace OHOS