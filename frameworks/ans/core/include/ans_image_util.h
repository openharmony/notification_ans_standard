/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_ANS_IMAGE_UTIL_H
#define BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_ANS_IMAGE_UTIL_H

#include <memory>
#include <string>
#include "pixel_map.h"

namespace OHOS {
namespace Notification {
class AnsImageUtil {
public:
    static const std::string IMAGE_FORMAT_JPEG;
    static const uint8_t IMAGE_QUALITY;
    static const uint8_t SHIFT_FOUR;
    static const uint8_t NUM_TEN;
    static const size_t  TWO_TIMES;

    /**
     * @brief Packs an image to a string.
     *
     * @param pixelMap Indicates the image to be packaged.
     * @param format Indicates the format of the image.
     * @return Returns a string.
     */
    static std::string PackImage(
        const std::shared_ptr<Media::PixelMap> &pixelMap, const std::string &format = IMAGE_FORMAT_JPEG);

    /**
     * @brief Unpacks the string to an image.
     *
     * @param pixelMapStr Indicates the string of image.
     * @return Returns an image object.
     */
    static std::shared_ptr<Media::PixelMap> UnPackImage(const std::string &pixelMapStr);

    /**
     * @brief Packs an image to a file.
     *
     * @param pixelMap Indicates the image to be packaged.
     * @param outFilePath Indicates the path of the output file.
     * @param format Indicates the format of the image.
     * @return Returns true if succeed; returns false otherwise.
     */
    static bool PackImage2File(
        const std::shared_ptr<Media::PixelMap> &pixelMap,
        const std::string &outFilePath,
        const std::string &format = IMAGE_FORMAT_JPEG);

    /**
     * @brief Creates an image from a file.
     *
     * @param inFilePath Indicates the path of the input file.
     * @param format Indicates the format of the image.
     * @return Returns an image object.
     */
    static std::shared_ptr<Media::PixelMap> CreatePixelMap(
        const std::string &inFilePath, const std::string &format = IMAGE_FORMAT_JPEG);

    /**
     * @brief Converts a binary string to a hexadecimal string.
     *
     * @param strBin Indicates the input binary string.
     * @return Returns a hexadecimal string.
     */
    static std::string BinToHex(const std::string &strBin);

    /**
     * @brief Converts a hexadecimal string to a binary string.
     *
     * @param strHex Indicates the input hexadecimal string.
     * @return Returns a binary string.
     */
    static std::string HexToBin(const std::string &strHex);
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_ANS_IMAGE_UTIL_H
