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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_CONVERSATIONAL_CONTENT_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_CONVERSATIONAL_CONTENT_H

#include "message_user.h"
#include "notification_basic_content.h"
#include "notification_conversational_message.h"
#include "notification_json_convert.h"
#include "parcel.h"

namespace OHOS {
namespace Notification {
class NotificationConversationalContent : public NotificationBasicContent {
public:
    using MessagePtr = std::shared_ptr<NotificationConversationalMessage>;
    using MessageVector = std::vector<MessagePtr>;

    /**
     * @brief A constructor used to create a NotificationConversationalContent instance with the MessageUser specified.
     *
     * @param messageUser Indicates the MessageUser who sends all Message objects in this conversation-like
     * notification. This parameter cannot be null.
     */
    explicit NotificationConversationalContent(const MessageUser &messageUser);

    ~NotificationConversationalContent() = default;

    /**
     * @brief Obtains the message sender to be displayed for any messages sent by the user in this conversation-like
     * notification.
     *
     * @return Returns the message sender.
     */
    MessageUser GetMessageUser() const;

    /**
     * @brief Sets the title to be displayed for the conversation.The title set in this method will overwrite the one
     * set by calling setTitle(std::string).
     *
     * @param conversationTitle Indicates the title to be displayed for the conversation.
     */
    void SetConversationTitle(const std::string &conversationTitle);

    /**
     * @brief Obtains the title to be displayed for the conversation.
     *
     * @return Returns the title to be displayed for the conversation.
     */
    std::string GetConversationTitle() const;

    /**
     * @brief Checks whether this notification represents a group conversation.
     *
     * @return Returns true if this notification represents a group conversation; returns false otherwise.
     */
    bool IsConversationGroup() const;

    /**
     * @brief Sets whether this notification represents a group conversation.
     * The big icon, if any, set for this notification by calling NotificationRequest::setBigIcon(PixelMap)
     * will be displayed only when this method is set to true.
     *
     * @param isGroup Specifies whether this notification represents a group conversation.
     */
    void SetConversationGroup(bool isGroup);

    /**
     * @brief Adds a message to this conversation-like notification based on the specified message content, timestamp,
     * and MessageUser.All messages will be displayed in the order they are added.
     *
     * @param text Indicates the text to be displayed as the message content.
     * @param timestamp Indicates the time when the message arrived.
     * @param sender Indicates the MessageUser who sent the message.
     */
    void AddConversationalMessage(
        const std::string &text, int64_t timestamp, const MessageUser &sender);

    /**
     * @brief Adds a specified message to this conversation-like notification.All messages will be displayed in the
     * order they are added.
     *
     * @param message Indicates the ConversationalMessage object to add.
     */
    void AddConversationalMessage(const MessagePtr &message);

    /**
     * @brief Obtains all messages included in this conversation-like notification.
     *
     * @return Returns the list of all Message objects included.
     */
    MessageVector GetAllConversationalMessages() const;

    /**
     * @brief Returns a string representation of the object.
     *
     * @return Returns a string representation of the object.
     */
    std::string Dump() override;

    /**
     * @brief Converts a NotificationConversationalContent object into a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns true if succeed; returns false otherwise.
     */
    virtual bool ToJson(nlohmann::json &jsonObject) const override;

    /**
     * @brief Creates a NotificationConversationalContent object from a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns the NotificationConversationalContent.
     */
    static NotificationConversationalContent *FromJson(const nlohmann::json &jsonObject);

    /**
     * @brief Marshal a object into a Parcel.
     *
     * @param parcel Indicates the object into the parcel.
     * @return Returns true if succeed; returns false otherwise.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshal object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns the NotificationConversationalContent.
     */
    static NotificationConversationalContent *Unmarshalling(Parcel &parcel);

protected:
    /**
     * @brief Read a NotificationConversationalContent object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns true if succeed; returns false otherwise.
     */
    bool ReadFromParcel(Parcel &parcel) override;

private:
    NotificationConversationalContent() = default;

private:
    MessageUser messageUser_ {};
    std::string conversationTitle_ {};
    bool isGroup_ {false};
    MessageVector messages_ {};
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_CONVERSATIONAL_CONTENT_H