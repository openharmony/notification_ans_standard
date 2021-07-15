# 事件通知子系统

## 简介

OpenHarmony通过ANS（Advanced Notification Service，通知系统服务）对通知类型的消息进行管理，支持多种通知类型，包括文本，长文本，多文本，图片，社交，媒体等。所有系统服务以及应用都可以通过通知接口发送通知消息，用户可以通过SystemUI系统APP查看所有通知消息。  为应用程序提供订阅、发布、取消订阅的能力。

通知常见的使用场景：

- 显示接收到短消息、即时消息等。
- 显示应用的推送消息，如广告、版本更新等。
- 显示当前正在进行的事件，如播放音乐、导航、下载等。

### 架构图

![](figures\ans_logical_architecture_diagram.png "通知服务架构图")

## 目录

```
/base/notification/ans_standard/
├── innerkits    # 对内接口目录
├── kits         # 对外接口目录
├── sa_profile   # 组件服务配置
├── services     # 组件服务实现
├── tools        # 组件工具实现
│── ohos.build   # 组件编译脚本

```

## 使用说明

以下模块的JS接口为非正式API，仅供SystemUI等系统应用使用，不排除对这些接口进行变更的可能性，后续版本将提供正式API。

- notificationSlot.d.ts
- notificationRequest.d.ts
- notificationContent.d.ts
- notificationActionButton.ts
- triggerInfo.d.ts
- wantAgentInfo.d.ts
- @ohos.notification.d.ts
- @ohos.wantAgent.d.ts

## 相关仓

事件通知子系统

**notification_ans_standard**

