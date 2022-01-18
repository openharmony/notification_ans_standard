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
#include <functional>

#include "ans_inner_errors.h"
#include "ans_manager_proxy.h"
#include "advanced_notification_service.h"
#include "context.h"
#include "datetime_ex.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "notification_helper.h"
#include "notification_json_convert.h"
#include "mock_bundle_manager.h"
#include "mock_ipc_skeleton.h"
#include "system_ability_definition.h"
#include "want_agent_info.h"
#include "want_agent_helper.h"
#include "want_params.h"
#include "ohos/aafwk/base/int_wrapper.h"

using namespace testing::ext;
namespace OHOS {
namespace Notification {
static sptr<ISystemAbilityManager> systemAbilityManager =
    SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
bool OnConsumedReceived = false;
bool OnCanceledReceived = false;
bool OnWantReceived = false;
const int32_t SLEEP_TIME = 5;
const int32_t ACTIVE_NUMS = 2;
const int32_t CASE_ONE = 1;
const int32_t CASE_TWO = 2;
const int32_t CASE_THREE = 3;
const int32_t CASE_FOUR = 4;
const int32_t CASE_FIVE = 5;
const int32_t CASE_SIX = 6;
const int32_t CASE_SEVEN = 7;
const int32_t CASE_EIGHT = 8;
const int32_t CASE_NINE = 9;
const int32_t CASE_TEN = 10;
const int32_t CASE_ELEVEN = 11;
const int32_t CASE_TWELVE = 12;
const int32_t CASE_THIRTEEN = 13;
const int32_t CASE_FOURTEEN = 14;
const int32_t CALLING_UID = 9999;

const int32_t PIXEL_MAP_TEST_WIDTH = 32;
const int32_t PIXEL_MAP_TEST_HEIGHT = 32;

const int32_t CANCELGROUP_NID = 10101;

std::mutex g_subscribe_mtx;
std::mutex g_consumed_mtx;
std::mutex g_unsubscribe_mtx;
std::mutex g_send_finished_mtx;
AAFwk::Want g_want;

const time_t TIME_OUT_SECONDS_LIMIT = 5;
const std::string CLASSIFICATION_ALARM {"alarm"};

class TestAnsSubscriber : public NotificationSubscriber {
public:
    virtual void OnConnected() override
    {
        g_subscribe_mtx.unlock();
    }

    virtual void OnDisconnected() override
    {
        g_unsubscribe_mtx.unlock();
    }

    virtual void OnDied() override
    {}

    virtual void OnUpdate(const std::shared_ptr<NotificationSortingMap> &sortingMap) override
    {}

    virtual void OnDoNotDisturbDateChange(const std::shared_ptr<NotificationDoNotDisturbDate> &date) override
    {}

    virtual void OnCanceled(const std::shared_ptr<Notification> &request) override
    {
        GTEST_LOG_(INFO) << "ANS_Interface_MT::OnCanceled request : " << request->GetNotificationRequest().Dump();
        OnCanceledReceived = true;
    }

    virtual void OnCanceled(const std::shared_ptr<Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap, int deleteReason) override
    {}

    virtual void OnConsumed(const std::shared_ptr<Notification> &request) override
    {
        OnConsumedReceived = true;
        g_consumed_mtx.unlock();
        NotificationRequest notificationRequest = request->GetNotificationRequest();
        if (CASE_ONE == notificationRequest.GetNotificationId()) {
            CheckCaseOneResult(notificationRequest);
        } else if (CASE_TWO == notificationRequest.GetNotificationId()) {
            CheckCaseTwoResult(notificationRequest);
        } else if (CASE_THREE == notificationRequest.GetNotificationId()) {
            CheckCaseThreeResult(notificationRequest);
        } else if (CASE_FOUR == notificationRequest.GetNotificationId()) {
            CheckCaseFourResult(notificationRequest);
        } else if (CASE_FIVE == notificationRequest.GetNotificationId()) {
            CheckCaseFiveResult(notificationRequest);
        } else if (CASE_SIX == notificationRequest.GetNotificationId()) {
            CheckCaseSixResult(notificationRequest);
        } else if (CASE_SEVEN == notificationRequest.GetNotificationId()) {
            CheckCaseSevenResult(notificationRequest);
        } else if (CASE_EIGHT == notificationRequest.GetNotificationId()) {
            CheckCaseEightResult(notificationRequest);
        } else if (CASE_NINE == notificationRequest.GetNotificationId()) {
            CheckCaseNineResult(notificationRequest);
        } else if (CASE_TEN == notificationRequest.GetNotificationId()) {
            EXPECT_EQ(NotificationConstant::SERVICE_REMINDER, notificationRequest.GetSlotType());
        } else if (CASE_ELEVEN == notificationRequest.GetNotificationId()) {
            EXPECT_EQ(NotificationConstant::SOCIAL_COMMUNICATION, notificationRequest.GetSlotType());
        } else if (CASE_TWELVE == notificationRequest.GetNotificationId()) {
            EXPECT_EQ(NotificationConstant::CUSTOM, notificationRequest.GetSlotType());
        } else if (CASE_THIRTEEN == notificationRequest.GetNotificationId()) {
            EXPECT_EQ(NotificationRequest::GroupAlertType::ALL, notificationRequest.GetGroupAlertType());
            EXPECT_EQ(true, notificationRequest.IsGroupOverview());
        } else if (CASE_FOURTEEN == notificationRequest.GetNotificationId()) {
            CheckCaseFourteenResult(notificationRequest);
        } else {
            GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish::OnConsumed do nothing!!!!!";
        }
    }

