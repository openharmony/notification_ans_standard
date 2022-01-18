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

    static std::string PackImage(
        const std::shared_ptr<Media::PixelMap> &pixelMap, const std::string &format = IMAGE_FORMAT_JPEG);

    static std::shared_ptr<Media::PixelMap> UnPackImage(const std::string &pixelMapStr);

    static bool PackImage2File(
        const std::shared_ptr<Media::PixelMap> &pixelMap,
        const std::string &outFilePath,
        const std::string &format = IMAGE_FORMAT_JPEG);

    static std::shared_ptr<Media::PixelMap> CreatePixelMap(
        const std::string &inFilePath, const std::string &format = IMAGE_FORMAT_JPEG);

    static std::string BinToHex(const std::string &strBin);

    static std::string HexToBin(const std::string &strHex);
};
}  // namespace Notification
}  // namespace OHOS

#endif  // BASE_NOTIFICATION_ANS_STANDARD_FRAMEWORKS_ANS_CORE_INCLUDE_ANS_IMAGE_UTIL_H
