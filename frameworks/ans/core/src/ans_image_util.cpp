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
#include "ans_image_util.h"
#include "ans_log_wrapper.h"
#include "image_packer.h"
#include "image_source.h"

namespace OHOS {
namespace Notification {
const std::string AnsImageUtil::IMAGE_FORMAT_JPEG {"image/jpeg"};
const uint8_t AnsImageUtil::IMAGE_QUALITY {100};
const uint8_t AnsImageUtil::SHIFT_FOUR {4};
const uint8_t AnsImageUtil::NUM_TEN {10};
const size_t  AnsImageUtil::TWO_TIMES {2};

std::string AnsImageUtil::PackImage(const std::shared_ptr<Media::PixelMap> &pixelMap, const std::string &format)
{
    if (!pixelMap || format.empty()) {
        ANS_LOGW("invalid parameters");
        return {};
    }

    Media::ImagePacker imagePacker;
    Media::PackOption option;
    option.format     = format;
    option.quality    = IMAGE_QUALITY;
    option.numberHint = 1;

    std::set<std::string> formats;
    auto ret = imagePacker.GetSupportedFormats(formats);
    if (ret) {
        ANS_LOGE("image packer get supported format failed, ret : %{public}u", ret);
        return {};
    }

    auto size = static_cast<uint32_t>(pixelMap->GetByteCount());
    ANS_LOGI("size of pixelMap : %{public}u", size);
    auto pbuf = new (std::nothrow) uint8_t [size];
    if (pbuf == nullptr) {
        ANS_LOGE("create buffer failed");
        return {};
    }

    imagePacker.StartPacking(pbuf, size, option);
    imagePacker.AddImage(*pixelMap);
    int64_t packedSize {0};
    imagePacker.FinalizePacking(packedSize);
    ANS_LOGI("packed size : %{public}" PRId64, packedSize);

    std::string pixelMapStr(reinterpret_cast<char*>(pbuf), static_cast<size_t>(packedSize));
    delete [] pbuf;
    pbuf = nullptr;

    return BinToHex(pixelMapStr);
}

std::shared_ptr<Media::PixelMap> AnsImageUtil::UnPackImage(const std::string &pixelMapStr)
{
    if (pixelMapStr.empty()) {
        return {};
    }

    auto binStr = HexToBin(pixelMapStr);

    uint32_t errorCode {0};
    Media::SourceOptions opts;
    auto imageSource = Media::ImageSource::CreateImageSource(
        reinterpret_cast<const uint8_t*>(binStr.data()),
        static_cast<uint32_t>(binStr.length()),
        opts, errorCode);
    if (errorCode || !imageSource) {
        ANS_LOGE("create imageSource failed");
        return {};
    }

    Media::DecodeOptions decodeOpts;
    auto pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    if (errorCode || !pixelMap) {
        ANS_LOGE("create pixelMap failed");
        return {};
    }

    return pixelMap;
}

bool AnsImageUtil::PackImage2File(
    const std::shared_ptr<Media::PixelMap> &pixelMap,
    const std::string &outFilePath,
    const std::string &format)
{
    if (!pixelMap || outFilePath.empty() || format.empty()) {
        ANS_LOGW("invalid parameters");
        return false;
    }

    Media::ImagePacker imagePacker;
    Media::PackOption option;
    option.format     = format;
    option.quality    = IMAGE_QUALITY;
    option.numberHint = 1;

    std::set<std::string> formats;
    auto ret = imagePacker.GetSupportedFormats(formats);
    if (ret) {
        ANS_LOGE("image packer get supported format failed, ret : %{public}u", ret);
        return false;
    }

    imagePacker.StartPacking(outFilePath, option);
    imagePacker.AddImage(*pixelMap);
    int64_t packedSize {0};
    imagePacker.FinalizePacking(packedSize);
    ANS_LOGI("packed size : %{public}" PRId64, packedSize);
    return true;
}

std::shared_ptr<Media::PixelMap> AnsImageUtil::CreatePixelMap(const std::string &inFilePath, const std::string &format)
{
    if (inFilePath.empty() || format.empty()) {
        ANS_LOGW("invalid parameters");
        return {};
    }

    uint32_t errorCode {0};
    Media::SourceOptions opts;
    opts.formatHint = format;
    auto imageSource = Media::ImageSource::CreateImageSource(inFilePath, opts, errorCode);
    if (errorCode || !imageSource) {
        ANS_LOGE("create imageSource failed");
        return {};
    }

    std::set<std::string> formats;
    auto ret = imageSource->GetSupportedFormats(formats);
    if (ret) {
        ANS_LOGE("image packer get supported format failed, ret : %{public}u", ret);
        return {};
    }

    Media::DecodeOptions decodeOpts;
    auto pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    if (errorCode || !pixelMap) {
        ANS_LOGE("create pixelMap failed");
        return {};
    }

    return pixelMap;
}

std::string AnsImageUtil::BinToHex(const std::string &strBin)
{
    if (strBin.empty()) {
        return {};
    }

    std::string strHex;
    strHex.resize(strBin.size() * TWO_TIMES);
    for (size_t i = 0; i < strBin.size(); i++) {
        uint8_t cTemp = strBin[i];
        for (size_t j = 0; j < TWO_TIMES; j++) {
            uint8_t cCur = (cTemp & 0x0f);
            if (cCur < NUM_TEN) {
                cCur += '0';
            } else {
                cCur += ('a' - NUM_TEN);
            }
            strHex[TWO_TIMES * i + 1 - j] = cCur;
            cTemp >>= SHIFT_FOUR;
        }
    }

    return strHex;
}

std::string AnsImageUtil::HexToBin(const std::string &strHex)
{
    if (strHex.size() % TWO_TIMES != 0) {
        return {};
    }

    std::string strBin;
    strBin.resize(strHex.size() / TWO_TIMES);
    for (size_t i = 0; i < strBin.size(); i++) {
        uint8_t cTemp = 0;
        for (size_t j = 0; j < TWO_TIMES; j++) {
            char cCur = strHex[TWO_TIMES * i + j];
            uint8_t value = static_cast<uint8_t>(cTemp << SHIFT_FOUR);
            if (cCur >= '0' && cCur <= '9') {
                value += (cCur - '0');
            } else if (cCur >= 'a' && cCur <= 'f') {
                value += (cCur - 'a' + NUM_TEN);
            } else if (cCur >= 'A' && cCur <= 'F') {
                value += (cCur - 'A' + NUM_TEN);
            } else {
                return {};
            }
            cTemp = static_cast<uint8_t>(value);
        }
        strBin[i] = cTemp;
    }

    return strBin;
}
}  // namespace Notification
}  // namespace OHOS