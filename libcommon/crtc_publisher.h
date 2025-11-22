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

	virtual	void OnSignalingChange(
		webrtc::PeerConnectionInterface::SignalingState new_state) override;


		// 好家伙  webrtc封装太好 ^_^  接口定义 PeerConnectionObserver
	virtual	void OnAddTrack(
		webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
			const std::vector<webrtc::scoped_refptr<webrtc::MediaStreamInterface>>&
			streams) override;
	virtual	void OnRemoveTrack(
		webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
	virtual	void OnDataChannel(
		webrtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}
	virtual	void OnRenegotiationNeeded() override {}
	virtual	void OnIceConnectionChange(
			webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
	virtual	void OnIceGatheringChange(
			webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
	virtual	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
	virtual	void OnIceConnectionReceivingChange(bool receiving) override {}


	 
	virtual void OnCapture(bool enable) override;

	protected:

		// This callback transfers the ownership of the |desc|.
		  // TODO(deadbeef): Make this take an std::unique_ptr<> to avoid confusion
		  // around ownership.
		virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc)  ;
		// The OnFailure callback takes an RTCError, which consists of an
		// error code and a string.
		// RTCError is non-copyable, so it must be passed using std::move.
		// Earlier versions of the API used a string argument. This version
		// is deprecated; in order to let clients remove the old version, it has a
		// default implementation. If both versions are unimplemented, the
		// result will be a runtime error (stack overflow). This is intentional.
		virtual void OnFailure(webrtc::RTCError error);
		virtual void OnFailure(const std::string& error);



	private:
		void  _add_tracks();
	protected:
		//~crtc_publisher() {}
	private:
		// Signaling and worker threads.
		crtc_publisher::clistener *			m_callback_ptr;
		std::unique_ptr<webrtc::Thread> networkThread;
		std::unique_ptr<webrtc::Thread> signalingThread;
		std::unique_ptr<webrtc::Thread> workerThread;

		webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
		webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_;
		 

	 
		webrtc::scoped_refptr < cdata_channel>			m_data_channel_ptr;
	};
}


#endif // _C_RTC_PUBLLISHER_H_