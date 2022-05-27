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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_MEDIA_CONTENT_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_MEDIA_CONTENT_H

#include "notification_basic_content.h"
#include "parcel.h"

namespace OHOS {
namespace Notification {
class AVToken;

class NotificationMediaContent : public NotificationBasicContent {
public:
    NotificationMediaContent() = default;

    ~NotificationMediaContent() = default;

    /**
     * @brief Attaches a specified AVToken to this media playback notification.
     * After an AVToken is attached, this notification can interact with the associated AVSession
     * so that users can control media playback in this notification.
     *
     * @param avToken Indicates the AVToken to attach.
     */
    void SetAVToken(const std::shared_ptr<AVToken> &avToken);

    /**
     * @brief Obtains the AVToken attached to this media playback notification.
     *
     * @return Returns the AVToken that attached to this notification.
     */
    const std::shared_ptr<AVToken> GetAVToken() const;

    /**
     * @brief Sets up to three NotificationActionButton objects to be shown in this media playback notification.
     * Before publishing this notification, you should also call
     * NotificationRequest::addActionButton(NotificationActionButton) to add specified NotificationActionButton objects
     * for this notification so that this method can take effect. The added action buttons will be assigned sequence
     * numbers in the order they were added. The sequence numbers specified in the value of actions for this method
     * must match those assigned to added action buttons. Otherwise, the notification will fail to publish. By default,
     * the sequence number starts from 0. If you want to show three action buttons, the value of actions should be 0,
     * 1, 2.
     *
     * @param actions Indicates the list of sequence numbers representing the NotificationActionButton objects
     * to be shown in this notification.
     */
    void SetShownActions(const std::vector<uint32_t> &actions);

    /**
     * @brief Obtains the list of sequence numbers representing the NotificationActionButton objects
     * to be shown in this media playback notification.
     *
     * @return Returns the list of the action buttons to be shown.
     */
    std::vector<uint32_t> GetShownActions() const;

    /**
     * @brief Returns a string representation of the object.
     *
     * @return Returns a string representation of the object.
     */
    std::string Dump() override;

    /**
     * @brief Converts a NotificationMediaContent object into a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns true if succeed; returns false otherwise.
     */
    virtual bool ToJson(nlohmann::json &jsonObject) const override;

    /**
     * @brief Creates a NotificationMediaContent object from a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns the NotificationMediaContent object.
     */
    static NotificationMediaContent *FromJson(const nlohmann::json &jsonObject);

    /**
     * @brief Marshal a object into a Parcel.
     * @param parcel the object into the parcel.
     * @return Returns true if succeed; returns false otherwise.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshal object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns the NotificationMediaContent object.
     */
    static NotificationMediaContent *Unmarshalling(Parcel &parcel);

protected:
    /**
     * @brief Read a NotificationMediaContent object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns true if succeed; returns false otherwise.
     */
    bool ReadFromParcel(Parcel &parcel) override;

private:
    std::shared_ptr<AVToken> avToken_ {};
    std::vector<uint32_t> sequenceNumbers_ {};
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_MEDIA_CONTENT_H