/***********************************************************************************************
created: 		2023-02-13

author:			chensong

purpose:		rtc_status _logger 

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
 
#ifndef _C_RTC_STATUS_LOGGER_H_
#define _C_RTC_STATUS_LOGGER_H_



#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "api/audio/audio_mixer.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_options.h"
#include "api/create_peerconnection_factory.h"
#include "api/rtp_sender_interface.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"

#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "p2p/base/port_allocator.h"
#include "pc/video_track_source.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/ref_counted_object.h"
#include "rtc_base/rtc_certificate_generator.h"
#include <windows.h>
#include <cstdlib>
//#include "cclient.h"
//#include "ccapturer_track_source.h"
#include "ccapturer_tracksource.h"
//#include "external_video_encoder_factory.h"

#include "api/rtp_transceiver_interface.h"
#include "media/base/rid_description.h"
#include "cinput_device.h"
#include "clog.h"
#include "cdesktop_capture.h"
//#include "api/stats/rtcstats_collector_callback.h"
#include "api/stats/rtcstats_objects.h"
#include "httplib.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#include <vector>


namespace chen {

	/**
	*  @author chensong
	*  @date 2023-02-13
	*  @brief WebRTC 统计上报工具类（WebRTC Statistics Reporting Tool Class）
	*  
	*  RtcStatsLogger类用于收集和上报WebRTC推流端的实时统计信息。
	*  继承自webrtc::RTCStatsCollectorCallback，通过PeerConnection::GetStats异步回调获取统计数据。
	*  
	*  WebRTC统计上报工具架构（WebRTC Statistics Reporting Tool Architecture）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |                    RtcStatsLogger Class                       |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  PeerConnection::GetStats() 异步调用                        | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  OnStatsDelivered() 回调处理                                | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  解析 RTCOutboundRtpStreamStats（视频统计）              | | |
	*   |  |  |  - 分辨率、帧率、编码器、发送统计                        | | |
	*   |  |  |  - 编码延迟、发送延迟、采集延迟                          | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  解析 RTCIceCandidatePairStats（网络统计）                | | |
	*   |  |  |  - RTT、可用带宽、收发字节数、包数                        | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  解析 RTCTransportStats（传输层统计）                     | | |
	*   |  |  |  - DTLS状态、传输字节数                                  | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  构造JSON并输出日志                                         | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  通过HTTP POST推送到PushStatisServer（可选）                | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  统计信息处理流程（Statistics Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. PeerConnection::GetStats() 异步获取统计报告                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. OnStatsDelivered() 回调被调用                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 遍历统计报告中的所有统计项                                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 根据统计类型解析数据：                                      |
	*   |  |  - RTCOutboundRtpStreamStats: 视频编码和发送统计            |
	*   |  |  - RTCIceCandidatePairStats: 网络候选对统计                |
	*   |  |  - RTCTransportStats: 传输层统计                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 计算延迟指标：编码延迟、发送延迟、采集延迟、网络延迟        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 计算当前码流（基于字节数增量和时间戳差值）                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 构造JSON对象（video、network、extra）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  8. 输出格式化日志到控制台                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  9. 如果启用上传，通过HTTP POST推送到PushStatisServer          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  统计信息分类（Statistics Categories）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  视频统计（Video Statistics）:                                 |
	*   |  - 分辨率（width、height）                                     |
	*   |  - 帧率（fps）                                                 |
	*   |  - 编码帧数（framesEncoded）                                   |
	*   |  - 发送字节数（bytesSent）                                     |
	*   |  - 发送包数（packetsSent）                                     |
	*   |  - 编码器实现（encoder）                                       |
	*   |  - SSRC（同步源标识）                                         |
	*   |  - 编码延迟（encodeDelayMs）                                   |
	*   |  - 发送延迟（sendDelayMs）                                    |
	*   |  - 采集延迟（captureDelayMs）                                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  网络统计（Network Statistics）:                               |
	*   |  - RTT（往返时延，rttMs）                                      |
	*   |  - 可用上行带宽（availableOutgoingBitrateKbps）                |
	*   |  - 可用下行带宽（availableIncomingBitrateKbps）                |
	*   |  - 发送字节数（bytesSent）                                     |
	*   |  - 接收字节数（bytesReceived）                                 |
	*   |  - 发送包数（packetsSent）                                     |
	*   |  - 接收包数（packetsReceived）                                 |
	*   |  - 当前码流（currentBitrateMBps）                              |
	*   |  - 网络延迟（networkDelayMs）                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  传输层统计（Transport Statistics）:                           |
	*   |  - 传输发送字节数（transportBytesSent）                        |
	*   |  - 传输接收字节数（transportBytesReceived）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note 继承自webrtc::RTCStatsCollectorCallback，用于异步回调
	*  @note 支持WebRTC m142版本的统计API
	*  @note 使用线程安全的码流计算，支持多路推流
	*  @note 支持HTTP和HTTPS两种协议推送统计信息
	*  @note 统计信息会同时输出到日志和推送到监控服务器
	*  
	*  使用示例：
	*  @code
	*  crtc_publisher::StatsEndpoint endpoint;
	*  endpoint.host = "127.0.0.1";
	*  endpoint.port = 8089;
	*  endpoint.path = "/api/stats";
	*  endpoint.secure = false;
	*  
	*  auto logger = webrtc::make_ref_counted<RtcStatsLogger>(
	*      "stream-001", true, endpoint);
	*  peer_connection->GetStats(logger.get());
	*  @endcode
	*/
	class RtcStatsLogger : public webrtc::RTCStatsCollectorCallback
	{
	public:
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 构造函数（Constructor）
		*  
		*  创建RtcStatsLogger对象，初始化推流端标识和上报配置。
		*  
		*  @param stream_id 推流端唯一标识（Stream ID），通常由机器名和进程号组成
		*  @param enable_upload 是否允许向PushStatisServer上报（Enable Upload Flag）
		*  @param endpoint PushStatisServer的连接参数（Connection Endpoint）
		*                  - host: 服务器地址
		*                  - port: 服务器端口
		*                  - path: API路径
		*                  - secure: 是否使用HTTPS
		*  
		*  @note 使用std::move优化参数传递，避免不必要的拷贝
		*  @note stream_id用于标识不同的推流端，便于监控面板区分
		*/
		RtcStatsLogger(std::string stream_id,
			bool enable_upload,
			crtc_publisher::StatsEndpoint endpoint)
			: stream_id_(std::move(stream_id)),
			upload_enabled_(enable_upload),
			endpoint_(std::move(endpoint)) {
		}

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief WebRTC统计报告回调函数（WebRTC Statistics Report Callback）
		*  
		*  PeerConnection::GetStats()的异步回调函数。当统计报告准备好时被调用。
		*  
		*  @param report WebRTC统计报告智能指针（Statistics Report Smart Pointer）
		*  
		*  @note report的生命周期仅在函数体内有效，不应保存引用
		*  @note 函数会解析报告中的所有统计项，提取关键指标
		*  @note 如果启用上传，会在独立线程中异步推送统计信息
		*  @note 统计信息会同时输出到日志，便于本地调试
		*  
		*  处理流程：
		*  1. 遍历统计报告中的所有统计项
		*  2. 根据统计类型（视频/网络/传输）解析数据
		*  3. 计算延迟指标和当前码流
		*  4. 构造JSON对象
		*  5. 输出日志
		*  6. 如果启用，推送统计信息到监控服务器
		*/
		void OnStatsDelivered(const webrtc::scoped_refptr<const webrtc::RTCStatsReport>& report) override;

	private:
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 辅助输出整型字段模板函数（Helper Template Function for Integral Output）
		*  
		*  将WebRTC统计成员中的整型值输出到字符串流。
		*  支持WebRTC m142版本的RTCStatsMember类型，自动判断字段是否定义。
		*  
		*  @tparam MemberT WebRTC统计成员类型（RTCStatsMember<T>）
		*  @param label 字段标签（Field Label），用于标识字段名称
		*  @param member 统计成员引用（Statistics Member Reference），支持operator bool()判断是否定义
		*  @param oss 输出字符串流（Output String Stream），用于构建格式化字符串
		*  
		*  @note 使用operator bool()判断成员是否定义（m142兼容）
		*  @note 使用operator*()获取成员值
		*  @note 输出格式：label=value 
		*  @note 如果成员未定义，不输出任何内容
		*/
		template <typename MemberT>
		static void AppendIntegral(const char* label, const MemberT& member, std::ostringstream& oss)
		{
			if (member)
			{
				oss << label << '=' << static_cast<long long>(*member) << ' ';
			}
		}

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 辅助输出字符串字段模板函数（Helper Template Function for String Output）
		*  
		*  将WebRTC统计成员中的字符串值输出到字符串流。
		*  
		*  @tparam MemberT WebRTC统计成员类型（RTCStatsMember<T>）
		*  @param label 字段标签（Field Label），用于标识字段名称
		*  @param member 统计成员引用（Statistics Member Reference），支持operator bool()判断是否定义
		*  @param oss 输出字符串流（Output String Stream），用于构建格式化字符串
		*  
		*  @note 使用std::quoted()为字符串添加引号，便于阅读
		*  @note 输出格式：label="value" 
		*  @note 如果成员未定义，不输出任何内容
		*/
		template <typename MemberT>
		static void AppendString(const char* label, const MemberT& member, std::ostringstream& oss)
		{
			if (member)
			{
				oss << label << '=' << std::quoted(*member) << ' ';
			}
		}

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 辅助输出布尔字段模板函数（Helper Template Function for Boolean Output）
		*  
		*  将WebRTC统计成员中的布尔值输出到字符串流。
		*  
		*  @tparam MemberT WebRTC统计成员类型（RTCStatsMember<T>）
		*  @param label 字段标签（Field Label），用于标识字段名称
		*  @param member 统计成员引用（Statistics Member Reference），支持operator bool()判断是否定义
		*  @param oss 输出字符串流（Output String Stream），用于构建格式化字符串
		*  
		*  @note 布尔值输出为"true"或"false"字符串
		*  @note 输出格式：label=true 或 label=false
		*  @note 如果成员未定义，不输出任何内容
		*/
		template <typename MemberT>
		static void AppendBool(const char* label, const MemberT& member, std::ostringstream& oss)
		{
			if (member)
			{
				oss << label << '=' << (*member ? "true" : "false") << ' ';
			}
		}

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 辅助输出浮点字段模板函数（Helper Template Function for Double Output）
		*  
		*  将WebRTC统计成员中的浮点值输出到字符串流，支持单位转换和单位显示。
		*  
		*  @tparam MemberT WebRTC统计成员类型（RTCStatsMember<T>）
		*  @param label 字段标签（Field Label），用于标识字段名称
		*  @param member 统计成员引用（Statistics Member Reference），支持operator bool()判断是否定义
		*  @param multiplier 单位转换乘数（Unit Conversion Multiplier），用于单位转换（如秒转毫秒：1000.0）
		*  @param unit 单位字符串（Unit String），可选，用于显示单位（如"ms"、"kbps"）
		*  @param oss 输出字符串流（Output String Stream），用于构建格式化字符串
		*  
		*  @note 使用std::fixed和std::setprecision(2)固定小数点后2位
		*  @note 输出格式：label=valueunit （如果unit不为空）
		*  @note 如果成员未定义，不输出任何内容
		*  
		*  使用示例：
		*  @code
		*  AppendDouble("rtt_ms", pair.current_round_trip_time, 1000.0, "ms", oss);
		*  // 输出：rtt_ms=45.23ms
		*  @endcode
		*/
		template <typename MemberT>
		static void AppendDouble(const char* label, const MemberT& member, double multiplier, const char* unit, std::ostringstream& oss)
		{
			if (!member)
			{
				return;
			}
			std::ostringstream value;
			value << std::fixed << std::setprecision(2) << (*member * multiplier);
			oss << label << '=' << value.str();
			if (unit)
			{
				oss << unit;
			}
			oss << ' ';
		}

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 格式化视频统计信息（Format Video Statistics）
		*  
		*  将RTCOutboundRtpStreamStats统计信息格式化为易读的字符串，便于在日志中快速定位。
		*  
		*  @param outbound 出站RTP流统计对象（Outbound RTP Stream Statistics Object）
		*  
		*  @return 格式化后的字符串（Formatted String），包含所有视频相关统计信息
		*  
		*  @note 输出格式：[RTC Video] track="..." ssrc=12345 width=1920 height=1080 fps=30.00 ...
		*  @note 包含字段：track ID、SSRC、分辨率、帧率、编码帧数、发送字节数、发送包数、编码器
	*  
		*  使用示例：
		*  @code
		*  std::string video_info = FormatVideo(outbound_stats);
		*  // 输出：[RTC Video] track="video_track_0" ssrc=12345 width=1920 height=1080 fps=30.00 ...
		*  @endcode
		*/
		static std::string FormatVideo(const webrtc::RTCOutboundRtpStreamStats& outbound)
		{
			std::ostringstream oss;
			oss << "[RTC Video] ";
			AppendString("track", outbound.media_source_id, oss);
			AppendIntegral("ssrc", outbound.ssrc, oss);
			AppendIntegral("width", outbound.frame_width, oss);
			AppendIntegral("height", outbound.frame_height, oss);
			AppendDouble("fps", outbound.frames_per_second, 1.0, "", oss);
			AppendIntegral("frames_encoded", outbound.frames_encoded, oss);
			AppendIntegral("bytes_sent", outbound.bytes_sent, oss);
			AppendIntegral("packets_sent", outbound.packets_sent, oss);
			AppendString("encoder", outbound.encoder_implementation, oss);
			return oss.str();
		}

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 格式化ICE候选对统计信息（Format ICE Candidate Pair Statistics）
		*  
		*  将RTCIceCandidatePairStats统计信息格式化为易读的字符串。
		*  
		*  @param pair ICE候选对统计对象（ICE Candidate Pair Statistics Object）
		*  
		*  @return 格式化后的字符串（Formatted String），包含所有网络相关统计信息
		*  
		*  @note 输出格式：[RTC Network] pair=xxx state="succeeded" nominated=true rtt_ms=45.23ms ...
		*  @note 包含字段：候选对ID、状态、提名标志、RTT、可用带宽、收发字节数、收发包数、本地/远程候选ID
		*  
		*  使用示例：
		*  @code
		*  std::string network_info = FormatCandidate(pair_stats);
		*  // 输出：[RTC Network] pair=xxx state="succeeded" nominated=true rtt_ms=45.23ms ...
		*  @endcode
		*/
		static std::string FormatCandidate(const webrtc::RTCIceCandidatePairStats& pair)
		{
			std::ostringstream oss;
			oss << "[RTC Network] pair=" << pair.id() << ' ';
			AppendString("state", pair.state, oss);
			AppendBool("nominated", pair.nominated, oss);
			AppendDouble("rtt_ms", pair.current_round_trip_time, 1000.0, "ms", oss);
			AppendDouble("available_out_kbps", pair.available_outgoing_bitrate, 0.001, "kbps", oss);
			AppendIntegral("bytes_sent", pair.bytes_sent, oss);
			AppendIntegral("bytes_recv", pair.bytes_received, oss);
			AppendIntegral("packets_sent", pair.packets_sent, oss);
			AppendIntegral("packets_recv", pair.packets_received, oss);
			AppendString("local", pair.local_candidate_id, oss);
			AppendString("remote", pair.remote_candidate_id, oss);
			return oss.str();
		}

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 格式化传输层统计信息（Format Transport Statistics）
		*  
		*  将RTCTransportStats统计信息格式化为易读的字符串。
		*  
		*  @param transport 传输层统计对象（Transport Statistics Object）
		*  
		*  @return 格式化后的字符串（Formatted String），包含所有传输层相关统计信息
		*  
		*  @note 输出格式：[RTC Transport] id=xxx dtls_state="connected" candidate_pair="xxx" ...
		*  @note 包含字段：传输ID、DTLS状态、候选对ID、收发字节数
		*  
		*  使用示例：
		*  @code
		*  std::string transport_info = FormatTransport(transport_stats);
		*  // 输出：[RTC Transport] id=xxx dtls_state="connected" ...
		*  @endcode
		*/
		static std::string FormatTransport(const webrtc::RTCTransportStats& transport)
		{
			std::ostringstream oss;
			oss << "[RTC Transport] id=" << transport.id() << ' ';
			AppendString("dtls_state", transport.dtls_state, oss);
			AppendString("candidate_pair", transport.selected_candidate_pair_id, oss);
			AppendIntegral("bytes_sent", transport.bytes_sent, oss);
			AppendIntegral("bytes_recv", transport.bytes_received, oss);
			return oss.str();
		}

		std::string stream_id_;                    /**< 推流端唯一标识（Stream ID），用于区分不同的推流端，通常由机器名和进程号组成 */
		bool upload_enabled_;                     /**< 是否向PushStatisServer上报标志（Upload Enabled Flag），true表示启用HTTP POST推送 */
		crtc_publisher::StatsEndpoint endpoint_;  /**< PushStatisServer连接参数（Connection Endpoint），包含host、port、path、secure等信息 */
	};
}

#endif // 