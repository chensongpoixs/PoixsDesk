

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
#				#	-D_UNICODE
#				#	-DUNICODE
#				#	-D_AFXDLL
#)


add_definitions(-D_AFXDLL


# curl  libsrtp  opus 
-DHAVE_CONFIG_H


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



libcross_platform_collection_render
		 
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
			
obj/pc/audio_rtp_receiver.lib
obj/pc/audio_track.lib
obj/pc/channel.lib
obj/pc/codec_vendor.lib
obj/pc/connection_context.lib
obj/pc/data_channel_controller.lib
obj/pc/data_channel_utils.lib
obj/pc/dtls_srtp_transport.lib
obj/pc/dtls_transport.lib
obj/pc/dtmf_sender.lib
obj/pc/external_hmac.lib
obj/pc/ice_server_parsing.lib
obj/pc/ice_transport.lib
obj/pc/jitter_buffer_delay.lib
obj/pc/jsep_transport.lib
obj/pc/jsep_transport_collection.lib
obj/pc/jsep_transport_controller.lib
obj/pc/legacy_stats_collector.lib
obj/pc/local_audio_source.lib
obj/pc/media_options.lib
obj/pc/media_protocol_names.lib
obj/pc/media_session.lib
obj/pc/media_stream.lib
obj/pc/media_stream_observer.lib
obj/pc/peer_connection.lib
obj/pc/peer_connection_factory.lib
obj/pc/peer_connection_message_handler.lib
obj/pc/remote_audio_source.lib
obj/pc/rtcp_mux_filter.lib
obj/pc/rtc_stats_collector.lib
obj/pc/rtc_stats_traversal.lib
obj/pc/rtp_media_utils.lib
obj/pc/rtp_parameters_conversion.lib
obj/pc/rtp_receiver.lib
obj/pc/rtp_sender.lib
obj/pc/rtp_transceiver.lib
obj/pc/rtp_transmission_manager.lib
obj/pc/rtp_transport.lib
obj/pc/sctp_data_channel.lib
obj/pc/sctp_transport.lib
obj/pc/sctp_utils.lib
obj/pc/sdp_munging_detector.lib
obj/pc/sdp_offer_answer.lib
obj/pc/session_description.lib
obj/pc/simulcast_description.lib
obj/pc/simulcast_sdp_serializer.lib
obj/pc/srtp_session.lib
obj/pc/srtp_transport.lib
obj/pc/track_media_info_map.lib
obj/pc/transceiver_list.lib
obj/pc/transport_stats.lib
obj/pc/typed_codec_vendor.lib
obj/pc/usage_pattern.lib
obj/pc/video_rtp_receiver.lib
obj/pc/video_rtp_track_source.lib
obj/pc/video_track.lib
obj/pc/video_track_source.lib
obj/pc/video_track_source_proxy.lib
obj/pc/webrtc_sdp.lib
obj/pc/webrtc_session_description_factory.lib
obj/media/adapted_video_track_source.lib
obj/media/codec.lib
obj/media/codec_list.lib
obj/media/media_channel_impl.lib
obj/media/media_constants.lib
obj/media/media_engine.lib
obj/media/rid_description.lib
obj/media/rtc_audio_video.lib
obj/media/rtc_data_dcsctp_transport.lib
obj/media/rtc_data_sctp_transport_factory.lib
obj/media/rtc_internal_video_codecs.lib
obj/media/rtc_sdp_video_format_utils.lib
obj/media/rtc_simulcast_encoder_adapter.lib
obj/media/rtp_utils.lib
obj/media/stream_params.lib
obj/media/turn_utils.lib
obj/media/video_adapter.lib
obj/media/video_broadcaster.lib
obj/media/video_common.lib
obj/media/video_source_base.lib
obj/api/video_codecs/builtin_video_decoder_factory.lib
obj/api/video_codecs/builtin_video_encoder_factory.lib
obj/api/video_codecs/libaom_av1_encoder_factory.lib
obj/api/video_codecs/rtc_software_fallback_wrappers.lib
obj/api/video_codecs/scalability_mode.lib
obj/api/video_codecs/scalability_mode_helper.lib
obj/api/video_codecs/simple_encoder_wrapper.lib
obj/api/video_codecs/video_codecs_api.lib
obj/api/video_codecs/vp8_temporal_layers_factory.lib
obj/rtc_base/async_dns_resolver.lib
obj/rtc_base/async_packet_socket.lib
obj/rtc_base/async_socket.lib
obj/rtc_base/async_tcp_socket.lib
obj/rtc_base/async_udp_socket.lib
obj/rtc_base/base64.lib
obj/rtc_base/bitrate_tracker.lib
obj/rtc_base/bitstream_reader.lib
obj/rtc_base/bit_buffer.lib
obj/rtc_base/buffer_queue.lib
obj/rtc_base/byte_buffer.lib
obj/rtc_base/callback_list.lib
obj/rtc_base/checks.lib
obj/rtc_base/copy_on_write_buffer.lib
obj/rtc_base/cpu_info.lib
obj/rtc_base/crc32.lib
obj/rtc_base/criticalsection.lib
obj/rtc_base/crypto_random.lib
obj/rtc_base/denormal_disabler.lib
obj/rtc_base/digest.lib
obj/rtc_base/event_tracer.lib
obj/rtc_base/frequency_tracker.lib
obj/rtc_base/histogram_percentile_counter.lib
obj/rtc_base/ip_address.lib
obj/rtc_base/logging.lib
obj/rtc_base/network.lib
obj/rtc_base/network_constants.lib
obj/rtc_base/network_route.lib
obj/rtc_base/net_helper.lib
obj/rtc_base/net_helpers.lib
obj/rtc_base/null_socket_server.lib
obj/rtc_base/platform_thread.lib
obj/rtc_base/platform_thread_types.lib
obj/rtc_base/race_checker.lib
obj/rtc_base/random.lib
obj/rtc_base/rate_limiter.lib
obj/rtc_base/rate_statistics.lib
obj/rtc_base/rate_tracker.lib
obj/rtc_base/rtc_certificate_generator.lib
obj/rtc_base/rtc_event.lib
obj/rtc_base/rtc_numerics.lib
obj/rtc_base/rtc_operations_chain.lib
obj/rtc_base/rtc_task_queue_win.lib
obj/rtc_base/rtp_to_ntp_estimator.lib
obj/rtc_base/sample_counter.lib
obj/rtc_base/socket.lib
obj/rtc_base/socket_adapters.lib
obj/rtc_base/socket_address.lib
obj/rtc_base/ssl.lib
obj/rtc_base/ssl_adapter.lib
obj/rtc_base/stream.lib
obj/rtc_base/stringutils.lib
obj/rtc_base/threading.lib
obj/rtc_base/timestamp_aligner.lib
obj/rtc_base/timeutils.lib
obj/rtc_base/unique_id_generator.lib
obj/rtc_base/weak_ptr.lib
obj/rtc_base/win32.lib
obj/rtc_base/zero_memory.lib
obj/modules/desktop_capture/desktop_capture.lib
obj/modules/desktop_capture/desktop_capture_differ_sse2.lib
obj/modules/desktop_capture/primitives.lib
obj/webrtc.lib
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
set_property(TARGET ${PROJECT_NAME}  				PROPERTY FOLDER LiveRtcDemo) 
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})