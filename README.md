# PoixsDesk 项目说明文档

## 📖 项目简介

PoixsDesk 是一个基于 WebRTC 技术的远程桌面/云电脑系统，支持通过 Web 浏览器远程访问和控制 Windows 桌面。该系统实现了低延迟的屏幕共享、音视频传输和双向输入控制功能。
 

![云电脑架构图](img/CloudComputerArchitecture.png)
 

## ✨ 核心功能

### 1. 实时屏幕共享
- 基于 WebRTC 的低延迟桌面流媒体传输
- 支持全屏和窗口捕获
- 自动帧率调整和码率控制

### 2. 音视频同步传输
- 高质量音频传输（Opus 编码）
- H.264/VP9 视频编码
- 音视频同步处理

### 3. 远程输入控制
- 鼠标移动和点击控制
- 键盘输入支持
- 鼠标滚轮支持
- 多按键组合支持

### 4. 双向数据传输
- WebRTC 数据通道（DataChannel）
- 可靠和不可靠传输模式
- 支持二进制和JSON格式消息

### 5. WebSocket 信令
- 基于 WebSocket 的信令交换
- SDP 交换和ICE候选收集
- 房间管理和用户认证

## 🏗️ 系统架构

### 整体架构

```
┌──────────────┐         ┌──────────────┐
│  Web 客户端   │◄───────►│  桌面端程序   │
│              │         │              │
│ - 浏览器播放  │         │ - 屏幕捕获    │
│ - 输入控制    │         │ - 音视频编码  │
└──────┬───────┘         └──────┬───────┘
       │                        │
       │   WebSocket 信令        │
       │◄──────────────────────►│
       │                        │
       │   WebRTC 媒体传输       │
       │◄──────────────────────►│
       │                        │
```

### 模块组成

- **libcommon**：公共库，包含 RTC 客户端、发布者、输入设备等核心功能
- **libdevice**：Windows 设备控制库，负责输入事件发送
- **PoixsDesk**：主程序，程序入口和生命周期管理
- **Win**：Windows GUI 程序，提供图形界面
- **Tools**：工具程序，包括 Windows 服务等
- **www**：Web 前端，浏览器客户端界面

## 📦 项目结构

```
PoixsDesk/
├── libcommon/              # 公共库模块
│   ├── client.h/cpp        # RTC客户端
│   ├── crtc_publisher.h/cpp # RTC发布者
│   ├── cinput_device.h/cpp # 输入设备管理
│   ├── cdesktop_capture.h/cpp # 桌面捕获
│   ├── cdata_channel.h/cpp # 数据通道
│   └── ...
├── libdevice/              # 设备控制模块
│   └── window/
│       ├── device.h/cpp    # 输入设备控制
│       └── misc.h/cpp      # 辅助函数
├── PoixsDesk/              # 主程序
│   └── main.cpp            # 程序入口
├── Win/                    # Windows GUI程序
├── Tools/                  # 工具程序
├── www/                    # Web前端
└── build/                  # 构建目录
```

详细的架构说明请参考 [ARCHITECTURE.md](ARCHITECTURE.md)

英文文档请参考 [README_EN.md](README_EN.md)

## 🚀 快速开始

### 环境要求

- **操作系统**：Windows 10/11
- **开发环境**：Visual Studio 2017 或更高版本
- **CMake**：3.8 或更高版本
- **WebRTC**：需要自行编译 Google WebRTC 源码

### 编译步骤

1. **克隆项目**
   ```bash
   git clone <repository_url>
   cd PoixsDesk
   ```

2. **准备 WebRTC 依赖**
   - 下载并编译 Google WebRTC 源码
   - 在 CMakeLists.txt 中配置 WebRTC 路径

3. **配置 CMake**
   ```bash
   mkdir build
   cd build
   cmake .. -DWebRTC_ROOT=D:/Work/webrtc_google/src
   ```

4. **编译项目**
   ```bash
   cmake --build . --config Release
   ```

5. **运行程序**
   ```bash
   ./PoixsDesk/PoixsDesk.exe ws://localhost:8080/rtc
   ```

### 运行参数

- `ws://localhost:8080/rtc`：WebSocket 信令服务器地址

## 📖 使用说明

### 桌面端配置

1. **启动桌面端程序**
   ```bash
   PoixsDesk.exe ws://your-signaling-server/rtc
   ```

2. **程序会自动**
   - 连接到信令服务器
   - 创建 WebRTC 连接
   - 开始捕获桌面和音频
   - 等待客户端连接

### Web 客户端使用

1. **打开浏览器**
   - 访问 `http://your-server/player.html`

2. **连接到桌面**
   - 输入房间名称和用户名
   - 点击连接按钮

3. **远程控制**
   - 使用鼠标点击和移动
   - 使用键盘输入
   - 使用滚轮滚动

### Windows 服务模式

可以使用 Windows 服务模式运行，以便开机自启：

```bash
# 安装服务
PoisxDeskService.exe install

# 启动服务
PoisxDeskService.exe start

# 停止服务
PoisxDeskService.exe stop
```

