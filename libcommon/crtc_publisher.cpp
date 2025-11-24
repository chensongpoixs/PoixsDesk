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

#include "crtc_publisher.h"

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

namespace {

std::string GenerateStreamId()
{
	char computer_name[MAX_COMPUTERNAME_LENGTH + 1] = "PoixsDesk";
	DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
	if (!GetComputerNameA(computer_name, &size))
	{
		strcpy_s(computer_name, "PoixsDesk");
	}
	std::ostringstream oss;
	oss << computer_name << "-" << GetCurrentProcessId();
	return oss.str();
}

bool ParseStatsEndpoint(const std::string& url, chen::crtc_publisher::StatsEndpoint& endpoint)
{
	if (url.empty())
	{
		return false;
	}
	auto scheme_pos = url.find("://");
	if (scheme_pos == std::string::npos)
	{
		return false;
	}
	std::string scheme = url.substr(0, scheme_pos);
	endpoint.secure = (scheme == "https");
	std::string remainder = url.substr(scheme_pos + 3);
	auto path_pos = remainder.find('/');
	std::string host_port = path_pos == std::string::npos ? remainder : remainder.substr(0, path_pos);
	std::string path = path_pos == std::string::npos ? "/" : remainder.substr(path_pos);
	auto colon_pos = host_port.find(':');
	std::string host = colon_pos == std::string::npos ? host_port : host_port.substr(0, colon_pos);
	uint16_t port = 0;
	if (colon_pos == std::string::npos)
	{
		port = endpoint.secure ? 443 : 8089;
	}
	else
	{
		port = static_cast<uint16_t>(std::stoi(host_port.substr(colon_pos + 1)));
	}
	endpoint.host = host;
	endpoint.path = path.empty() ? "/" : path;
	endpoint.port = port;
	return !endpoint.host.empty() && endpoint.port != 0;
}

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

namespace chen {
	
	namespace
	{


		class DesktopTrackSource : public webrtc::VideoTrackSource {
		public:
			static webrtc::scoped_refptr<DesktopTrackSource> Create(
				 ) {
#if 0
				std::unique_ptr<TestVideoCapturer> capturer =
					CreateCapturer(task_queue_factory);
				if (capturer) {
					capturer->Start();
					return webrtc::make_ref_counted<DesktopTrackSource>(std::move(capturer));
				}
#endif
				std::unique_ptr<chen::DesktopCapture> capturer(
					chen::DesktopCapture::Create(60, 0));
				if (capturer) {
					capturer->StartCapture();
					return webrtc::make_ref_counted<DesktopTrackSource>(std::move(capturer));
				}
				return nullptr;
			}
			void StopCapture()
			{
				capturer_->StopCapture();
			}

		protected:
			explicit DesktopTrackSource(
				std::unique_ptr<chen::DesktopCapture> capturer)
				: VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {
			}

		private:
			webrtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
				return capturer_.get();
			}
			// std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
			std::unique_ptr<chen::DesktopCapture> capturer_;
		};

		class StatsLogger : public webrtc::RTCStatsCollectorCallback
		{
		public:
			StatsLogger(std::string stream_id,
				bool enable_upload,
				crtc_publisher::StatsEndpoint endpoint)
				: stream_id_(std::move(stream_id)),
				upload_enabled_(enable_upload),
				endpoint_(std::move(endpoint)) {}