    virtual void OnConsumed(const std::shared_ptr<Notification> &request,
        const std::shared_ptr<NotificationSortingMap> &sortingMap) override
    {}

private:
    void CheckCaseOneResult(NotificationRequest notificationRequest)
    {
        std::shared_ptr<NotificationContent> notificationContent = notificationRequest.GetContent();
        if (notificationContent != nullptr) {
            EXPECT_EQ(NotificationContent::Type::MEDIA, notificationContent->GetContentType());
            std::shared_ptr<NotificationMediaContent> notificationMediaContent =
                std::static_pointer_cast<NotificationMediaContent>(notificationContent->GetNotificationContent());
            if (notificationMediaContent != nullptr) {
                EXPECT_EQ(nullptr, notificationMediaContent->GetAVToken());
                for (auto it : notificationMediaContent->GetShownActions()) {
                    EXPECT_EQ((uint32_t)0, it);
                }
            }
        }
        EXPECT_EQ(NotificationConstant::OTHER, notificationRequest.GetSlotType());
        for (auto str : notificationRequest.GetNotificationUserInputHistory()) {
            EXPECT_EQ("style", str);
        }
        EXPECT_EQ("bundleName", notificationRequest.GetOwnerBundleName());
        EXPECT_EQ("bundleName", notificationRequest.GetCreatorBundleName());
        EXPECT_EQ("ANS_Interface_MT_Publish_00100", notificationRequest.GetLabel());
        std::shared_ptr<NotificationRequest> NotificationRequestPtr = notificationRequest.GetPublicNotification();
        if (NotificationRequestPtr != nullptr) {
            EXPECT_EQ("ANS_Interface_MT_Publish_00100_REQUEST", NotificationRequestPtr->GetLabel());
        }

        // pixelmap
        auto littleIcon = notificationRequest.GetLittleIcon();
        Media::ImageInfo outImageInfo;
        littleIcon->GetImageInfo(outImageInfo);
        GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00100 :: littleIcon :: width : " << outImageInfo.size.width;
        GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00100 :: littleIcon :: height : " << outImageInfo.size.height;

        EXPECT_EQ(outImageInfo.size.width, PIXEL_MAP_TEST_WIDTH);
        EXPECT_EQ(outImageInfo.size.height, PIXEL_MAP_TEST_HEIGHT);
        EXPECT_EQ(outImageInfo.pixelFormat, Media::PixelFormat::ALPHA_8);
        EXPECT_EQ(outImageInfo.colorSpace, Media::ColorSpace::SRGB);

        EXPECT_NE(nullptr, notificationRequest.GetBigIcon());
        EXPECT_NE(nullptr, notificationRequest.GetLittleIcon());
        EXPECT_NE(nullptr, notificationRequest.GetLittleIcon());
        std::vector<std::shared_ptr<MessageUser>> messageUser = notificationRequest.GetMessageUsers();
        for (auto user : messageUser) {
            if (user != nullptr) {
                EXPECT_EQ("ANS_Interface_MT_Publish_00100_Message_User", user->GetName());
                EXPECT_EQ("key", user->GetKey());
                EXPECT_EQ(nullptr, user->GetPixelMap());
                EXPECT_EQ(Uri("."), user->GetUri());
                EXPECT_EQ(false, user->IsMachine());
                EXPECT_EQ(false, user->IsUserImportant());
            }
        }
    }

    void CheckCaseTwoResult(NotificationRequest notificationRequest)
    {
        EXPECT_EQ(NotificationConstant::OTHER, notificationRequest.GetSlotType());
        std::vector<std::shared_ptr<NotificationActionButton>> actionButtons = notificationRequest.GetActionButtons();
        for (auto actionButton : actionButtons) {
            std::vector<std::shared_ptr<NotificationUserInput>> userInputs = actionButton->GetUserInputs();
            for (auto userInput : userInputs) {
                EXPECT_EQ(NotificationConstant::FREE_FORM_INPUT, userInput->GetInputsSource(g_want));
                EXPECT_EQ(nullptr, userInput->GetInputsFromWant(g_want));
                std::map<std::string, std::shared_ptr<Uri>> map = userInput->GetMimeInputsFromWant(g_want, "");
                EXPECT_EQ(unsigned(0), map.size());
                EXPECT_EQ("inputKey", userInput->GetInputKey());
                EXPECT_NE(nullptr, userInput->GetAdditionalData());
                EXPECT_EQ(NotificationConstant::InputEditType::EDIT_DISABLED, userInput->GetEditType());
                for (auto option : userInput->GetOptions()) {
                    EXPECT_EQ("", option);
                }
                for (auto type : userInput->GetPermitMimeTypes()) {
                    EXPECT_EQ("mimeType", type);
                }
                EXPECT_EQ(false, userInput->IsMimeTypeOnly());
                EXPECT_EQ("tag", userInput->GetTag());
                EXPECT_EQ(false, userInput->IsPermitFreeFormInput());
            }
        }
    }

    void CheckCaseThreeResult(NotificationRequest notificationRequest)
    {
        EXPECT_EQ(NotificationConstant::OTHER, notificationRequest.GetSlotType());
        std::vector<std::shared_ptr<NotificationActionButton>> actionButtons = notificationRequest.GetActionButtons();
        for (auto actionButton : actionButtons) {
            EXPECT_EQ(nullptr, actionButton->GetIcon());
            EXPECT_EQ("title", actionButton->GetTitle());
            EXPECT_EQ(nullptr, actionButton->GetWantAgent());
            EXPECT_NE(nullptr, actionButton->GetAdditionalData());
            EXPECT_EQ(NotificationConstant::NONE_ACTION_BUTTON, actionButton->GetSemanticActionButton());
            for (auto userInputs : actionButton->GetMimeTypeOnlyUserInputs()) {
                EXPECT_EQ("onlyUserInputKey", userInputs->GetInputKey());
            }
            EXPECT_EQ(true, actionButton->IsAutoCreatedReplies());
            EXPECT_EQ(true, actionButton->IsContextDependent());
        }
    }

    void CheckCaseFourResult(NotificationRequest notificationRequest)
    {
        std::shared_ptr<NotificationContent> notificationContent = notificationRequest.GetContent();
        if (notificationContent != nullptr) {
            EXPECT_EQ(NotificationContent::Type::CONVERSATION, notificationContent->GetContentType());
            std::shared_ptr<NotificationConversationalContent> notificationConversationContent =
                std::static_pointer_cast<NotificationConversationalContent>(
                    notificationContent->GetNotificationContent());
            if (notificationConversationContent != nullptr) {
                EXPECT_EQ("ConversationTitle", notificationConversationContent->GetConversationTitle());
                EXPECT_EQ(false, notificationConversationContent->IsConversationGroup());
                std::vector<std::shared_ptr<NotificationConversationalMessage>> messagesPtr =
                    notificationConversationContent->GetAllConversationalMessages();
                EXPECT_EQ(unsigned(CASE_TWO), messagesPtr.size());
                MessageUser user = notificationConversationContent->GetMessageUser();
                EXPECT_EQ("key", user.GetKey());
                EXPECT_EQ("Name", user.GetName());
                EXPECT_EQ(nullptr, user.GetPixelMap());
                EXPECT_EQ(Uri("."), user.GetUri());
                EXPECT_EQ(false, user.IsUserImportant());
                EXPECT_EQ(false, user.IsMachine());
            }
        }
        EXPECT_EQ(NotificationConstant::OTHER, notificationRequest.GetSlotType());
    }

