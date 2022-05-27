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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_PICTURE_CONTENT_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_PICTURE_CONTENT_H

#include "notification_basic_content.h"
#include "parcel.h"
#include "pixel_map.h"

namespace OHOS {
namespace Notification {
class NotificationPictureContent : public NotificationBasicContent {
public:
    NotificationPictureContent() = default;

    ~NotificationPictureContent() = default;

    /**
     * @brief Sets the title to be displayed when this picture-attached notification is expanded.
     * After this title is set, the title set by setTitle(string) will be displayed only
     * when this notification is in the collapsed state.
     *
     * @param exTitle Indicates the title to be displayed when this notification is expanded.
     */
    void SetExpandedTitle(const std::string &exTitle);

    /**
     * @brief Obtains the title that will be displayed for this picture-attached notification when it is expanded.
     *
     * @return Returns the title to be displayed when this notification is expanded.
     */
    std::string GetExpandedTitle() const;

    /**
     * @brief Sets the brief text to be included in a picture-attached notification.
     * The brief text is a summary of a picture-attached notification and is displayed in the first line of the
     * notification. Similar to setAdditionalText(string), the font of the brief text is also smaller than the
     * notification text. The positions where the brief text and additional text will display may conflict. If both
     * texts are set, only the additional text will be displayed.
     *
     * @param briefText Indicates the brief text to be included.
     */
    void SetBriefText(const std::string &briefText);

    /**
     * @brief Obtains the brief text of a picture-attached notification specified by calling the setBriefText(string)
     * method.
     *
     * @return Returns the brief text of the picture-attached notification.
     */
    std::string GetBriefText() const;

    /**
     * @brief Sets the picture to be included in a notification.
     *
     * @param bigPicture Indicates the PixelMap of the picture to be included.
     */
    void SetBigPicture(const std::shared_ptr<Media::PixelMap> &bigPicture);

    /**
     * @brief Obtains the PixelMap of the picture specified by calling the setBigPicture(PixelMap) method.
     *
     * @return Returns the PixelMap of the picture included in the notification.
     */
    const std::shared_ptr<Media::PixelMap> GetBigPicture() const;

    /**
     * @brief Returns a string representation of the object.
     *
     * @return Returns a string representation of the object.
     */
    std::string Dump() override;

    /**
     * @brief Converts a NotificationPictureContent object into a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns true if succeed; returns false otherwise.
     */
    virtual bool ToJson(nlohmann::json &jsonObject) const override;

    /**
     * @brief Creates a NotificationPictureContent object from a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns the NotificationPictureContent object.
     */
    static NotificationPictureContent *FromJson(const nlohmann::json &jsonObject);

    /**
     * @brief Marshal a object into a Parcel.
     *
     * @param parcel the object into the parcel.
     * @return Returns true if succeed; returns false otherwise.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshal object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns the NotificationPictureContent object.
     */
    static NotificationPictureContent *Unmarshalling(Parcel &parcel);

protected:
    /**
     * @brief Read a NotificationPictureContent object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns true if succeed; returns false otherwise.
     */
    bool ReadFromParcel(Parcel &parcel) override;

private:
    std::string expandedTitle_ {};
    std::string briefText_ {};
    std::shared_ptr<Media::PixelMap> bigPicture_ {};
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_PICTURE_CONTENT_H