## 🔧 配置说明

### 视频编码配置

在 `crtc_publisher` 中可以配置：
- **编码器**：H.264 或 VP9
- **分辨率**：支持多种分辨率
- **帧率**：可调整帧率上限
- **码率**：可设置目标码率

### 音频编码配置

- **编码器**：Opus（默认）或 G.711
- **采样率**：48kHz（默认）
- **声道**：立体声或单声道

### 网络配置

- **ICE服务器**：可在 SDP 中配置 STUN/TURN 服务器
- **传输模式**：UDP（默认）或 TCP
- **数据通道**：可靠或不可靠传输

## 📝 API 文档

### RTC 客户端 API

#### 初始化客户端

```cpp
chen::crtc_client client;
client.init(0);  // gpu_index: GPU索引
client.Loop("ws://localhost:8080/rtc");  // 连接信令服务器
```

#### 客户端状态

```cpp
enum ERtc_Type {
    ERtc_None,              // 未初始化
    ERtc_WebSocket_Init,    // WebSocket初始化
    ERtc_WebSocket,         // WebSocket已连接
    ERtc_WebSocket_Wait,    // 等待响应
    ERtc_Destory,           // 销毁中
    ERtc_Exit               // 退出
};
```

### 输入设备 API

#### 鼠标控制

```cpp
// 绝对鼠标移动
chen::abs_mouse(event, x, y);

// 相对鼠标移动
chen::move_mouse(event, deltaX, deltaY);

// 鼠标按键
chen::button_mouse(event, x, y, button, release);
// button: 0=左键, 1=中键, 2=右键, 3+=扩展键
```

#### 键盘控制

```cpp
// 按键按下/释放
chen::keyboard_update(event, vk_code, release, flags);
// vk_code: 虚拟键码
// release: true=释放, false=按下
```

## 🔍 故障排查

### 常见问题

#### 1. 输入事件无效

**问题**：鼠标或键盘输入无法控制远程桌面

**解决方案**：
- 检查程序是否以管理员权限运行
- 确认桌面同步是否成功
- 检查输入桌面句柄是否有效

#### 2. 视频不显示

**问题**：Web 客户端无法看到桌面画面

**解决方案**：
- 检查 SDP 交换是否成功
- 确认 ICE 连接是否建立
- 检查防火墙设置
- 查看浏览器控制台错误信息

#### 3. 高延迟

**问题**：远程操作有明显延迟

**解决方案**：
- 检查网络带宽是否充足
- 降低视频分辨率和帧率
- 使用更低的编码码率
- 检查系统资源占用

#### 4. 音频问题

**问题**：没有声音或声音卡顿

**解决方案**：
- 检查音频设备是否正常
- 确认音频编码器配置
- 检查网络丢包情况

### 调试工具

- **日志输出**：程序会输出详细的日志信息
- **WebRTC日志**：可以启用 WebRTC 内部日志
- **网络抓包**：使用 Wireshark 分析网络流量

## 🔒 安全建议

### 认证和授权

- 实现用户认证机制
- 添加访问控制列表（ACL）
- 使用 TLS 加密 WebSocket 连接

### 数据加密

- **SRTP**：媒体流加密（已内置）
- **DTLS**：数据通道加密（已内置）
- **TLS**：信令通道加密（建议实现）

### 输入验证

- 验证输入事件坐标范围
- 限制输入事件频率
- 实现黑名单机制

## 🤝 贡献指南

### 代码规范

- 使用 C++17 标准
- 遵循项目现有的代码风格
- 添加详细的注释（Doxygen 格式）
- 确保代码通过编译和测试

### 提交流程

1. Fork 项目
2. 创建功能分支
3. 提交更改
4. 创建 Pull Request

## 📄 许可证

请查看 [LICENSE](LICENSE) 文件了解详细许可证信息。

## 👥 作者

- **chensong** - 项目创建者和维护者

## 🙏 致谢

- Google WebRTC 团队
- 所有贡献者和用户

## 📚 相关文档

- [架构文档](ARCHITECTURE.md) - 详细的系统架构说明（中文）
- [架构文档（英文）](ARCHITECTURE_EN.md) - 详细的系统架构说明（英文）
- [README（英文）](README_EN.md) - 英文项目说明文档
- [API 文档](docs/API.md) - API 参考文档
- [开发指南](docs/DEVELOPMENT.md) - 开发指南

## 🐛 问题反馈

如果遇到问题或有建议，请通过以下方式反馈：

- 提交 Issue
- 发送邮件
- 创建 Pull Request

## 🔮 未来计划

### 功能增强
- [ ] 支持多用户同时连接
- [ ] 实现文件传输功能
- [ ] 支持剪贴板同步
- [ ] 实现远程打印

### 平台扩展
- [ ] Linux 桌面端支持
- [ ] macOS 桌面端支持
- [ ] 移动端客户端（iOS/Android）

### 性能优化
- [ ] GPU 加速编码
- [ ] 自适应码率算法优化
- [ ] 网络传输优化

---

**最后更新**：2025-01-XX  
**版本**：1.0