    void CheckCaseFiveResult(NotificationRequest notificationRequest)
    {
        std::shared_ptr<NotificationContent> notificationContent = notificationRequest.GetContent();
        if (notificationContent != nullptr) {
            EXPECT_EQ(NotificationContent::Type::MULTILINE, notificationContent->GetContentType());
            std::shared_ptr<NotificationMultiLineContent> notificationMultineContent =
                std::static_pointer_cast<NotificationMultiLineContent>(notificationContent->GetNotificationContent());
            if (notificationMultineContent != nullptr) {
                EXPECT_EQ("expandedtitle", notificationMultineContent->GetExpandedTitle());
                EXPECT_EQ("brieftext", notificationMultineContent->GetBriefText());
                for (auto allLine : notificationMultineContent->GetAllLines()) {
                    EXPECT_EQ("singleLine", allLine);
                }
            }
        }
        EXPECT_EQ(NotificationConstant::OTHER, notificationRequest.GetSlotType());
    }

    void CheckCaseSixResult(NotificationRequest notificationRequest)
    {
        std::shared_ptr<NotificationContent> notificationContent = notificationRequest.GetContent();
        if (notificationContent != nullptr) {
            EXPECT_EQ(NotificationContent::Type::PICTURE, notificationContent->GetContentType());
            std::shared_ptr<NotificationPictureContent> notificationPictureContent =
                std::static_pointer_cast<NotificationPictureContent>(notificationContent->GetNotificationContent());
            if (notificationPictureContent != nullptr) {
                EXPECT_EQ("expendedtitle", notificationPictureContent->GetExpandedTitle());
                EXPECT_EQ("brieftext", notificationPictureContent->GetBriefText());
                EXPECT_EQ(nullptr, notificationPictureContent->GetBigPicture());
            }
        }
        EXPECT_EQ(NotificationConstant::OTHER, notificationRequest.GetSlotType());
    }

    void CheckCaseSevenResult(NotificationRequest notificationRequest)
    {
        std::shared_ptr<NotificationContent> notificationContent = notificationRequest.GetContent();
        if (notificationContent != nullptr) {
            EXPECT_EQ(NotificationContent::Type::LONG_TEXT, notificationContent->GetContentType());
            std::shared_ptr<NotificationLongTextContent> notificationLongContent =
                std::static_pointer_cast<NotificationLongTextContent>(notificationContent->GetNotificationContent());
            if (notificationLongContent != nullptr) {
                EXPECT_EQ("expendedtitle", notificationLongContent->GetExpandedTitle());
                EXPECT_EQ("brieftext", notificationLongContent->GetBriefText());
                EXPECT_EQ("longtext", notificationLongContent->GetLongText());
            }
        }
        EXPECT_EQ(NotificationConstant::OTHER, notificationRequest.GetSlotType());
    }

    void CheckCaseEightResult(NotificationRequest notificationRequest)
    {
        EXPECT_EQ(NotificationConstant::CONTENT_INFORMATION, notificationRequest.GetSlotType());
        EXPECT_EQ(false, notificationRequest.IsInProgress());
        EXPECT_EQ(false, notificationRequest.IsUnremovable());
        EXPECT_EQ(0, notificationRequest.GetBadgeNumber());
        EXPECT_EQ(0, notificationRequest.GetDeliveryTime());
        EXPECT_EQ(false, notificationRequest.IsShowDeliveryTime());
        EXPECT_EQ(false, notificationRequest.IsPermitSystemGeneratedContextualActionButtons());
        EXPECT_EQ(false, notificationRequest.IsAlertOneTime());
        EXPECT_EQ(0, notificationRequest.GetAutoDeletedTime());
        EXPECT_EQ("classification", notificationRequest.GetClassification());
        EXPECT_EQ((uint32_t)0, notificationRequest.GetColor());
    }

    void CheckCaseNineResult(NotificationRequest notificationRequest)
    {
        EXPECT_EQ(NotificationConstant::OTHER, notificationRequest.GetSlotType());
        EXPECT_EQ(false, notificationRequest.IsColorEnabled());
        EXPECT_EQ(false, notificationRequest.IsCountdownTimer());
        EXPECT_EQ("groupvalue", notificationRequest.GetGroupName());
        EXPECT_EQ(true, notificationRequest.IsOnlyLocal());
        EXPECT_EQ("setting text", notificationRequest.GetSettingsText());
        EXPECT_EQ(false, notificationRequest.IsShowStopwatch());
        EXPECT_EQ("sortingkey", notificationRequest.GetSortingKey());
        EXPECT_EQ("statusbartext", notificationRequest.GetStatusBarText());
        EXPECT_EQ(false, notificationRequest.IsTapDismissed());
        auto visibleness = notificationRequest.GetVisibleness();
        EXPECT_EQ((int)NotificationConstant::VisiblenessType::PUBLIC, (int)visibleness);
        EXPECT_EQ(NotificationRequest::BadgeStyle::NONE, notificationRequest.GetBadgeIconStyle());
        EXPECT_EQ("shortcutid", notificationRequest.GetShortcutId());
        EXPECT_EQ(false, notificationRequest.IsFloatingIcon());
        EXPECT_EQ(0, notificationRequest.GetProgressMax());
        EXPECT_EQ(0, notificationRequest.GetProgressValue());
        EXPECT_EQ(false, notificationRequest.IsProgressIndeterminate());
    }

