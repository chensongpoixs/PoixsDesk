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
#include <mutex>
#include <unordered_map>

namespace  chen {
	namespace {

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 码流采样结构体（Bitrate Sample Structure）
		*  
		*  用于存储码流计算的采样数据，包括时间戳和累计发送字节数。
		*  
		*  码流采样结构体布局（Bitrate Sample Structure Layout）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |                    BitrateSample Structure                    |
		*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
		*   |  |  timestamp_s: 时间戳（秒，double类型）                     | |
		*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
		*   |  |  bytes_sent: 累计发送字节数（uint64_t类型）                 | |
		*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @note 用于计算当前码流，通过两次采样之间的字节数增量除以时间差得到
		*  @note timestamp_s使用WebRTC统计报告的时间戳（秒）
		*  @note bytes_sent使用累计发送字节数，确保单调递增
		*/
		struct BitrateSample
		{
			double timestamp_s = 0.0;  /**< 时间戳（Timestamp），单位：秒（double类型），记录采样时间 */
			uint64_t bytes_sent = 0;   /**< 累计发送字节数（Cumulative Bytes Sent），uint64_t类型，记录累计发送的字节数 */
		};

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 码流采样互斥锁（Bitrate Sample Mutex）
		*  
		*  保护码流采样映射表的线程安全访问。
		*  
		*  @note 使用std::mutex保护g_bitrate_samples的并发访问
		*  @note 支持多路推流并发计算码流
		*/
		std::mutex g_bitrate_mutex;
		
		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief 码流采样映射表（Bitrate Sample Map）
		*  
		*  存储每路推流的码流采样数据，键为stream_id:ssrc，值为BitrateSample。
		*  
		*  @note 键格式：stream_id:ssrc（如："stream-001:12345"）
		*  @note 支持多路推流，每路推流独立计算码流
		*  @note 使用std::unordered_map提供O(1)查找性能
		*  @note 线程安全，通过g_bitrate_mutex保护
		*/
		std::unordered_map<std::string, BitrateSample> g_bitrate_samples;

		/**
		*  @author chensong
		*  @date 2023-02-13
		*  @brief HTTP POST JSON数据封装函数（HTTP POST JSON Data Wrapper Function）
		*  
		*  简单的HTTP POST封装，用于把统计结果推送到PushStatisServer。
		*  支持HTTP和HTTPS两种协议。
		*  
		*  @param endpoint PushStatisServer连接参数（Connection Endpoint）
		*                  - host: 服务器地址
		*                  - port: 服务器端口
		*                  - path: API路径（如"/api/stats"）
		*                  - secure: 是否使用HTTPS
		*  @param payload JSON格式的请求体（Request Body），包含统计信息
		*  
		*  @return true 请求成功（HTTP状态码2xx），false 请求失败或异常
		*  
		*  @note 使用httplib库实现HTTP/HTTPS客户端
		*  @note 连接超时设置为5秒
		*  @note Content-Type设置为"application/json"
		*  @note 如果启用HTTPS但httplib未编译SSL支持，会输出警告并返回false
		*  @note 捕获所有异常，避免崩溃
		*  
		*  使用示例：
		*  @code
		*  crtc_publisher::StatsEndpoint endpoint;
		*  endpoint.host = "127.0.0.1";
		*  endpoint.port = 8089;
		*  endpoint.path = "/api/stats";
		*  endpoint.secure = false;
		*  std::string json = R"({"streamId":"stream-001","video":{...}})";
		*  if (HttpPostJson(endpoint, json)) {
		*      // 推送成功
		*  }
		*  @endcode
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
	*  @author chensong
	*  @date 2023-02-13
	*  @brief 解析WebRTC统计报告并输出日志/构造JSON实现（Parse WebRTC Statistics Report and Output Log/Construct JSON Implementation）
	*  
	*  PeerConnection::GetStats()的异步回调函数实现。当统计报告准备好时被调用。
	*  解析报告中的所有统计项，提取关键指标，构造JSON对象，输出日志，并可选择性地推送到监控服务器。
	*  
	*  @param report WebRTC统计报告智能指针（Statistics Report Smart Pointer）
	*  
	*  @note report的生命周期仅在函数体内有效，不应保存引用
	*  @note 函数会遍历报告中的所有统计项，根据类型分别处理
	*  @note 支持三种统计类型：RTCOutboundRtpStreamStats、RTCIceCandidatePairStats、RTCTransportStats
	*  @note 计算延迟指标：编码延迟、发送延迟、采集延迟、网络延迟
	*  @note 计算当前码流：基于字节数增量和时间戳差值
	*  @note 如果启用上传，会在独立线程中异步推送统计信息
	*  @note 统计信息会同时输出到日志，便于本地调试
	*  
	*  处理流程（Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 获取统计报告时间戳                                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 遍历报告中的所有统计项                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 根据统计类型分类处理：                                      |
	*   |  |  - RTCOutboundRtpStreamStats: 视频编码和发送统计            |
	*   |  |  - RTCIceCandidatePairStats: 网络候选对统计                |
	*   |  |  - RTCTransportStats: 传输层统计                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 提取关键指标并构造JSON对象                                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 计算延迟指标和当前码流                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 输出格式化日志到控制台                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 如果启用上传，异步推送统计信息到监控服务器                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	void RtcStatsLogger::OnStatsDelivered(const webrtc::scoped_refptr<const webrtc::RTCStatsReport>& report)
	{
		std::vector<std::string> log_lines;
		nlohmann::json video_json = nlohmann::json::object();
		nlohmann::json network_json = nlohmann::json::object();
		nlohmann::json extra_json = nlohmann::json::object();
		const double report_timestamp_s = report->timestamp().us() / 1000000.0;
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
					double bitrate_MBps = 0.0;
					std::string bitrate_key = stream_id_;
					if (outbound.ssrc)
					{
						bitrate_key += ":" + std::to_string(*outbound.ssrc);
					}
					{
						std::lock_guard<std::mutex> lock(g_bitrate_mutex);
						auto& sample = g_bitrate_samples[bitrate_key];
						if (sample.timestamp_s > 0.0 &&
							report_timestamp_s > sample.timestamp_s &&
							*outbound.bytes_sent >= sample.bytes_sent)
						{
							double delta_bytes = static_cast<double>(*outbound.bytes_sent - sample.bytes_sent);
							double delta_time = report_timestamp_s - sample.timestamp_s;
							if (delta_time > 0.0)
							{
								bitrate_MBps = (delta_bytes / 1024.0 / 1024.0) / delta_time;
							}
						}
						sample.timestamp_s = report_timestamp_s;
						sample.bytes_sent = *outbound.bytes_sent;
					}
					if (bitrate_MBps > 0.0)
					{
						network_json["currentBitrateMBps"] = bitrate_MBps;
					}
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