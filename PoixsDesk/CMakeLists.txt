

project(PoixsDesk)
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


add_definitions(
#-D_AFXDLL

 	#-D_UNICODE
 	#-DUNICODE
# curl  libsrtp  opus 
 

)

include_directories(
	.
	#../
	##../libcommon
	##../libdevice/window
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


)





add_executable(${PROJECT_NAME} ${all_src})
#add_dependencies(${PROJECT_NAME} libcommon)
#add_dependencies(${PROJECT_NAME} libdevice)
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


		 
								
			#libcommon
			#video_coding_lib


			# #####
			
 

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
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE /SAFESEH:NO")
#target_link_options(${PROJECT_NAME} PRIVATE
#		/ENTRY:wWinMainCRTStartup
#)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

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