    void CheckCaseFourteenResult(const NotificationRequest& notificationRequest) const
    {
        std::shared_ptr<NotificationTemplate> notiTemplate = notificationRequest.GetTemplate();
        if (notiTemplate != nullptr) {
            EXPECT_EQ("process", notiTemplate->GetTemplateName());
            std::shared_ptr<AAFwk::WantParams> param = notiTemplate->GetTemplateData();
            int value = AAFwk::Integer::Unbox(AAFwk::IInteger::Query(param->GetParam("process")));
            EXPECT_EQ(20, value); // 20 test input
        }
        EXPECT_EQ(NotificationConstant::OTHER, notificationRequest.GetSlotType());
    }
};

class CompletedCallbackTest : public WantAgent::CompletedCallback {
    void OnSendFinished(
        const AAFwk::Want &want, int resultCode, const std::string &resultData, const AAFwk::WantParams &resultExtras)
    {
        (void)want;
        (void)resultCode;
        (void)resultData;
        (void)resultExtras;

        g_send_finished_mtx.unlock();
        OnWantReceived = true;
    }
};

class AnsInterfaceModulePublishTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void WaitOnSubscribeResult();
    void WaitOnConsumed();
    void WaitOnUnsubscribeResult();
    void CheckJsonConverter(const NotificationRequest *request);
};

void AnsInterfaceModulePublishTest::SetUpTestCase()
{
    sptr<AdvancedNotificationService> service = OHOS::Notification::AdvancedNotificationService::GetInstance();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::ADVANCED_NOTIFICATION_SERVICE_ABILITY_ID, service, saExtraProp);
    IPCSkeleton::SetCallingUid(CALLING_UID);
}

void AnsInterfaceModulePublishTest::TearDownTestCase()
{}

void AnsInterfaceModulePublishTest::SetUp()
{}

void AnsInterfaceModulePublishTest::TearDown()
{
    OnConsumedReceived = false;
    g_subscribe_mtx.unlock();
    g_consumed_mtx.unlock();
    g_unsubscribe_mtx.unlock();
}

void AnsInterfaceModulePublishTest::WaitOnSubscribeResult()
{
    struct tm subscribeTime = {0};
    EXPECT_EQ(OHOS::GetSystemCurrentTime(&subscribeTime), true);
    struct tm subscribeDoingTime = {0};
    EXPECT_EQ(OHOS::GetSystemCurrentTime(&subscribeDoingTime), true);
    int64_t subscribeSeconds = 0;
    while (!g_subscribe_mtx.try_lock()) {
        EXPECT_EQ(OHOS::GetSystemCurrentTime(&subscribeDoingTime), true);
        subscribeSeconds = OHOS::GetSecondsBetween(subscribeTime, subscribeDoingTime);
        if (subscribeSeconds >= TIME_OUT_SECONDS_LIMIT) {
            GTEST_LOG_(INFO) << "g_subscribe_mtx.try_lock overtime";
            break;
        }
    }
}

void AnsInterfaceModulePublishTest::WaitOnConsumed()
{
    struct tm publishTime = {0};
    EXPECT_EQ(OHOS::GetSystemCurrentTime(&publishTime), true);
    struct tm publishDoingTime = {0};
    EXPECT_EQ(OHOS::GetSystemCurrentTime(&publishDoingTime), true);
    int64_t publishSeconds = 0;
    while (!g_consumed_mtx.try_lock()) {
        EXPECT_EQ(OHOS::GetSystemCurrentTime(&publishDoingTime), true);
        publishSeconds = OHOS::GetSecondsBetween(publishTime, publishDoingTime);
        if (publishSeconds >= TIME_OUT_SECONDS_LIMIT) {
            GTEST_LOG_(INFO) << "g_consumed_mtx.try_lock overtime";
            break;
        }
    }
    EXPECT_EQ(OnConsumedReceived, true);
}

void AnsInterfaceModulePublishTest::WaitOnUnsubscribeResult()
{
    struct tm unsubscribeTime = {0};
    EXPECT_EQ(OHOS::GetSystemCurrentTime(&unsubscribeTime), true);
    struct tm unsubscribeDoingTime = {0};
    EXPECT_EQ(OHOS::GetSystemCurrentTime(&unsubscribeDoingTime), true);
    int64_t unSubscribeseconds = 0;
    while (!g_unsubscribe_mtx.try_lock()) {
        EXPECT_EQ(OHOS::GetSystemCurrentTime(&unsubscribeTime), true);
        unSubscribeseconds = OHOS::GetSecondsBetween(unsubscribeTime, unsubscribeDoingTime);
        if (unSubscribeseconds >= TIME_OUT_SECONDS_LIMIT) {
            GTEST_LOG_(INFO) << "Ag_unsubscribe_mtx.try_lock overtime";
            break;
        }
    }
}

