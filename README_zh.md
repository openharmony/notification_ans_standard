# 事件通知子系统

## 简介

OpenHarmony通过ANS（Advanced Notification Service，通知系统服务）对通知类型的消息进行管理，支持多种通知类型，包括文本，长文本，多文本，图片，社交，媒体等。所有系统服务以及应用都可以通过通知接口发送通知消息，用户可以通过SystemUI查看所有通知消息。

通知常见的使用场景：

- 显示接收到短消息、即时消息等。
- 显示应用的推送消息，如广告、版本更新等。
- 显示当前正在进行的事件，如导航、下载等。

### 架构图

![](figures/ans_logical_architecture_diagram.png)

## 目录

```
/base/notification/ans_standard/
├── frameworks   # 接口实现
├── interfaces
│   ├── innerkits    # 对内接口目录
│   └── kits         # 对外接口目录
├── sa_profile   # 组件服务配置
├── services     # 组件服务实现
└── tools        # 组件工具实现

```

## 使用说明

ANS支持4种类型的通道，分别为：
- 社交通讯
- 服务提醒
- 内容资讯
- 其他

应用开发者可以根据使用场景创建通道。通道创建后，提醒方式均为系统默认提醒方式。终端用户可以通过设置应用调整各通道的提醒方式。

应用开发者发可发布以下类型的通知：
- 普通文本
- 长文本
- 图片
- 多行文本
- 社交

并且可调整通知的分发时间、角标数字、分组、触发事件等属性。

## 核心API说明
#### 导入模块

```
import notify from '@ohos.notification';
```

#### 发布通知 （callback形式）

- 接口说明

  发布通知（callback形式）

- publish参数描述


| 名称     | 读写属性 | 类型                | 必填 | 描述                                        |
| -------- | -------- | ------------------- | ---- | ------------------------------------------- |
| request  | 只读     | NotificationRequest | 是   | 设置要发布通知内容的NotificationRequest对象 |
| callback | 只读     | AsyncCallback<void> | 是   | 被指定的回调方法                            |

- NotificationRequest类型说明

| 名称              | 读写属性 | 类型                            | 必填 | 描述                       |
| ----------------- | -------- | ------------------------------- | ---- | -------------------------- |
| content           | 读、写   | NotificationContent             | 是   | 通知内容                   |
| id                | 读、写   | number                          | 否   | 通知ID                     |
| slotType          | 读、写   | SlotType                        | 否   | 通道类型                   |
| isOngoing         | 读、写   | boolean                         | 否   | 是否进行时通知             |
| isUnremovable     | 读、写   | boolean                         | 否   | 是否可移除                 |
| deliveryTime      | 读、写   | number                          | 否   | 通知发送时间               |
| tapDismissed      | 读、写   | boolean                         | 否   | 通知是否自动清除           |
| autoDeletedTime   | 读、写   | number                          | 否   | 自动清除的时间             |
| wantAgent         | 读、写   | WantAgent                       | 否   | 点击跳转的WantAgent        |
| extraInfo         | 读、写   | {[key: string]: any}            | 否   | 扩展参数                   |
| color             | 读、写   | number                          | 否   | 通知背景颜色               |
| colorEnabled      | 读、写   | boolean                         | 否   | 通知背景颜色是否使能       |
| isAlertOnce       | 读、写   | boolean                         | 否   | 设置是否仅有一次此通知警报 |
| isStopwatch       | 读、写   | boolean                         | 否   | 是否显示已用时间           |
| isCountDown       | 读、写   | boolean                         | 否   | 是否显示倒计时时间         |
| isFloatingIcon    | 读、写   | boolean                         | 否   | 是否显示状态栏图标         |
| isFloatingIcon    | 读、写   | boolean                         | 否   | 是否显示状态栏图标         |
| label             | 读、写   | string                          | 否   | 通知标签                   |
| badgeIconStyle    | 读、写   | number                          | 否   | 通知角标类型               |
| showDeliveryTime  | 读、写   | boolean                         | 否   | 是否显示分发时间           |
| actionButtons     | 读、写   | Array<NotificationActionButton> | 否   | 通知按钮，最多两个按钮     |
| smallIcon         | 读、写   | PixelMap                        | 否   | 通知小图标                 |
| largeIcon         | 读、写   | PixelMap                        | 否   | 通知大图标                 |
| creatorBundleName | 只读     | string                          | 否   | 创建通知的包名             |
| creatorUid        | 只读     | number                          | 否   | 创建通知的UID              |
| creatorPid        | 只读     | number                          | 否   | 创建通知的PID              |
| hashCode          | 只读     | string                          | 否   | 通知唯一标识               |
| classification    | 读、写   | string                          | 否   | 通知分类                   |

NotificationContent类型说明

| 名称        | 读写属性 | 类型                         | 必填 | 描述           |
| ----------- | -------- | ---------------------------- | ---- | -------------- |
| contentType | 读、写   | ContentType                  | 是   | 通知内容类型   |
| normal      | 读、写   | NotificationBasicContent     | 否   | 通知基本内容   |
| longText    | 读、写   | NotificationLongTextContent  | 否   | 通知长文本内容 |
| multiLine   | 读、写   | NotificationMultiLineContent | 否   | 通知多行内容   |

