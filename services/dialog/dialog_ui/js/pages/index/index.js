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

import router from '@ohos.router'

export default {
    data: {
        message: router.getParams().message,
        requestNotification: router.getParams().requestNotification,
        allowButton: router.getParams().allowButton,
        cancelButton: router.getParams().cancelButton,
        uid: router.getParams().uid,
    },
    onInit() {
        console.info('getParams: ' + JSON.stringify(router.getParams()));
        this.requestNotification = this.$t('message.requestNotification');
        this.allowButton = this.$t('message.allowButton');
        this.cancelButton = this.$t('message.cancelButton');
    },
    onAllow() {
        console.info('clicked allow');
        callNativeHandler("EVENT_ALLOW", router.getParams().uid.toString());
    },
    onCancel() {
        console.info('clicked cancel');
        callNativeHandler("EVENT_CANCEL", router.getParams().uid.toString());
    },
}
