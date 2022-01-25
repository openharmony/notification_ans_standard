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

#include "ans_log_wrapper.h"
#include "reminder_request_calendar.h"
#include "reminder_helper.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {
class ReminderRequestCalendarTest : public testing::Test {
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

    std::shared_ptr<ReminderRequestCalendar> CreateCalendar(tm &nowTime)
    {
        time_t now;
        time(&now);  // unit is seconds.
        tm *tmp = localtime(&now);
        if (tmp == nullptr) {
            return nullptr;
        }
        nowTime = *tmp;
        nowTime.tm_year = 0;
        nowTime.tm_mon = 0;
        nowTime.tm_mday = 1;
        nowTime.tm_hour = 1;
        nowTime.tm_min = 1;
        std::vector<uint8_t> repeatMonths;
        std::vector<uint8_t> repeatDays;
        repeatMonths.push_back(1);
        repeatDays.push_back(1);
        auto calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
        return calendar;
    }

    bool IsVectorEqual(std::vector<uint8_t> &vectorA, std::vector<uint8_t> &vectorB)
    {
        if (vectorA.size() != vectorB.size()) {
            return false;
        }
        if (vectorA.size() == 0) {
            return true;
        }
        auto vitA = vectorA.begin();
        auto vitB = vectorB.begin();
        while (vitA != vectorA.end()) {
            if (*vitA != *vitB) {
                return false;
            }
            ++vitA;
            ++vitB;
        }
        return true;
    }
};

/**
 * @tc.number    : initDateTime_00100
 * @tc.name      :
 * @tc.desc      : Check firstDesignateYear set successfully.
 */
HWTEST_F(ReminderRequestCalendarTest, initDateTime_00100, Function | SmallTest | Level1)
{
    struct tm nowTime;
    auto calendar = ReminderRequestCalendarTest::CreateCalendar(nowTime);
    if (calendar == nullptr) {
        EXPECT_TRUE(false) << "calendar is null";
    }
    int firstDesignateYear = calendar->GetActualTime(ReminderRequest::TimeTransferType::YEAR, nowTime.tm_year);
    EXPECT_TRUE(firstDesignateYear == calendar->GetFirstDesignateYear()) << "Set first designate year error.";
}

/**
 * @tc.number    : initDateTime_00200
 * @tc.name      :
 * @tc.desc      : Check firstDesignateMonth set successfully.
 */
HWTEST_F(ReminderRequestCalendarTest, initDateTime_00200, Function | SmallTest | Level1)
{
    struct tm nowTime;
    auto calendar = ReminderRequestCalendarTest::CreateCalendar(nowTime);
    if (calendar == nullptr) {
        EXPECT_TRUE(false) << "calendar is null";
    }
    int firstDesignateMonth = calendar->GetActualTime(ReminderRequest::TimeTransferType::MONTH, nowTime.tm_mon);
    EXPECT_TRUE(firstDesignateMonth == calendar->GetFirstDesignageMonth()) << "Set first designate month error.";
}

/**
 * @tc.number    : initDateTime_00300
 * @tc.name      :
 * @tc.desc      : Check firstDesignateDay set successfully.
 */
HWTEST_F(ReminderRequestCalendarTest, initDateTime_00300, Function | SmallTest | Level1)
{
    struct tm nowTime;
    auto calendar = ReminderRequestCalendarTest::CreateCalendar(nowTime);
    if (calendar == nullptr) {
        EXPECT_TRUE(false) << "calendar is null";
    }
    int firstDesignateDay = nowTime.tm_mday;
    EXPECT_TRUE(firstDesignateDay == calendar->GetFirstDesignateDay()) << "Set first designate day error.";
}

/**
 * @tc.number    : initDateTime_00400
 * @tc.name      :
 * @tc.desc      : Check repeatMonth set with normal value successfully.
 */
HWTEST_F(ReminderRequestCalendarTest, initDateTime_00400, Function | SmallTest | Level1)
{
    time_t now;
    time(&now);  // unit is seconds.
    tm *tmp = localtime(&now);
    if (tmp == nullptr) {
        EXPECT_TRUE(false) << "calendar is null";
    }
    struct tm nowTime = *tmp;

    std::vector<uint8_t> repeatMonths;
    std::vector<uint8_t> repeatDays;
    repeatMonths.push_back(1);
    repeatDays.push_back(1);
    auto calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
    std::vector<uint8_t> actualRepeatMonths = calendar->GetRepeatMonths();
    EXPECT_TRUE(ReminderRequestCalendarTest::IsVectorEqual(repeatMonths, actualRepeatMonths))
        << "Set repeat month with 1 error.";

    repeatMonths.clear();
    repeatMonths.push_back(12);
    calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
    actualRepeatMonths = calendar->GetRepeatMonths();
    EXPECT_TRUE(ReminderRequestCalendarTest::IsVectorEqual(repeatMonths, actualRepeatMonths))
        << "Set repeat month with 12 error.";

    repeatMonths.clear();
    for (uint8_t i = 1; i <= 12; i++) {
        repeatMonths.push_back(i);
    }
    calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
    actualRepeatMonths = calendar->GetRepeatMonths();
    EXPECT_TRUE(ReminderRequestCalendarTest::IsVectorEqual(repeatMonths, actualRepeatMonths))
        << "Set repeat month with 1~12 error.";
}

/**
 * @tc.number    : initDateTime_00500
 * @tc.name      :
 * @tc.desc      : Check repeatMonth set with exception value successfully.
 */
