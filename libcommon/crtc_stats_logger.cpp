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
#include "crtc_stats_logger.h"

namespace  chen {
	namespace {

		/**
		* @brief 简单的 HTTP POST 封装，用于把统计结果推送到 PushStatisServer
		* @return 请求是否成功（2xx）
		*/
		bool HttpPostJson(const chen::crtc_publisher::StatsEndpoint& endpoint, const std::string& payload)
		{
			if (endpoint.host.empty() || endpoint.port == 0)
			{
				return false;
			}

			const char* content_type = "application/json";
			try
			{
				if (endpoint.secure)
				{
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
					httplib::SSLClient client(endpoint.host, endpoint.port);
					client.set_connection_timeout(5);
					if (auto res = client.Post(endpoint.path.c_str(), payload, content_type))
					{
						return res->status >= 200 && res->status < 300;
					}
#else
					WARNING_EX_LOG("Push stats endpoint requires HTTPS but httplib SSL support is disabled");
#endif
				}
				else
				{
					httplib::Client client(endpoint.host, endpoint.port);
					client.set_connection_timeout(5);
					if (auto res = client.Post(endpoint.path.c_str(), payload, content_type))
					{
						return res->status >= 200 && res->status < 300;
					}
				}
			}
			catch (const std::exception& ex)
			{
				WARNING_EX_LOG("Push stats request exception: %s", ex.what());
			}

			return false;
		}

	} // namespace
	/**
	* @brief 解析 WebRTC stats 并输出日志 / 构造 JSON
	*/
	void RtcStatsLogger::OnStatsDelivered(const webrtc::scoped_refptr<const webrtc::RTCStatsReport>& report)
	{
		std::vector<std::string> log_lines;
		nlohmann::json video_json = nlohmann::json::object();
		nlohmann::json network_json = nlohmann::json::object();
		nlohmann::json extra_json = nlohmann::json::object();
		for (const auto& stats : *report)
		{
			// 视频编码信息：尺寸、帧率、编码器、发送统计等
			if (stats.type() == webrtc::RTCOutboundRtpStreamStats::kType)
			{
				const auto& outbound = stats.cast_to<webrtc::RTCOutboundRtpStreamStats>();
				log_lines.emplace_back(FormatVideo(outbound));

				if (outbound.frame_width)
				{
					video_json["width"] = *outbound.frame_width;
				}
				if (outbound.frame_height)
				{
					video_json["height"] = *outbound.frame_height;
				}
				if (outbound.frames_per_second)
				{
					video_json["fps"] = *outbound.frames_per_second;
				}
				if (outbound.bytes_sent)
				{
					video_json["bytesSent"] = *outbound.bytes_sent;
				}
				if (outbound.packets_sent)
				{
					video_json["packetsSent"] = *outbound.packets_sent;
				}
				if (outbound.frames_encoded)
				{
					video_json["framesEncoded"] = *outbound.frames_encoded;
				}
				if (outbound.encoder_implementation)
				{
					video_json["encoder"] = *outbound.encoder_implementation;
				}
				if (outbound.ssrc)
				{
					video_json["ssrc"] = *outbound.ssrc;
				}

				if (outbound.total_encode_time && outbound.frames_encoded && *outbound.frames_encoded > 0)
				{
					double encode_delay =
						(*outbound.total_encode_time / *outbound.frames_encoded) * 1000.0;
					video_json["encodeDelayMs"] = encode_delay;
				}
				if (outbound.total_packet_send_delay && outbound.packets_sent && *outbound.packets_sent > 0)
				{
					double send_delay =
						(*outbound.total_packet_send_delay / *outbound.packets_sent) * 1000.0;
					video_json["sendDelayMs"] = send_delay;
				}
				video_json["captureDelayMs"] = GetCaptureProcessDelayMs();
			}
			// 网络候选对信息：可用带宽 / RTT / 发包统计
			else if (stats.type() == webrtc::RTCIceCandidatePairStats::kType)
			{
				const auto& pair = stats.cast_to<webrtc::RTCIceCandidatePairStats>();
				if (pair.nominated && !*pair.nominated)
				{
					continue;
				}
				if (pair.state && *pair.state != "succeeded")
				{
					continue;
				}
				log_lines.emplace_back(FormatCandidate(pair));

				if (pair.current_round_trip_time)
				{
					double rtt = *pair.current_round_trip_time;
					network_json["currentRoundTripTime"] = rtt;
					network_json["rttMs"] = rtt * 1000.0;
				}
				if (pair.available_outgoing_bitrate)
				{
					double bw = *pair.available_outgoing_bitrate;
					network_json["availableOutgoingBitrateKbps"] = bw / 1000.0;
				}
				if (pair.available_incoming_bitrate)
				{
					double bw = *pair.available_incoming_bitrate;
					network_json["availableIncomingBitrateKbps"] = bw / 1000.0;
				}
				if (pair.bytes_sent)
				{
					network_json["bytesSent"] = *pair.bytes_sent;
				}
				if (pair.bytes_received)
				{
					network_json["bytesReceived"] = *pair.bytes_received;
				}
				if (pair.packets_sent)
				{
					network_json["packetsSent"] = *pair.packets_sent;
				}
				if (pair.packets_received)
				{
					network_json["packetsReceived"] = *pair.packets_received;
				}
				extra_json["candidatePairId"] = pair.id();
				if (pair.local_candidate_id)
				{
					extra_json["localCandidateId"] = *pair.local_candidate_id;
				}
				if (pair.remote_candidate_id)
				{
					extra_json["remoteCandidateId"] = *pair.remote_candidate_id;
				}
			}
			// 传输层信息：DTLS 状态、传输字节数
			else if (stats.type() == webrtc::RTCTransportStats::kType)
			{
				const auto& transport = stats.cast_to<webrtc::RTCTransportStats>();
				log_lines.emplace_back(FormatTransport(transport));
				if (transport.bytes_sent)
				{
					network_json["transportBytesSent"] = *transport.bytes_sent;
				}
				if (transport.bytes_received)
				{
					network_json["transportBytesReceived"] = *transport.bytes_received;
				}
				extra_json["transportId"] = transport.id();
			}
		}
		if (network_json.contains("rttMs"))
		{
			network_json["networkDelayMs"] = network_json["rttMs"];
		}

		for (const auto& line : log_lines)
		{
			NORMAL_EX_LOG("%s", line.c_str());
		}

		if (upload_enabled_)
		{
			nlohmann::json payload = {
				{"streamId", stream_id_},
				{"video", video_json},
				{"network", network_json},
				{"extra", extra_json}
			};

			std::string body = payload.dump();
			std::thread([endpoint = endpoint_, data = std::move(body)]() {
				if (!HttpPostJson(endpoint, data))
				{
					WARNING_EX_LOG("Failed to send stats to PushStatisServer");
				}
				}).detach();
		}
	}

}