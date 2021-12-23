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

#include <gtest/gtest.h>

#include "reminder_request.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {
class ReminderRequestChild : public ReminderRequest {
public:
    ReminderRequestChild() : ReminderRequest() {};
};

class ReminderRequestTest : public testing::Test {
public:
    static void SetUpTestCase(){};
    static void TearDownTestCase(){};
    void SetUp(){};
    void TearDown(){};

    static const uint8_t REMINDER_STATUS_SHOWING;
};

const uint8_t ReminderRequestTest::REMINDER_STATUS_SHOWING = 4;

/**
 * @tc.number    : CanRemove_00100
 * @tc.name      :
 * @tc.desc      : When reminder init, CanRemove should return true.
 */
HWTEST_F(ReminderRequestTest, CanRemove_00100, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    EXPECT_TRUE(rrc->CanRemove()) << "When init, canRemove should be false";
}

/**
 * @tc.number    : CanRemove_00200
 * @tc.name      :
 * @tc.desc      : When reminder is shown, CanRemove should return false.
 */
HWTEST_F(ReminderRequestTest, CanRemove_00200, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    rrc->OnShow(false, true);
    EXPECT_FALSE(rrc->CanRemove()) << "When shown, canRemove should be false";
}

/**
 * @tc.number    : CanRemove_00300
 * @tc.name      :
 * @tc.desc      : When reminder close, CanRemove should return true.
 */
HWTEST_F(ReminderRequestTest, CanRemove_00300, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    rrc->OnShow(false, true);
    rrc->OnClose(false);
    EXPECT_TRUE(rrc->CanRemove()) << "When reminder is expired and closed, can remove should be false";
}

/**
 * @tc.number    : CanRemove_00400
 * @tc.name      :
 * @tc.desc      : When reminder is covered as same notification id, CanRemove should return true.
 */
HWTEST_F(ReminderRequestTest, CanRemove_00400, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    rrc->OnShow(false, true);
    rrc->OnSameNotificationIdCovered();
    EXPECT_TRUE(rrc->CanRemove()) << "When reminder is expired and covered by \
        sameNotification id, can remove should be true";
}

/**
 * @tc.number    : StateCheck_00100
 * @tc.name      :
 * @tc.desc      : When reminder init, state should be 0.
 */
HWTEST_F(ReminderRequestTest, StateCheck_00100, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    EXPECT_EQ(rrc->GetState(), 0) << "When init, state should be 0";
}

/**
 * @tc.number    : StateCheck_00200
 * @tc.name      :
 * @tc.desc      : When reminder close with param true, state REMINDER_STATUS_SHOWING should be unset.
 */
HWTEST_F(ReminderRequestTest, StateCheck_00200, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    rrc->OnClose(true);
    EXPECT_TRUE((rrc->GetState() & ReminderRequestTest::REMINDER_STATUS_SHOWING) == 0);
}

/**
 * @tc.number    : StateCheck_00300
 * @tc.name      :
 * @tc.desc      : When reminder close with param false, state REMINDER_STATUS_SHOWING should be unset.
 */
HWTEST_F(ReminderRequestTest, StateCheck_00300, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    rrc->OnClose(false);
    EXPECT_TRUE((rrc->GetState() & ReminderRequestTest::REMINDER_STATUS_SHOWING) == 0);
}

/**
 * @tc.number    : StateCheck_00400
 * @tc.name      :
 * @tc.desc      : When reminder is covered as same notification id, state REMINDER_STATUS_SHOWING should be unset.
 */
HWTEST_F(ReminderRequestTest, StateCheck_00400, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    rrc->OnSameNotificationIdCovered();
    EXPECT_TRUE((rrc->GetState() & ReminderRequestTest::REMINDER_STATUS_SHOWING) == 0);
}

/**
 * @tc.number    : StateCheck_00500
 * @tc.name      :
 * @tc.desc      : When reminder is shown with param true,true, state REMINDER_STATUS_SHOWING should be set.
 */
HWTEST_F(ReminderRequestTest, StateCheck_00500, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    rrc->OnShow(true, true);
    EXPECT_TRUE((rrc->GetState() & ReminderRequestTest::REMINDER_STATUS_SHOWING) != 0);
}

/**
 * @tc.number    : StateCheck_00600
 * @tc.name      :
 * @tc.desc      : When reminder is shown with param false,true, state REMINDER_STATUS_SHOWING should be set.
 */