HWTEST_F(ReminderRequestCalendarTest, initDateTime_00500, Function | SmallTest | Level1)
{
    time_t now;
    time(&now);  // unit is seconds.
    tm *tmp = localtime(&now);
    if (tmp == nullptr) {
        EXPECT_TRUE(false) << "calendar is null";
    }
    tm nowTime = *tmp;
    nowTime.tm_year += 1;
    std::vector<uint8_t> repeatMonths;
    std::vector<uint8_t> repeatDays;
    repeatMonths.push_back(-1);
    repeatDays.push_back(1);
    auto calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
    std::vector<uint8_t> actualRepeatMonths = calendar->GetRepeatMonths();
    EXPECT_TRUE(actualRepeatMonths.size() == 0) << "Set repeat month with -1 error.";

    repeatMonths.clear();
    repeatMonths.push_back(13);
    calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
    actualRepeatMonths = calendar->GetRepeatMonths();
    EXPECT_TRUE(actualRepeatMonths.size() == 0) << "Set repeat month with 13 error.";

    for (uint8_t i = 1; i <= 12; i++) {
        repeatMonths.push_back(i);
    }
    repeatMonths.push_back(1);
    try {
        auto calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
        EXPECT_TRUE(false) << "length of repeat month > 12 should throw exception.";
    } catch (const std::invalid_argument &e) {
        ANSR_LOGI("length of repeat month > 12 throw exception.");
    }
}

/**
 * @tc.number    : initDateTime_00600
 * @tc.name      :
 * @tc.desc      : Check repeatDay set with nomal value successfully.
 */
HWTEST_F(ReminderRequestCalendarTest, initDateTime_00600, Function | SmallTest | Level1)
{
    time_t now;
    time(&now);  // unit is seconds.
    tm *tmp = localtime(&now);
    if (tmp == nullptr) {
        EXPECT_TRUE(false) << "calendar is null";
    }
    tm nowTime = *tmp;
    std::vector<uint8_t> repeatMonths;
    std::vector<uint8_t> repeatDays;
    repeatMonths.push_back(1);
    repeatDays.push_back(1);
    auto calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
    std::vector<uint8_t> actualRepeatDays = calendar->GetRepeatDays();
    EXPECT_TRUE(ReminderRequestCalendarTest::IsVectorEqual(repeatDays, actualRepeatDays))
        << "Set repeat day with 1 error.";

    repeatDays.clear();
    repeatDays.push_back(31);
    calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
    actualRepeatDays = calendar->GetRepeatDays();
    EXPECT_TRUE(ReminderRequestCalendarTest::IsVectorEqual(repeatDays, actualRepeatDays))
        << "Set repeat day with 31 error.";

    repeatDays.clear();
    for (uint8_t i = 1; i <= 31; i++) {
        repeatDays.push_back(i);
    }
    calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
    actualRepeatDays = calendar->GetRepeatDays();
    EXPECT_TRUE(ReminderRequestCalendarTest::IsVectorEqual(repeatDays, actualRepeatDays))
        << "Set repeat day with 1~31 error.";
}

/**
 * @tc.number    : initDateTime_00700
 * @tc.name      :
 * @tc.desc      : Check repeatDay set with exception value successfully.
 */
HWTEST_F(ReminderRequestCalendarTest, initDateTime_00700, Function | SmallTest | Level1)
{
    time_t now;
    time(&now);  // unit is seconds.
    tm *tmp = localtime(&now);
    if (tmp == nullptr) {
        EXPECT_TRUE(false) << "calendar is null";
    }
    tm nowTime = *tmp;
    nowTime.tm_year += 1;
    std::vector<uint8_t> repeatMonths;
    std::vector<uint8_t> repeatDays;
    repeatMonths.push_back(-1);
    repeatDays.push_back(-1);
    auto calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
    std::vector<uint8_t> actualRepeatDays = calendar->GetRepeatDays();
    EXPECT_TRUE(actualRepeatDays.size() == 0) << "Set repeat day with -1 error.";

    repeatDays.clear();
    repeatDays.push_back(32);
    calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
    actualRepeatDays = calendar->GetRepeatDays();
    EXPECT_TRUE(actualRepeatDays.size() == 0) << "Set repeat day with 32 error.";

    repeatDays.clear();
    for (uint8_t i = 1; i <= 31; i++) {
        repeatDays.push_back(i);
    }
    repeatDays.push_back(1);
    try {
        calendar = std::make_shared<ReminderRequestCalendar>(nowTime, repeatMonths, repeatDays);
        ANSR_LOGI("length of repeat month > 32 should throw exception.");
    } catch (const std::invalid_argument &e) {
        ANSR_LOGI("length of repeat month > 32 throw exception.");
    }
}

/**
 * @tc.number    : initDateTime_00800
 * @tc.name      :
 * @tc.desc      : Check hour set successfully.
 */
HWTEST_F(ReminderRequestCalendarTest, initDateTime_00800, Function | SmallTest | Level1)
{
    struct tm nowTime;
    auto calendar = ReminderRequestCalendarTest::CreateCalendar(nowTime);
    if (calendar == nullptr) {
        EXPECT_TRUE(false) << "calendar is null";
    }
    EXPECT_TRUE(1 == calendar->GetHour()) << "Set hour error.";
}

/**
 * @tc.number    : initDateTime_00900
 * @tc.name      :
 * @tc.desc      : Check minut set successfully.
 */
HWTEST_F(ReminderRequestCalendarTest, initDateTime_00900, Function | SmallTest | Level1)
{
    struct tm nowTime;
    auto calendar = ReminderRequestCalendarTest::CreateCalendar(nowTime);
    if (calendar == nullptr) {
        EXPECT_TRUE(false) << "calendar is null";
    }
    EXPECT_TRUE(1 == calendar->GetMinute()) << "Set minute error.";
    EXPECT_TRUE(0 == calendar->GetSecond()) << "Set seconds error.";
}
}
}