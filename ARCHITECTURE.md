# PoixsDesk 项目架构文档

## 1. 项目概述

PoixsDesk 是一个基于 WebRTC 技术的远程桌面/云电脑系统，支持通过 Web 浏览器远程访问和控制 Windows 桌面。该系统实现了低延迟的屏幕共享、音视频传输和输入控制功能。

### 1.1 核心特性

- **实时屏幕共享**：基于 WebRTC 的低延迟桌面流媒体传输
- **音视频同步**：支持音频和视频的同步传输
- **远程输入控制**：支持鼠标、键盘等输入设备的远程控制
- **WebSocket 信令**：基于 WebSocket 的信令交换机制
- **多平台支持**：Windows 桌面端和 Web 客户端

### 1.2 技术栈

- **WebRTC**：实时通信框架（Google WebRTC）
- **WebSocket**：信令协议（httplib）
- **Windows API**：桌面捕获和输入控制
- **CMake**：跨平台构建系统
- **C++17**：编程语言标准

## 2. 系统架构

### 2.1 整体架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                        PoixsDesk 系统架构                         │
└─────────────────────────────────────────────────────────────────┘

┌──────────────────┐         ┌──────────────────┐
│   Web 客户端      │         │   桌面端程序      │
│                  │         │                  │
│  - 浏览器播放器   │ ◄──────►│  - 屏幕捕获      │
│  - 输入事件发送   │         │  - 音视频编码    │
│  - 信令客户端     │         │  - 输入控制      │
└──────────────────┘         └──────────────────┘
         │                            │
         │                            │
         ▼                            ▼
┌──────────────────────────────────────────────┐
│          信令服务器 (WebSocket)                │
│                                              │
│  - SDP 交换                                  │
│  - ICE 候选交换                              │
│  - 房间管理                                  │
└──────────────────────────────────────────────┘
         │                            │
         │                            │
         ▼                            ▼
┌──────────────────┐         ┌──────────────────┐
│  WebRTC 媒体传输  │         │  WebRTC 媒体传输  │
│  (SRTP/SRTP)     │ ◄──────►│  (SRTP/SRTP)     │
│                  │         │                  │
│  - 视频流        │         │  - 视频流        │
│  - 音频流        │         │  - 音频流        │
│  - 数据通道      │         │  - 数据通道      │
└──────────────────┘         └──────────────────┘
```

### 2.2 模块架构

```
┌─────────────────────────────────────────────────────────────┐
│                    PoixsDesk 模块架构                         │
└─────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────┐
│  PoixsDesk (主程序)                                         │
│  - main.cpp: 程序入口                                       │
│  - 会话监控和生命周期管理                                    │
└───────────────────────────────────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
        ▼                  ▼                  ▼
┌───────────────┐  ┌───────────────┐  ┌───────────────┐
│  libcommon    │  │  libdevice    │  │     Win       │
│  (公共库)      │  │  (设备控制)    │  │   (GUI程序)    │
└───────────────┘  └───────────────┘  └───────────────┘
        │                  │                  │
        │                  │                  │
   ┌────┴────┐        ┌────┴────┐        ┌────┴────┐
   │         │        │         │        │         │
┌──▼──┐  ┌──▼──┐  ┌──▼──┐  ┌──▼──┐  ┌──▼──┐  ┌──▼──┐
│客户端│  │RTC  │  │输入 │  │桌面 │  │窗口 │  │对话框│
│     │  │发布 │  │设备 │  │控制 │  │界面 │  │界面 │
└─────┘  └─────┘  └─────┘  └─────┘  └─────┘  └─────┘
```

## 3. 核心模块详解

### 3.1 libcommon - 公共库模块

#### 3.1.1 crtc_client (RTC客户端)

**职责**：
- 管理 WebSocket 信令连接
- 创建和管理 RTC 发布者
- 处理客户端状态机转换
- 信令消息的发送和接收

**类结构**：
```cpp
class crtc_client : public crtc_publisher::clistener
{
    // 状态管理
    ERtc_Type m_status;
    bool m_stoped;
    
    // RTC发布者
    std::unique_ptr<crtc_publisher> m_rtc_publisher;
    
    // 信令相关
    std::string m_room_name;
    std::string m_user_name;
    
