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
#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_MESSAGE_USER_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_MESSAGE_USER_H

#include "notification_json_convert.h"
#include "pixel_map.h"
#include "parcel.h"
#include "uri.h"

namespace OHOS {
namespace Notification {
class MessageUser final : public Parcelable, public NotificationJsonConvertionBase {
public:
    MessageUser();

    ~MessageUser();

    /**
     * @brief Sets the key used to uniquely identify this MessageUser.If no key is set, the name set by SetName(string)
     * is used to uniquely identify a MessageUser.
     *
     * @param key Indicates the key to set.
     */
    void SetKey(const std::string &key);

    /**
     * @brief Obtains the key of this MessageUser.
     *
     * @return Returns the key of this MessageUser.
     */
    std::string GetKey() const;

    /**
     * @brief Sets the name of this MessageUser.
     *
     * @name Indicates the name to set.
     */
    void SetName(const std::string &name);

    /**
     * @brief Obtains the name of this MessageUser.
     *
     * @return Returns the name of this MessageUser.
     */
    std::string GetName() const;

    /**
     * @brief Sets the pixel map of this MessageUser.
     *
     * @param pixelMap Indicates the pixel map to set.
     */
    void SetPixelMap(const std::shared_ptr<Media::PixelMap> &pixelMap);

    /**
     * @brief Obtains the pixel map of this MessageUser.
     *
     * @return Returns the pixel map of this MessageUser.
     */
    const std::shared_ptr<Media::PixelMap> GetPixelMap() const;

    /**
     * @brief Sets the URI of this MessageUser.
     *
     * @param uri Indicates the URI to set.
     */
    void SetUri(const Uri &uri);

    /**
     * @brief Obtains the URI of this MessageUser.
     *
     * @return Returns the URI of this MessageUser.
     */
    Uri GetUri() const;

    /**
     * @brief Sets whether this MessageUser is a machine.
     *
     * @param machine Specifies whether this MessageUser is a machine.The value true indicates that it is,
     * and the value false indicates not.
     */
    void SetMachine(bool machine);

    /**
     * @brief Checks whether this MessageUser is a machine.
     *
     * @return Returns true if this MessageUser is a machine; returns false otherwise.
     */
    bool IsMachine() const;

    /**
     * @brief Sets whether this MessageUser is important.This method can be used to denote users who frequently
     * interact with the user of this device.
     *
     * @param userImportant Specifies whether this MessageUser is important.The value true indicates that it is
     * important, and the value false indicates not.
     */
    void SetUserAsImportant(bool userImportant);

    /**
     * @brief Checks whether this MessageUser is important.
     *
     * @return Returns true if this MessageUser is important; returns false otherwise.
     */
    bool IsUserImportant() const;

    /**
     * @brief Dumps a string representation of the object.
     *
     * @return Returns a string representation of the object.
     */
    std::string Dump() const;

    /**
     * @brief Converts a MessageUser object into a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns true if succeed; returns false otherwise.
     */
    bool ToJson(nlohmann::json &jsonObject) const override;

    /**
     * @brief Creates a MessageUser object from a Json.
     *
     * @param jsonObject Indicates the Json object.
     * @return Returns the MessageUser object.
     */
    static MessageUser *FromJson(const nlohmann::json &jsonObject);

    /**
     * @brief Marshals a MessageUser object into a Parcel.
     *
     * @param parcel Indicates the Parcel object for marshalling.
     * @return Returns true if the marshalling is successful; returns false otherwise.
     */
    bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshals a MessageUser object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns true if the unmarshalling is successful; returns false otherwise.
     */
    static MessageUser *Unmarshalling(Parcel &parcel);

private:
    /**
     * @brief Read NotificationSlot object from a Parcel.
     *
     * @param parcel Indicates the parcel object.
     * @return Returns true if succeed; returns false otherwise.
     */
    bool ReadFromParcel(Parcel &parcel);

private:
    std::string key_ {};
    std::string name_ {};
    std::shared_ptr<Media::PixelMap> pixelMap_ {nullptr};
    Uri uri_;
    bool isMachine_ {false};
    bool isUserImportant_ {false};

    // no object in parcel
    static constexpr int32_t VALUE_NULL = -1;
    // object exist in parcel
    static constexpr int32_t VALUE_OBJECT = 1;
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_INTERFACES_INNER_API_MESSAGE_USER_H