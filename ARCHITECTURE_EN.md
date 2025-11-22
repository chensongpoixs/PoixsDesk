# PoixsDesk Project Architecture Documentation

## 1. Project Overview

PoixsDesk is a remote desktop/cloud computer system based on WebRTC technology, supporting remote access and control of Windows desktops through web browsers. The system implements low-latency screen sharing, audio/video transmission, and input control functionality.

### 1.1 Core Features

- **Real-time Screen Sharing**: Low-latency desktop streaming based on WebRTC
- **Audio/Video Synchronization**: Supports synchronized audio and video transmission
- **Remote Input Control**: Supports remote control of mouse, keyboard, and other input devices
- **WebSocket Signaling**: Signaling exchange mechanism based on WebSocket
- **Multi-platform Support**: Windows desktop client and web client

### 1.2 Technology Stack

- **WebRTC**: Real-time communication framework (Google WebRTC)
- **WebSocket**: Signaling protocol (httplib)
- **Windows API**: Desktop capture and input control
- **CMake**: Cross-platform build system
- **C++17**: Programming language standard

## 2. System Architecture

### 2.1 Overall Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                   PoixsDesk System Architecture                   │
└─────────────────────────────────────────────────────────────────┘

┌──────────────────┐         ┌──────────────────┐
│   Web Client     │         │   Desktop App    │
│                  │         │                  │
│  - Browser Player│ ◄──────►│  - Screen Capture│
│  - Input Events  │         │  - A/V Encoding  │
│  - Signaling     │         │  - Input Control │
└──────────────────┘         └──────────────────┘
         │                            │
         │                            │
         ▼                            ▼
┌──────────────────────────────────────────────┐
│       Signaling Server (WebSocket)            │
│                                              │
│  - SDP Exchange                              │
│  - ICE Candidate Exchange                    │
│  - Room Management                           │
└──────────────────────────────────────────────┘
         │                            │
         │                            │
         ▼                            ▼
┌──────────────────┐         ┌──────────────────┐
│  WebRTC Media    │         │  WebRTC Media    │
│  Transport       │ ◄──────►│  Transport       │
│  (SRTP/SRTP)     │         │  (SRTP/SRTP)     │
│                  │         │                  │
│  - Video Stream  │         │  - Video Stream  │
│  - Audio Stream  │         │  - Audio Stream  │
│  - Data Channel  │         │  - Data Channel  │
└──────────────────┘         └──────────────────┘
```

### 2.2 Module Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                  PoixsDesk Module Architecture                │
└─────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────┐
│  PoixsDesk (Main Program)                                  │
│  - main.cpp: Program entry point                           │
│  - Session monitoring and lifecycle management             │
└───────────────────────────────────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
        ▼                  ▼                  ▼
┌───────────────┐  ┌───────────────┐  ┌───────────────┐
│  libcommon    │  │  libdevice    │  │     Win       │
│  (Common Lib) │  │  (Device Ctrl)│  │   (GUI App)   │
└───────────────┘  └───────────────┘  └───────────────┘
        │                  │                  │
        │                  │                  │
   ┌────┴────┐        ┌────┴────┐        ┌────┴────┐
   │         │        │         │        │         │
┌──▼──┐  ┌──▼──┐  ┌──▼──┐  ┌──▼──┐  ┌──▼──┐  ┌──▼──┐
│Client│  │RTC  │  │Input│  │Desk │  │Window│  │Dialog│
│      │  │Pub  │  │Dev  │  │Ctrl │  │ UI  │  │ UI  │
└─────┘  └─────┘  └─────┘  └─────┘  └─────┘  └─────┘
```

## 3. Core Modules

### 3.1 libcommon - Common Library Module

#### 3.1.1 crtc_client (RTC Client)

**Responsibilities**:
- Manage WebSocket signaling connections
- Create and manage RTC publisher
- Handle client state machine transitions
- Send and receive signaling messages

**Class Structure**:
```cpp
class crtc_client : public crtc_publisher::clistener
{
    // State management
    ERtc_Type m_status;
    bool m_stoped;
    
    // RTC publisher
    std::unique_ptr<crtc_publisher> m_rtc_publisher;
    
    // Signaling related
    std::string m_room_name;
    std::string m_user_name;
    
    // Methods
    void init(int gpu_index);
    void Loop(const std::string& rtc_url);
    void destroy();
};
```