    // 方法
    void init(int gpu_index);
    void Loop(const std::string& rtc_url);
    void destroy();
};
```

**状态机**：
```
ERtc_None → ERtc_WebSocket_Init → ERtc_WebSocket → 
ERtc_WebSocket_Wait → ERtc_WebSocket_Close → ERtc_Destory → ERtc_Exit
```

#### 3.1.2 crtc_publisher (RTC发布者)

**职责**：
- 创建和管理 WebRTC PeerConnection
- 处理 SDP 创建和交换
- 管理音视频轨道
- 处理数据通道（DataChannel）
- ICE 候选收集和交换

**类结构**：
```cpp
class crtc_publisher : public PeerConnectionObserver,
                       public CreateSessionDescriptionObserver
{
    // PeerConnection
    rtc::scoped_refptr<PeerConnectionInterface> peer_connection_;
    
    // 音视频源
    rtc::scoped_refptr<VideoTrackSourceInterface> video_track_source_;
    
    // 数据通道
    std::unique_ptr<cdata_channel> m_data_channel_ptr;
    
    // 回调接口
    clistener* m_callback_ptr;
    
    // 方法
    void create_offer();
    void set_remoter_description(const std::string& sdp);
    void InitializePeerConnection();
};
```

#### 3.1.3 cinput_device (输入设备)

**职责**：
- 接收远程输入事件（鼠标、键盘）
- 解析输入事件消息（二进制和JSON格式）
- 坐标转换（归一化、反量化）
- 事件分发到 Windows 系统

**类结构**：
```cpp
class cinput_device
{
    // 数据通道
    rtc::scoped_refptr<DataChannelInterface> dataChannel;
    
    // 消息队列
    std::list<rtc::CopyOnWriteBuffer> m_messages;
    std::mutex m_messages_lock;
    
    // 工作线程
    std::thread m_work_thread;
    
    // 方法
    void OnMessage(const webrtc::DataBuffer& buffer);
    void OnMouseMove(...);
    void OnKeyDown(...);
    // ... 其他输入事件处理
};
```

**输入事件流程**：
```
远程事件 → DataChannel接收 → 消息队列 → 工作线程处理 → 
坐标转换 → Windows API → 本地输入
```

#### 3.1.4 cdesktop_capture (桌面捕获)

**职责**：
- Windows 桌面屏幕捕获
- 视频帧采集和编码
- 支持 DXGI 和 Windows Graphics Capture (WGC)

### 3.2 libdevice - 设备控制模块

#### 3.2.1 device (输入设备控制)

**职责**：
- Windows 输入事件发送
- 鼠标移动和按键控制
- 键盘按键控制
- 桌面同步处理

**主要函数**：
```cpp
// 鼠标控制
void abs_mouse(FEvent& input, float x, float y);      // 绝对鼠标移动
void move_mouse(FEvent& input, int deltaX, int deltaY); // 相对鼠标移动
void button_mouse(FEvent& input, int32_t posX, int32_t posY, int button, bool release); // 鼠标按键
void scroll(FEvent& input, int distance);             // 垂直滚动
void hscroll(FEvent& input, int distance);            // 水平滚动

// 键盘控制
void keyboard_update(FEvent& input, uint16_t modcode, bool release, uint8_t flags);

// 输入发送
void send_input(INPUT& i);
```

**坐标映射**：
- 屏幕坐标 → Windows虚拟坐标空间（0-65535）
- 公式：`virtual_x = pixel_x * (65535.0 / screen_width)`

### 3.3 PoixsDesk - 主程序

**职责**：
- 程序入口和初始化
- 会话监控（Windows Session）
- 生命周期管理
- 优雅退出处理

**关键功能**：
- Windows Session 监控窗口
- 控制台关闭处理
- 优雅关闭机制

### 3.4 Tools - 工具模块

#### 3.4.1 PoisxDeskService (Windows服务)

**职责**：
- Windows 服务封装
- 会话变更监控（锁屏/解锁）
- 子进程管理（Job Object）
- 日志文件管理

## 4. 数据流架构

### 4.1 视频流

```
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│ 桌面捕获      │─────►│ 视频编码      │─────►│ WebRTC传输   │
│              │      │              │      │              │
│ DXGI/WGC     │      │ H.264/VP9    │      │ RTP/SRTP     │
└──────────────┘      └──────────────┘      └──────────────┘
                                                    │
                                                    ▼
                                            ┌──────────────┐
                                            │  Web 客户端   │
                                            │  视频播放     │
                                            └──────────────┘
