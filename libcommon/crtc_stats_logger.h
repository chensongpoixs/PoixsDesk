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
	* @brief WebRTC 统计上报工具
	*
	* - 继承自 webrtc::RTCStatsCollectorCallback，供 PeerConnection::GetStats 异步回调使用。
	* - 解析 report 中的关键指标，转为内部 JSON（视频信息、网络信息、额外字段）。
	* - 视配置决定是否通过 HTTP 推送到 PushStatisServer，用于推流监控面板。
	*/
	class RtcStatsLogger : public webrtc::RTCStatsCollectorCallback
	{
	public:
		/**
		* @param stream_id			推流端唯一 ID（通常由机器名和进程号组成）
		* @param enable_upload		是否允许向 PushStatisServer 上报
		* @param endpoint			PushStatisServer 的连接参数
		*/
		RtcStatsLogger(std::string stream_id,
			bool enable_upload,
			crtc_publisher::StatsEndpoint endpoint)
			: stream_id_(std::move(stream_id)),
			upload_enabled_(enable_upload),
			endpoint_(std::move(endpoint)) {
		}

		/**
		* @brief WebRTC stats 回调入口，report 生命周期仅在函数体内有效
		*/
		void OnStatsDelivered(const webrtc::scoped_refptr<const webrtc::RTCStatsReport>& report) override;

	private:
		/**
		* @brief 辅助输出整型字段（含 is_defined 判断）
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
		* @brief 辅助输出字符串字段
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
		* @brief 辅助输出布尔字段
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
		* @brief 辅助输出浮点字段，可附带单位
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
		* @brief 将视频相关 stats 格式化为易读字符串，便于在日志中快速定位
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
		* @brief 将候选对（ICE）统计格式化为日志
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
		* @brief 将 transport 统计格式化为日志字符串
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

		std::string stream_id_;                    ///< 推流端唯一标识
		bool upload_enabled_;                     ///< 是否向 PushStatisServer 上报
		crtc_publisher::StatsEndpoint endpoint_;  ///< PushStatisServer 连接参数
	};
}

#endif // 