**State Machine**:
```
ERtc_None → ERtc_WebSocket_Init → ERtc_WebSocket → 
ERtc_WebSocket_Wait → ERtc_WebSocket_Close → ERtc_Destory → ERtc_Exit
```

#### 3.1.2 crtc_publisher (RTC Publisher)

**Responsibilities**:
- Create and manage WebRTC PeerConnection
- Handle SDP creation and exchange
- Manage audio/video tracks
- Handle data channels (DataChannel)
- ICE candidate collection and exchange

**Class Structure**:
```cpp
class crtc_publisher : public PeerConnectionObserver,
                       public CreateSessionDescriptionObserver
{
    // PeerConnection
    rtc::scoped_refptr<PeerConnectionInterface> peer_connection_;
    
    // Audio/Video source
    rtc::scoped_refptr<VideoTrackSourceInterface> video_track_source_;
    
    // Data channel
    std::unique_ptr<cdata_channel> m_data_channel_ptr;
    
    // Callback interface
    clistener* m_callback_ptr;
    
    // Methods
    void create_offer();
    void set_remoter_description(const std::string& sdp);
    void InitializePeerConnection();
};
```

#### 3.1.3 cinput_device (Input Device)

**Responsibilities**:
- Receive remote input events (mouse, keyboard)
- Parse input event messages (binary and JSON formats)
- Coordinate transformation (normalization, dequantization)
- Dispatch events to Windows system

**Class Structure**:
```cpp
class cinput_device
{
    // Data channel
    rtc::scoped_refptr<DataChannelInterface> dataChannel;
    
    // Message queue
    std::list<rtc::CopyOnWriteBuffer> m_messages;
    std::mutex m_messages_lock;
    
    // Worker thread
    std::thread m_work_thread;
    
    // Methods
    void OnMessage(const webrtc::DataBuffer& buffer);
    void OnMouseMove(...);
    void OnKeyDown(...);
    // ... other input event handlers
};
```

**Input Event Flow**:
```
Remote Event → DataChannel Receive → Message Queue → Worker Thread Process → 
Coordinate Transform → Windows API → Local Input
```

#### 3.1.4 cdesktop_capture (Desktop Capture)

**Responsibilities**:
- Windows desktop screen capture
- Video frame acquisition and encoding
- Support for DXGI and Windows Graphics Capture (WGC)

### 3.2 libdevice - Device Control Module

#### 3.2.1 device (Input Device Control)

**Responsibilities**:
- Send Windows input events
- Mouse movement and button control
- Keyboard button control
- Desktop synchronization handling

**Main Functions**:
```cpp
// Mouse control
void abs_mouse(FEvent& input, float x, float y);      // Absolute mouse move
void move_mouse(FEvent& input, int deltaX, int deltaY); // Relative mouse move
void button_mouse(FEvent& input, int32_t posX, int32_t posY, int button, bool release); // Mouse button
void scroll(FEvent& input, int distance);             // Vertical scroll
void hscroll(FEvent& input, int distance);            // Horizontal scroll

// Keyboard control
void keyboard_update(FEvent& input, uint16_t modcode, bool release, uint8_t flags);

// Input sending
void send_input(INPUT& i);
```

**Coordinate Mapping**:
- Screen coordinates → Windows virtual coordinate space (0-65535)
- Formula: `virtual_x = pixel_x * (65535.0 / screen_width)`

### 3.3 PoixsDesk - Main Program

**Responsibilities**:
- Program entry and initialization
- Session monitoring (Windows Session)
- Lifecycle management
- Graceful shutdown handling

**Key Features**:
- Windows Session monitoring window
- Console close handling
- Graceful shutdown mechanism

### 3.4 Tools - Tools Module

#### 3.4.1 PoisxDeskService (Windows Service)

**Responsibilities**:
- Windows service wrapper
- Session change monitoring (lock/unlock)
- Child process management (Job Object)
- Log file management

## 4. Data Flow Architecture

### 4.1 Video Stream

```
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│Desktop Capture│─────►│Video Encoding│─────►│WebRTC Transport│
│              │      │              │      │              │
│ DXGI/WGC     │      │ H.264/VP9    │      │ RTP/SRTP     │
└──────────────┘      └──────────────┘      └──────────────┘
                                                    │
                                                    ▼
                                            ┌──────────────┐
                                            │ Web Client   │
                                            │ Video Player │
                                            └──────────────┘
```