```

### 4.2 音频流

```
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│ 音频捕获      │─────►│ 音频编码      │─────►│ WebRTC传输   │
│              │      │              │      │              │
│ Windows Audio│      │ Opus/G.711   │      │ RTP/SRTP     │
└──────────────┘      └──────────────┘      └──────────────┘
```

### 4.3 输入事件流

```
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│ Web 客户端    │─────►│ 信令/数据通道  │─────►│ 输入设备处理  │
│              │      │              │      │              │
│ 鼠标/键盘事件 │      │ JSON/Binary  │      │ 坐标转换      │
└──────────────┘      └──────────────┘      └──────────────┘
                                                    │
                                                    ▼
                                            ┌──────────────┐
                                            │ Windows API  │
                                            │ SendInput    │
                                            └──────────────┘
```

### 4.4 信令流

```
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│  桌面端       │      │ 信令服务器    │      │  Web 客户端   │
│              │      │              │      │              │
│ Offer SDP    │─────►│              │◄─────│ 信令连接      │
│              │      │ 信令转发      │      │              │
│ ICE Candidate│─────►│              │◄─────│ Answer SDP   │
│              │      │              │      │              │
│ Answer SDP   │◄─────│              │─────►│ ICE Candidate│
└──────────────┘      └──────────────┘      └──────────────┘
```

## 5. 信令协议

### 5.1 WebSocket 信令消息格式

#### 5.1.1 Offer SDP 消息

```json
{
    "type": "offer",
    "room_name": "room123",
    "user_name": "user1",
    "sdp": "v=0\r\no=- ..."
}
```

#### 5.1.2 Answer SDP 消息

```json
{
    "type": "answer",
    "room_name": "room123",
    "user_name": "user2",
    "sdp": "v=0\r\no=- ..."
}
```

#### 5.1.3 ICE Candidate 消息

```json
{
    "type": "ice-candidate",
    "room_name": "room123",
    "user_name": "user1",
    "candidate": {
        "candidate": "candidate:...",
        "sdpMLineIndex": 0,
        "sdpMid": "0"
    }
}
```

### 5.2 数据通道消息格式

#### 5.2.1 二进制格式

```
┌──────────┬──────────┬──────────┬──────────┐
│ 消息类型  │ 按键标志  │ 坐标X    │ 坐标Y    │
│ (1 byte) │ (1 byte) │ (2 bytes)│ (2 bytes)│
└──────────┴──────────┴──────────┴──────────┘
```

#### 5.2.2 JSON 格式

```json
{
    "type": "mouse_move",
    "x": 100,
    "y": 200,
    "deltaX": 10,
    "deltaY": 20
}
```

## 6. 技术细节

### 6.1 WebRTC 配置

- **视频编码器**：H.264, VP9
- **音频编码器**：Opus, G.711
- **传输协议**：RTP/SRTP over UDP
- **数据通道**：SCTP over DTLS

### 6.2 坐标系统

- **远程坐标**：归一化坐标（0.0 - 1.0）或量化坐标（0 - 65535）
- **本地坐标**：像素坐标（0 - screen_width/height）
- **转换公式**：`pixel = normalized * screen_dimension`

### 6.3 桌面同步

- **问题**：Windows 输入桌面（Input Desktop）与默认桌面隔离
- **解决方案**：使用 `syncThreadDesktop()` 同步线程到输入桌面
- **重试机制**：输入发送失败时自动同步桌面并重试

### 6.4 扫描码转换

- **规范化键码**：使用 `VK_TO_SCANCODE_MAP` 直接查找扫描码
- **非规范化键码**：使用 `MapVirtualKey()` 动态转换
- **特殊键处理**：VK_LWIN, VK_RWIN, VK_PAUSE 需要特殊处理

## 7. 项目目录结构

```
PoixsDesk/
├── CMakeLists.txt          # 主构建文件
├── README.md               # 项目说明
├── ARCHITECTURE.md         # 架构文档（本文档）
├── libcommon/              # 公共库
│   ├── client.h/cpp        # RTC客户端
│   ├── crtc_publisher.h/cpp # RTC发布者
│   ├── cinput_device.h/cpp # 输入设备
│   ├── cdesktop_capture.h/cpp # 桌面捕获
│   ├── cdata_channel.h/cpp # 数据通道
│   └── ...
├── libdevice/              # 设备控制库
│   └── window/
│       ├── device.h/cpp    # 输入设备控制
│       └── misc.h/cpp      # 辅助函数
├── PoixsDesk/              # 主程序
│   └── main.cpp            # 程序入口
├── Win/                    # Windows GUI程序
│   ├── LiveWin32.cpp       # 主窗口
│   └── Dlg*.cpp            # 对话框
├── Tools/                  # 工具程序
│   ├── PoisxDeskService.cpp # Windows服务
│   └── audio.cpp           # 音频处理
├── www/                    # Web前端
│   ├── index.html          # 主页
│   ├── player.html         # 播放器页面
│   └── scripts/            # JavaScript脚本
└── build/                  # 构建目录
```

## 8. 构建说明

### 8.1 依赖项

- **WebRTC**：Google WebRTC 源码（需要自行编译）
- **httplib**：HTTP/WebSocket 库（已包含）
- **json**：JSON 解析库（nlohmann/json，已包含）
- **CMake**：3.8 或更高版本
- **Visual Studio**：2017 或更高版本（Windows）

### 8.2 编译步骤

```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 配置CMake（需要指定WebRTC路径）
cmake .. -DWebRTC_ROOT=/path/to/webrtc

