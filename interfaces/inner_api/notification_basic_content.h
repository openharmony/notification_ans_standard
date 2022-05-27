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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_BASIC_CONTENT_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_BASIC_CONTENT_H

#include "notification_json_convert.h"
#include "parcel.h"

namespace OHOS {
namespace Notification {
class NotificationBasicContent : public Parcelable, public NotificationJsonConvertionBase {
public:
    virtual ~NotificationBasicContent();

    /**
     * @brief Sets the additional text to be included in a notification.
     * The additional text is mainly a supplement to the notification text set by calling setText(std::string).
     * The font of the additional text is smaller than the notification text and is displayed in a separate line.
     *
     * @param text Indicates the additional text to be included.
     */
    virtual void SetAdditionalText(const std::string &additionalText);

    /**
     * @brief Obtains the additional text of a notification specified by calling setAdditionalText(std::string).
     *
     * @return Returns the additional text of the notification.
     */
    virtual std::string GetAdditionalText() const;

    /**
     * @brief Sets the text to be included in a notification.
     *
     * @param text Indicates the text to be included.
     */
    virtual void SetText(const std::string &text);

    /**
     * @brief Obtains the text of a notification specified by calling setText(std::string).
     *
     * @return Returns the text of the notification.
     */
    virtual std::string GetText() const;

    /**
     * @brief Sets the title of a notification.
     *
     * @param title Indicates the title of the notification.
     */
    virtual void SetTitle(const std::string &title);

    /**
     * @brief Obtains the title of a notification specified by calling the setTitle(std::string) method.
     *
     * @return Returns the title of the notification.
     */
    virtual std::string GetTitle() const;

    /**
     * @brief Returns a string representation of the object.
     *
     * @return Returns a string representation of the object.
     */
    virtual std::string Dump();

    /**
     * @brief Converts a NotificationBasicContent object into a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns true if succeed; returns false otherwise.
     */
    virtual bool ToJson(nlohmann::json &jsonObject) const override;

    /**
     * @brief Marshal a object into a Parcel.
     *
     * @param parcel the object into the parcel.
     * @return Returns true if succeed; returns false otherwise.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

protected:
    NotificationBasicContent() = default;

    /**
     * @brief Read data from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns true if read success; returns false otherwise.
     */
    virtual bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Creates a NotificationBasicContent object from a Json.
     *
     * @param jsonObject Indicates the Json object.
     */
    void ReadFromJson(const nlohmann::json &jsonObject);

protected:
    std::string text_ {};
    std::string title_ {};
    std::string additionalText_ {};
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_NOTIFICATION_BASIC_CONTENT_H