void AnsInterfaceModulePublishTest::CheckJsonConverter(const NotificationRequest *request)
{
    nlohmann::json jsonObject;
    auto ret0 = NotificationJsonConverter::ConvertToJosn(request, jsonObject);
    EXPECT_EQ(ret0, true);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_08000::ConvertToJosn object dump ==========>" << jsonObject.dump();

    std::string jsonString;
    auto ret1 = NotificationJsonConverter::ConvertToJosnString(request, jsonString);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_08000::ConvertToJosnString ret1 ==========>"
        << (ret1 ? "true" : "false");
    EXPECT_EQ(ret1, true);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_08000::ConvertToJosnString string ==========>" << jsonString;

    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_08000::convert Json sleep start ==========>";
    sleep(SLEEP_TIME);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_08000::convert Json sleep end ==========>";

    auto pRequest1 = NotificationJsonConverter::ConvertFromJosn<NotificationRequest>(jsonObject);
    EXPECT_NE(pRequest1, nullptr);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_08000::ConvertFromJosn jsonObject dump request ==========>"
        << pRequest1->Dump();

    auto pRequest2 = NotificationJsonConverter::ConvertFromJosnString<NotificationRequest>(jsonString);
    EXPECT_NE(pRequest2, nullptr);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_08000::ConvertFromJosnString jsonString dump request ==========>"
        << pRequest2->Dump();

    nlohmann::json jsonObject2;
    auto ret2 = NotificationJsonConverter::ConvertToJosn(pRequest1, jsonObject2);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_08000::ConvertToJosn ret2 ==========>" << (ret2 ? "true" : "false");
    EXPECT_EQ(ret2, true);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_08000::FromJson -> ToJson object dump ==========>"
        << jsonObject2.dump();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_00100
 * @tc.name      : Publish_00100
 * @tc.desc      : Add notification slot(type is OTHER), make a subscriber and publish a media notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a media notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_00100, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::OTHER);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00100::SubscribeInfo:" << info.Dump();
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    WaitOnSubscribeResult();
    std::shared_ptr<NotificationMediaContent> mediaContent = std::make_shared<NotificationMediaContent>();
    EXPECT_NE(mediaContent, nullptr);
    mediaContent->SetAVToken(nullptr);
    std::vector<uint32_t> actions;
    actions.push_back(0);
    mediaContent->SetShownActions(actions);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00100::mediaContent:" << mediaContent->Dump();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(mediaContent);
    EXPECT_NE(content, nullptr);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00100::Content:" << content->Dump();
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::OTHER);
    req.SetNotificationId(CASE_ONE);
    std::vector<std::string> style;
    style.push_back("style");
    req.SetNotificationUserInputHistory(style);
    req.SetOwnerBundleName("ownerbundlename");
    req.SetCreatorBundleName("creatorbundlename");
    req.SetLabel("ANS_Interface_MT_Publish_00100");
    std::shared_ptr<NotificationRequest> requestPtr = std::make_shared<NotificationRequest>();
    EXPECT_NE(requestPtr, nullptr);
    requestPtr->SetLabel("ANS_Interface_MT_Publish_00100_REQUEST");
    req.SetPublicNotification(requestPtr);

    // pixelmap
    auto pixelMap = std::make_shared<Media::PixelMap>();
    EXPECT_NE(pixelMap, nullptr);
    Media::ImageInfo imageInfo;
    imageInfo.size.width = PIXEL_MAP_TEST_WIDTH;
    imageInfo.size.height = PIXEL_MAP_TEST_HEIGHT;
    imageInfo.pixelFormat = Media::PixelFormat::ALPHA_8;
    imageInfo.colorSpace = Media::ColorSpace::SRGB;
    pixelMap->SetImageInfo(imageInfo);
    int32_t rowDataSize = (PIXEL_MAP_TEST_WIDTH + 3) / 4 * 4;
    uint32_t bufferSize = rowDataSize * PIXEL_MAP_TEST_HEIGHT;
    void *buffer = malloc(bufferSize);
    if (buffer != nullptr) {
        pixelMap->SetPixelsAddr(buffer, nullptr, bufferSize, Media::AllocatorType::HEAP_ALLOC, nullptr);
    }
    EXPECT_NE(buffer, nullptr);

    req.SetBigIcon(pixelMap);
    req.SetLittleIcon(pixelMap);
    std::shared_ptr<MessageUser> messageUserPtr = std::make_shared<MessageUser>();
    EXPECT_NE(messageUserPtr, nullptr);
    messageUserPtr->SetName("ANS_Interface_MT_Publish_00100_Message_User");
    messageUserPtr->SetKey("key");
    messageUserPtr->SetPixelMap(nullptr);
    messageUserPtr->SetUri(Uri("."));
    messageUserPtr->SetMachine(false);
    messageUserPtr->SetUserAsImportant(false);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00100::messageUser is::" << messageUserPtr->Dump();
    req.AddMessageUser(messageUserPtr);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00100::messageUser is::" << req.Dump();
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_00200
 * @tc.name      : Publish_00200
 * @tc.desc      : Add notification slot(type is OTHER), make a subscriber and publish a local notification with input
 *                 box.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local notification with input box
 *                 success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_00200, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::OTHER);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    WaitOnSubscribeResult();
    std::shared_ptr<NotificationMediaContent> mediaContent = std::make_shared<NotificationMediaContent>();
    EXPECT_NE(mediaContent, nullptr);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(mediaContent);
    EXPECT_NE(content, nullptr);
    WantAgent::WantAgentInfo paramsInfo;
    std::shared_ptr<WantAgent::WantAgent> wantAgent = WantAgent::WantAgentHelper::GetWantAgent(paramsInfo);
    std::shared_ptr<NotificationActionButton> actionButton =
        NotificationActionButton::Create(nullptr, "title", wantAgent);
    std::shared_ptr<NotificationUserInput> userInput = NotificationUserInput::Create("inputKey");
    std::vector<std::shared_ptr<NotificationUserInput>> userInputs;
    AAFwk::WantParams additional;
    std::map<std::string, std::shared_ptr<Uri>> results;
    std::vector<std::string> options;
    std::set<std::string> permitMimeTypes;
    std::shared_ptr<AAFwk::WantParams> additionalPtr;
    userInput->SetInputsSource(g_want, NotificationConstant::FREE_FORM_INPUT);
    userInput->AddInputsToWant(userInputs, g_want, additional);
    std::shared_ptr<NotificationUserInput> userInputMine = NotificationUserInput::Create(
        "Key", "tag", options, false, permitMimeTypes, additionalPtr, NotificationConstant::EDIT_AUTO);
    userInput->AddMimeInputToWant(*userInputMine, g_want, results);
    userInput->AddAdditionalData(additional);
    userInput->SetEditType(NotificationConstant::EDIT_DISABLED);
    userInput->SetOptions(options);
    userInput->SetPermitMimeTypes("mimeType", false);
    userInput->SetTag("tag");
    userInput->SetPermitFreeFormInput(false);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00200::userInput is::" << userInput->Dump();
    actionButton->AddNotificationUserInput(userInput);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::OTHER);
    req.AddActionButton(actionButton);
    req.SetNotificationId(CASE_TWO);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_00300
 * @tc.name      : Publish_00300
 * @tc.desc      : Add notification slot(type is OTHER), make a subscriber and publish a local notification with
 *                 ActionButton.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local notification with ActionButton
 *                 success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_00300, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::OTHER);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    WaitOnSubscribeResult();
    std::shared_ptr<NotificationMediaContent> mediaContent = std::make_shared<NotificationMediaContent>();
    EXPECT_NE(mediaContent, nullptr);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(mediaContent);
    EXPECT_NE(content, nullptr);
    WantAgent::WantAgentInfo paramsInfo;
    std::shared_ptr<WantAgent::WantAgent> wantAgent = WantAgent::WantAgentHelper::GetWantAgent(paramsInfo);
    if (nullptr == wantAgent) {
        GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00300::wantAgent is nullptr";
    }
    std::shared_ptr<NotificationActionButton> actionButton =
        NotificationActionButton::Create(nullptr, "title", wantAgent);
    std::shared_ptr<NotificationUserInput> onlyUserInput = NotificationUserInput::Create("onlyUserInputKey");
    AAFwk::WantParams additional;
    actionButton->AddAdditionalData(additional);
    actionButton->SetSemanticActionButton(NotificationConstant::NONE_ACTION_BUTTON);
    actionButton->SetAutoCreatedReplies(true);
    actionButton->AddMimeTypeOnlyUserInput(onlyUserInput);
    actionButton->SetContextDependent(true);
    GTEST_LOG_(INFO) << actionButton->Dump();
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::OTHER);
    req.AddActionButton(actionButton);
    req.SetNotificationId(CASE_THREE);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_00400
 * @tc.name      : Publish_00400
 * @tc.desc      : Add notification slot(type is OTHER), make a subscriber and publish a local conversation
 * notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local conversation notification
 * success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_00400, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::OTHER);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    WaitOnSubscribeResult();
    MessageUser messageUser, sender;
    messageUser.SetKey("key");
    messageUser.SetName("Name");
    messageUser.SetPixelMap(nullptr);
    messageUser.SetUri(Uri("."));
    messageUser.SetUserAsImportant(false);
    messageUser.SetMachine(false);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00400::messgaeUser is::" << messageUser.Dump();
    std::shared_ptr<NotificationConversationalContent> conversationContent =
        std::make_shared<NotificationConversationalContent>(messageUser);
    EXPECT_NE(conversationContent, nullptr);
    conversationContent->SetConversationTitle("ConversationTitle");
    conversationContent->SetConversationGroup(false);
    conversationContent->AddConversationalMessage("text", 0, sender);
    std::shared_ptr<NotificationConversationalMessage> messagePtr =
        std::make_shared<NotificationConversationalMessage>("messageptr", 0, sender);
    EXPECT_NE(messagePtr, nullptr);
    conversationContent->AddConversationalMessage(messagePtr);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00400::conversationContent is::" << conversationContent->Dump();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(conversationContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::OTHER);
    req.SetNotificationId(CASE_FOUR);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_00500
 * @tc.name      : Publish_00500
 * @tc.desc      : Add notification slot(type is OTHER), make a subscriber and publish a local multiline notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local multiline notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_00500, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::OTHER);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    WaitOnSubscribeResult();
    MessageUser messageUser;
    std::shared_ptr<NotificationMultiLineContent> multiLineContent = std::make_shared<NotificationMultiLineContent>();
    EXPECT_NE(multiLineContent, nullptr);
    multiLineContent->SetExpandedTitle("expandedtitle");
    multiLineContent->SetBriefText("brieftext");
    multiLineContent->AddSingleLine("singleLine");
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00500::multiLineContent::" << multiLineContent->Dump();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(multiLineContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::OTHER);
    req.SetNotificationId(CASE_FIVE);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_00600
 * @tc.name      : Publish_00600
 * @tc.desc      : Add notification slot(type is OTHER), make a subscriber and publish a local picture notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local picture notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_00600, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::OTHER);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    WaitOnSubscribeResult();
    MessageUser messageUser;
    std::shared_ptr<NotificationPictureContent> pictureContent = std::make_shared<NotificationPictureContent>();
    EXPECT_NE(pictureContent, nullptr);
    pictureContent->SetExpandedTitle("expendedtitle");
    pictureContent->SetBriefText("brieftext");
    pictureContent->SetBigPicture(nullptr);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00600::pictureContent is::" << pictureContent->Dump();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(pictureContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::OTHER);
    req.SetNotificationId(CASE_SIX);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_00700
 * @tc.name      : Publish_00700
 * @tc.desc      : Add notification slot(type is OTHER), make a subscriber and publish a local long text notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local long text notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_00700, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::OTHER);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    WaitOnSubscribeResult();
    MessageUser messageUser;
    std::shared_ptr<NotificationLongTextContent> longTextContent =
        std::make_shared<NotificationLongTextContent>("longtext");
    EXPECT_NE(longTextContent, nullptr);
    longTextContent->SetExpandedTitle("expendedtitle");
    longTextContent->SetBriefText("brieftext");
    longTextContent->SetLongText("longtext");
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00700::longTextContentis::" << longTextContent->Dump();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(longTextContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::OTHER);
    req.SetNotificationId(CASE_SEVEN);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_00800
 * @tc.name      : Publish_00800
 * @tc.desc      : Add notification slot(type is CONTENT_INFORMATION), make a subscriber and publish a local normal
 *                 notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local normal notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_00800, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::CONTENT_INFORMATION);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber));
    WaitOnSubscribeResult();
    MessageUser messageUser;
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    EXPECT_NE(normalContent, nullptr);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00800::normalContentis::" << normalContent->Dump();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::CONTENT_INFORMATION);
    req.SetNotificationId(CASE_EIGHT);
    req.SetInProgress(false);
    req.SetUnremovable(false);
    req.SetBadgeNumber(0);
    req.SetDeliveryTime(0);
    req.SetShowDeliveryTime(false);
    req.SetPermitSystemGeneratedContextualActionButtons(false);
    req.SetAlertOneTime(false);
    req.SetAutoDeletedTime(0);
    req.SetClassification("classification");
    req.SetColor(0);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_00900
 * @tc.name      : Publish_00900
 * @tc.desc      : Add notification slot(type is OTHER), make a subscriber and publish a local normal
 *                 notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local normal notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_00900, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::OTHER);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber));
    WaitOnSubscribeResult();
    MessageUser messageUser;
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    EXPECT_NE(normalContent, nullptr);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::OTHER);
    req.SetNotificationId(CASE_NINE);
    req.SetColorEnabled(false);
    req.SetCountdownTimer(false);
    req.SetGroupName("groupvalue");
    req.SetOnlyLocal(true);
    req.SetSettingsText("setting text");
    req.SetShowStopwatch(false);
    req.SetSortingKey("sortingkey");
    req.SetStatusBarText("statusbartext");
    req.SetTapDismissed(false);
    req.SetVisibleness(NotificationConstant::VisiblenessType::PUBLIC);
    req.SetBadgeIconStyle(NotificationRequest::BadgeStyle::NONE);
    req.SetShortcutId("shortcutid");
    req.SetFloatingIcon(false);
    req.SetProgressBar(0, 0, false);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_01000
 * @tc.name      : Publish_01000
 * @tc.desc      : Add notification slot(type is SERVICE_REMINDER), make a subscriber and publish a local normal
 *                 notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local normal notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_01000, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::SERVICE_REMINDER);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    // Wait OnSubscribeResult
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber));
    WaitOnSubscribeResult();
    MessageUser messageUser;
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    EXPECT_NE(normalContent, nullptr);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::SERVICE_REMINDER);
    req.SetNotificationId(CASE_TEN);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
    WaitOnUnsubscribeResult();
    sleep(SLEEP_TIME);
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_02000
 * @tc.name      : Publish_02000
 * @tc.desc      : Add notification slot(type is SOCIAL_COMMUNICATION), make a subscriber and publish a local normal
 *                 notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local normal notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_02000, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::SOCIAL_COMMUNICATION);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber));
    WaitOnSubscribeResult();
    MessageUser messageUser;
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    EXPECT_NE(normalContent, nullptr);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::SOCIAL_COMMUNICATION);
    req.SetNotificationId(CASE_ELEVEN);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_03000
 * @tc.name      : Publish_03000
 * @tc.desc      : Add notification slot(type is CUSTOM), make a subscriber and publish a local normal
 *                 notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local normal notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_03000, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::CUSTOM);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber));
    WaitOnSubscribeResult();
    MessageUser messageUser;
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    EXPECT_NE(normalContent, nullptr);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::CUSTOM);
    req.SetNotificationId(CASE_TWELVE);
    GTEST_LOG_(INFO) << "request is ::" << req.Dump();
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_07000
 * @tc.name      : Publish_07000
 * @tc.desc      : Add notification slot(type is SOCIAL_COMMUNICATION), make a subscriber and publish a local group
 *                 notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a local group notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_07000, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::SOCIAL_COMMUNICATION);
    slot.EnableBypassDnd(true);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber));
    WaitOnSubscribeResult();
    MessageUser messageUser;
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    EXPECT_NE(normalContent, nullptr);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::SOCIAL_COMMUNICATION);
    req.SetGroupName("groupnotifcation");
    req.SetGroupOverview(true);
    req.SetNotificationId(CASE_THIRTEEN);
    req.SetGroupAlertType(NotificationRequest::GroupAlertType::ALL);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_FW_MT_GetActiveNotifications_00100
 * @tc.name      : GetActiveNotifications_00100
 * @tc.desc      : Get the active notifications and the number of active notifications of the current application.
 * @tc.expected  : Get the active notifications success and the number of active notifications of the current
 *                 application is right.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_GetActiveNotifications_00100, Function | MediumTest | Level1)
{
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    WaitOnSubscribeResult();
    std::shared_ptr<NotificationLongTextContent> implContent = std::make_shared<NotificationLongTextContent>();
    EXPECT_NE(implContent, nullptr);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(implContent);
    EXPECT_NE(content, nullptr);
    EXPECT_EQ((int)ERR_OK, (int)NotificationHelper::CancelAllNotifications());
    sleep(SLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, true);
    int countBefor = 0;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotificationNums(countBefor));
    EXPECT_EQ(0, countBefor);
    std::string label1 = "Label1";
    NotificationRequest req1(0);
    req1.SetLabel(label1);
    req1.SetContent(content);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req1));
    WaitOnConsumed();
    OnConsumedReceived = false;
    g_consumed_mtx.unlock();
    std::string label2 = "Label2";
    NotificationRequest req2(0);
    req2.SetLabel(label2);
    req2.SetContent(content);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req2));
    WaitOnConsumed();
    int countAfter = 0;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotificationNums(countAfter));
    EXPECT_EQ(ACTIVE_NUMS, countAfter);
    std::vector<sptr<NotificationRequest>> requests;
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotifications(requests));
    EXPECT_EQ("Label1", requests[0]->GetLabel());
    EXPECT_EQ("Label2", requests[1]->GetLabel());
    EXPECT_EQ((int)ERR_OK, (int)NotificationHelper::RemoveNotifications());
    sleep(SLEEP_TIME);
    EXPECT_EQ(OnCanceledReceived, true);
    EXPECT_EQ((int)ERR_OK, NotificationHelper::GetActiveNotificationNums(countAfter));
    EXPECT_EQ(0, countAfter);
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber));
    WaitOnUnsubscribeResult();
    OnCanceledReceived = false;
}