# 3. 编译
cmake --build . --config Release

# 4. 运行
./PoixsDesk/PoixsDesk.exe ws://localhost:8080/rtc
```

## 9. 扩展点

### 9.1 自定义视频编码器

- 修改 `cdesktop_capture` 模块
- 实现自定义 `VideoEncoderInterface`

### 9.2 自定义信令协议

- 实现新的信令客户端
- 替换 `crtc_client` 中的 WebSocket 实现

### 9.3 多显示器支持

- 修改桌面捕获逻辑
- 支持屏幕选择和切换

### 9.4 音频处理增强

- 实现音频效果处理（降噪、回声消除）
- 支持多音频源混合

## 10. 性能优化建议

### 10.1 视频优化

- **编码参数调整**：根据网络情况动态调整码率和分辨率
- **帧率控制**：限制最大帧率，减少CPU占用
- **硬件加速**：使用GPU进行视频编码（NVENC, Quick Sync）

### 10.2 网络优化

- **自适应码率**：根据网络带宽自动调整码率
- **丢包重传**：使用RTX（Retransmission）机制
- **网络拥塞控制**：使用TWCC（Transport-wide Congestion Control）

### 10.3 延迟优化

- **低延迟模式**：减少编码缓冲和网络缓冲
- **预测性编码**：使用低延迟编码模式
- **快速ICE**：优化ICE候选收集时间

## 11. 故障排查

### 11.1 常见问题

**问题1：输入事件无效**
- 检查桌面同步是否成功
- 确认进程有管理员权限
- 检查输入桌面句柄是否有效

**问题2：视频不显示**
- 检查SDP交换是否成功
- 确认ICE连接是否建立
- 检查防火墙设置

**问题3：高延迟**
- 检查网络带宽
- 调整编码参数
- 检查系统资源占用

### 11.2 调试工具

- **WebRTC日志**：启用详细日志输出
- **网络抓包**：使用Wireshark抓包分析
- **性能分析**：使用Visual Studio性能分析器

## 12. 安全考虑

### 12.1 认证和授权

- 实现用户认证机制
- 添加访问控制列表（ACL）
- 使用TLS加密WebSocket连接

### 12.2 数据加密

- **SRTP**：媒体流加密（已内置）
- **DTLS**：数据通道加密（已内置）
- **TLS**：信令通道加密（建议实现）

### 12.3 输入验证

- 验证输入事件坐标范围
- 限制输入事件频率
- 实现黑名单机制

## 13. 未来规划

### 13.1 功能增强

- [ ] 支持多用户同时连接
- [ ] 实现文件传输功能
- [ ] 支持剪贴板同步
- [ ] 实现远程打印

### 13.2 平台扩展

- [ ] Linux桌面端支持
- [ ] macOS桌面端支持
- [ ] 移动端客户端（iOS/Android）

### 13.3 性能优化

- [ ] GPU加速编码
- [ ] 自适应码率算法优化
- [ ] 网络传输优化

---

**文档版本**：1.0  
**最后更新**：2025-01-XX  
**作者**：chensong

