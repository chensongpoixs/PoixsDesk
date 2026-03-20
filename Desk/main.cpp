/***********************************************************************************************
created:        2026-03-19

author:         chensong

purpose:        Desk 主程序入口（在用户会话中运行）

说明：
    本程序由 DeskServiceCRTCSvc 服务在用户会话中启动，负责执行实际的业务逻辑：
    - 设备注册和心跳
    - RTC 推流（桌面捕获）
    - 与服务器通信
    
    与服务的关系：
    - DeskServiceCRTCSvc (SYSTEM - Session 0) 负责监控和启动
    - Desk.exe (用户会话 - Session 1/2/3) 负责业务逻辑
    
    这种架构解决了 SYSTEM 账户无法访问用户桌面的问题。
************************************************************************************************/

#include <windows.h>
#include <iostream>
#include <csignal>
#include <atomic>
#include <ctime>
#include "DeskLogic.h"
#include "clog.h"

using namespace chen;

// 全局停止标志
static std::atomic<bool> g_stop_requested(false);

// Ctrl-C 信号处理器
void SignalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        NORMAL_EX_LOG("Received stop signal, shutting down gracefully...");
        g_stop_requested = true;
    }
}

// 控制台事件处理器（用于处理服务发送的 Ctrl-C）
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
        NORMAL_EX_LOG("Received console control event, shutting down...");
        g_stop_requested = true;
        return TRUE;
    default:
        return FALSE;
    }
}

int main(int argc, char* argv[])
{
    // ========== 启动诊断日志 ==========
    // 立即写入启动标记，用于诊断
    FILE* startup_log = fopen("C:\\Windows\\Temp\\desk_startup01.log", "wb+");
    if (startup_log)
    {
        time_t now = time(NULL);
        fprintf(startup_log, "\n========================================\n");
        fprintf(startup_log, "[%s] Desk.exe starting...\n", ctime(&now));
        fprintf(startup_log, "argc: %d\n", argc);
        for (int i = 0; i < argc; i++)
        {
            fprintf(startup_log, "argv[%d]: %s\n", i, argv[i]);
        }
        
        // 获取当前进程信息
        DWORD pid = GetCurrentProcessId();
        DWORD session_id = 0;
        ProcessIdToSessionId(pid, &session_id);
        fprintf(startup_log, "Process ID: %lu\n", pid);
        fprintf(startup_log, "Session ID: %lu\n", session_id);
        
        fflush(startup_log);
        fclose(startup_log);
    }
    
    // ========== 初始化日志系统 ==========
    // 使用仅文件日志（不输出到屏幕，因为服务启动的进程没有控制台）
    if (!clog::init(chen::ELogStorageFile))
    {
        // 日志初始化失败，写入事件日志
        FILE* error_log = fopen("C:\\Windows\\Temp\\desk_startup02.log", "wb+");
        if (error_log)
        {
            fprintf(error_log, "[ERROR] Failed to initialize clog system\n");
            fflush(error_log);
            fclose(error_log);
        }
        
        // 写入 Windows 事件日志
        HANDLE hEventLog = RegisterEventSourceA(NULL, "Desk");
        if (hEventLog)
        {
            const char* msg = "Desk.exe: Failed to initialize log system";
            ReportEventA(hEventLog, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 1, 0, &msg, NULL);
            DeregisterEventSource(hEventLog);
        }
        
        return 1;
    }
    
    NORMAL_EX_LOG("========================================");
    NORMAL_EX_LOG("Desk Application Starting...");
    NORMAL_EX_LOG("========================================");
    
    // 获取会话信息
    DWORD session_id = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &session_id))
    {
        NORMAL_EX_LOG("Running in Session ID: %lu", session_id);
        if (session_id == 0)
        {
            WARNING_EX_LOG("WARNING: Running in Session 0 (SYSTEM account)");
            WARNING_EX_LOG("Desktop capture may not work properly!");
        }
        else
        {
            NORMAL_EX_LOG("Running in user session (Session %lu) - OK", session_id);
        }
    }
    
    // 注册信号处理器
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
    
    // 创建并初始化业务逻辑
    DeskLogic desk_logic;
    
    NORMAL_EX_LOG("Initializing DeskLogic...");
    
    // 添加异常捕获
    try
    {
        if (!desk_logic.init())
        {
            ERROR_EX_LOG("Failed to initialize DeskLogic");
            clog::destroy();
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        ERROR_EX_LOG("Exception during DeskLogic initialization: %s", e.what());
        clog::destroy();
        return 1;
    }
    catch (...)
    {
        ERROR_EX_LOG("Unknown exception during DeskLogic initialization");
        clog::destroy();
        return 1;
    }
    
    NORMAL_EX_LOG("DeskLogic initialized successfully");
    NORMAL_EX_LOG("Desk is now running. Press Ctrl+C to stop.");
    
    // 主循环：等待停止信号
    while (!g_stop_requested)
    {
        Sleep(1000);
    }
    
    NORMAL_EX_LOG("========================================");
    NORMAL_EX_LOG("Desk Application Shutting Down...");
    NORMAL_EX_LOG("========================================");
    
    // 停止业务逻辑
    desk_logic.stop();
    desk_logic.destory();
    
    NORMAL_EX_LOG("DeskLogic stopped");
    
    // 清理日志系统
    clog::destroy();
    
    NORMAL_EX_LOG("Desk Application Stopped");
    
    // 如果是被服务请求关闭，返回特殊退出码
    // 这样服务就知道是正常关闭，而不是崩溃
    return 0;
}
