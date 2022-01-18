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

#include <memory>

#include "gtest/gtest.h"

#include "distributed_screen_status_manager.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {
class DistributedScreenStatusManagerTest : public testing::Test {
public:
    static void SetUpTestCase(){};
    static void TearDownTestCase(){};
    void SetUp() override;
    void TearDown() override;
protected:
    std::shared_ptr<DistributedScreenStatusManager> DistributedScreenStatusManager_;
};

void DistributedScreenStatusManagerTest::SetUp()
{
    DistributedScreenStatusManager_ = DistributedScreenStatusManager::GetInstance();
}

void DistributedScreenStatusManagerTest::TearDown()
{
    DistributedScreenStatusManager_ = nullptr;
    DistributedScreenStatusManager::DestroyInstance();
}

/**
 * @tc.name      : DistributedScreenStatusManager_CheckRemoteDevicesIsUsing_00100
 * @tc.number    : CheckRemoteDevicesIsUsing_00100
 * @tc.desc      : Test CheckRemoteDevicesIsUsing function.
 */
HWTEST_F(DistributedScreenStatusManagerTest, CheckRemoteDevicesIsUsing_00100, Function | SmallTest | Level1)
{
    bool isUsing = true;

    EXPECT_EQ(DistributedScreenStatusManager_->CheckRemoteDevicesIsUsing(isUsing), ERR_OK);
}

/**
 * @tc.name      : DistributedScreenStatusManager_SetLocalScreenStatus_00100
 * @tc.number    : SetLocalScreenStatus_00100
 * @tc.desc      : Get distributed notification enable.
 */
HWTEST_F(DistributedScreenStatusManagerTest, SetLocalScreenStatus_00100, Function | SmallTest | Level1)
{
    bool screenOn = false;

    EXPECT_EQ(DistributedScreenStatusManager_->SetLocalScreenStatus(screenOn), ERR_OK);
}
}  // namespace Notification
}  // namespace OHOS