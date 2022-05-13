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

#include "notification_conversational_content.h"
#include "ans_log_wrapper.h"

namespace OHOS {
namespace Notification {
NotificationConversationalContent::NotificationConversationalContent(const MessageUser &messageUser)
    : messageUser_(messageUser)
{}

MessageUser NotificationConversationalContent::GetMessageUser() const
{
    return messageUser_;
}

void NotificationConversationalContent::SetConversationTitle(const std::string &conversationTitle)
{
    conversationTitle_ = conversationTitle;
}

std::string NotificationConversationalContent::GetConversationTitle() const
{
    return conversationTitle_;
}

bool NotificationConversationalContent::IsConversationGroup() const
{
    return isGroup_;
}

void NotificationConversationalContent::SetConversationGroup(bool isGroup)
{
    isGroup_ = isGroup;
}

void NotificationConversationalContent::AddConversationalMessage(
    const std::string &text, int64_t timestamp, const MessageUser &sender)
{
    auto message = std::make_shared<NotificationConversationalMessage>(text, timestamp, sender);
    if (!message) {
        ANS_LOGE("Failed to create message");
        return;
    }

    messages_.emplace_back(message);
}

void NotificationConversationalContent::AddConversationalMessage(const MessagePtr &message)
{
    if (!message) {
        ANS_LOGE("Message can not be null");
        return;
    }

    messages_.emplace_back(message);
}

NotificationConversationalContent::MessageVector NotificationConversationalContent::GetAllConversationalMessages() const
{
    return messages_;
}

std::string NotificationConversationalContent::Dump()
{
    std::string messages = "";
    for (auto &message : messages_) {
        if (!message) {
            messages += "nullptr, ";
            continue;
        }
        messages += message->Dump();
        messages += ", ";
    }
    return "NotificationConversationalContent{ " + NotificationBasicContent::Dump() +
            ", conversationTitle = " + conversationTitle_ +
            ", isGroup = " + (isGroup_ ? "true" : "false") +
            ", messageUser = " + messageUser_.Dump() +
            ", messages = " + messages +
            " }";
}

bool NotificationConversationalContent::ToJson(nlohmann::json &jsonObject) const
{
    if (!NotificationBasicContent::ToJson(jsonObject)) {
        ANS_LOGE("Cannot convert basicContent to JSON");
        return false;
    }

    nlohmann::json userObj;
    if (!NotificationJsonConverter::ConvertToJson(&messageUser_, userObj)) {
        ANS_LOGE("Cannot convert messageUser to JSON");
        return false;
    }
    jsonObject["messageUser"] = userObj;

    jsonObject["conversationTitle"] = conversationTitle_;
    jsonObject["isGroup"]           = isGroup_;

    nlohmann::json msgsArr = nlohmann::json::array();
    for (auto &msg : messages_) {
        if (!msg) {
            continue;
        }

        nlohmann::json msgObj;
        if (!NotificationJsonConverter::ConvertToJson(msg.get(), msgObj)) {
            ANS_LOGE("Cannot convert conversationalMessage to JSON");
            return false;
        }
        msgsArr.emplace_back(msgObj);
    }
    jsonObject["messages"] = msgsArr;

    return true;
}

NotificationConversationalContent *NotificationConversationalContent::FromJson(const nlohmann::json &jsonObject)
{
    if (jsonObject.is_null() or !jsonObject.is_object()) {
        ANS_LOGE("Invalid JSON object");
        return nullptr;
    }

    auto pContent = new (std::nothrow) NotificationConversationalContent();
    if (pContent == nullptr) {
        ANS_LOGE("Failed to create conversationalContent instance");
        return nullptr;
    }

    pContent->ReadFromJson(jsonObject);

    const auto &jsonEnd = jsonObject.cend();
    if (jsonObject.find("messageUser") != jsonEnd) {
        auto userObj = jsonObject.at("messageUser");
        auto pUser = NotificationJsonConverter::ConvertFromJson<MessageUser>(userObj);
        if (pUser != nullptr) {
            pContent->messageUser_ = *pUser;

            delete pUser;
            pUser = nullptr;
        }
    }

    if (jsonObject.find("messages") != jsonEnd) {
        nlohmann::json msgsArr = jsonObject.at("messages");
        for (auto &msgObj : msgsArr) {
            auto pMsg = NotificationJsonConverter::ConvertFromJson<NotificationConversationalMessage>(msgObj);
            if (pMsg == nullptr) {
                ANS_LOGE("Failed to parse message ");

                delete pContent;
                pContent = nullptr;
                return nullptr;
            }

            pContent->messages_.emplace_back(pMsg);
        }
    }

    return pContent;
}

bool NotificationConversationalContent::Marshalling(Parcel &parcel) const
{
    if (!NotificationBasicContent::Marshalling(parcel)) {
        ANS_LOGE("Failed to write basic");
        return false;
    }

    if (!parcel.WriteString(conversationTitle_)) {
        ANS_LOGE("Failed to write conversation title");
        return false;
    }

    if (!parcel.WriteBool(isGroup_)) {
        ANS_LOGE("Failed to write flag group");
        return false;
    }

    if (!parcel.WriteParcelable(&messageUser_)) {
        ANS_LOGE("Failed to write messageUser");
        return false;
    }

    if (!parcel.WriteUint64(messages_.size())) {
        ANS_LOGE("Failed to write the size of messages");
        return false;
    }

    for (auto it = messages_.begin(); it != messages_.end(); ++it) {
        auto valid = (*it) ? true : false;
        if (!parcel.WriteBool(valid)) {
            ANS_LOGE("Failed to write the flag which indicate whether message is null");
            return false;
        }

        if (!valid) {
            ANS_LOGE("Invalid message, write to parcel failed");
            return false;
        }

        if (!parcel.WriteParcelable(it->get())) {
            ANS_LOGE("Failed to write message");
            return false;
        }
    }

    return true;
}

NotificationConversationalContent *NotificationConversationalContent::Unmarshalling(Parcel &parcel)
{
    auto pContent = new (std::nothrow) NotificationConversationalContent();
    if ((pContent != nullptr) && !pContent->ReadFromParcel(parcel)) {
        delete pContent;
        pContent = nullptr;
    }

    return pContent;
}

bool NotificationConversationalContent::ReadFromParcel(Parcel &parcel)
{
    if (!NotificationBasicContent::ReadFromParcel(parcel)) {
        ANS_LOGE("Failed to read basic");
        return false;
    }

    if (!parcel.ReadString(conversationTitle_)) {
        ANS_LOGE("Failed to read conversation title");
        return false;
    }

    isGroup_ = parcel.ReadBool();

    auto pUser = parcel.ReadParcelable<MessageUser>();
    if (pUser == nullptr) {
        ANS_LOGE("Failed to read messageUser");
        return false;
    }
    messageUser_ = *pUser;

    auto vsize = parcel.ReadUint64();
    for (uint64_t it = 0; it < vsize; ++it) {
        auto valid = parcel.ReadBool();
        if (!valid) {
            ANS_LOGE("Invalid message, read from parcel failed");
            return false;
        }

        auto member = parcel.ReadParcelable<NotificationConversationalMessage>();
        if (member == nullptr) {
            ANS_LOGE("Failed to read message");
            return false;
        }

        messages_.emplace_back(member);
    }

    return true;
}
}  // namespace Notification
}  // namespace OHOS