### 4.2 Audio Stream

```
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│Audio Capture │─────►│Audio Encoding│─────►│WebRTC Transport│
│              │      │              │      │              │
│ Windows Audio│      │ Opus/G.711   │      │ RTP/SRTP     │
└──────────────┘      └──────────────┘      └──────────────┘
```

### 4.3 Input Event Stream

```
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│ Web Client   │─────►│Signaling/Data│─────►│Input Device │
│              │      │    Channel   │      │   Handler    │
│Mouse/Keyboard│      │ JSON/Binary  │      │Coord Transform│
└──────────────┘      └──────────────┘      └──────────────┘
                                                    │
                                                    ▼
                                            ┌──────────────┐
                                            │ Windows API  │
                                            │ SendInput    │
                                            └──────────────┘
```

### 4.4 Signaling Flow

```
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│  Desktop     │      │Signaling Server│      │ Web Client  │
│              │      │              │      │              │
│ Offer SDP    │─────►│              │◄─────│Signaling Conn│
│              │      │  Signaling   │      │              │
│ICE Candidate │─────►│   Forward    │◄─────│ Answer SDP   │
│              │      │              │      │              │
│ Answer SDP   │◄─────│              │─────►│ICE Candidate │
└──────────────┘      └──────────────┘      └──────────────┘
```

## 5. Signaling Protocol

### 5.1 WebSocket Signaling Message Format

#### 5.1.1 Offer SDP Message

```json
{
    "type": "offer",
    "room_name": "room123",
    "user_name": "user1",
    "sdp": "v=0\r\no=- ..."
}
```

#### 5.1.2 Answer SDP Message

```json
{
    "type": "answer",
    "room_name": "room123",
    "user_name": "user2",
    "sdp": "v=0\r\no=- ..."
}
```

#### 5.1.3 ICE Candidate Message

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

### 5.2 Data Channel Message Format

#### 5.2.1 Binary Format

```
┌──────────┬──────────┬──────────┬──────────┐
│Msg Type  │Key Flag  │Coord X   │Coord Y   │
│(1 byte)  │(1 byte)  │(2 bytes) │(2 bytes) │
└──────────┴──────────┴──────────┴──────────┘
```

#### 5.2.2 JSON Format

```json
{
    "type": "mouse_move",
    "x": 100,
    "y": 200,
    "deltaX": 10,
    "deltaY": 20
}
```

## 6. Technical Details

### 6.1 WebRTC Configuration

- **Video Codec**: H.264, VP9
- **Audio Codec**: Opus, G.711
- **Transport Protocol**: RTP/SRTP over UDP
- **Data Channel**: SCTP over DTLS

### 6.2 Coordinate System

- **Remote Coordinates**: Normalized coordinates (0.0 - 1.0) or quantized coordinates (0 - 65535)
- **Local Coordinates**: Pixel coordinates (0 - screen_width/height)
- **Conversion Formula**: `pixel = normalized * screen_dimension`

### 6.3 Desktop Synchronization

- **Problem**: Windows Input Desktop is isolated from the default desktop
- **Solution**: Use `syncThreadDesktop()` to synchronize thread to input desktop
- **Retry Mechanism**: Automatically sync desktop and retry when input send fails

### 6.4 Scan Code Conversion

- **Normalized Key Code**: Use `VK_TO_SCANCODE_MAP` to directly lookup scan code
- **Non-normalized Key Code**: Use `MapVirtualKey()` for dynamic conversion
- **Special Key Handling**: VK_LWIN, VK_RWIN, VK_PAUSE require special handling

## 7. Project Directory Structure

```
PoixsDesk/
├── CMakeLists.txt          # Main build file
├── README.md               # Project readme
├── ARCHITECTURE_EN.md      # Architecture documentation (this document)
├── libcommon/              # Common library
│   ├── client.h/cpp        # RTC client
│   ├── crtc_publisher.h/cpp # RTC publisher
│   ├── cinput_device.h/cpp # Input device
│   ├── cdesktop_capture.h/cpp # Desktop capture
│   ├── cdata_channel.h/cpp # Data channel
│   └── ...
├── libdevice/              # Device control library
│   └── window/
│       ├── device.h/cpp    # Input device control
│       └── misc.h/cpp      # Helper functions
├── PoixsDesk/              # Main program
│   └── main.cpp            # Program entry point
├── Win/                    # Windows GUI program
│   ├── LiveWin32.cpp       # Main window
│   └── Dlg*.cpp            # Dialogs
├── Tools/                  # Tool programs
│   ├── PoisxDeskService.cpp # Windows service
│   └── audio.cpp           # Audio processing
├── www/                    # Web frontend
│   ├── index.html          # Home page
│   ├── player.html         # Player page
│   └── scripts/            # JavaScript scripts
└── build/                  # Build directory
```

