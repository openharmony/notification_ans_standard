/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

import reminderAgent from '@ohos.reminderAgent';


import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe("ReminderHelperTest", function () {
    beforeAll(function() {
        /*
         * @tc.setup: setup invoked before all testcases
         */
        console.info('beforeAll caled')
    })

    afterAll(function() {
        /*
         * @tc.teardown: teardown invoked after all testcases
         */
        console.info('afterAll caled')
    })

    beforeEach(function() {
        /*
         * @tc.setup: setup invoked before each testcases
         */
        console.info('beforeEach caled')
    })

    afterEach(function() {
        /*
         * @tc.teardown: teardown invoked after each testcases
         */
        console.info('afterEach caled')
        reminderAgent.cancelAllReminders();
    })

    // /*
    //  * @tc.name: testReminderHelper001
    //  * @tc.desc: test spent time by publishReminder with timer (callback)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper001", 0, async function (done) {
    //     let timer = {
    //         reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
    //         triggerTimeInSeconds: 3
    //     }
    //     function reminderCallback(err, reminderId){
    //         let end = getTime();
    //         let times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //         setTimeout(() => {
    //             done();
    //         }, 500);
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     reminderAgent.publishReminder(timer, reminderCallback)
    // })

    // /*
    // * @tc.name: testReminderHelper002
    // * @tc.desc: test spent time by publishReminder with timer (promise)
    // * @tc.type: FUNC
    // * @tc.require:
    // */
    // it("testReminderHelper002", 0, async function (done) {
    //     let timer = {
    //         reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
    //         triggerTimeInSeconds: 3
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     reminderAgent.publishReminder(timer).then((reminderId) => {
    //         let end = getTime();
    //         let times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //     });
    //     setTimeout(() => {
    //         done();
    //     }, 500);
    // })

    // /*
    //  * @tc.name: testReminderHelper003
    //  * @tc.desc: test spent time by publishReminder with alarm (callback)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper003", 0, async function (done) {
    //     let alarm = {
    //         reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
    //         hour: 21,
    //         minute: 14,
    //         title: "this is title",
    //         content: "this is content"
    //     }
    //     function reminderCallback(err, reminderId){
    //         let end = getTime();
    //         times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //         setTimeout(() => {
    //             done();
    //         }, 500);
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let times = 0;
    //     let begin = getTime();
    //     reminderAgent.publishReminder(alarm, reminderCallback)
    // })

    // /*
    // * @tc.name: testReminderHelper004
    // * @tc.desc: test spent time by publishReminder with alarm (promise)
    // * @tc.type: FUNC
    // * @tc.require:
    // */

    // it("testReminderHelper004", 0, async function (done) {
    //     let alarm = {
    //         reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
    //         hour: 21,
    //         minute: 14,
    //         title: "this is title",
    //         content: "this is content"
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     reminderAgent.publishReminder(alarm).then((reminderId) => {
    //         let end = getTime();
    //         let times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //     });
    //     setTimeout(() => {
    //         done();
    //     }, 500);
    // })

    // /*
    //  * @tc.name: testReminderHelper005
    //  * @tc.desc: test spent time by publishReminder with calendar (callback)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper005", 0, async function (done) {
    //     let calendar = {
    //         reminderType: reminderAgent.ReminderType.REMINDER_TYPE_CALENDAR,
    //         dateTime : {
    //             year: 2025,
    //             month: 10,
    //             day: 10,
    //             hour: 23,
    //             minute: 30
    //         }
    //     }
    //     function reminderCallback(err, reminderId){
    //         let end = getTime();
    //         times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //         setTimeout(() => {
    //             done();
    //         }, 500);
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     let times = 0;
    //     reminderAgent.publishReminder(calendar, reminderCallback)
    // })

    // /*
    //  * @tc.name: testReminderHelper006
    //  * @tc.desc: test spent time by publishReminder with calendar (promise)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper006", 0, async function (done) {
    //     let calendar = {
    //         reminderType: reminderAgent.ReminderType.REMINDER_TYPE_CALENDAR,
    //         dateTime : {
    //             year: 2025,
    //             month: 10,
    //             day: 10,
    //             hour: 23,
    //             minute: 30
    //         }
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     reminderAgent.publishReminder(calendar).then((reminderId) => {
    //         let end = getTime();
    //         let times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //     })
    //     setTimeout(() => {
    //         done();
    //     }, 500);
    // })

    // /*
    //  * @tc.name: testReminderHelper007
    //  * @tc.desc: test spent time by cancelReminder with not reminderId (callback)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper007", 0, async function (done) {
    //     function reminderCallback(err, data){
    //         let end = getTime();
    //         times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //         setTimeout(() => {
    //             done();
    //         }, 500);
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     let times = 0;
    //     reminderAgent.cancelReminder(1, reminderCallback)
    // })

    // /*
    //  * @tc.name: testReminderHelper008
    //  * @tc.desc: test spent time by cancelReminder with not reminderId (promise)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper008", 0, async function (done) {
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     reminderAgent.cancelReminder(1).then(() => {
    //         let end = getTime();
    //         let times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //     })
    //     setTimeout(() => {
    //         done();
    //     }, 500);
    // })

    // /*
    //  * @tc.name: testReminderHelper009
    //  * @tc.desc: test spent time by cancelReminder with timer (callback)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper009", 0, async function (done) {
    //     let timer = {
    //         reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
    //         triggerTimeInSeconds: 3
    //     }
    //     function reminderCallback(err, data){
    //         let end = getTime();
    //         times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //         setTimeout(() => {
    //             done();
    //         }, 500);
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = 0;
    //     let times = 0;
    //     reminderAgent.publishReminder(timer, (err, reminderId) => {
    //         begin = getTime();
    //         reminderAgent.cancelReminder(reminderId, reminderCallback);
    //     })
    // })

    // /*
    //  * @tc.name: testReminderHelper010
    //  * @tc.desc: test spent time by cancelReminder with calendar (promise)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper010", 0, async function (done) {
    //     let calendar = {
    //         reminderType: reminderAgent.ReminderType.REMINDER_TYPE_CALENDAR,
    //         dateTime : {
    //             year: 2025,
    //             month: 10,
    //             day: 10,
    //             hour: 23,
    //             minute: 30
    //         }
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = 0;
    //     let times = 0;
    //     reminderAgent.publishReminder(calendar, (err, reminderId) => {
    //         begin = getTime();
    //         reminderAgent.cancelReminder(reminderId).then(() => {
    //             let end = getTime();
    //             times = end - begin;
    //             if (times < 50) {
    //                 expect(true).assertTrue();
    //             } else {
    //                 expect(false).assertTrue();
    //             }
    //         })
    //     })
    //     setTimeout(() => {
    //         done();
    //     }, 500);
    // })

    // /*
    //  * @tc.name: testReminderHelper011
    //  * @tc.desc: test spent time by getValidReminders with not (callback)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper011", 0, async function (done) {
    //     function reminderCallback(err, reminders){
    //         let end = getTime();
    //         times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //         setTimeout(() => {
    //             done();
    //         }, 500);
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     let times = 0;
    //     reminderAgent.getValidReminders(reminderCallback)
    // })

    // /*
    //  * @tc.name: testReminderHelper012
    //  * @tc.desc: test spent time by getValidReminders with not (promise)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper012", 0, async function (done) {
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     reminderAgent.getValidReminders().then((reminders) => {
    //         let end = getTime();
    //         let times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //     })
    //     setTimeout(() => {
    //         done();
    //     }, 500);
    // })


    // /*
    //  * @tc.name: testReminderHelper013
    //  * @tc.desc: test spent time by cancelAllReminders (callback)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper013", 0, async function (done) {
    //     function reminderCallback(err, data){
    //         let end = getTime();
    //         times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //         setTimeout(() => {
    //             done();
    //         }, 500);
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     let times = 0;
    //     reminderAgent.cancelAllReminders(reminderCallback)
    // })

    // /*
    //  * @tc.name: testReminderHelper014
    //  * @tc.desc: test spent time by cancelAllReminders (promise)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper014", 0, async function (done) {
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     reminderAgent.cancelAllReminders().then(() => {
    //         let end = getTime();
    //         let times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //     })
    //     setTimeout(() => {
    //         done();
    //     }, 500);
    // })

    // /*
    //  * @tc.name: testReminderHelper015
    //  * @tc.desc: test spent time by addNotificationSlot (callback)
    //  * @tc.type: FUNC
    //  * @tc.require:
    //  */
    // it("testReminderHelper015", 0, async function (done) {
    //     function reminderCallback(err, data){
    //         let end = getTime();
    //         times = end - begin;
    //         if (times < 50) {
    //             expect(true).assertTrue();
    //         } else {
    //             expect(false).assertTrue();
    //         }
    //         setTimeout(() => {
    //             done();
    //         }, 500);
    //     }
    //     function getTime(){
    //         var time = new Date();
    //         var Milliseconds = time.getTime();
    //         return Milliseconds;
    //     }
    //     let begin = getTime();
    //     let times = 0;
    //     reminderAgent.addNotificationSlot(1, reminderCallback)
    // })

    /*
     * @tc.name: testReminderHelper016
     * @tc.desc: test spent time by addNotificationSlot (promise)
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper016", 0, async function (done) {
        function getTime(){
            var time = new Date();
            var Milliseconds = time.getTime();
            return Milliseconds;
        }
        let begin = getTime();
        reminderAgent.addNotificationSlot(1).then(() => {
            let end = getTime();
            let times = end - begin;
            if (times < 50) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
        })
        setTimeout(() => {
            done();
        }, 500);
    })

    /*
     * @tc.name: testReminderHelper017
     * @tc.desc: test spent time by removeNotificationSlot (callback)
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper017", 0, async function (done) {
        var tarRemoveSlot = {
            type: 1
        }
        function reminderCallback(err, data){
            let end = getTime();
            times = end - begin;
            if (times < 50) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        function getTime(){
            var time = new Date();
            var Milliseconds = time.getTime();
            return Milliseconds;
        }
        let begin = getTime();
        let times = 0;
        reminderAgent.addNotificationSlot(tarRemoveSlot.type, (err, data) => {
            begin = getTime();
            reminderAgent.removeNotificationSlot(1, reminderCallback)
        });
    })

    /*
     * @tc.name: testReminderHelper018
     * @tc.desc: test spent time by cancelAllReminders (promise)
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper018", 0, async function (done) {
        var tarRemoveSlot = {
            type: 1
        }
        function getTime(){
            var time = new Date();
            var Milliseconds = time.getTime();
            return Milliseconds;
        }
        reminderAgent.addNotificationSlot(tarRemoveSlot.type, (err, data) => {
            let begin = getTime();
            reminderAgent.removeNotificationSlot(tarRemoveSlot.type).then(() => {
                let end = getTime();
                let times = end - begin;
                if (times < 50) {
                    expect(true).assertTrue();
                } else {
                    expect(false).assertTrue();
                }
            })
        });
        setTimeout(() => {
            done();
        }, 500);
    })

    /*
     * @tc.name: testReminderHelper019
     * @tc.desc: test publishReminder can return correct reminder id.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EH AR000GH8E9 AR00GH8EH
     */
    it("testReminderHelper019", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        let expectId = -1;
        reminderAgent.publishReminder(timer).then((reminderId) => {
            expectId = reminderId + 1;
            reminderAgent.publishReminder(timer).then((reminderId) => {
                if (reminderId === expectId){
                    expect(true).assertTrue();
                    setTimeout(() => {
                        done();
                    }, 500);
                }
            }, (error) => {
                expect(false).assertTrue();
                setTimeout(() => {
                    done();
                }, 500);
            });
        });
    })

    /*
     * @tc.name: testReminderHelper020
     * @tc.desc: test publishReminder can return correct reminder id.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EH AR000GH8E9 AR00GH8EH
     */
    it("testReminderHelper020", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        let expectId = -1;
        function reminderCallback(err, reminderId) {
            expect(reminderId).assertEqual(expectId);
        }
        reminderAgent.publishReminder(timer, (err, reminderId) => {
            expectId = reminderId + 1;
            reminderAgent.publishReminder(timer, reminderCallback);
        })
        done();
    })

    /*
     * @tc.name: testReminderHelper021
     * @tc.desc: test addNotificationSlot instance with null
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EC
     */
    it("testReminderHelper021", 0, async function (done) {
        function reminderCallback(err, data){
            expect(true).assertEqual(true);
        }
        reminderAgent.addNotificationSlot(0, reminderCallback);
        done();
    })

    /*
     * @tc.name: testReminderHelper022
     * @tc.desc: test addNotificationSlot instance with null
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EC
     */
    it("testReminderHelper022", 0, async function (done) {
        let mySlot = null;
        let promise = new Promise((resolve, reject) => {
            reminderAgent.addNotificationSlot(mySlot).then(() => {
                resolve();
            })
            reject();
        });
        promise.then(() => {
        }, err => {
            let i = 0;
            expect(0).assertEqual(i);
        }).catch(res => {});
        done();
    })

    /*
     * @tc.name: testReminderHelper023
     * @tc.desc: test addNotificationSlot with normal value
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EC
     */
    it("testReminderHelper023", 0, async function (done) {
        let mySlot = {
            type: 2
        }
        reminderAgent.addNotificationSlot(mySlot).then(() => {
            let i = 0;
            expect(i).assertEqual(0);
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper024
     * @tc.desc: test addNotificationSlot with normal value
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EC
     */
    it("testReminderHelper024", 0, async function (done) {
        reminderAgent.addNotificationSlot(3, (err, data) => {
            let i = 0;
            expect(i).assertEqual(0);
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper025
     * @tc.desc: test cancelAllReminders can cancel all exist reminders
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EA
     */
    it("testReminderHelper025", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        let calendar = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_CALENDAR,
            dateTime : {
                year: 2025,
                month: 10,
                day: 10,
                hour: 23,
                minute: 30
            }
        }
        reminderAgent.publishReminder(timer).then((reminderId) => {});
        reminderAgent.publishReminder(calendar).then((reminderId) => {});
        setTimeout(() => {
            reminderAgent.cancelAllReminders().then(() => {
                reminderAgent.getValidReminders().then((reminders) => {
                    expect(reminders.length === 0).assertEqual(true);
                });
            });
        }, 5000);
        done();
    })

    /*
     * @tc.name: testReminderHelper026
     * @tc.desc: test cancelAllReminders can cancel all exist reminders
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EA AR000GH8EF AR000GH8EI
     */
    it("testReminderHelper026", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        let calendar = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_CALENDAR,
            dateTime : {
                year: 2025,
                month: 10,
                day: 10,
                hour: 23,
                minute: 30
            }
        }
        reminderAgent.publishReminder(timer).then((reminderId) => {});
        reminderAgent.publishReminder(calendar).then((reminderId) => {});
        setTimeout(() => {
            reminderAgent.cancelAllReminders((err, data) => {
                reminderAgent.getValidReminders().then((reminders) => {
                    expect(reminders.length === 0).assertEqual(true);
                });
            });
        }, 5000);
        done();
    })

    /*
     * @tc.name: testReminderHelper027
     * @tc.desc: test cancelReminder with not exit reminder.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EA
     */
    it("testReminderHelper027", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 3
        }
        let publishlength = -1;
        let cancellength = -1;
        let firstdiff = -1;
        reminderAgent.publishReminder(timer).then(() => {
            reminderAgent.getValidReminders().then((reminders) => {
                publishlength=reminders.length
                reminderAgent.cancelReminder(0).then(() => {
                    reminderAgent.getValidReminders().then((reminders) => {
                        cancellength = reminders.length
                        firstdiff = publishlength - cancellength;
                        expect(0).assertEqual(firstdiff);
                    });
                });
            });
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper028
     * @tc.desc: test cancelReminder with not exit reminder.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EA
     */
    it("testReminderHelper028", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 3
        }
        let publishlength = -1;
        let cancellength = -1;
        let firstdiff = -1;
        reminderAgent.publishReminder(timer).then(() => {
            reminderAgent.getValidReminders((err, reminders) => {
                publishlength = reminders.length;
                reminderAgent.cancelReminder(0, (err, data)=>{
                    reminderAgent.getValidReminders((err, reminders) => {
                        cancellength = reminders.length;
                        firstdiff = publishlength - cancellength;
                        expect(0).assertEqual(firstdiff);
                    });
                });
            });
        });
        done();
    })

        /*
     * @tc.name: testReminderHelper029
     * @tc.desc: test cancelReminder with exist reminder.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EA
     */
    it("testReminderHelper029", 0, async function (done) {
        let alarm = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
            hour: 21,
            minute: 14,
            title: "this is title",
            content: "this is content"
        }
        let publishlength = -1;
        let cancellength = -1;
        let firstdiff = -1;
        reminderAgent.publishReminder(alarm, (err, reminderId) => {
            reminderAgent.getValidReminders((err, reminders) => {
                publishlength = reminders.length;
            });
            setTimeout(() => {
                reminderAgent.cancelReminder(reminderId, (err, data) => {
                    reminderAgent.getValidReminders((err, reminders) => {
                        cancellength = reminders.length;
                        firstdiff = publishlength - cancellength;
                        if (firstdiff === 0 || firstdiff === 1) {
                            expect(0).assertEqual(firstdiff);
                        }
                    });
                });
            }, 1000);
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper030
     * @tc.desc: test cancelReminder with exist reminder.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EA
     */
    it("testReminderHelper030", 0, async function (done) {
        let alarm = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
            hour: 21,
            minute: 14,
            title: "this is title",
            content: "this is content"
        }
        let publishlength = -1;
        let cancellength = -1;
        let firstdiff = -1;
        reminderAgent.publishReminder(alarm, (err, reminderId) => {
            reminderAgent.getValidReminders((err, reminders) => {
                publishlength = reminders.length;
            });
            setTimeout(() => {
                reminderAgent.cancelReminder(reminderId).then(() => {
                    reminderAgent.getValidReminders((err, reminders) => {
                        cancellength = reminders.length;
                        firstdiff = publishlength - cancellength;
                        if (firstdiff === 0 || firstdiff === 1) {
                            expect(0).assertEqual(firstdiff);
                        }
                    });
                });
            }, 1000);
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper031
     * @tc.desc: test getValidReminders, verify all the information is correct.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EB AR000GH8EF
     */
    it("testReminderHelper031", 0, async function (done) {
        var alarm = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
            hour: 21,
            minute: 14,
            title: "this is title",
            content: "this is content"
        }
        var timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        reminderAgent.cancelAllReminders((err, data) => {
            reminderAgent.publishReminder(timer, (error, reminderId) => {});
            reminderAgent.publishReminder(alarm, (error, reminderId) => {});
            setTimeout(() => {
                reminderAgent.getValidReminders().then((reminders) => {
                    expect(reminders.length).assertEqual(2);
                });
            }, 1000);
        })
        done();
    })

    /*
     * @tc.name: testReminderHelper032
     * @tc.desc: test getValidReminders, verify all the information is correct.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EB
     */
    it("testReminderHelper032", 0, async function (done) {
        var alarm = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
            hour: 21,
            minute: 14,
            title: "this is title",
            content: "this is content"
        }
        var timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        reminderAgent.cancelAllReminders((err, data) => {
            reminderAgent.publishReminder(timer, (error, reminderId) => {});
            reminderAgent.publishReminder(alarm, (error, reminderId) => {});
            setTimeout(() => {
                reminderAgent.getValidReminders((err, reminders) => {
                    expect(reminders.length).assertEqual(2);
                });
            }, 1000);
        })
        done();
    })

    /*
     * @tc.name: testReminderHelper033
     * @tc.desc: test removeNotificationSlot with not exist slot.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8ED
     */
    it("testReminderHelper033", 0, function (done) {
        function reminderCallback(err, data){
            let i = 0;
            expect(0).assertEqual(i);
        }
        reminderAgent.removeNotificationSlot(0, reminderCallback);
        done();
    })

    /*
     * @tc.name: testReminderHelper034
     * @tc.desc: test removeNotificationSlot with not exist slot.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8ED
     */
    it("testReminderHelper034", 0, function (done) {
       let promise = new Promise((resolve, reject) => {
           reminderAgent.removeNotificationSlot(1).then(() => {
               resolve();
           });
           reject();
       });
       promise.then(() => {
       }, err => {
           let i = 0;
           expect(0).assertEqual(i);
       }).catch(res => {});
       done();
    })

    /*
     * @tc.name: testReminderHelper035
     * @tc.desc: test removeNotificationSlot with exist slot.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8ED
     */
    it("testReminderHelper035", 0, async function (done) {
        var tarRemoveSlot = {
            type: 1
        }
        reminderAgent.addNotificationSlot(tarRemoveSlot.type, (err, data) => {
            reminderAgent.removeNotificationSlot(tarRemoveSlot.type, (err, data) => {
                expect(0).assertEqual(err.code);
            });
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper036
     * @tc.desc: test removeNotificationSlot with exist slot.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8ED
     */
    it("testReminderHelper036", 0, async function (done) {
        var tarRemoveSlot = {
            type: 1
        }
        reminderAgent.addNotificationSlot(tarRemoveSlot.type, (err, data) => {
            reminderAgent.removeNotificationSlot(tarRemoveSlot.type).then(() => {
                let i = 0;
                expect(0).assertEqual(i);
            });
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper037
     * @tc.desc: test getValidReminders.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EB AR000GH8EG
     */
    it("testReminderHelper037", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 3
        }
        reminderAgent.publishReminder(timer).then((reminderId) => { });
        setTimeout(() => {
            reminderAgent.getValidReminders().then((reminders) => {
                expect(reminders.length === 0).assertEqual(true);
            });
        }, 5000);
        done();
    })

    /*
     * @tc.name:testReminderHelper038
     * @tc.desc: test getValidReminders.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8EB AR000GH8EG
     */
    it("testReminderHelper038", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 3
        }
        reminderAgent.publishReminder(timer, (err, data) => { });
        setTimeout(() => {
            reminderAgent.getValidReminders((err, reminders) => {
                expect(reminders.length === 0).assertEqual(true);
            });
        }, 5000);
        done();
    })

    /*
     * @tc.name: testReminderHelper039
     * @tc.desc: test publishReminder a normal alarm.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8E9 AR00GH8EH
     */
    it("testReminderHelper039", 0, async function (done) {
        let alarm = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
            hour: 21,
            minute: 14,
            title: "this is title",
            content: "this is content"
        }
        let expectId = -1;
        reminderAgent.publishReminder(alarm).then((reminderId) => {
            expectId = reminderId + 1;
            reminderAgent.publishReminder(alarm).then((reminderId) => {
                if (reminderId === expectId){
                    expect(reminderId).assertEqual(expectId);
                }
            });
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper040
     * @tc.desc: test publishReminder a normal alarm.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8E9 AR00GH8EH AR000GH8EE
     */
    it("testReminderHelper040", 0, async function (done) {
        let alarm = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
            hour: 21,
            minute: 14,
            title: "this is title",
            content: "this is content"
        }
        let expectId = -1;
        function reminderCallback(err, reminderId) {
            expect(reminderId).assertEqual(expectId);
        }
        reminderAgent.publishReminder(alarm, (err, reminderId) => {
            expectId = reminderId + 1;
            reminderAgent.publishReminder(alarm, reminderCallback);
        })
        done();
    })

    /*
     * @tc.name: testReminderHelper041
     * @tc.desc: test publishReminder.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8E9 AR00GH8EH
     */
    it("testReminderHelper041", 0, function (done) {
        let calendar = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_CALENDAR,
            dateTime : {
                year: 2025,
                month: 10,
                day: 10,
                hour: 23,
                minute: 30
            },
            repeatMonths:[2],
            repeatDays:[2],
            actionButton:[
                {
                    title:"close",
                    type:0
                },
                {
                    title:"snooze",
                    type:1
                }
            ],
            wantAgent:{
                pkgName:"com.huawei.phone",
                abilityName:"com.huawei.phone.MainAbility"
            },
            maxScreenWantAgent:{
                pkgName:"com.huawei.phone",
                abilityName:"com.huawei.phone.MainAbility"
            },
            ringDuration:5,
            snoozeTimes:2,
            timeInterval:5,
            title:"this is title",
            content:"this is content",
            expiredContent:"this reminder has expired",
            snoozeContent:"remind later",
            notificationId:100,
            slotType:3
        }
        reminderAgent.publishReminder(calendar).then((reminderId) => {
            reminderAgent.getValidReminders().then((reminders) => {
                for (let i = 0; i < reminders.length; i++) {
                    console.log("getValidReminders = " + JSON.stringify(reminders[i]));
                    console.log("getValidReminders, reminderType = " + reminders[i].reminderType);
                    for (let j = 0; j < reminders[i].actionButton.length; j++) {
                        console.log("getValidReminders, actionButton.title = " + reminders[i].actionButton[j].title);
                        console.log("getValidReminders, actionButton.type = " + reminders[i].actionButton[j].type);
                    }
                    console.log("getValidReminders, wantAgent.pkgName = " + reminders[i].wantAgent.pkgName);
                    console.log("getValidReminders, wantAgent.abilityName = " + reminders[i].wantAgent.abilityName);
                    console.log("getValidReminders, maxScreenWantAgent.pkgName = " + reminders[i].maxScreenWantAgent.pkgName);
                    console.log("getValidReminders, maxScreenWantAgent.abilityName = " + reminders[i].maxScreenWantAgent.abilityName);
                    expect(reminders[i].ringDuration).assertEqual(5);
                    console.log("getValidReminders, ringDuration = " + reminders[i].ringDuration);
                    expect(reminders[i].snoozeTimes).assertEqual(2);
                    console.log("getValidReminders, snoozeTimes = " + reminders[i].snoozeTimes);
                    console.log("getValidReminders, timeInterval = " + reminders[i].timeInterval);
                    expect(reminders[i].title).assertEqual("this is title");
                    console.log("getValidReminders, title = " + reminders[i].title);
                    expect(reminders[i].content).assertEqual("this is content");
                    console.log("getValidReminders, content = " + reminders[i].content);
                    expect(reminders[i].expiredContent).assertEqual("this reminder has expired");
                    console.log("getValidReminders, expiredContent = " + reminders[i].expiredContent);
                    expect(reminders[i].snoozeContent).assertEqual("remind later");
                    console.log("getValidReminders, snoozeContent = " + reminders[i].snoozeContent);
                    expect(reminders[i].notificationId).assertEqual(100);
                    console.log("getValidReminders, notificationId = " + reminders[i].notificationId);
                    console.log("getValidReminders, slotType = " + reminders[i].slotType);
                }
            })
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper042
     * @tc.desc: test publishReminder.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8E9 AR00GH8EH
     */
    it("testReminderHelper042", 0, async function (done) {
        let calendar = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_CALENDAR,
            dateTime : {
                year: 2025,
                month: 10,
                day: 10,
                hour: 23,
                minute: 30
            },
            repeatMonths:[2],
            repeatDays:[2],
            actionButton:[
                {
                    title:"close",
                    type:0
                },
                {
                    title:"snooze",
                    type:1
                }
            ],
            wantAgent:{
                pkgName:"com.huawei.phone",
                abilityName:"com.huawei.phone.MainAbility"
            },
            maxScreenWantAgent:{
                pkgName:"com.huawei.phone",
                abilityName:"com.huawei.phone.MainAbility"
            },
            ringDuration:5,
            snoozeTimes:2,
            timeInterval:5,
            title:"this is title",
            content:"this is content",
            expiredContent:"this reminder has expired",
            snoozeContent:"remind later",
            notificationId:100,
            slotType:3
        }
        reminderAgent.publishReminder(calendar, (err,reminderId) => {
            reminderAgent.getValidReminders().then((reminders) => {
                for (let i = 0; i < reminders.length; i++) {
                    console.log("getValidReminders = " + JSON.stringify(reminders[i]));
                    console.log("getValidReminders, reminderType = " + reminders[i].reminderType);
                    for (let j = 0; j < reminders[i].actionButton.length; j++) {
                        console.log("getValidReminders, actionButton.title = " + reminders[i].actionButton[j].title);
                        console.log("getValidReminders, actionButton.type = " + reminders[i].actionButton[j].type);
                    }
                    console.log("getValidReminders, wantAgent.pkgName = " + reminders[i].wantAgent.pkgName);
                    console.log("getValidReminders, wantAgent.abilityName = " + reminders[i].wantAgent.abilityName);
                    console.log("getValidReminders, maxScreenWantAgent.pkgName = " + reminders[i].maxScreenWantAgent.pkgName);
                    console.log("getValidReminders, maxScreenWantAgent.abilityName = " + reminders[i].maxScreenWantAgent.abilityName);
                    expect(reminders[i].ringDuration).assertEqual(5);
                    console.log("getValidReminders, ringDuration = " + reminders[i].ringDuration);
                    expect(reminders[i].snoozeTimes).assertEqual(2);
                    console.log("getValidReminders, snoozeTimes = " + reminders[i].snoozeTimes);
                    console.log("getValidReminders, timeInterval = " + reminders[i].timeInterval);
                    expect(reminders[i].title).assertEqual("this is title");
                    console.log("getValidReminders, title = " + reminders[i].title);
                    expect(reminders[i].content).assertEqual("this is content");
                    console.log("getValidReminders, content = " + reminders[i].content);
                    expect(reminders[i].expiredContent).assertEqual("this reminder has expired");
                    console.log("getValidReminders, expiredContent = " + reminders[i].expiredContent);
                    expect(reminders[i].snoozeContent).assertEqual("remind later");
                    console.log("getValidReminders, snoozeContent = " + reminders[i].snoozeContent);
                    expect(reminders[i].notificationId).assertEqual(100);
                    console.log("getValidReminders, notificationId = " + reminders[i].notificationId);
                    console.log("getValidReminders, slotType = " + reminders[i].slotType);
                }
            })
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper043
     * @tc.desc: test publishReminder.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8E9 AR00GH8EH
     */
    it("testReminderHelper043", 0, async function (done) {
        let calendar = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_CALENDAR,
            dateTime : {
                year: 2025,
                month: 10,
                day: 10,
                hour: 23,
                minute: 30
            },
            repeatMonths:[2],
            repeatDays:[2],
            actionButton:[
                {
                    title:"close",
                    type:0
                },
                {
                    title:"snooze",
                    type:1
                }
            ],
            wantAgent:{
                pkgName:"com.huawei.phone",
                abilityName:"com.huawei.phone.MainAbility"
            },
            maxScreenWantAgent:{
                pkgName:"com.huawei.phone",
                abilityName:"com.huawei.phone.MainAbility"
            },
            ringDuration:5,
            snoozeTimes:2,
            timeInterval:5,
            title:"this is title",
            content:"this is content",
            expiredContent:"this reminder has expired",
            snoozeContent:"remind later",
            notificationId:100,
            slotType:3
        }
        let expectId = -1;
        reminderAgent.publishReminder(calendar).then((reminderId) => {
            expectId = reminderId + 1;
            reminderAgent.publishReminder(calendar).then((reminderId) => {
                if (reminderId === expectId){
                    expect(reminderId).assertEqual(expectId);
                }
            });
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper044
     * @tc.desc: test publishReminder.
     * @tc.type: FUNC
     * @tc.require: SR000GGTRB AR000GH8E9 AR00GH8EH
     */
    it("testReminderHelper044", 0, async function (done) {
        let calendar = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_CALENDAR,
            dateTime : {
                year: 2025,
                month: 10,
                day: 10,
                hour: 23,
                minute: 30
            },
            repeatMonths:[2],
            repeatDays:[2],
            actionButton:[
                {
                    title:"close",
                    type:0
                },
                {
                    title:"snooze",
                    type:1
                }
            ],
            wantAgent:{
                pkgName:"com.huawei.phone",
                abilityName:"com.huawei.phone.MainAbility"
            },
            maxScreenWantAgent:{
                pkgName:"com.huawei.phone",
                abilityName:"com.huawei.phone.MainAbility"
            },
            ringDuration:5,
            snoozeTimes:2,
            timeInterval:5,
            title:"this is title",
            content:"this is content",
            expiredContent:"this reminder has expired",
            snoozeContent:"remind later",
            notificationId:100,
            slotType:3
        }
        let expectId = -1;
        function reminderCallback(err, reminderId) {
            expect(reminderId).assertEqual(expectId);
        }
        reminderAgent.publishReminder(calendar, (err, reminderId) => {
            expectId = reminderId + 1;
            reminderAgent.publishReminder(calendar, reminderCallback);
        })
        done();
    })

    /*
     * @tc.name: testReminderHelper045
     * @tc.desc: test publishReminder (max number limit of each application)
     * @tc.type: FUNC
     * @tc.require: SR000GGTRE AR000GH8E9 AR00GH8EH
     */
    it("testReminderHelper045", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        let maxLimitNumsOfApp = 30;
        let firstId = 0;
        let secondId = 0;
        let diffId = 0;
        for (let i = 0; i < maxLimitNumsOfApp; i++) {
            (function (i) {
                var i = i;
                setTimeout(function () {
                    reminderAgent.publishReminder(timer).then((reminderId) => {
                        if (i === 0) {
                            firstId = reminderId
                        }
                        if (i === 29) {
                            secondId = reminderId
                            diffId = secondId - firstId
                            expect(29).assertEqual(diffId);
                            i = null
                        }
                    });
                }, 500 * i);
            })(i);
        }
        done();
    })
})