/**
 * @tc.number    : ANS_FW_MT_CancelGroup_10100
 * @tc.name      : CancelGroup_10100
 * @tc.desc      : Cancel the notifications of the same group.
 * @tc.expected  : All notifications of the same group have been cancelled.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_CancelGroup_10100, Function | MediumTest | Level1)
{
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");

    auto subscriber = TestAnsSubscriber();
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    WaitOnSubscribeResult();

    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    EXPECT_NE(normalContent, nullptr);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_CancelGroup_10100::normalContentis::" << normalContent->Dump();
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    EXPECT_NE(content, nullptr);

    NotificationRequest req;
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::OTHER);
    req.SetNotificationId(CANCELGROUP_NID);
    req.SetLabel("CancelGroup_10100");
    req.SetGroupName("group10100");

    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    OnConsumedReceived = false;

    GTEST_LOG_(INFO) << "ANS_Interface_MT_CancelGroup_10100:: call CancelGroup : effective parameters";
    EXPECT_EQ(0, NotificationHelper::CancelGroup("group10100"));

    sleep(SLEEP_TIME);
    OnCanceledReceived = false;

    GTEST_LOG_(INFO) << "ANS_Interface_MT_CancelGroup_10100:: call CancelGroup : invalid parameters";
    EXPECT_EQ(0, NotificationHelper::CancelGroup("ngroup"));

    sleep(SLEEP_TIME);
    OnCanceledReceived = false;

    req.SetOwnerBundleName("mybundlename");
    req.SetCreatorBundleName("mybundlename");
    g_consumed_mtx.unlock();
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    OnConsumedReceived = false;

    NotificationBundleOption bo("bundlename", 0);
    GTEST_LOG_(INFO) << "ANS_Interface_MT_CancelGroup_10100:: call RemoveGroupByBundle : effective parameters";
    EXPECT_NE(0, NotificationHelper::RemoveGroupByBundle(bo, "group10100"));

    sleep(SLEEP_TIME);
    OnCanceledReceived = false;

    GTEST_LOG_(INFO) << "ANS_Interface_MT_CancelGroup_10100:: call RemoveGroupByBundle : invalid parameters";
    EXPECT_NE(0, NotificationHelper::RemoveGroupByBundle(bo, "ngroup"));

    sleep(SLEEP_TIME);
    OnCanceledReceived = false;

    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_04000
 * @tc.name      : Publish_04000
 * @tc.desc      : Add notification slot(type is OTHER), make a subscriber and publish a template notification.
 * @tc.expected  : Add notification slot success, make a subscriber and publish a ltemplate notification success.
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_04000, Function | MediumTest | Level1)
{
    NotificationSlot slot(NotificationConstant::OTHER);
    EXPECT_EQ(0, NotificationHelper::AddNotificationSlot(slot));
    auto subscriber = TestAnsSubscriber();
    NotificationSubscribeInfo info = NotificationSubscribeInfo();
    info.AddAppName("bundleName");
    g_subscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::SubscribeNotification(subscriber, info));
    WaitOnSubscribeResult();

    std::shared_ptr<NotificationTemplate> notiTemplate = std::make_shared<NotificationTemplate>();
    EXPECT_NE(notiTemplate, nullptr);
    notiTemplate->SetTemplateName("process");
    // [{'process':20}]
    AAFwk::WantParams wantParams;
    std::string key("process");
    int resultValue = 20;
    wantParams.SetParam(key,  AAFwk::Integer::Box(resultValue));
    notiTemplate->SetTemplateData(std::make_shared<AAFwk::WantParams>(wantParams));
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_04000::notiTemplate::" << notiTemplate->Dump();
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    EXPECT_NE(normalContent, nullptr);
    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    EXPECT_NE(content, nullptr);
    NotificationRequest req;
    req.SetContent(content);
    req.SetTemplate(notiTemplate);
    req.SetSlotType(NotificationConstant::OTHER);
    req.SetNotificationId(CASE_FOURTEEN);
    g_consumed_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::PublishNotification(req));
    WaitOnConsumed();
    g_unsubscribe_mtx.lock();
    EXPECT_EQ(0, NotificationHelper::UnSubscribeNotification(subscriber, info));
    WaitOnUnsubscribeResult();
}

/**
 * @tc.number    : ANS_Interface_MT_Publish_08000
 * @tc.name      : Publish_08000
 * @tc.desc      : .
 * @tc.expected  : .
 */
