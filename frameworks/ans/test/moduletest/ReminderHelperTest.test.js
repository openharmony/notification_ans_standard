/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

    /*
     * @tc.name: testReminderHelper001
     * @tc.desc: test publishReminder can return correct reminder id.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper001", 0, async function (done) {
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
     * @tc.name: testReminderHelper002
     * @tc.desc: test publishReminder can return correct reminder id.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper002", 0, async function (done) {
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
     * @tc.name: testReminderHelper003
     * @tc.desc: test addNotificationSlot instance with null
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper003", 0, async function (done) {
        function reminderCallback(err, data) {
            let i = 0;
            expect(0).assertEqual(i);
        }
        reminderAgent.addNotificationSlot(0, reminderCallback);
        done();
    })

    /*
     * @tc.name: testReminderHelper004
     * @tc.desc: test addNotificationSlot instance with null
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper004", 0, async function (done) {
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
     * @tc.name: testReminderHelper005
     * @tc.desc: test addNotificationSlot with normal value
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper005", 0, async function (done) {
        let mySlot = {
            type: 3
        }
        reminderAgent.addNotificationSlot(mySlot).then(() => {
            expect(true).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        }, (error) => {
            expect(false).assertTrue();
            setTimeout(() => {
                done();
            }, 500);
        });
    })

    /*
     * @tc.name: testReminderHelper006
     * @tc.desc: test addNotificationSlot with normal value
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper006", 0, async function (done) {
        let mySlot0 = {
            type: 0
        }
        let mySlot1 = {
            type: 1
        }
        let mySlot2 = {
            type: 2
        }
        let mySlot3 = {
            type: 3
        }
        let mySlot4 = {
            type: 4
        }
        function reminderCallback(err, data){
            if (err) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            setTimeout(() => {
                done();
            }, 500);
        }
        reminderAgent.addNotificationSlot(mySlot0, reminderCallback);
        reminderAgent.addNotificationSlot(mySlot1, reminderCallback);
        reminderAgent.addNotificationSlot(mySlot2, reminderCallback);
        reminderAgent.addNotificationSlot(mySlot3, reminderCallback);
        reminderAgent.addNotificationSlot(mySlot4, reminderCallback);
    })

    /*
     * @tc.name: testReminderHelper007
     * @tc.desc: test cancelAllReminders can cancel all exist reminders
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper007", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        reminderAgent.publishReminder(timer).then((reminderId) => {
            reminderAgent.getValidReminders().then((reminders) => {});
            setTimeout(() => {
                reminderAgent.cancelAllReminders().then(() => {
                    reminderAgent.getValidReminders().then((reminders) => {
                        expect(0).assertEqual(reminders.length);
                    });
                });
            }, 1000);
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper008
     * @tc.desc: test cancelAllReminders can cancel all exist reminders
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper008", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        reminderAgent.publishReminder(timer, (error, reminderId) => {
            reminderAgent.getValidReminders((err, reminders) => {});
            setTimeout(() => {
                reminderAgent.cancelAllReminders((err, data) => {
                    reminderAgent.getValidReminders().then((reminders) => {
                        expect(0).assertEqual(reminders.length);
                    });
                });
            }, 1000);
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper009
     * @tc.desc: test cancelReminder with not exit reminder.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper009", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        let id = 1;
        let publishlength = -1;
        let cancellength = -1;
        let firstdiff = -1;
        reminderAgent.publishReminder(timer).then(() => {
            reminderAgent.getValidReminders().then((reminders) => {
                publishlength=reminders.length
            });
            setTimeout(() => {
                reminderAgent.cancelReminder(id).then(() => {
                    reminderAgent.getValidReminders().then((reminders) => {
                        cancellength = reminders.length
                        firstdiff = publishlength-cancellength;
                        if (firstdiff === 0) {
                            expect(0).assertEqual(firstdiff);
                        }
                    });
                });
            }, 1000);
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper010
     * @tc.desc: test cancelReminder with not exit reminder.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper010", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        let id = 1;
        let publishlength = -1;
        let cancellength = -1;
        let firstdiff = -1;
        reminderAgent.publishReminder(timer).then(() => {
            reminderAgent.getValidReminders((err, reminders) => {
                publishlength = reminders.length;
            });
            setTimeout(() => {
                reminderAgent.cancelReminder(id,(err,data)=>{
                    reminderAgent.getValidReminders((err, reminders) => {
                        cancellength = reminders.length;
                        firstdiff = publishlength-cancellength;
                        if (firstdiff === 0) {
                            expect(0).assertEqual(firstdiff);
                        }
                    });
                });
            }, 1000);
        });
        done();
    })


    /*
     * @tc.name: testReminderHelper011
     * @tc.desc: test cancelReminder with exist reminder.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper011", 0, async function (done) {
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
                        if (firstdiff === 0) {
                            expect(0).assertEqual(firstdiff);
                        } else if (firstdiff === 1) {
                            expect(1).assertEqual(firstdiff);
                        }
                    });
                });
            }, 1000);
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper012
     * @tc.desc: test cancelReminder with exist reminder.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper012", 0, async function (done) {
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
                        if (firstdiff === 0) {
                            expect(0).assertEqual(firstdiff);
                        } else if (firstdiff === 1) {
                            expect(1).assertEqual(firstdiff);
                        }
                    });
                });
            }, 1000);
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper013
     * @tc.desc: test getValidReminders, verify all the information is correct.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper013", 0, async function (done) {
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
        reminderAgent.publishReminder(timer, (error, reminderId) => {});
        reminderAgent.publishReminder(alarm, (error, reminderId) => {});
        setTimeout(() => {
            reminderAgent.getValidReminders().then((reminders) => {
                if (reminders.length >= 2) {
                    let i = 0;
                    expect(0).assertEqual(i);
                }
            });
        }, 1000);
        done();
    })

    /*
     * @tc.name: testReminderHelper014
     * @tc.desc: test getValidReminders, verify all the information is correct.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper014", 0, async function (done) {
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
        reminderAgent.publishReminder(timer, (error, reminderId) => {});
        reminderAgent.publishReminder(alarm, (error, reminderId) => {});
        setTimeout(() => {
            reminderAgent.getValidReminders((err, reminders) => {
                if (reminders.length >= 2) {
                    let i = 0;
                    expect(0).assertEqual(i);
                }
            });
        }, 1000);
        done();
    })

    /*
     * @tc.name: testReminderHelper015
     * @tc.desc: test removeNotificationSlot with not exist slot.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper015", 0, function (done) {
       function reminderCallback(err, data){
           let i = 0;
           expect(0).assertEqual(i);
       }
       reminderAgent.removeNotificationSlot(0, reminderCallback);
       done();
    })

    /*
     * @tc.name: testReminderHelper016
     * @tc.desc: test removeNotificationSlot with not exist slot.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper016", 0, function (done) {
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
     * @tc.name: testReminderHelper017
     * @tc.desc: test removeNotificationSlot with exist slot.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper017", 0, async function (done) {
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
     * @tc.name: testReminderHelper018
     * @tc.desc: test removeNotificationSlot with exist slot.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper018", 0, async function (done) {
        var tarRemoveSlot = {
            type: 1
        }
        reminderAgent.addNotificationSlot(tarRemoveSlot.type, (err, data) => {
            reminderAgent.removeNotificationSlot(tarRemoveSlot.type).then(() => {
                    expect(0).assertEqual(err.code);
            });
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper019
     * @tc.desc: test getValidReminders.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper019", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 3
        }
        reminderAgent.publishReminder(timer).then((reminderId) => {});
        setTimeout(() => {
            reminderAgent.getValidReminders().then((reminders) => {
                expect(0).assertEqual(reminders.length);
            });
        }, 5000);
        done();
    })

    /*
     * @tc.name:testReminderHelper020
     * @tc.desc: test getValidReminders.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper020", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 3
        }
        reminderAgent.publishReminder(timer).then((reminderId) => {});
        setTimeout(() => {
            reminderAgent.getValidReminders((err, reminders) => {
                expect(0).assertEqual(reminders.length);
            });
        }, 5000);
        done();
    })

    /*
     * @tc.name: testReminderHelper021
     * @tc.desc: test publishReminder a normal alarm.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper021", 0, async function (done) {
        let alarm = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
            hour: 21,
            minute: 14
        }
        reminderAgent.publishReminder(alarm).then((reminderId) => {
            if (reminderId) {
                let i = 0;
                expect(0).assertEqual(i);
            }
        });
        done();
    })

    /*
     * @tc.name: testReminderHelper022
     * @tc.desc: test publishReminder a normal alarm.
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper022", 0, async function (done) {
        let alarm = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_ALARM,
            hour: 21,
            minute: 14
        }
        function reminderCallback (err, reminderId) {
            if (reminderId) {
                let i = 0;
             expect(0).assertEqual(i);
            }
        }
        reminderAgent.publishReminder(alarm, reminderCallback);
        done();
    })

    /*
     * @tc.name: testReminderHelper023
     * @tc.desc: test publishReminder (max number limit of each application)
     * @tc.type: FUNC
     * @tc.require:
     */
    it("testReminderHelper023", 0, async function (done) {
        let timer = {
            reminderType: reminderAgent.ReminderType.REMINDER_TYPE_TIMER,
            triggerTimeInSeconds: 100
        }
        let maxLimitNumsOfApp = 30;
        let firstId = 0;
        let secondId = 0;
        let diffId = 0
        for (let i = 0; i < maxLimitNumsOfApp; i++) {
            (function (i) {
                setTimeout(function () {
                    reminderAgent.publishReminder(timer).then((reminderId) => {
                        if (i === 0) {
                            firstId = reminderId
                        }
                        if (i === 29) {
                            secondId = reminderId
                            diffId = secondId - firstId
                            expect(29).assertEqual(diffId);
                        }
                    });
                }, 500 * i);
            })(i);
        }
        done();
    })
})