## 8. Build Instructions

### 8.1 Dependencies

- **WebRTC**: Google WebRTC source code (requires self-compilation)
- **httplib**: HTTP/WebSocket library (included)
- **json**: JSON parsing library (nlohmann/json, included)
- **CMake**: Version 3.8 or higher
- **Visual Studio**: 2017 or higher (Windows)

### 8.2 Build Steps

```bash
# 1. Create build directory
mkdir build
cd build

# 2. Configure CMake (need to specify WebRTC path)
cmake .. -DWebRTC_ROOT=/path/to/webrtc

# 3. Build
cmake --build . --config Release

# 4. Run
./PoixsDesk/PoixsDesk.exe ws://localhost:8080/rtc
```

## 9. Extension Points

### 9.1 Custom Video Encoder

- Modify `cdesktop_capture` module
- Implement custom `VideoEncoderInterface`

### 9.2 Custom Signaling Protocol

- Implement new signaling client
- Replace WebSocket implementation in `crtc_client`

### 9.3 Multi-Monitor Support

- Modify desktop capture logic
- Support screen selection and switching

### 9.4 Audio Processing Enhancement

- Implement audio effects processing (noise reduction, echo cancellation)
- Support multi-audio source mixing

## 10. Performance Optimization Recommendations

### 10.1 Video Optimization

- **Encoder Parameter Adjustment**: Dynamically adjust bitrate and resolution based on network conditions
- **Frame Rate Control**: Limit maximum frame rate to reduce CPU usage
- **Hardware Acceleration**: Use GPU for video encoding (NVENC, Quick Sync)

### 10.2 Network Optimization

- **Adaptive Bitrate**: Automatically adjust bitrate based on network bandwidth
- **Packet Retransmission**: Use RTX (Retransmission) mechanism
- **Network Congestion Control**: Use TWCC (Transport-wide Congestion Control)

### 10.3 Latency Optimization

- **Low Latency Mode**: Reduce encoding buffer and network buffer
- **Predictive Encoding**: Use low latency encoding mode
- **Fast ICE**: Optimize ICE candidate collection time

## 11. Troubleshooting

### 11.1 Common Issues

**Issue 1: Input Events Invalid**
- Check if desktop synchronization is successful
- Confirm process has administrator privileges
- Check if input desktop handle is valid

**Issue 2: Video Not Displaying**
- Check if SDP exchange is successful
- Confirm if ICE connection is established
- Check firewall settings

**Issue 3: High Latency**
- Check network bandwidth
- Adjust encoding parameters
- Check system resource usage

### 11.2 Debug Tools

- **WebRTC Logs**: Enable detailed log output
- **Network Capture**: Use Wireshark for packet analysis
- **Performance Profiling**: Use Visual Studio Performance Profiler

## 12. Security Considerations

### 12.1 Authentication and Authorization

- Implement user authentication mechanism
- Add access control list (ACL)
- Use TLS to encrypt WebSocket connections

### 12.2 Data Encryption

- **SRTP**: Media stream encryption (built-in)
- **DTLS**: Data channel encryption (built-in)
- **TLS**: Signaling channel encryption (recommended)

### 12.3 Input Validation

- Validate input event coordinate ranges
- Limit input event frequency
- Implement blacklist mechanism

## 13. Future Roadmap

### 13.1 Feature Enhancements

- [ ] Support multiple users simultaneously
- [ ] Implement file transfer functionality
- [ ] Support clipboard synchronization
- [ ] Implement remote printing

### 13.2 Platform Extensions

- [ ] Linux desktop client support
- [ ] macOS desktop client support
- [ ] Mobile client (iOS/Android)

### 13.3 Performance Optimization

- [ ] GPU accelerated encoding
- [ ] Adaptive bitrate algorithm optimization
- [ ] Network transmission optimization

---

**Document Version**: 1.0  
**Last Updated**: 2025-01-XX  
**Author**: chensong

