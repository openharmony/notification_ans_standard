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

#include "distributed_preferences.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {
class DistributedPreferencesTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

protected:
    std::shared_ptr<DistributedPreferences> distributedPreferences_;
};

void DistributedPreferencesTest::SetUp()
{
    distributedPreferences_ = DistributedPreferences::GetInstance();
}

void DistributedPreferencesTest::TearDown()
{
    distributedPreferences_ = nullptr;
    DistributedPreferences::DestroyInstance();
}

/**
 * @tc.name      : DistributedPreferences_SetDistributedEnable_00100
 * @tc.number    : SetDistributedEnable_00100
 * @tc.desc      : Set distributed notification enable.
 */
HWTEST_F(DistributedPreferencesTest, SetDistributedEnable_00100, Function | SmallTest | Level1)
{
    bool enable = true;

    EXPECT_EQ(distributedPreferences_->SetDistributedEnable(enable), ERR_OK);
}

/**
 * @tc.name      : DistributedPreferences_GetDistributedEnable_00100
 * @tc.number    : GetDistributedEnable_00100
 * @tc.desc      : Get distributed notification enable.
 */
HWTEST_F(DistributedPreferencesTest, GetDistributedEnable_00100, Function | SmallTest | Level1)
{
    bool enable;

    EXPECT_EQ(distributedPreferences_->GetDistributedEnable(enable), ERR_OK);
}

/**
 * @tc.name      : DistributedPreferences_SetDistributedBundleEnable_00100
 * @tc.number    : SetDistributedBundleEnable_00100
 * @tc.desc      : Set distributed notification enable of a bundle.
 */
HWTEST_F(DistributedPreferencesTest, SetDistributedBundleEnable_00100, Function | SmallTest | Level1)
{
    bool enable = true;
    sptr<NotificationBundleOption> bundleOption = new NotificationBundleOption("<bundleName>", 783);

    EXPECT_EQ(distributedPreferences_->SetDistributedBundleEnable(bundleOption, enable), ERR_OK);
}

/**
 * @tc.name      : DistributedPreferences_GetDistributedBundleEnable_00100
 * @tc.number    : GetDistributedBundleEnable_00100
 * @tc.desc      : Get distributed notification enable of a bundle.
 */
HWTEST_F(DistributedPreferencesTest, GetDistributedBundleEnable_00100, Function | SmallTest | Level1)
{
    bool enable = true;
    sptr<NotificationBundleOption> bundleOption = new NotificationBundleOption("<bundleName>", 783);

    EXPECT_EQ(distributedPreferences_->SetDistributedBundleEnable(bundleOption, enable), ERR_OK);
}

/**
 * @tc.name      : DistributedPreferences_ClearDataInRestoreFactorySettings_00100
 * @tc.number    : ClearDataInRestoreFactorySettings_00100
 * @tc.desc      : Clear all data when system restore factory settings.
 */
HWTEST_F(DistributedPreferencesTest, ClearDataInRestoreFactorySettings_00100, Function | SmallTest | Level1)
{
    EXPECT_EQ(distributedPreferences_->ClearDataInRestoreFactorySettings(), ERR_OK);
}
}  // namespace Notification
}  // namespace OHOS