			void OnStatsDelivered(const webrtc::scoped_refptr<const webrtc::RTCStatsReport>& report) override
			{
				std::vector<std::string> log_lines;
				nlohmann::json video_json = nlohmann::json::object();
				nlohmann::json network_json = nlohmann::json::object();
				nlohmann::json extra_json = nlohmann::json::object();
				for (const auto& stats : *report)
				{
					 if (stats.type() == webrtc::RTCOutboundRtpStreamStats::kType)
					{
						const auto& outbound = stats.cast_to<webrtc::RTCOutboundRtpStreamStats>();
						if ( outbound.content_type  &&  *outbound.content_type != "video")
						{
							continue;
						}
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
						if (outbound.bytes_sent )
						{
							video_json["bytesSent"] = *outbound.bytes_sent;
						}
						if (outbound.packets_sent )
						{
							video_json["packetsSent"] = *outbound.packets_sent;
						}
						if (outbound.frames_encoded )
						{
							video_json["framesEncoded"] = *outbound.frames_encoded;
						}
						if (outbound.encoder_implementation )
						{
							video_json["encoder"] = *outbound.encoder_implementation;
						}
						if (outbound.ssrc )
						{
							video_json["ssrc"] = *outbound.ssrc;
						}
					}
					else if (stats.type() == webrtc::RTCIceCandidatePairStats::kType)
					{
						const auto& pair = stats.cast_to<webrtc::RTCIceCandidatePairStats>();
						if (pair.nominated  && !*pair.nominated)
						{
							continue;
						}
						if (pair.state  && *pair.state != "succeeded")
						{
							continue;
						}
						log_lines.emplace_back(FormatCandidate(pair));

						if (pair.current_round_trip_time )
						{
							double rtt = *pair.current_round_trip_time;
							network_json["currentRoundTripTime"] = rtt;
							network_json["rttMs"] = rtt * 1000.0;
						}
						if (pair.available_outgoing_bitrate )
						{
							double bw = *pair.available_outgoing_bitrate;
							network_json["availableOutgoingBitrateKbps"] = bw / 1000.0;
						}
						if (pair.available_incoming_bitrate )
						{
							double bw = *pair.available_incoming_bitrate;
							network_json["availableIncomingBitrateKbps"] = bw / 1000.0;
						}
						if (pair.bytes_sent )
						{
							network_json["bytesSent"] = *pair.bytes_sent;
						}
						if (pair.bytes_received )
						{
							network_json["bytesReceived"] = *pair.bytes_received;
						}
						if (pair.packets_sent )
						{
							network_json["packetsSent"] = *pair.packets_sent;
						}
						if (pair.packets_received )
						{
							network_json["packetsReceived"] = *pair.packets_received;
						}
						extra_json["candidatePairId"] = pair.id();
						if (pair.local_candidate_id )
						{
							extra_json["localCandidateId"] = *pair.local_candidate_id;
						}
						if (pair.remote_candidate_id )
						{
							extra_json["remoteCandidateId"] = *pair.remote_candidate_id;
						}
					}
					else if (stats.type() == webrtc::RTCTransportStats::kType)
					{
						const auto& transport = stats.cast_to<webrtc::RTCTransportStats>();
						log_lines.emplace_back(FormatTransport(transport));
						if (transport.bytes_sent )
						{
							network_json["transportBytesSent"] = *transport.bytes_sent;
						}
						if (transport.bytes_received )
						{
							network_json["transportBytesReceived"] = *transport.bytes_received;
						}
						extra_json["transportId"] = transport.id();
					}
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

		private:
			template <typename MemberT>
			static void AppendIntegral(const char* label, const MemberT& member, std::ostringstream& oss)
			{
				if (member )
				{
					oss << label << '=' << static_cast<long long>(*member) << ' ';
				}
			}

			template <typename MemberT>
			static void AppendString(const char* label, const MemberT& member, std::ostringstream& oss)
			{
				if (member )
				{
					oss << label << '=' << std::quoted(*member) << ' ';
				}
			}

			template <typename MemberT>
			static void AppendBool(const char* label, const MemberT& member, std::ostringstream& oss)
			{
				if (member )
				{
					oss << label << '=' << (*member ? "true" : "false") << ' ';
				}
			}

			template <typename MemberT>
			static void AppendDouble(const char* label, const MemberT& member, double multiplier, const char* unit, std::ostringstream& oss)
			{
				if (!member )
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

			std::string stream_id_;
			bool upload_enabled_;
			crtc_publisher::StatsEndpoint endpoint_;
		};
	}
	class DummySetSessionDescriptionObserver
		: public webrtc::SetSessionDescriptionObserver {
	public:
		static DummySetSessionDescriptionObserver* Create() {
			return new webrtc::RefCountedObject<DummySetSessionDescriptionObserver>();
		}
		virtual void OnSuccess() { RTC_LOG(LS_INFO) << __FUNCTION__; }
		virtual void OnFailure(webrtc::RTCError error) {
			RTC_LOG(LS_INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
				<< error.message();
		}
	};
	//const char kAudioLabel[] = "";
	
	const char kAudioLabel[] = "audio_label";
	const char kVideoLabel[] = "video_label";
	const char kStreamId[] = "stream_id";
	static webrtc::scoped_refptr<DesktopTrackSource>   m_video_track_source_ptr;
	crtc_publisher::crtc_publisher(crtc_publisher::clistener * callback)
		: m_callback_ptr(callback)
	{
		stats_stream_id_ = GenerateStreamId();
		const char* endpoint_env = std::getenv("PUSH_STAT_ENDPOINT");
		if (endpoint_env && endpoint_env[0])
		{
			stats_endpoint_url_ = endpoint_env;
		}
		else
		{
			stats_endpoint_url_ = "http://127.0.0.1:8089/api/stats";
		}
		stats_endpoint_ready_ = ParseStatsEndpoint(stats_endpoint_url_, stats_endpoint_);
		if (!stats_endpoint_ready_)
		{
			WARNING_EX_LOG("Failed to parse stats endpoint url: %s", stats_endpoint_url_.c_str());
		}
	}

	bool crtc_publisher::create_offer()
	{

		if (!InitializePeerConnection())
		{
			WARNING_EX_LOG("[%s][%d] init peer connect failed !!!\n", __FUNCTION__, __LINE__);
			return false;
		}
		webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
		
		peer_connection_->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
		return true;
	}

	bool crtc_publisher::InitializePeerConnection()
	{
		  networkThread = webrtc::Thread::CreateWithSocketServer();
		   signalingThread = webrtc::Thread::Create();
		   workerThread = webrtc::Thread::Create();
		 
			  networkThread->SetName("network_thread", nullptr);
		  signalingThread->SetName("signaling_thread", nullptr);
		  workerThread->SetName("worker_thread", nullptr);

		 if ( ! networkThread->Start()  ||  ! signalingThread->Start()   || ! workerThread->Start() )
		{
			 
		}
 
		//std::unique_ptr<rtc::Thread>   work_thread = rtc::Thread::Create();
		peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
			networkThread.get() /* network_thread */,  
			workerThread.get() /* worker_thread */,
			signalingThread.get() /* signaling_thread */,
			nullptr /* default_adm */,
			webrtc::CreateBuiltinAudioEncoderFactory(),
			webrtc::CreateBuiltinAudioDecoderFactory(),
		//	CreateBuiltinExternalVideoEncoderFactory(),
			webrtc::CreateBuiltinVideoEncoderFactory(),
			webrtc::CreateBuiltinVideoDecoderFactory(), nullptr /* audio_mixer */,
			nullptr /* audio_processing */);

		if (!peer_connection_factory_)
		{
			/*main_wnd_->MessageBox("Error", "Failed to initialize PeerConnectionFactory",
				true);
			DeletePeerConnection();*/
			return false;
		}
 
		 
		// 设置SDP  ->马流是否加密哈DTLS
		if (!CreatePeerConnection(/*dtls=*/true))
		{
			/*main_wnd_->MessageBox("Error", "CreatePeerConnection failed", true);
			DeletePeerConnection();*/
		}

		//AddTracks();
		_add_tracks();

		return peer_connection_ != nullptr;
		//return true;
	}
	bool crtc_publisher::CreatePeerConnection(bool dtls)
	{

#if 0
		webrtc::PeerConnectionInterface::RTCConfiguration config;
		config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan; //这个 
		//config.enable_dtls_srtp = dtls; //是否加密
		//webrtc::PeerConnectionInterface::IceServer server;
		//server.uri = GetPeerConnectionString();
		//config.servers.push_back(server);
		printf("[%s][%d] config.sdp_semantics = %d\n", __FUNCTION__, __LINE__, config.sdp_semantics);

		peer_connection_ = peer_connection_factory_->CreatePeerConnectionOrError(config, this);
		printf("[%s][%d] fff  config.sdp_semantics = %d\n", __FUNCTION__, __LINE__, config.sdp_semantics);
#else 

		webrtc::PeerConnectionInterface::RTCConfiguration config;
		config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
		//webrtc::PeerConnectionInterface::IceServer server;
		//server.uri = GetPeerConnectionString();
		//config.servers.push_back(server);

		webrtc::PeerConnectionDependencies pc_dependencies(this);
		auto error_or_peer_connection =
			peer_connection_factory_->CreatePeerConnectionOrError(
				config, std::move(pc_dependencies));
		if (error_or_peer_connection.ok()) {
			peer_connection_ = std::move(error_or_peer_connection.value());
		}
#endif 
		return peer_connection_ != nullptr;
	}
	void crtc_publisher::set_remoter_description(std::string sdp)
	{
		
		// Replace message type from "offer" to "answer"
		std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
			webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp);
		peer_connection_->SetRemoteDescription(
			DummySetSessionDescriptionObserver::Create(),
			session_description.release());
	}
 
