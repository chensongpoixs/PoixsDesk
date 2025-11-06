

project(Win32LiveRtcDemo)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS  "####################################")
MESSAGE(STATUS  "#############${PROJECT_NAME}###############")
MESSAGE(STATUS  "####################################")

#add_definitions(
#-DHAVE_STRING_H
#-D__ICL
#-DWIN32
#-DHIGH_BIT_DEPTH=0
#-DBIT_DEPTH=8
#-D_WIN32_WINNT=0x0600
#-D_DEBUG
#				#	-DWIN32
#				#	-D_DEBUG
#				##	-D_WINDOWS

#				#	-D_AFXDLL
#)


add_definitions(-D_AFXDLL

 	#-D_UNICODE
 	#-DUNICODE
# curl  libsrtp  opus 
 

)

include_directories(
	.
	#../
	../libcommon
	#../../
	#../../libwebrtc
	#../../deps/libyuv/include
	#../../deps/abseil-cpp/
	#../../deps/libcurl/include
	#../../deps/jsoncpp/source/include
	#../../libice 
)

link_directories( 
.
D:/Work/webrtc_google/src/out/vs2022_debug_chensong
)


#

file(GLOB all_src
	*.h
	*.cpp 
	LiveWin32.rc 


)





add_executable(${PROJECT_NAME} ${all_src})
add_dependencies(${PROJECT_NAME} libcommon)
target_link_libraries(${PROJECT_NAME} 



 
		 
		ole32.lib
        oleaut32.lib
        strmiids.lib
        user32.lib
		 Secur32.lib
		 
		Ws2_32.lib
		Wldap32.lib
		# api
		 
	 

		 

		d3d11.lib
      dxgi.lib
		

		 

			Imm32 
			comctl32
			ws2_32.lib
			winmm.lib


		 
								
			libcommon
			#video_coding_lib


			# #####
			
obj/pc/audio_rtp_receiver 
obj/pc/audio_track 
obj/pc/channel 
obj/pc/codec_vendor 
obj/pc/connection_context 
obj/pc/data_channel_controller 
obj/pc/data_channel_utils 
obj/pc/dtls_srtp_transport 
obj/pc/dtls_transport 
obj/pc/dtmf_sender 
obj/pc/external_hmac 
obj/pc/ice_server_parsing 
obj/pc/ice_transport 
obj/pc/jitter_buffer_delay 
obj/pc/jsep_transport 
obj/pc/jsep_transport_collection 
obj/pc/jsep_transport_controller 
obj/pc/legacy_stats_collector 
obj/pc/local_audio_source 
obj/pc/media_options 
obj/pc/media_protocol_names 
obj/pc/media_session 
obj/pc/media_stream 
obj/pc/media_stream_observer 
obj/pc/peer_connection 
obj/pc/peer_connection_factory 
obj/pc/peer_connection_message_handler 
obj/pc/remote_audio_source 
obj/pc/rtcp_mux_filter 
obj/pc/rtc_stats_collector 
obj/pc/rtc_stats_traversal
obj/pc/rtp_media_utils
obj/pc/rtp_parameters_conversion
obj/pc/rtp_receiver
obj/pc/rtp_sender
obj/pc/rtp_transceiver
obj/pc/rtp_transmission_manager
obj/pc/rtp_transport
obj/pc/sctp_data_channel
obj/pc/sctp_transport
obj/pc/sctp_utils
obj/pc/sdp_munging_detector
obj/pc/sdp_offer_answer
obj/pc/session_description
obj/pc/simulcast_description
obj/pc/simulcast_sdp_serializer
obj/pc/srtp_session
obj/pc/srtp_transport
obj/pc/track_media_info_map
obj/pc/transceiver_list
obj/pc/transport_stats
obj/pc/typed_codec_vendor
obj/pc/usage_pattern
obj/pc/video_rtp_receiver
obj/pc/video_rtp_track_source
obj/pc/video_track
obj/pc/video_track_source
obj/pc/video_track_source_proxy
obj/pc/webrtc_sdp
obj/pc/webrtc_session_description_factory
obj/media/adapted_video_track_source
obj/media/codec
obj/media/codec_list
obj/media/media_channel_impl
obj/media/media_constants
obj/media/media_engine
obj/media/rid_description
obj/media/rtc_audio_video
obj/media/rtc_data_dcsctp_transport
obj/media/rtc_data_sctp_transport_factory
obj/media/rtc_internal_video_codecs
obj/media/rtc_sdp_video_format_utils
obj/media/rtc_simulcast_encoder_adapter
obj/media/rtp_utils
obj/media/stream_params
obj/media/turn_utils
obj/media/video_adapter
obj/media/video_broadcaster
obj/media/video_common
obj/media/video_source_base
obj/api/video_codecs/builtin_video_decoder_factory
obj/api/video_codecs/builtin_video_encoder_factory
obj/api/video_codecs/libaom_av1_encoder_factory
obj/api/video_codecs/rtc_software_fallback_wrappers
obj/api/video_codecs/scalability_mode
obj/api/video_codecs/scalability_mode_helper
obj/api/video_codecs/simple_encoder_wrapper
obj/api/video_codecs/video_codecs_api
obj/api/video_codecs/vp8_temporal_layers_factory
obj/rtc_base/async_dns_resolver
obj/rtc_base/async_packet_socket
obj/rtc_base/async_socket
obj/rtc_base/async_tcp_socket
obj/rtc_base/async_udp_socket
obj/rtc_base/base64
obj/rtc_base/bitrate_tracker
obj/rtc_base/bitstream_reader
obj/rtc_base/bit_buffer
obj/rtc_base/buffer_queue
obj/rtc_base/byte_buffer
obj/rtc_base/callback_list
obj/rtc_base/checks
obj/rtc_base/copy_on_write_buffer
obj/rtc_base/cpu_info
obj/rtc_base/crc32
obj/rtc_base/criticalsection
obj/rtc_base/crypto_random
obj/rtc_base/denormal_disabler
obj/rtc_base/digest
obj/rtc_base/event_tracer
obj/rtc_base/frequency_tracker
obj/rtc_base/histogram_percentile_counter
obj/rtc_base/ip_address
obj/rtc_base/logging
obj/rtc_base/network
obj/rtc_base/network_constants
obj/rtc_base/network_route
obj/rtc_base/net_helper
obj/rtc_base/net_helpers
obj/rtc_base/null_socket_server
obj/rtc_base/platform_thread
obj/rtc_base/platform_thread_types
obj/rtc_base/race_checker
obj/rtc_base/random
obj/rtc_base/rate_limiter
obj/rtc_base/rate_statistics
obj/rtc_base/rate_tracker
obj/rtc_base/rtc_certificate_generator
obj/rtc_base/rtc_event
obj/rtc_base/rtc_numerics
obj/rtc_base/rtc_operations_chain
obj/rtc_base/rtc_task_queue_win
obj/rtc_base/rtp_to_ntp_estimator
obj/rtc_base/sample_counter
obj/rtc_base/socket
obj/rtc_base/socket_adapters
obj/rtc_base/socket_address
obj/rtc_base/ssl
obj/rtc_base/ssl_adapter
obj/rtc_base/stream
obj/rtc_base/stringutils
obj/rtc_base/threading
obj/rtc_base/timestamp_aligner
obj/rtc_base/timeutils
obj/rtc_base/unique_id_generator
obj/rtc_base/weak_ptr
obj/rtc_base/win32
obj/rtc_base/zero_memory
obj/modules/desktop_capture/desktop_capture
obj/modules/desktop_capture/desktop_capture_differ_sse2
obj/modules/desktop_capture/primitives
obj/webrtc

d3d11.lib
dxgi.lib
shcore.lib
advapi32.lib
comdlg32.lib
dbghelp.lib
dnsapi.lib
gdi32.lib
msimg32.lib
odbc32.lib
odbccp32.lib
oleaut32.lib
shell32.lib
shlwapi.lib
user32.lib
usp10.lib
uuid.lib
version.lib
wininet.lib
winmm.lib
winspool.lib
ws2_32.lib
delayimp.lib
kernel32.lib
ole32.lib
crypt32.lib
iphlpapi.lib
secur32.lib
dmoguids.lib
wmcodecdspuuid.lib
amstrmid.lib
msdmo.lib
bcrypt.lib
strmiids.lib
dwmapi.lib
			)


			#RTC_LOG_F(LS_INFO) << rtcp_block.ToString();
			#Windows (/SUBSYSTEM:WINDOWS)
			# Console (/SUBSYSTEM:CONSOLE)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")
#target_link_options(${PROJECT_NAME} PRIVATE
#		/ENTRY:wWinMainCRTStartup
#)

set(AllFile ${all_src})
foreach(fileItem ${AllFile})       
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${fileItem}" DIRECTORY)
	
    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
	#message(status   "---> fileItem = ${fileItem}     GROUP ---->  ${GROUP}")
    # Group into "Source Files" and "Header Files"
    set(GROUP "${GROUP}")
    source_group("${GROUP}" FILES "${fileItem}")
endforeach()
#set_property(TARGET ${PROJECT_NAME}  				PROPERTY FOLDER LiveRtcDemo) 
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})