/***********************************************************************************************
created: 		2023-02-13

author:			chensong

purpose:		api_rtc_publish

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/

#ifndef _C_RTC_PUBLLISHER_H_
#define _C_RTC_PUBLLISHER_H_
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include <string>
#include "cnet_types.h"
#include "cdata_channel.h"
#include "ccapturer_tracksource.h"
namespace chen {
	
	/**
	*  @author chensong
	*  @date 2023-02-13
	*  @brief RTC发布者类（RTC Publisher Class）
	*  
	*  crtc_publisher类用于管理WebRTC PeerConnection，负责创建和管理
	*  WebRTC连接、处理SDP交换、管理音视频轨道和数据通道。
	*  
	*  RTC发布者架构（RTC Publisher Architecture）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |                      crtc_publisher                             |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | PeerConnectionFactory: 创建PeerConnection和媒体轨道          | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | PeerConnection: WebRTC连接管理                                | |
	*   |  |   - SDP创建和交换                                            | |
	*   |  |   - ICE候选收集                                              | |
	*   |  |   - 连接状态管理                                              | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | 音视频轨道: 视频和音频流                                      | |
	*   |  |   - VideoTrackSource: 视频源                                  | |
	*   |  |   - AudioTrackSource: 音频源                                  | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | DataChannel: 数据通道                                        | |
	*   |  |   - 双向数据传输                                              | |
	*   |  |   - 可靠/不可靠传输模式                                       | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | Listener: 事件回调接口                                        | |
	*   |  |   - SDP创建回调                                              | |
	*   |  |   - 连接失败回调                                              | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  WebRTC信令流程（WebRTC Signaling Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. InitializePeerConnection() - 初始化PeerConnection          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. CreatePeerConnection(dtls) - 创建PeerConnection           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. _add_tracks() - 添加音视频轨道                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. create_offer() - 创建Offer SDP                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. OnSuccess(desc) - SDP创建成功                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. send_create_offer_sdp() - 发送Offer到信令服务器            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. set_remoter_description() - 设置Answer SDP                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  8. OnIceCandidate() - ICE候选收集                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  9. 发送ICE候选到信令服务器                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  10. OnIceConnectionChange() - ICE连接建立                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  SDP消息格式（SDP Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  v=0                                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  o=- 4611731400430051336 2 IN IP4 127.0.0.1                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  s=-                                                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  t=0 0                                                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  m=video 9 UDP/TLS/RTP/SAVPF 96                                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  c=IN IP4 0.0.0.0                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  a=rtpmap:96 H264/90000                                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ... (更多SDP行)                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  ICE候选格式（ICE Candidate Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  candidate: <foundation> <component> <protocol> <priority>     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |         <candidate-ip> <candidate-port> <typ> <related-ip>     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |         <related-port> generation <generation>                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note crtc_publisher实现PeerConnectionObserver和CreateSessionDescriptionObserver接口
	*  @note 使用clistener接口通知外部事件
	*  @note 支持DTLS加密传输
	*  
	*  使用示例：
	*  @code
	*  auto listener = std::make_unique<MyListener>();
	*  auto publisher = webrtc::scoped_refptr<crtc_publisher>(
	*      new rtc::RefCountedObject<crtc_publisher>(listener.get()));
	*  publisher->InitializePeerConnection();
	*  publisher->create_offer();
	*  @endcode
	*/
	class crtc_publisher : public webrtc::PeerConnectionObserver  , 
		public webrtc::CreateSessionDescriptionObserver
	{
	public:
		/**
	*  @author chensong
	*  @date 2023-02-13
	*  @brief RTC发布者监听器接口（RTC Publisher Listener Interface）
	*
	*  clistener接口定义了RTC发布者的事件回调方法。当RTC发布者发生重要事件时
	*  （如创建Offer SDP、连接失败等），会通过此接口通知监听器。
	*
	*  监听器接口设计（Listener Interface Design）：
	*
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |                        clistener                                |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | send_create_offer_sdp(sdp, create)                          | |
	*   |  |   - 当Offer SDP创建完成时调用                                | |
	*   |  |   - 用于发送SDP到信令服务器                                  | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | connect_rtc_failed()                                         | |
	*   |  |   - 当RTC连接失败时调用                                      | |
	*   |  |   - 用于错误处理和重连逻辑                                    | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*
	*  @note 实现此接口的类需要处理RTC发布者的事件回调
	*  @note crtc_client类实现了此接口
	*
	*  使用示例：
	*  @code
	*  class MyListener : public crtc_publisher::clistener {
	*      void send_create_offer_sdp(const std::string& sdp, bool create) override {
	*          // 发送SDP到信令服务器
	*      }
	*      void connect_rtc_failed() override {
	*          // 处理连接失败
	*      }
	*  };
	*  @endcode
	*/
		class clistener
		{
		public:
			virtual ~clistener() = default;

		public:
			/**
			*  @author chensong
			*  @date 2023-02-13
			*  @brief 发送创建的Offer SDP（Send Created Offer SDP）
			*
			*  该方法在RTC发布者创建Offer SDP后被调用。实现者应该将SDP发送到
			*  信令服务器进行交换。
			*
			*  @param sdp Offer SDP字符串，包含媒体会话描述信息
			*  @param create 是否为创建新会话，true表示创建，false表示更新
			*  @note 该方法由crtc_publisher在创建Offer SDP后自动调用
			*/
			virtual void send_create_offer_sdp(const std::string& sdp, bool create = true) = 0;

			/**
			*  @author chensong
			*  @date 2023-02-13
			*  @brief RTC连接失败回调（RTC Connection Failed Callback）
			*
			*  该方法在RTC连接失败时被调用。实现者应该处理错误并可能尝试重连。
			*
			*  @note 该方法由crtc_publisher在连接失败时自动调用
			*/
			virtual void connect_rtc_failed() = 0;

		protected:
		private:
		};

	public:
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于创建RTC发布者实例。初始化监听器回调指针。
		*  
		*  @param callback 监听器回调指针，用于接收RTC发布者事件，不能为空
		*  @note 监听器用于接收SDP创建和连接失败等事件
		*/
		crtc_publisher(crtc_publisher::clistener * callback);

	public:
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 创建Offer SDP（Create Offer SDP）
		*  
		*  该方法用于创建WebRTC Offer SDP。Offer SDP包含本端的媒体能力描述，
		*  用于发起WebRTC连接。
		*  
		*  Offer创建流程（Offer Creation Flow）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 检查PeerConnection是否已创建                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 调用peer_connection_->CreateOffer()                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 异步创建Offer SDP                                           |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. OnSuccess()回调被调用，包含创建的SDP                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  5. 通过listener->send_create_offer_sdp()发送SDP                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @return 返回true表示创建请求成功发送，false表示创建失败
		*  @note Offer创建是异步的，结果通过OnSuccess()回调返回
		*  @note 创建的SDP会自动通过listener回调发送
		*  
		*  使用示例：
		*  @code
		*  if (publisher->create_offer()) {
		*      // Offer创建请求已发送
		*  }
		*  @endcode
		*/
		bool create_offer();

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 初始化PeerConnection（Initialize PeerConnection）
		*  
		*  该方法用于初始化WebRTC PeerConnection。创建PeerConnectionFactory
		*  和必要的线程（网络线程、信令线程、工作线程）。
		*  
		*  初始化流程（Initialization Flow）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 创建networkThread（网络线程）                               |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 创建signalingThread（信令线程）                             |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 创建workerThread（工作线程）                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. 创建PeerConnectionFactory                                  |
		*   |     - 设置网络线程                                              |
		*   |     - 设置信令线程                                              |
		*   |     - 设置工作线程                                              |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  5. 初始化完成，可以创建PeerConnection                          |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @return 返回true表示初始化成功，false表示初始化失败
		*  @note 需要在CreatePeerConnection()之前调用
		*  @note 线程用于处理WebRTC的异步操作
		*  
		*  使用示例：
		*  @code
		*  if (publisher->InitializePeerConnection()) {
		*      publisher->CreatePeerConnection(true);
		*  }
		*  @endcode
		*/
		bool InitializePeerConnection();

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 创建PeerConnection（Create PeerConnection）
		*  
		*  该方法用于创建WebRTC PeerConnection。PeerConnection是WebRTC的核心，
		*  负责管理连接、媒体轨道和数据通道。
		*  
		*  PeerConnection配置（PeerConnection Configuration）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  PeerConnectionInterface::RTCConfiguration:                    |
		*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
		*   |  | type: "all" | "relay" | "none"                             | |
		*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
		*   |  | ice_servers: STUN/TURN服务器列表                            | |
		*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
		*   |  | sdp_semantics: "unified-plan"                              | |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  MediaConstraints:                                             |
		*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
		*   |  | DTLS: enabled/disabled                                      | |
		*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
		*  
		*  @param dtls 是否启用DTLS加密，true表示启用，false表示禁用
		*  @return 返回true表示创建成功，false表示创建失败
		*  @note 需要在InitializePeerConnection()之后调用
		*  @note DTLS用于加密RTP/SRTP媒体传输
		*  
		*  使用示例：
		*  @code
		*  if (publisher->CreatePeerConnection(true)) {
		*      // PeerConnection创建成功，可以添加轨道
		*  }
		*  @endcode
		*/
		bool CreatePeerConnection(bool dtls);

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 设置远程描述（Set Remote Description）
		*  
		*  该方法用于设置远程端（对端）的SDP描述。通常在接收到Answer SDP后调用。
		*  
		*  设置远程描述流程（Set Remote Description Flow）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 解析SDP字符串为SessionDescription                         |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 创建SessionDescriptionInterface对象                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 调用peer_connection_->SetRemoteDescription()               |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. 触发ICE候选收集                                            |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  5. OnIceCandidate()回调被调用                                 |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param sdp 远程SDP字符串，通常是Answer SDP，不能为空
		*  @note 需要在创建Offer并发送后，接收对端Answer时调用
		*  @note SDP设置后，WebRTC会开始ICE候选收集
		*  
		*  使用示例：
		*  @code
		*  std::string answer_sdp = "...";  // 从信令服务器接收的Answer SDP
		*  publisher->set_remoter_description(answer_sdp);
		*  @endcode
		*/
		void set_remoter_description(std::string  sdp);

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 销毁RTC发布者（Destroy RTC Publisher）
		*  
		*  该方法用于销毁RTC发布者。清理所有资源，包括PeerConnection、Factory和线程。
		*  
		*  销毁流程（Destruction Flow）：
		*  1. 关闭PeerConnection
		*  2. 释放PeerConnectionFactory
		*  3. 停止并释放所有线程
		*  4. 清理数据通道
		*  
		*  @note 销毁后对象不能再使用
		*  @note 会自动释放所有WebRTC资源
		*/
		void Destory();
		 
	protected:
		//
		// PeerConnectionObserver implementation.
		//
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 信令状态变更回调（Signaling State Change Callback）
		*  
		*  该方法是PeerConnectionObserver接口的实现。当PeerConnection的信令状态
		*  改变时，WebRTC会自动调用此方法。
		*  
		*  信令状态说明（Signaling State Description）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kStable: 信令状态稳定，没有正在进行的SDP交换                  |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kHaveLocalOffer: 已创建并设置本地Offer                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kHaveLocalPrAnswer: 已创建并设置本地临时Answer                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kHaveRemoteOffer: 已接收并设置远程Offer                       |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kHaveRemotePrAnswer: 已接收并设置远程临时Answer               |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kClosed: PeerConnection已关闭                                 |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param new_state 新的信令状态，来自SignalingState枚举
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*  @note 信令状态反映SDP交换的当前阶段
		*/
		virtual	void OnSignalingChange(
			webrtc::PeerConnectionInterface::SignalingState new_state) override;

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 添加轨道回调（Add Track Callback）
		*  
		*  该方法是PeerConnectionObserver接口的实现。当远程端添加新的媒体轨道时，
		*  WebRTC会自动调用此方法。
		*  
		*  轨道添加处理流程（Track Addition Handling Flow）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 远程端添加媒体轨道（视频或音频）                           |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. WebRTC创建RtpReceiver接收轨道数据                          |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. OnAddTrack()回调被触发                                    |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. receiver->track(): 获取MediaStreamTrack                   |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  5. streams: 获取轨道所属的MediaStream列表                    |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  6. 处理轨道（显示视频、播放音频等）                          |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param receiver RTP接收器接口，用于接收媒体数据
		*  @param streams 媒体流列表，包含此轨道的MediaStream对象
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*  @note 通常用于接收远程端的音视频轨道
		*/
		virtual	void OnAddTrack(
			webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
				const std::vector<webrtc::scoped_refptr<webrtc::MediaStreamInterface>>&
				streams) override;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 移除轨道回调（Remove Track Callback）
		*  
		*  该方法是PeerConnectionObserver接口的实现。当远程端移除媒体轨道时，
		*  WebRTC会自动调用此方法。
		*  
		*  @param receiver RTP接收器接口，被移除的接收器
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*  @note 需要清理与此轨道相关的资源
		*/
		virtual	void OnRemoveTrack(
			webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 数据通道创建回调（Data Channel Creation Callback）
		*  
		*  该方法是PeerConnectionObserver接口的实现。当远程端创建数据通道时，
		*  WebRTC会自动调用此方法。
		*  
		*  @param channel 数据通道接口，用于双向数据传输
		*  @note 当前实现为空，可以根据需要添加数据通道处理逻辑
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*/
		virtual	void OnDataChannel(
			webrtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 需要重新协商回调（Renegotiation Needed Callback）
		*  
		*  该方法是PeerConnectionObserver接口的实现。当需要重新进行SDP协商时，
		*  WebRTC会自动调用此方法（例如添加或移除轨道后）。
		*  
		*  @note 当前实现为空，可以根据需要触发重新协商
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*/
		virtual	void OnRenegotiationNeeded() override {}
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief ICE连接状态变更回调（ICE Connection State Change Callback）
		*  
		*  该方法是PeerConnectionObserver接口的实现。当ICE连接状态改变时，
		*  WebRTC会自动调用此方法。
		*  
		*  ICE连接状态说明（ICE Connection State Description）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kIceConnectionNew: ICE连接刚创建                              |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kIceConnectionChecking: 正在检查ICE候选                       |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kIceConnectionConnected: ICE连接已建立                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kIceConnectionCompleted: ICE连接完成                          |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kIceConnectionFailed: ICE连接失败                             |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kIceConnectionDisconnected: ICE连接断开                       |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  kIceConnectionClosed: ICE连接已关闭                           |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param new_state 新的ICE连接状态，来自IceConnectionState枚举
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*  @note 连接失败时应该通知listener->connect_rtc_failed()
		*/
		virtual	void OnIceConnectionChange(
				webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief ICE候选收集状态变更回调（ICE Gathering State Change Callback）
		*  
		*  该方法是PeerConnectionObserver接口的实现。当ICE候选收集状态改变时，
		*  WebRTC会自动调用此方法。
		*  
		*  @param new_state 新的ICE收集状态（kIceGatheringNew/kIceGatheringGathering/kIceGatheringComplete）
		*  @note 当前实现为空，可以根据需要添加处理逻辑
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*/
		virtual	void OnIceGatheringChange(
				webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief ICE候选收集回调（ICE Candidate Callback）
		*  
		*  该方法是PeerConnectionObserver接口的实现。当收集到新的ICE候选时，
		*  WebRTC会自动调用此方法。ICE候选用于建立媒体传输路径。
		*  
		*  ICE候选收集流程（ICE Candidate Gathering Flow）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. WebRTC开始ICE候选收集                                     |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 发现新的网络候选（主机、服务器反射、中继）                 |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. OnIceCandidate()回调被触发，包含候选信息                   |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. 将候选信息编码为SDP格式                                   |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  5. 通过信令服务器发送候选到对端                              |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  6. 对端接收并添加候选                                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  ICE候选格式（ICE Candidate Format）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  candidate: <foundation> <component> <protocol> <priority>     |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |         <candidate-ip> <candidate-port> <typ> <related-ip>     |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |         <related-port> generation <generation>                  |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  sdp_mid: 媒体描述标识符                                       |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  sdp_mline_index: 媒体行索引                                   |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param candidate ICE候选接口指针，包含候选信息，不能为空
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*  @note 需要将候选信息发送到对端才能建立连接
		*  @note candidate为nullptr表示候选收集完成
		*/
		virtual	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief ICE连接接收状态变更回调（ICE Connection Receiving Change Callback）
		*  
		*  该方法是PeerConnectionObserver接口的实现。当ICE连接开始或停止接收数据时，
		*  WebRTC会自动调用此方法。
		*  
		*  @param receiving 是否正在接收数据，true表示正在接收，false表示未接收
		*  @note 当前实现为空，可以根据需要添加处理逻辑
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*/
		virtual	void OnIceConnectionReceivingChange(bool receiving) override {}
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 捕获控制回调（Capture Control Callback）
		*  
		*  该方法是自定义的回调接口。用于控制音视频捕获的开始和停止。
		*  
		*  @param enable 是否启用捕获，true表示启用，false表示禁用
		*  @note 用于外部控制音视频捕获
		*/
		virtual void OnCapture(bool enable) override;

	protected:
		//
		// CreateSessionDescriptionObserver implementation.
		//
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief SDP创建成功回调（SDP Creation Success Callback）
		*  
		*  该方法是CreateSessionDescriptionObserver接口的实现。当SDP创建成功时，
		*  WebRTC会自动调用此方法。
		*  
		*  SDP创建成功处理流程（SDP Creation Success Handling Flow）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. create_offer()创建Offer SDP请求                            |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. WebRTC异步创建SDP                                          |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. SDP创建成功，OnSuccess()回调被触发                         |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. desc->ToString(): 将SDP转换为字符串                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  5. 设置本地描述: peer_connection_->SetLocalDescription()      |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  6. 通过listener->send_create_offer_sdp()发送SDP到信令服务器  |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param desc SessionDescriptionInterface指针，包含创建的SDP描述，不能为空
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*  @note 此回调转移desc的所有权，需要负责释放desc
		*  @note SDP创建成功后需要设置本地描述并发送到对端
		*/
		virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc)  ;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief SDP创建失败回调 - RTCError版本（SDP Creation Failure Callback - RTCError Version）
		*  
		*  该方法是CreateSessionDescriptionObserver接口的实现。当SDP创建失败时，
		*  WebRTC会自动调用此方法。
		*  
		*  @param error RTCError对象，包含错误代码和错误信息
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*  @note RTCError是非拷贝的，需要使用std::move传递
		*  @note 需要处理错误并通知listener->connect_rtc_failed()
		*/
		virtual void OnFailure(webrtc::RTCError error);
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief SDP创建失败回调 - 字符串版本（SDP Creation Failure Callback - String Version）
		*  
		*  该方法是CreateSessionDescriptionObserver接口的旧版本实现。
		*  当SDP创建失败时，WebRTC会调用此方法（如果新版本未实现）。
		*  
		*  @param error 错误信息字符串，描述失败原因
		*  @note 该方法已废弃，建议使用RTCError版本
		*  @note 该方法由WebRTC自动调用，无需手动调用
		*  @note 如果两个版本都未实现，会导致运行时错误（栈溢出）
		*/
		virtual void OnFailure(const std::string& error);



	private:
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 添加音视频轨道（Add Tracks）
		*  
		*  该私有方法用于向PeerConnection添加音视频轨道。创建VideoTrackSource和
		*  AudioTrackSource，并将它们添加到PeerConnection中。
		*  
		*  添加轨道流程（Add Tracks Flow）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 创建VideoTrackSource（视频源）                              |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 创建VideoTrack并添加到PeerConnection                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 创建AudioTrackSource（音频源，如果需要）                    |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. 创建AudioTrack并添加到PeerConnection                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  5. 轨道添加完成，可以开始捕获和发送                            |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @note 该方法在CreatePeerConnection()后调用
		*  @note 需要先创建轨道才能创建Offer SDP
		*/
		void  _add_tracks();
		
	protected:
	private:
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 监听器回调指针（Listener Callback Pointer）
		*  
		*  该成员变量用于存储监听器回调指针。通过此指针可以通知外部事件，
		*  如SDP创建完成、连接失败等。
		*  
		*  @note 在构造函数中初始化，不能为空
		*  @note 用于回调SDP创建和连接失败事件
		*/
		crtc_publisher::clistener *			m_callback_ptr;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 网络线程（Network Thread）
		*  
		*  该成员变量用于存储WebRTC网络线程。网络线程负责处理网络I/O操作，
		*  包括RTP/RTCP数据包的发送和接收。
		*  
		*  线程职责（Thread Responsibilities）：
		*  - 处理UDP/TCP网络I/O
		*  - 发送和接收RTP/RTCP包
		*  - 管理网络连接
		*  
		*  @note 在InitializePeerConnection()中创建
		*  @note 使用std::unique_ptr管理生命周期
		*  @note 网络线程用于所有网络相关操作
		*/
		std::unique_ptr<webrtc::Thread> networkThread;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 信令线程（Signaling Thread）
		*  
		*  该成员变量用于存储WebRTC信令线程。信令线程负责处理PeerConnection的
		*  信令操作，如SDP创建、设置描述等。
		*  
		*  线程职责（Thread Responsibilities）：
		*  - 处理PeerConnection信令操作
		*  - 创建和设置SDP
		*  - 管理轨道添加/移除
		*  
		*  @note 在InitializePeerConnection()中创建
		*  @note 使用std::unique_ptr管理生命周期
		*  @note 信令线程用于所有PeerConnection操作
		*/
		std::unique_ptr<webrtc::Thread> signalingThread;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 工作线程（Worker Thread）
		*  
		*  该成员变量用于存储WebRTC工作线程。工作线程负责处理媒体编解码、
		*  数据处理等计算密集型任务。
		*  
		*  线程职责（Thread Responsibilities）：
		*  - 处理音视频编解码
		*  - 处理媒体数据处理
		*  - 执行计算密集型任务
		*  
		*  @note 在InitializePeerConnection()中创建
		*  @note 使用std::unique_ptr管理生命周期
		*  @note 工作线程用于所有媒体处理操作
		*/
		std::unique_ptr<webrtc::Thread> workerThread;

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief PeerConnection接口（PeerConnection Interface）
		*  
		*  该成员变量用于存储WebRTC PeerConnection接口的智能指针。
		*  PeerConnection是WebRTC的核心，负责管理连接、媒体轨道和数据通道。
		*  
		*  PeerConnection功能（PeerConnection Functions）：
		*  - CreateOffer(): 创建Offer SDP
		*  - SetLocalDescription(): 设置本地SDP描述
		*  - SetRemoteDescription(): 设置远程SDP描述
		*  - AddTrack(): 添加媒体轨道
		*  - CreateDataChannel(): 创建数据通道
		*  - Close(): 关闭连接
		*  
		*  @note 使用webrtc::scoped_refptr管理引用计数
		*  @note 在CreatePeerConnection()中创建
		*  @note PeerConnection是所有WebRTC操作的核心接口
		*/
		webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief PeerConnection工厂接口（PeerConnection Factory Interface）
		*  
		*  该成员变量用于存储WebRTC PeerConnectionFactory接口的智能指针。
		*  PeerConnectionFactory用于创建PeerConnection、媒体轨道和编码器等。
		*  
		*  Factory功能（Factory Functions）：
		*  - CreatePeerConnection(): 创建PeerConnection
		*  - CreateVideoTrack(): 创建视频轨道
		*  - CreateAudioTrack(): 创建音频轨道
		*  - CreateVideoEncoderFactory(): 创建视频编码器工厂
		*  - CreateVideoDecoderFactory(): 创建视频解码器工厂
		*  
		*  @note 使用webrtc::scoped_refptr管理引用计数
		*  @note 在InitializePeerConnection()中创建
		*  @note Factory是创建所有WebRTC对象的工厂
		*/
		webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 数据通道指针（Data Channel Pointer）
		*  
		*  该成员变量用于存储数据通道的智能指针。数据通道提供双向数据传输功能，
		*  用于发送控制消息等非媒体数据。
		*  
		*  数据通道功能（Data Channel Functions）：
		*  - Send(): 发送消息（文本或二进制）
		*  - state(): 查询当前状态
		*  - buffered_amount(): 查询缓冲区大小
		*  
		*  @note 使用webrtc::scoped_refptr管理引用计数
		*  @note 数据通道在PeerConnection创建后创建
		*  @note 用于发送控制消息和文件传输等
		*/
		webrtc::scoped_refptr < cdata_channel>			m_data_channel_ptr;
	};
}


#endif // _C_RTC_PUBLLISHER_H_