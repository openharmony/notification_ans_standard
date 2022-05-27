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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_CONVERSATIONAL_MESSAGE_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_CONVERSATIONAL_MESSAGE_H

#include "message_user.h"
#include "notification_json_convert.h"
#include "parcel.h"
#include "uri.h"

namespace OHOS {
namespace Notification {
class NotificationConversationalMessage : public Parcelable, public NotificationJsonConvertionBase {
public:
    /**
     * @brief A constructor used to create a NotificationConversationalMessage instance with the input parameters
     * passed.
     *
     * @param text Indicates the text to be displayed as the message content. This parameter cannot be null.
     * @param timestamp Indicates the time when the message arrived.
     * @param sender Indicates the MessageUser who sent the message.
     */
    NotificationConversationalMessage(
        const std::string &text, int64_t timestamp, const MessageUser &sender);

    ~NotificationConversationalMessage() = default;

    /**
     * @brief Obtains the text to be displayed as the content of this message.
     *
     * @return Returns the message content.
     */
    std::string GetText() const;

    /**
     * @brief Obtains the time when this message arrived.
     *
     * @return Returns the time when this message arrived.
     */
    int64_t GetArrivedTime() const;

    /**
     * @brief Obtains the sender of this message.
     *
     * @return Returns the message sender.
     */
    MessageUser GetSender() const;

    /**
     * @brief Sets the MIME type and URI of this message.
     *
     * @param mimeType Indicates the MIME type of this message.
     * @param uri Indicates the URI that points to the message content whose type is specified by the given MIME type.
     */
    void SetData(const std::string &mimeType, const std::shared_ptr<Uri> &uri);

    /**
     * @brief Obtains the MIME type of this message.
     *
     * @return Returns the MIME type of this message.
     */
    std::string GetMimeType() const;

    /**
     * @brief Obtains the URI of the message content with the specific MIME type.
     *
     * @return Returns the URI of the message content with the specific MIME type.
     */
    const std::shared_ptr<Uri> GetUri() const;

    /**
     * @brief Returns a string representation of the object.
     *
     * @return Returns a string representation of the object.
     */
    std::string Dump();

    /**
     * @brief Converts a NotificationConversationalMessage object into a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns true if succeed; returns false otherwise.
     */
    bool ToJson(nlohmann::json &jsonObject) const override;

    /**
     * @brief Creates a NotificationConversationalMessage object from a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns the NotificationConversationalMessage.
     */
    static NotificationConversationalMessage *FromJson(const nlohmann::json &jsonObject);

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
     * @return Returns the NotificationConversationalMessage.
     */
    static NotificationConversationalMessage *Unmarshalling(Parcel &parcel);

private:
    NotificationConversationalMessage() = default;

    /**
     * @brief Read a NotificationConversationalMessage object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns true if succeed; returns false otherwise.
     */
    bool ReadFromParcel(Parcel &parcel);

private:
    int64_t arrivedTime_ {0};
    std::string text_ {};
    MessageUser sender_ {};
    std::shared_ptr<Uri> uri_ {};
    std::string mimeType_ {};
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_CONVERSATIONAL_MESSAGE_H