	void crtc_publisher::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
	{
		NORMAL_EX_LOG("OnSignalingChange new_state = %d", new_state);
		if (  (new_state == webrtc::PeerConnectionInterface::kClosed || new_state == webrtc::PeerConnectionInterface::kHaveRemotePrAnswer))
		{
			m_callback_ptr->connect_rtc_failed();
		}
		 
	}
	void crtc_publisher::OnAddTrack(webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
		const std::vector<webrtc::scoped_refptr<webrtc::MediaStreamInterface>>& streams)
	{
		     


	}


	void crtc_publisher::Destory()
	{
		StopStatsLogger();
		
		if (peer_connection_)
		{
			peer_connection_->StopRtcEventLog();
			 peer_connection_ = nullptr;
		}
		if (peer_connection_factory_)
		{
			peer_connection_factory_->StopAecDump();
			//peer_connection_factory_->Release();
			peer_connection_factory_ = nullptr;
		}
		if (m_video_track_source_ptr)
		{
			m_video_track_source_ptr->StopCapture();
			m_video_track_source_ptr = nullptr;
		}
		if (networkThread)
		{
			networkThread->Stop();
		}
		if (signalingThread)
		{
			signalingThread->Stop();
		}
		if (workerThread)  
		{
			workerThread->Stop();
		}


	}
	void crtc_publisher::OnRemoveTrack(webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
	{
	}
	 
	void crtc_publisher::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
	{
		NORMAL_EX_LOG("OnIceConnectionChange new_state = %d", new_state);
		if (    (new_state == webrtc::PeerConnectionInterface::kClosed || new_state == webrtc::PeerConnectionInterface::kHaveRemotePrAnswer))
		{
			m_callback_ptr->connect_rtc_failed();
		}
		 
	}
	void crtc_publisher::OnIceCandidate(const webrtc::IceCandidateInterface * candidate)
	{
	}
	
	void crtc_publisher::OnCapture(bool enable)
	{
		NORMAL_EX_LOG("==================enable=%u====>>>>", enable);
	}
	void crtc_publisher::OnSuccess(webrtc::SessionDescriptionInterface * desc)
	{
		// 得到本地视频基本信息 先设置本地 SDP 鸭
		peer_connection_->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), desc);

