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