HWTEST_F(AnsInterfaceModulePublishTest, ANS_Interface_MT_Publish_08000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_08000::convert Json start ############==========>";

    NotificationRequest req;
    std::shared_ptr<NotificationNormalContent> normalContent = std::make_shared<NotificationNormalContent>();
    EXPECT_NE(normalContent, nullptr);
    normalContent->SetTitle("normal_title");
    normalContent->SetText("normal_text");
    normalContent->SetAdditionalText("normal_additional_text");
    GTEST_LOG_(INFO) << "ANS_Interface_MT_Publish_00800::normalContent::" << normalContent->Dump();

    std::shared_ptr<NotificationContent> content = std::make_shared<NotificationContent>(normalContent);
    EXPECT_NE(content, nullptr);

    req.SetNotificationId(8000);
    req.SetContent(content);
    req.SetSlotType(NotificationConstant::CONTENT_INFORMATION);
    req.SetClassification(NotificationRequest::CLASSIFICATION_ALARM);
    req.SetLabel("label");
    req.SetOwnerBundleName("owner");

    auto wAgent1 = std::make_shared<WantAgent::WantAgent>();
    req.SetWantAgent(wAgent1);

    auto wAgent2 = std::make_shared<WantAgent::WantAgent>();
    std::shared_ptr<Media::PixelMap> dummyIcon;
    auto ab1 = NotificationActionButton::Create(dummyIcon, "ab1_title", wAgent2);

    auto spUserInput3 = NotificationUserInput::Create("uikey3");
    auto spUserInput2 = NotificationUserInput::Create("uikey2");
    ab1->AddNotificationUserInput(spUserInput3);
    ab1->AddNotificationUserInput(spUserInput2);

    auto spOnlyUserInput1 = NotificationUserInput::Create("uionlykey1");
    spOnlyUserInput1->SetPermitFreeFormInput(false);
    spOnlyUserInput1->SetPermitMimeTypes("uionly", true);
    auto spOnlyUserInput4 = NotificationUserInput::Create("uionlykey4");
    spOnlyUserInput4->SetPermitFreeFormInput(false);
    spOnlyUserInput4->SetPermitMimeTypes("uionly", true);

    ab1->AddMimeTypeOnlyUserInput(spOnlyUserInput1);
    ab1->AddMimeTypeOnlyUserInput(spOnlyUserInput4);

    std::shared_ptr<WantAgent::WantAgent> dummyWantAgent;
    auto ab2 = NotificationActionButton::Create(dummyIcon, "ab2_title", dummyWantAgent);

    req.AddActionButton(ab1);
    req.AddActionButton(ab2);

    std::vector<std::string> history {"uihistory3", "uihistory2", "uihistory1"};
    req.SetNotificationUserInputHistory(history);

    auto msgUser1 = std::make_shared<MessageUser>();
    msgUser1->SetKey("msgUser1_key");
    msgUser1->SetName("msgUser1_name");
    auto msgUser2 = std::make_shared<MessageUser>();
    msgUser2->SetKey("msgUser2_key");
    msgUser2->SetName("msgUser2_name");
    req.AddMessageUser(msgUser2);
    req.AddMessageUser(msgUser1);

    CheckJsonConverter(&req);
}
}  // namespace Notification
}  // namespace OHOS