HWTEST_F(ReminderRequestTest, StateCheck_00600, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    rrc->OnShow(false, true);
    EXPECT_TRUE((rrc->GetState() & ReminderRequestTest::REMINDER_STATUS_SHOWING) != 0);
}

/**
 * @tc.number    : StateCheck_00700
 * @tc.name      :
 * @tc.desc      : When reminder is shown with param true,false, state REMINDER_STATUS_SHOWING should not change.
 */
HWTEST_F(ReminderRequestTest, StateCheck_00700, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    uint8_t stateBefore = rrc->GetState();
    rrc->OnShow(true, false);
    EXPECT_EQ(rrc->GetState(), stateBefore);
}

/**
 * @tc.number    : StateCheck_00800
 * @tc.name      :
 * @tc.desc      : When reminder is shown with param false,false, state REMINDER_STATUS_SHOWING should be unset.
 */
HWTEST_F(ReminderRequestTest, StateCheck_00800, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    uint8_t stateBefore = rrc->GetState();
    rrc->OnShow(false, false);
    EXPECT_EQ(rrc->GetState(), stateBefore);
}

/**
 * @tc.number    : initReminderId_00100
 * @tc.name      :
 * @tc.desc      : When reminder create successfully, system should assign unique id to reminder.
 */
HWTEST_F(ReminderRequestTest, initReminderId_00100, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    rrc->InitReminderId();
    int32_t reminderIdBefore = rrc->GetReminderId();
    rrc->InitReminderId();
    int32_t reminderIdAfter = rrc->GetReminderId();
    EXPECT_EQ((reminderIdAfter - reminderIdBefore), 1);
}

/**
 * @tc.number    : setContent_00100
 * @tc.name      :
 * @tc.desc      : Test SetContent with normal parameters.
 */
HWTEST_F(ReminderRequestTest, setContent_00100, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    std::string content = "this is normal content";
    rrc->SetContent(content);
    EXPECT_EQ(rrc->GetContent(), content);
}

/**
 * @tc.number    : setContent_00200
 * @tc.name      :
 * @tc.desc      : Test SetContent parameters with special characters.
 */
HWTEST_F(ReminderRequestTest, setContent_00200, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    std::string content = "this is content with special characters: ~!@#$%^&*()-+";
    rrc->SetContent(content);
    EXPECT_EQ(rrc->GetContent(), content);
}

/**
 * @tc.number    : setExpiredContent_00100
 * @tc.name      :
 * @tc.desc      : Test SetExpiredContent with normal parameters.
 */
HWTEST_F(ReminderRequestTest, setExpiredContent_00100, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    std::string content = "this is normal content";
    rrc->SetExpiredContent(content);
    EXPECT_EQ(rrc->GetExpiredContent(), content);
}

/**
 * @tc.number    : setExpiredContent_00200
 * @tc.name      :
 * @tc.desc      : Test SetExpiredContent with special characters.
 */
HWTEST_F(ReminderRequestTest, setExpiredContent_00200, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    std::string content = "this is content with special characters: ~!@#$%^&*()-+";
    rrc->SetExpiredContent(content);
    EXPECT_EQ(rrc->GetExpiredContent(), content);
}

/**
 * @tc.number    : setTitle_00100
 * @tc.name      :
 * @tc.desc      : Test SetTitle with normal parameters.
 */
HWTEST_F(ReminderRequestTest, setTitle_00100, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    std::string content = "this is normal content";
    rrc->SetTitle(content);
    EXPECT_EQ(rrc->GetTitle(), content);
}

/**
 * @tc.number    : setTitle_00200
 * @tc.name      :
 * @tc.desc      : Test SetTitle with special characters.
 */
HWTEST_F(ReminderRequestTest, setTitle_00200, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    std::string content = "this is content with special characters: ~!@#$%^&*()-+";
    rrc->SetTitle(content);
    EXPECT_EQ(rrc->GetTitle(), content);
}

/**
 * @tc.number    : setNotificationId_00100
 * @tc.name      :
 * @tc.desc      : Test SetNotificationId parameters.
 */
HWTEST_F(ReminderRequestTest, setNotificationId_00100, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<ReminderRequestChild>();
    int32_t notificationId = 0;
    rrc->SetNotificationId(notificationId);
    EXPECT_EQ(rrc->GetNotificationId(), notificationId);
}
}
}