		std::string sdp;
		desc->ToString(&sdp);
		NORMAL_EX_LOG("[%s][%d][][sdp = %s] ", __FUNCTION__, __LINE__, sdp.c_str());
		if (m_callback_ptr)
		{
			m_callback_ptr->send_create_offer_sdp(sdp);
		}
	}
	void crtc_publisher::OnFailure(webrtc::RTCError error)
	{
		WARNING_EX_LOG("[error = %s]", error.message());
		m_callback_ptr->connect_rtc_failed();
	}
	void crtc_publisher::OnFailure(const std::string & error)
	{
		WARNING_EX_LOG("[error = %s]", error.c_str());
		m_callback_ptr->connect_rtc_failed();
	}

	void crtc_publisher::_add_tracks()
	{
		if (!peer_connection_->GetSenders().empty())
		{
			return;  // Already added tracks.
		}
#if 1
		webrtc::DataChannelInit dataChannelInit;
		dataChannelInit.ordered = true;
		dataChannelInit.protocol = "UDP";
		dataChannelInit.negotiated = false;
		dataChannelInit.id = 0;
		webrtc::scoped_refptr<webrtc::DataChannelInterface> webrtcDataChannel  = peer_connection_->CreateDataChannel("inputdevice", &dataChannelInit);
		if (!webrtcDataChannel.get())
		{
			WARNING_EX_LOG("create data channel failed !!!");
			 
		}
		m_data_channel_ptr = new webrtc::RefCountedObject<cdata_channel>(webrtcDataChannel);
#endif //
		///////////////////////////////////////////////AUDIO///////////////////////////////////////////////////////////
		webrtc::scoped_refptr<webrtc::AudioSourceInterface> audio_source_ptr = peer_connection_factory_->CreateAudioSource(webrtc::AudioOptions());

		webrtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track_ptr = peer_connection_factory_->CreateAudioTrack(kAudioLabel, audio_source_ptr.get());

		auto result_or_error = peer_connection_->AddTrack(audio_track_ptr, { kStreamId });
		if (!result_or_error.ok())
		{
			RTC_LOG(LS_ERROR) << "Failed to add audio track to PeerConnection: "
				<< result_or_error.error().message();
		}
		//////////////////////////////////////////VIDEO////////////////////////////////////////////////////////////////
		 m_video_track_source_ptr =  (DesktopTrackSource::Create());
		if (m_video_track_source_ptr)
		{ 

			webrtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_proxy_ptr = peer_connection_factory_->CreateVideoTrack(m_video_track_source_ptr, kVideoLabel);
			
			 

			 webrtc::RtpTransceiverInit transceiver_init;
			 

			webrtc::RTCErrorOr<webrtc::scoped_refptr<webrtc::RtpTransceiverInterface>> result = peer_connection_->AddTransceiver(video_track_proxy_ptr, transceiver_init);

			if (!result_or_error.ok())
			{
				RTC_LOG(LS_ERROR) << "Failed to add video track to PeerConnection: "
					<< result_or_error.error().message();
			}

		}
		else {
			RTC_LOG(LS_ERROR) << "OpenVideoCaptureDevice failed";
		}

		StartStatsLogger();
	}

	void crtc_publisher::StartStatsLogger()
	{
		if (stats_running_)
		{
			return;
		}
		stats_running_ = true;
		stats_thread_ = std::make_unique<std::thread>([this]() {
			while (stats_running_)
			{
				auto pc = peer_connection_;
				if (pc)
				{
					webrtc::scoped_refptr<StatsLogger> callback =
						webrtc::make_ref_counted<StatsLogger>(
							stats_stream_id_,
							stats_endpoint_ready_,
							stats_endpoint_);
					pc->GetStats(callback.get());
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			});
	}

	void crtc_publisher::StopStatsLogger()
	{
		if (!stats_running_)
		{
			return;
		}
		stats_running_ = false;
		if (stats_thread_ && stats_thread_->joinable())
		{
			stats_thread_->join();
		}
		stats_thread_.reset();
	}

}