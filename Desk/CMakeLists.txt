cmake_minimum_required(VERSION 3.15)

project(Desk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libcrtc
)

# 链接目录
link_directories(
    ${CMAKE_BINARY_DIR}/libcrtc/Debug
    ${CMAKE_BINARY_DIR}/libcrtc/Release
)

# Desk 可执行程序（控制台程序，用于在用户会话中运行）
add_executable(Desk
    main.cpp
    DeskLogic.h
    DeskLogic.cpp
    
    # 日志系统
    casync_log.h
    casync_log.cpp
    clog.h
    clog.cpp
    clog_define.h
    
    # 网络和时间工具
    cnet_types.h
    cnet_types.cpp
    ctime_api.h
    ctime_api.cpp
    ctime_const.h
    cutil.h
    cutil.cpp
    csingleton.h
    
    # 硬件信息
    HardwareInfo.h
    HardwareInfo.cpp
    
    # 注册表和字符串工具
    RegistryHelper.h
    RegistryHelper.cpp
    StringHelper.h
    StringHelper.cpp
    
    # JSON 和 HTTP
    json.hpp
    httplib.h
    pipe_server.h
    pipe_server.cpp 
    pipe_client.h
    pipe_client.cpp 
)

# 链接库
target_link_libraries(Desk
    libcrtc.lib
    ws2_32.lib
    wininet.lib
)

if (MSVC)
    target_compile_options(Desk PRIVATE /W4)
    
    # 设置为控制台程序
    set_target_properties(Desk PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
else()
    target_compile_options(Desk PRIVATE -Wall -Wextra -Wpedantic)
endif()