- NotificationBasicContent类型说明

| 名称           | 读写属性 | 类型   | 必填 | 描述               |
| -------------- | -------- | ------ | ---- | ------------------ |
| title          | 读、写   | string | 是   | 普通文本通知标题   |
| text           | 读、写   | string | 是   | 普通文本通知内容   |
| additionalText | 读、写   | string | 否   | 普通文本通知附加信 |

- NotificationLongTextContent类型说明

| 名称          | 读写属性 | 类型   | 必填 | 描述                   |
| ------------- | -------- | ------ | ---- | ---------------------- |
| longText      | 读、写   | string | 是   | 长文本通知长文本内容   |
| briefText     | 读、写   | string | 是   | 长文本通知简略文本内容 |
| expandedTitle | 读、写   | string | 是   | 长文本通知扩展标题     |

- NotificationMultiLineContent类型说明

| 名称      | 读写属性 | 类型          | 必填 | 描述                   |
| --------- | -------- | ------------- | ---- | ---------------------- |
| briefText | 读、写   | string        | 是   | 多行文本通知简略内容   |
| longTitle | 读、写   | string        | 是   | 多行文本通知长文本标题 |
| lines     | 读、写   | Array<String> | 是   | 长文本通知扩展标题     |

- NotificationActionButton类型说明

| 名称      | 读写属性 | 类型           | 必填 | 描述                      |
| --------- | -------- | -------------- | ---- | ------------------------- |
| title     | 读、写   | string         | 是   | 按钮标题                  |
| wantAgent | 读、写   | wantAgent      | 是   | 点击按钮时触发的WantAgent |
| extras    | 读、写   | Array<String>  | 否   | 按钮扩展信息              |
| icon      | 读、写   | image.PixelMap | 否   | 按钮图标                  |

- 

  返回值为void

- 示例

```js
//publish回调
function publishCallback(err) {
	console.info("==========================>publishCallback=======================>");
}
//通知Request对象
var notificationRequest = {
    id: 1,
    content: {
        contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
        normal: {
            title: "test_title",
            text: "test_text",
            additionalText: "test_additionalText"
        }
    }
}
Notification.publish(notificationRequest, publishCallback)
```



#### 发布通知（Promise形式）

- 接口说明

  发布通知（Promise形式）

- 

  返回值为Promise<**void**>

- 示例

```js
//通知Request对象
var notificationRequest = {
    notificationId: 1,
    content: {
        contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
        normal: {
            title: "test_title",
            text: "test_text",
            additionalText: "test_additionalText"
        }
    }
}
Notification.publish(notificationRequest).then((void) => {
	console.info("==========================>publishCallback=======================>");
});

```



#### 取消通知 （callback形式）

- 接口说明

  取消与指定id和label相匹配的已发布通知（callback形式）

- cancel参数描述

  | 名称     | 读写属性 | 类型                | 必填 | 描述                 |
  | -------- | -------- | ------------------- | ---- | -------------------- |
  | id       | 只读     | number              | 是   | 通知ID               |
  | lable    | 只读     | string              | 是   | 通知标签             |
  | callback | 只读     | AsyncCallback<void> | 是   | 表示被指定的回调方法 |

- 

  返回值为void

- 示例

```js
//cancel回调
function cancelCallback(err) {
	console.info("==========================>cancelCallback=======================>");
}
Notification.cancel(0, "label", cancelCallback)
```
#### 取消通知 （Promise形式）

- 接口说明

  取消与指定id相匹配的已发布通知,label可以指定也可以不指定（Promise形式)

- cancel参数描述

  | 名称  | 读写属性 | 类型   | 必填 | 描述     |
  | ----- | -------- | ------ | ---- | -------- |
  | id    | 只读     | number | 是   | 通知ID   |
  | lable | 只读     | string | 是   | 通知标签 |

- 

  返回值为Promise<**void**>

- 示例

```js
Notification.cancel(0).then((void) => {
	console.info("==========================>cancelCallback=======================>");
});
```


- 接口说明

  取消与指定id相匹配的已发布通知（callback形式)

- cancel参数描述

  | 名称     | 读写属性 | 类型                | 必填 | 描述                 |
  | -------- | -------- | ------------------- | ---- | -------------------- |
  | id       | 只读     | number              | 是   | 通知ID               |
  | callback | 只读     | AsyncCallback<void> | 是   | 表示被指定的回调方法 |

- 

  返回值为void

- 示例

```js
//cancel回调
function cancelCallback(err) {
	console.info("==========================>cancelCallback=======================>");
}
Notification.cancel(0, cancelCallback)
```


## 相关仓

事件通知子系统

**notification_ans_standard**

[aafwk\_standard](https://gitee.com/openharmony/aafwk_standard)

[appexecfwk\_standard](https://gitee.com/openharmony/appexecfwk_standard)

[notification_ces_standard](https://gitee.com/openharmony/notification_ces_standard)

[distributeddatamgr\_datamgr](https://gitee.com/openharmony/distributeddatamgr_datamgr)