/***********************************************************************************************
created: 		2022-01-19

author:			chensong

purpose:		input_device
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



for %%I in ("%~dp0") do set "ROOT_DIR=%%~fI"
ME=PoixsDeskService
set "SERVICE_BIN=%ROOT_DIR%PoisxDeskService.exe"


sc create MyWin32Service binPath= "\"%SERVICE_BIN%\""

sc start MyWin32Service

************************************************************************************************/
#if 1
#define WIN32_LEAN_AND_MEAN
#include <string>
#include <Windows.h>
#include <wtsapi32.h>

// PROC_THREAD_ATTRIBUTE_JOB_LIST is currently missing from MinGW headers
#ifndef PROC_THREAD_ATTRIBUTE_JOB_LIST
  #define PROC_THREAD_ATTRIBUTE_JOB_LIST ProcThreadAttributeValue(13, FALSE, TRUE, FALSE)
#endif

/**
*  @author chensong
*  @date 2022-01-19
*  @brief 服务状态句柄（Service Status Handle）
*  
*  该全局变量用于存储Windows服务控制管理器（SCM）返回的服务状态句柄。
*  服务状态句柄用于与服务控制管理器通信，报告服务状态。
*  
*  服务状态句柄说明：
*  - SERVICE_STATUS_HANDLE是一个不透明句柄，由RegisterServiceCtrlHandlerEx返回
*  - 用于调用SetServiceStatus报告服务状态给SCM
*  - 在服务停止后失效
*  
*  @note 初始化为NULL，在ServiceMain中通过RegisterServiceCtrlHandlerEx获取
*  @note 服务状态句柄用于与SCM通信
*/
SERVICE_STATUS_HANDLE service_status_handle;

/**
*  @author chensong
*  @date 2022-01-19
*  @brief 服务状态结构（Service Status Structure）
*  
*  该全局变量用于存储Windows服务的当前状态信息。服务状态结构包含
*  服务的类型、当前状态、控制接受标志等信息。
*  
*  服务状态结构（SERVICE_STATUS Structure）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  dwServiceType                                                 |
*   |  (32 bits: SERVICE_WIN32_OWN_PROCESS)                           |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  dwCurrentState                                                |
*   |  (32 bits: SERVICE_START_PENDING/RUNNING/STOP_PENDING/STOPPED) |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  dwControlsAccepted                                            |
*   |  (32 bits: SERVICE_ACCEPT_STOP/PRESHUTDOWN/SESSIONCHANGE)      |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  dwWin32ExitCode                                               |
*   |  (32 bits: NO_ERROR or error code)                              |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  dwServiceSpecificExitCode                                     |
*   |  (32 bits: service-specific exit code)                          |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  dwCheckPoint                                                  |
*   |  (32 bits: checkpoint for progress reporting)                   |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  dwWaitHint                                                    |
*   |  (32 bits: estimated time in milliseconds)                      |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  服务状态说明：
*  - dwServiceType: 服务类型，SERVICE_WIN32_OWN_PROCESS表示独立进程服务
*  - dwCurrentState: 当前状态（START_PENDING/RUNNING/STOP_PENDING/STOPPED）
*  - dwControlsAccepted: 接受的控制命令（STOP/PRESHUTDOWN/SESSIONCHANGE）
*  - dwWin32ExitCode: Win32错误代码，NO_ERROR表示成功
*  - dwServiceSpecificExitCode: 服务特定的退出代码
*  - dwCheckPoint: 检查点，用于报告进度
*  - dwWaitHint: 等待提示，估计的等待时间（毫秒）
*  
*  @note 服务状态结构在ServiceMain中初始化，通过SetServiceStatus报告给SCM
*  @note 服务状态会根据服务生命周期自动更新
*/
SERVICE_STATUS service_status;

/**
*  @author chensong
*  @date 2022-01-19
*  @brief 停止事件句柄（Stop Event Handle）
*  
*  该全局变量用于存储服务停止事件的句柄。停止事件是一个手动重置事件，
*  用于通知服务主循环停止运行。
*  
*  事件句柄说明：
*  - HANDLE是Windows事件对象的句柄
*  - 停止事件用于通知服务主循环停止运行
*  - 当收到停止命令时，通过SetEvent触发事件
*  
*  停止事件流程（Stop Event Flow）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  1. Service receives SERVICE_CONTROL_STOP                      |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  2. HandlerEx() sets service_status.dwCurrentState =           |
*   |     SERVICE_STOP_PENDING                                        |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  3. HandlerEx() calls SetEvent(stop_event)                      |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  4. ServiceMain() loop detects stop_event is set                |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  5. ServiceMain() exits loop and sets status to SERVICE_STOPPED|
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  @note 停止事件在ServiceMain中创建，在HandlerEx中触发，在主循环中检测
*  @note 停止事件是手动重置事件（manual-reset event）
*/
HANDLE stop_event;

/**
*  @author chensong
*  @date 2022-01-19
*  @brief 会话变更事件句柄（Session Change Event Handle）
*  
*  该全局变量用于存储会话变更事件的句柄。会话变更事件是一个自动重置事件，
*  用于通知服务主循环控制台会话已变更。
*  
*  会话变更事件说明：
*  - HANDLE是Windows事件对象的句柄
*  - 会话变更事件用于通知控制台会话已变更
*  - 当新的控制台会话连接时，需要重启PoixsDesk.exe
*  
*  会话变更事件流程（Session Change Event Flow）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  1. New console session connects (WTS_CONSOLE_CONNECT)         |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  2. SCM sends SERVICE_CONTROL_SESSIONCHANGE to service         |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  3. HandlerEx() checks dwEventType == WTS_CONSOLE_CONNECT      |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  4. HandlerEx() calls SetEvent(session_change_event)           |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  5. ServiceMain() loop detects session_change_event is set     |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  6. ServiceMain() terminates PoixsDesk.exe and restarts it     |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  @note 会话变更事件在ServiceMain中创建，在HandlerEx中触发，在主循环中检测
*  @note 会话变更事件是自动重置事件（auto-reset event）
*/
HANDLE session_change_event;

/**
*  @author chensong
*  @date 2022-01-19
*  @brief 服务名称定义（Service Name Definition）
*  
*  该宏定义用于存储Windows服务的名称。服务名称用于在SCM中注册和标识服务。
*  
*  服务名称说明：
*  - 服务名称用于在SCM中注册服务
*  - 服务名称用于与服务控制管理器通信
*  - 服务名称必须唯一
*  
*  @note 服务名称用于RegisterServiceCtrlHandlerEx和SERVICE_TABLE_ENTRY
*/
#define SERVICE_NAME "PoixsDeskService"

/**
*  @author chensong
*  @date 2022-01-19
*  @brief 服务控制处理器（Service Control Handler）
*  
*  该函数用于处理来自Windows服务控制管理器（SCM）的控制命令。
*  当SCM发送控制命令（如停止、暂停、会话变更等）时，会调用此函数。
*  
*  服务控制命令格式（Service Control Command Format）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  dwControl                                                     |
*   |  (32 bits: control command code)                               |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  dwEventType (for SERVICE_CONTROL_SESSIONCHANGE)               |
*   |  (32 bits: WTS_CONSOLE_CONNECT/DISCONNECT/LOCK/UNLOCK)         |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  lpEventData (for SERVICE_CONTROL_SESSIONCHANGE)               |
*   |  (pointer to WTSSESSION_NOTIFICATION structure)                |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  控制命令类型（Control Command Types）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  SERVICE_CONTROL_INTERROGATE (128)                             |
*   |  - SCM queries current service status                          |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  SERVICE_CONTROL_STOP (1)                                      |
*   |  - SCM requests service to stop                                |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  SERVICE_CONTROL_PRESHUTDOWN (14)                              |
*   |  - System is shutting down                                     |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  SERVICE_CONTROL_SESSIONCHANGE (11)                            |
*   |  - Console session has changed                                 |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  处理流程（Handling Flow）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  1. Receive control command from SCM                            |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  2. Check dwControl command type                                |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  3. For SERVICE_CONTROL_STOP/PRESHUTDOWN:                       |
*   |     - Set service_status.dwCurrentState = SERVICE_STOP_PENDING  |
*   |     - Set service_status.dwWaitHint = 30000 (30 seconds)        |
*   |     - Call SetServiceStatus() to report status                  |
*   |     - Set stop_event to trigger ServiceMain() loop exit        |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  4. For SERVICE_CONTROL_SESSIONCHANGE:                          |
*   |     - Check dwEventType == WTS_CONSOLE_CONNECT                  |
*   |     - Set session_change_event to trigger restart               |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  5. Return NO_ERROR to indicate success                         |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  @param dwControl 控制命令代码，指定SCM请求的操作类型
*  @param dwEventType 事件类型（仅用于SERVICE_CONTROL_SESSIONCHANGE），
*                     如WTS_CONSOLE_CONNECT、WTS_CONSOLE_DISCONNECT等
*  @param lpEventData 事件数据指针（仅用于SERVICE_CONTROL_SESSIONCHANGE），
*                     指向WTSSESSION_NOTIFICATION结构
*  @param lpContext 上下文指针，由RegisterServiceCtrlHandlerEx传入，未使用
*  @return 返回NO_ERROR表示成功，其他值表示错误
*  @note 该函数由SCM在单独的线程中调用，需要线程安全
*  @note SERVICE_CONTROL_STOP会触发服务停止流程
*  @note SERVICE_CONTROL_SESSIONCHANGE会触发PoixsDesk.exe重启
*  
*  使用示例：
*  @code
*  // 在ServiceMain中注册
*  service_status_handle = RegisterServiceCtrlHandlerEx(SERVICE_NAME, HandlerEx, NULL);
*  @endcode
*/
DWORD WINAPI HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) {
  switch (dwControl) {
    case SERVICE_CONTROL_INTERROGATE:
      return NO_ERROR;

    case SERVICE_CONTROL_SESSIONCHANGE:
      /**
      *  @brief 处理会话变更事件（Handle Session Change Events）
      *  
      *  会话变更事件处理流程（Session Change Event Handling Flow）：
      *  
      *    0                   1                   2                   3
      *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  1. Receive SERVICE_CONTROL_SESSIONCHANGE from SCM             |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  2. Check dwEventType event type                               |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  3. For WTS_CONSOLE_CONNECT:                                    |
      *   |     - New console session connects                              |
      *   |     - Trigger session_change_event to restart PoixsDesk.exe    |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  4. For WTS_SESSION_LOCK:                                       |
      *   |     - User session is locked (screen locked)                    |
      *   |     - PoixsDesk.exe may need special handling                   |
      *   |     - Currently: continue running, may pause/resume capture     |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  5. For WTS_SESSION_UNLOCK:                                     |
      *   |     - User session is unlocked (screen unlocked)                |
      *   |     - Resume normal operation of PoixsDesk.exe                  |
      *   |     - May trigger session_change_event to restart if needed    |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  6. For WTS_CONSOLE_DISCONNECT:                                 |
      *   |     - Console session disconnects                               |
      *   |     - May trigger session_change_event to handle disconnection  |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *  
      *  锁屏问题解决方案（Screen Lock Problem Solution）：
      *  
      *    0                   1                   2                   3
      *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  Problem: PoixsDesk.exe may fail when screen is locked         |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  Solution 1: Detect WTS_SESSION_LOCK event                      |
      *   |     - When screen locks, notify PoixsDesk.exe to pause capture  |
      *   |     - PoixsDesk.exe can stop video/audio capture                |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  Solution 2: Detect WTS_SESSION_UNLOCK event                    |
      *   |     - When screen unlocks, notify PoixsDesk.exe to resume       |
      *   |     - PoixsDesk.exe can restart video/audio capture             |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  Solution 3: Restart PoixsDesk.exe on unlock                    |
      *   |     - If capture fails after unlock, restart the process        |
      *   |     - Trigger session_change_event to restart                   |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *  
      *  @note WTS_CONSOLE_CONNECT: 新控制台会话连接，需要重启PoixsDesk.exe
      *  @note WTS_SESSION_LOCK: 会话锁定（锁屏），PoixsDesk.exe可能需要暂停捕获
      *  @note WTS_SESSION_UNLOCK: 会话解锁（解锁），PoixsDesk.exe可能需要恢复捕获
      *  @note WTS_CONSOLE_DISCONNECT: 控制台会话断开，可能需要处理断开逻辑
      */
      
      // If a new session connects to the console, restart PoixsDesk
      // to allow it to spawn inside the new console session.
      if (dwEventType == WTS_CONSOLE_CONNECT) {
        /**
        *  @brief 控制台会话连接事件处理
        *  
        *  当新的控制台会话连接时，触发session_change_event以重启PoixsDesk.exe。
        *  这确保PoixsDesk.exe能够在新的控制台会话中正确启动。
        *  
        *  处理流程：
        *  1. 检测到WTS_CONSOLE_CONNECT事件
        *  2. 设置session_change_event事件
        *  3. ServiceMain()主循环检测到事件并重启PoixsDesk.exe
        *  
        *  @note 此事件通常发生在用户登录或远程桌面连接时
        */
        SetEvent(session_change_event);
      }
      else if (dwEventType == WTS_SESSION_LOCK) {
        /**
        *  @brief 会话锁定事件处理（处理锁屏问题）
        *  
        *  当用户会话被锁定（屏幕锁定）时，触发此事件。
        *  锁屏可能导致PoixsDesk.exe无法正常捕获屏幕内容。
        *  
        *  锁屏问题解决方案：
        *  1. 检测到WTS_SESSION_LOCK事件
        *  2. 通知PoixsDesk.exe暂停视频/音频捕获
        *  3. 或者等待解锁后自动恢复
        *  
        *  处理流程（Lock Screen Handling Flow）：
        *  
        *    0                   1                   2                   3
        *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *   |  1. User locks screen (Win+L or timeout)                       |
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *   |  2. Windows sends WTS_SESSION_LOCK event                       |
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *   |  3. HandlerEx() receives WTS_SESSION_LOCK                      |
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *   |  4. Option A: Trigger session_change_event to restart         |
        *   |     - Restart PoixsDesk.exe to handle lock screen              |
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *   |  4. Option B: Continue running (current implementation)       |
        *   |     - PoixsDesk.exe continues, may pause capture               |
        *   |     - Capture will fail silently or show lock screen          |
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *  
        *  @note 当前实现：继续运行，不触发重启
        *  @note 如果需要重启：取消下面的注释，启用重启逻辑
        *  @note 锁屏时屏幕捕获会显示锁屏画面或黑屏
        *  
        *  可选处理方式（取消注释以启用）：
        *  // SetEvent(session_change_event);  // 触发重启以处理锁屏
        */
        // 当前实现：继续运行，PoixsDesk.exe可以检测锁屏并暂停捕获
        // 如果需要重启PoixsDesk.exe来处理锁屏问题，取消下面注释：
        // SetEvent(session_change_event);
      }
      else if (dwEventType == WTS_SESSION_UNLOCK) {
        /**
        *  @brief 会话解锁事件处理（处理解锁问题）
        *  
        *  当用户会话被解锁（屏幕解锁）时，触发此事件。
        *  解锁后需要恢复PoixsDesk.exe的正常操作。
        *  
        *  解锁问题解决方案：
        *  1. 检测到WTS_SESSION_UNLOCK事件
        *  2. 通知PoixsDesk.exe恢复视频/音频捕获
        *  3. 如果捕获失败，重启PoixsDesk.exe
        *  
        *  处理流程（Unlock Screen Handling Flow）：
        *  
        *    0                   1                   2                   3
        *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *   |  1. User unlocks screen (password/credential)                  |
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *   |  2. Windows sends WTS_SESSION_UNLOCK event                     |
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *   |  3. HandlerEx() receives WTS_SESSION_UNLOCK                    |
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *   |  4. Option A: Trigger session_change_event to restart         |
        *   |     - Restart PoixsDesk.exe to ensure clean state              |
        *   |     - Resolve any capture issues after unlock                  |
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *   |  4. Option B: Continue running (current implementation)       |
        *   |     - PoixsDesk.exe should resume capture automatically        |
        *   |     - If capture fails, may need manual restart                |
        *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        *  
        *  @note 当前实现：继续运行，假设PoixsDesk.exe能够自动恢复
        *  @note 如果解锁后捕获失败，建议启用重启逻辑
        *  @note 重启可以确保捕获功能在解锁后正常工作
        *  
        *  推荐处理方式（取消注释以启用）：
        *  SetEvent(session_change_event);  // 解锁后重启以确保捕获正常
        */
        // 当前实现：继续运行，假设PoixsDesk.exe能够自动恢复捕获
        // 如果解锁后捕获失败，建议取消下面注释以启用重启逻辑：
        // SetEvent(session_change_event);  // 重启以恢复捕获功能
      }
      else if (dwEventType == WTS_CONSOLE_DISCONNECT) {
        /**
        *  @brief 控制台会话断开事件处理
        *  
        *  当控制台会话断开时（如远程桌面断开），触发此事件。
        *  可能需要处理PoixsDesk.exe的断开逻辑。
        *  
        *  @note 当前实现：不处理，继续运行
        *  @note 如果需要处理断开，可以触发session_change_event
        */
        // 当前实现：不处理断开事件，继续运行
        // 如果需要处理断开，可以取消下面注释：
        // SetEvent(session_change_event);
      }
      return NO_ERROR;

    case SERVICE_CONTROL_PRESHUTDOWN:
      // The system is shutting down
    case SERVICE_CONTROL_STOP:
      // Let SCM know we're stopping in up to 30 seconds
      service_status.dwCurrentState = SERVICE_STOP_PENDING;
      service_status.dwControlsAccepted = 0;
      service_status.dwWaitHint = 30 * 1000;
      SetServiceStatus(service_status_handle, &service_status);

      // Trigger ServiceMain() to start cleanup
      SetEvent(stop_event);
      return NO_ERROR;

    default:
      return ERROR_CALL_NOT_IMPLEMENTED;
  }
}

/**
*  @author chensong
*  @date 2022-01-19
*  @brief 创建子进程作业对象（Create Job Object for Child Process）
*  
*  该函数用于创建一个Windows作业对象，用于管理PoixsDesk.exe子进程。
*  作业对象用于确保子进程在服务意外终止时能够正确清理。
*  
*  作业对象说明：
*  - Windows作业对象用于管理一组进程
*  - JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE：当作业对象关闭时，杀死所有子进程
*  - JOB_OBJECT_LIMIT_BREAKAWAY_OK：允许子进程脱离作业对象
*  
*  作业对象结构（Job Object Structure）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  Job Object Handle                                             |
*   |  (HANDLE: opaque handle to job object)                          |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  JOBOBJECT_EXTENDED_LIMIT_INFORMATION:                          |
*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
*   |  | BasicLimitInformation.LimitFlags                            | |
*   |  | - JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE (0x00002000)           | |
*   |  | - JOB_OBJECT_LIMIT_BREAKAWAY_OK (0x00000800)                | |
*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  作业对象创建流程（Job Object Creation Flow）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  1. CreateJobObjectW(NULL, NULL) creates job object            |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  2. Initialize JOBOBJECT_EXTENDED_LIMIT_INFORMATION             |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  3. Set LimitFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE       |
*   |     - Ensures child process is killed when job object closes   |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  4. Set LimitFlags |= JOB_OBJECT_LIMIT_BREAKAWAY_OK             |
*   |     - Allows child process to spawn breakaway processes        |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  5. SetInformationJobObject() applies limits to job object     |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  6. Return job object handle                                    |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  @return 返回作业对象句柄，如果创建失败返回NULL
*  @note 作业对象用于确保子进程在服务意外终止时能够正确清理
*  @note JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE确保不会有孤立的进程
*  @note JOB_OBJECT_LIMIT_BREAKAWAY_OK允许子进程创建脱离作业的进程
*  
*  使用示例：
*  @code
*  HANDLE job_handle = CreateJobObjectForChildProcess();
*  if (job_handle) {
*      // 使用作业对象创建子进程
*      // ...
*  }
*  @endcode
*/
HANDLE CreateJobObjectForChildProcess() {
  HANDLE job_handle = CreateJobObjectW(NULL, NULL);
  if (!job_handle) {
    return NULL;
  }

  JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_limit_info = {};

  // Kill PoixsDesk.exe when the final job object handle is closed (which will happen if we terminate unexpectedly).
  // This ensures we don't leave an orphaned PoixsDesk.exe running with an inherited handle to our log file.
  job_limit_info.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

  // Allow PoixsDesk.exe to use CREATE_BREAKAWAY_FROM_JOB when spawning processes to ensure they can to live beyond
  // the lifetime of PoixsDeskSvc.exe. This avoids unexpected user data loss if we crash or are killed.
  job_limit_info.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_BREAKAWAY_OK;

  if (!SetInformationJobObject(job_handle, JobObjectExtendedLimitInformation, &job_limit_info, sizeof(job_limit_info))) {
    CloseHandle(job_handle);
    return NULL;
  }

  return job_handle;
}

/**
*  @author chensong
*  @date 2022-01-19
*  @brief 分配进程线程属性列表（Allocate Process Thread Attribute List）
*  
*  该函数用于分配并初始化一个进程线程属性列表。属性列表用于
*  在创建子进程时指定继承的句柄和作业对象。
*  
*  进程线程属性列表说明：
*  - PROC_THREAD_ATTRIBUTE_LIST用于在创建进程时传递属性
*  - 属性列表可以包含多个属性（如句柄列表、作业对象等）
*  - 属性列表需要先分配内存，然后初始化
*  
*  进程线程属性列表结构（Process Thread Attribute List Structure）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  PROC_THREAD_ATTRIBUTE_LIST Header                             |
*   |  (variable size, opaque structure)                               |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  Attribute 0: PROC_THREAD_ATTRIBUTE_HANDLE_LIST                 |
*   |  - Contains handle list (log_file_handle)                       |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  Attribute 1: PROC_THREAD_ATTRIBUTE_JOB_LIST                    |
*   |  - Contains job object handle (job_handle)                      |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  属性列表分配流程（Attribute List Allocation Flow）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  1. InitializeProcThreadAttributeList(NULL, ...)                |
*   |     - Get required size for attribute list                      |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  2. HeapAlloc() allocates memory for attribute list             |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  3. InitializeProcThreadAttributeList(list, ...)                |
*   |     - Initialize attribute list with attribute_count entries   |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  4. Return allocated and initialized attribute list             |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  @param attribute_count 属性数量，指定属性列表中包含的属性个数
*  @return 返回分配的属性列表指针，如果分配失败返回NULL
*  @note 属性列表需要使用HeapFree释放内存
*  @note 属性列表用于在创建子进程时指定继承的句柄和作业对象
*  
*  使用示例：
*  @code
*  LPPROC_THREAD_ATTRIBUTE_LIST attr_list = AllocateProcThreadAttributeList(2);
*  if (attr_list) {
*      // 使用属性列表创建子进程
*      // ...
*  }
*  @endcode
*/
LPPROC_THREAD_ATTRIBUTE_LIST AllocateProcThreadAttributeList(DWORD attribute_count) {
  SIZE_T size;
  InitializeProcThreadAttributeList(NULL, attribute_count, 0, &size);

  auto list = (LPPROC_THREAD_ATTRIBUTE_LIST) HeapAlloc(GetProcessHeap(), 0, size);
  if (list == NULL) {
    return NULL;
  }

  if (!InitializeProcThreadAttributeList(list, attribute_count, 0, &size)) {
    HeapFree(GetProcessHeap(), 0, list);
    return NULL;
  }

  return list;
}

/**
*  @author chensong
*  @date 2022-01-19
*  @brief 复制会话令牌（Duplicate Token for Session）
*  
*  该函数用于复制当前进程的LocalSystem令牌，并修改为指定的控制台会话ID。
*  这样可以在指定会话中创建子进程。
*  
*  会话令牌说明：
*  - Windows令牌用于标识进程的安全上下文
*  - LocalSystem令牌具有最高权限
*  - 令牌可以指定会话ID，用于在不同会话中创建进程
*  
*  会话令牌结构（Session Token Structure）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  Token Handle                                                  |
*   |  (HANDLE: opaque handle to access token)                        |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  Token Information:                                             |
*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
*   |  | TokenUser: LocalSystem                                      | |
*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
*   |  | TokenSessionId: console_session_id (DWORD)                  | |
*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
*   |  | TokenImpersonationLevel: SecurityImpersonation              | |
*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
*   |  | TokenType: TokenPrimary                                     | |
*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  令牌复制流程（Token Duplication Flow）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  1. OpenProcessToken(GetCurrentProcess(), ...)                  |
*   |     - Get current process token (LocalSystem)                   |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  2. DuplicateTokenEx(current_token, ...)                        |
*   |     - Duplicate LocalSystem token                               |
*   |     - Set TokenPrimary and SecurityImpersonation                |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  3. SetTokenInformation(new_token, TokenSessionId, ...)         |
*   |     - Set token session ID to console_session_id                |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  4. Return duplicated token handle                              |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  @param console_session_id 控制台会话ID，指定要创建进程的会话
*  @return 返回复制的令牌句柄，如果复制失败返回NULL
*  @note 令牌需要使用CloseHandle释放
*  @note 令牌用于在指定会话中创建子进程
*  
*  使用示例：
*  @code
*  DWORD session_id = WTSGetActiveConsoleSessionId();
*  HANDLE token = DuplicateTokenForSession(session_id);
*  if (token) {
*      // 使用令牌创建子进程
*      // ...
*      CloseHandle(token);
*  }
*  @endcode
*/
HANDLE DuplicateTokenForSession(DWORD console_session_id) {
  HANDLE current_token;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE, &current_token)) {
    return NULL;
  }

  // Duplicate our own LocalSystem token
  HANDLE new_token;
  if (!DuplicateTokenEx(current_token, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &new_token)) {
    CloseHandle(current_token);
    return NULL;
  }

  CloseHandle(current_token);

  // Change the duplicated token to the console session ID
  if (!SetTokenInformation(new_token, TokenSessionId, &console_session_id, sizeof(console_session_id))) {
    CloseHandle(new_token);
    return NULL;
  }

  return new_token;
}

HANDLE OpenLogFileHandle() {
  WCHAR log_file_name[MAX_PATH];

  // Create PoixsDesk.log in the Temp folder (usually %SYSTEMROOT%\Temp)
  GetTempPathW(_countof(log_file_name), log_file_name);
  wcscat_s(log_file_name, L"poisxdesk.log");

  // The file handle must be inheritable for our child process to use it
  SECURITY_ATTRIBUTES security_attributes = {sizeof(security_attributes), NULL, TRUE};

  // Overwrite the old PoixsDesk.log
  return CreateFileW(log_file_name, GENERIC_WRITE, FILE_SHARE_READ, &security_attributes, CREATE_ALWAYS, 0, NULL);
}

bool RunTerminationHelper(HANDLE console_token, DWORD pid) {
  WCHAR module_path[MAX_PATH];
  GetModuleFileNameW(NULL, module_path, _countof(module_path));
  std::wstring command;

  command += L'"';
  command += module_path;
  command += L'"';
  command += L" --terminate " + std::to_wstring(pid);

  STARTUPINFOW startup_info = {};
  startup_info.cb = sizeof(startup_info);
  startup_info.lpDesktop = (LPWSTR) L"winsta0\\default";

  // Execute ourselves as a detached process in the user session with the --terminate argument.
  // This will allow us to attach to PoixsDesk's console and send it a Ctrl-C event.
  PROCESS_INFORMATION process_info;
  if (!CreateProcessAsUserW(console_token, module_path, (LPWSTR) command.c_str(), NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT | DETACHED_PROCESS, NULL, NULL, &startup_info, &process_info)) {
    return false;
  }

  // Wait for the termination helper to complete
  WaitForSingleObject(process_info.hProcess, INFINITE);

  // Check the exit status of the helper process
  DWORD exit_code;
  GetExitCodeProcess(process_info.hProcess, &exit_code);

  // Cleanup handles
  CloseHandle(process_info.hProcess);
  CloseHandle(process_info.hThread);

  // If the helper process returned 0, it succeeded
  return exit_code == 0;
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv) {
  service_status_handle = RegisterServiceCtrlHandlerEx(SERVICE_NAME, HandlerEx, NULL);
  if (service_status_handle == NULL) {
    // Nothing we can really do here but terminate ourselves
    ExitProcess(GetLastError());
    return;
  }

  // Tell SCM we're starting
  service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  service_status.dwServiceSpecificExitCode = 0;
  service_status.dwWin32ExitCode = NO_ERROR;
  service_status.dwWaitHint = 0;
  service_status.dwControlsAccepted = 0;
  service_status.dwCheckPoint = 0;
  service_status.dwCurrentState = SERVICE_START_PENDING;
  SetServiceStatus(service_status_handle, &service_status);

  // Create a manual-reset stop event
  stop_event = CreateEventA(NULL, TRUE, FALSE, NULL);
  if (stop_event == NULL) {
    // Tell SCM we failed to start
    service_status.dwWin32ExitCode = GetLastError();
    service_status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(service_status_handle, &service_status);
    return;
  }

  // Create an auto-reset session change event
  session_change_event = CreateEventA(NULL, FALSE, FALSE, NULL);
  if (session_change_event == NULL) {
    // Tell SCM we failed to start
    service_status.dwWin32ExitCode = GetLastError();
    service_status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(service_status_handle, &service_status);
    return;
  }

  auto log_file_handle = OpenLogFileHandle();
  if (log_file_handle == INVALID_HANDLE_VALUE) {
    // Tell SCM we failed to start
    service_status.dwWin32ExitCode = GetLastError();
    service_status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(service_status_handle, &service_status);
    return;
  }

  // We can use a single STARTUPINFOEXW for all the processes that we launch
  STARTUPINFOEXW startup_info = {};
  startup_info.StartupInfo.cb = sizeof(startup_info);
  startup_info.StartupInfo.lpDesktop = (LPWSTR) L"winsta0\\default";
  startup_info.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
  startup_info.StartupInfo.hStdInput = NULL;
  startup_info.StartupInfo.hStdOutput = log_file_handle;
  startup_info.StartupInfo.hStdError = log_file_handle;

  // Allocate an attribute list with space for 2 entries
  startup_info.lpAttributeList = AllocateProcThreadAttributeList(2);
  if (startup_info.lpAttributeList == NULL) {
    // Tell SCM we failed to start
    service_status.dwWin32ExitCode = GetLastError();
    service_status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(service_status_handle, &service_status);
    return;
  }

  // Only allow PoixsDesk.exe to inherit the log file handle, not all inheritable handles
  UpdateProcThreadAttribute(startup_info.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, &log_file_handle, sizeof(log_file_handle), NULL, NULL);

  // Tell SCM we're running (and stoppable now)
  service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PRESHUTDOWN | SERVICE_ACCEPT_SESSIONCHANGE;
  service_status.dwCurrentState = SERVICE_RUNNING;
  SetServiceStatus(service_status_handle, &service_status);

  /**
  *  @brief 服务主循环（Service Main Loop）
  *  
  *  该循环持续运行，每3秒检查一次控制台会话状态，确保PoixsDesk.exe在正确的会话中运行。
  *  当检测到会话变更（如锁屏/解锁）时，会重启PoixsDesk.exe以确保正常运行。
  *  
  *  主循环流程（Main Loop Flow）：
  *  
  *    0                   1                   2                   3
  *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  1. Wait for stop_event (3 seconds timeout)                    |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  2. Get active console session ID                              |
  *   |     - WTSGetActiveConsoleSessionId()                            |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  3. If no console session, continue loop                       |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  4. Duplicate token for console session                         |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  5. Create job object for child process                         |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  6. Start PoixsDesk.exe in console session                      |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  7. Wait for stop/session_change/process termination            |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  8. Handle events and restart if needed                         |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *  
  *  锁屏问题处理说明（Lock Screen Problem Handling）：
  *  
  *    0                   1                   2                   3
  *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  问题描述：                                                      |
  *   |  - 屏幕锁定（Win+L）时，PoixsDesk.exe可能无法正常捕获屏幕        |
  *   |  - 屏幕解锁后，捕获功能可能无法自动恢复                          |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  解决方案1：在HandlerEx中处理WTS_SESSION_LOCK/UNLOCK事件        |
  *   |  - 锁屏时：暂停捕获或重启PoixsDesk.exe                          |
  *   |  - 解锁时：恢复捕获或重启PoixsDesk.exe                          |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  解决方案2：在主循环中检测会话变更                              |
  *   |  - session_change_event被触发时，终止并重启PoixsDesk.exe        |
  *   |  - 这确保PoixsDesk.exe在新会话状态中正常运行                     |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *   |  解决方案3：PoixsDesk.exe内部处理锁屏                            |
  *   |  - PoixsDesk.exe检测到锁屏，自动暂停捕获                        |
  *   |  - 解锁后自动恢复捕获                                            |
  *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  *  
  *  @note 循环每3秒检查一次，确保及时发现会话变更
  *  @note 会话变更事件（如锁屏/解锁）会触发PoixsDesk.exe重启
  *  @note 锁屏问题的最终解决需要在PoixsDesk.exe内部实现
  */

  // Loop every 3 seconds until the stop event is set or PoixsDesk.exe is running
  while (WaitForSingleObject(stop_event, 3000) != WAIT_OBJECT_0) {
    /**
    *  @brief 获取活动控制台会话ID
    *  
    *  WTSGetActiveConsoleSessionId()获取当前活动控制台会话的ID。
    *  如果没有控制台会话（如纯服务器环境），返回0xFFFFFFFF。
    *  
    *  @note 返回0xFFFFFFFF表示没有控制台会话，需要等待
    *  @note 控制台会话ID用于在正确的会话中启动PoixsDesk.exe
    */
    auto console_session_id = WTSGetActiveConsoleSessionId();
    if (console_session_id == 0xFFFFFFFF) {
      // No console session yet
      continue;
    }

    auto console_token = DuplicateTokenForSession(console_session_id);
    if (console_token == NULL) {
      continue;
    }

    // Job objects cannot span sessions, so we must create one for each process
    auto job_handle = CreateJobObjectForChildProcess();
    if (job_handle == NULL) {
      CloseHandle(console_token);
      continue;
    }

    // Start PoixsDesk.exe inside our job object
    UpdateProcThreadAttribute(startup_info.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_JOB_LIST, &job_handle, sizeof(job_handle), NULL, NULL);

    PROCESS_INFORMATION process_info;
    if (!CreateProcessAsUserW(console_token, L"PoixsDesk.exe", NULL, NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, (LPSTARTUPINFOW) &startup_info, &process_info)) {
      CloseHandle(console_token);
      CloseHandle(job_handle);
      continue;
    }

    bool still_running;
    do {
      /**
      *  @brief 等待事件发生（Wait for Events）
      *  
      *  等待以下事件之一：
      *  1. stop_event: 服务停止事件
      *  2. process_info.hProcess: PoixsDesk.exe进程终止事件
      *  3. session_change_event: 会话变更事件（包括锁屏/解锁）
      *  
      *  会话变更事件处理（Session Change Event Handling）：
      *  
      *    0                   1                   2                   3
      *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  1. session_change_event被触发（锁屏/解锁/会话连接/断开）      |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  2. WaitForMultipleObjects返回WAIT_OBJECT_0 + 2                |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  3. 检查活动控制台会话ID是否改变                               |
      *   |     - 如果未改变：可能是误触发，继续运行                        |
      *   |     - 如果已改变：终止PoixsDesk.exe并重启                      |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  4. 终止当前PoixsDesk.exe进程                                  |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  5. 重新获取新的控制台会话ID并启动新的PoixsDesk.exe            |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *  
      *  锁屏问题处理流程（Lock Screen Problem Handling Flow）：
      *  
      *    0                   1                   2                   3
      *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  场景1：锁屏时（如果启用了锁屏重启）                            |
      *   |  1. 用户按下Win+L或屏幕超时锁定                                |
      *   |  2. HandlerEx()检测到WTS_SESSION_LOCK                          |
      *   |  3. HandlerEx()触发session_change_event                        |
      *   |  4. 主循环检测到session_change_event                           |
      *   |  5. 终止PoixsDesk.exe（如果配置为锁屏重启）                     |
      *   |  6. 等待解锁后重新启动                                          |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *   |  场景2：解锁时（如果启用了解锁重启）                            |
      *   |  1. 用户输入密码解锁屏幕                                        |
      *   |  2. HandlerEx()检测到WTS_SESSION_UNLOCK                        |
      *   |  3. HandlerEx()触发session_change_event                        |
      *   |  4. 主循环检测到session_change_event                           |
      *   |  5. 终止当前PoixsDesk.exe（如果还在运行）                       |
      *   |  6. 重新启动PoixsDesk.exe以恢复捕获功能                         |
      *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      *  
      *  @note 会话变更事件可能由锁屏、解锁、登录、注销等触发
      *  @note 如果启用了锁屏/解锁重启，此逻辑会重启PoixsDesk.exe
      *  @note 重启可以解决锁屏后捕获失败的问题
      */
      
      // Wait for the stop event to be set, PoixsDesk.exe to terminate, or the console session to change
      const HANDLE wait_objects[] = {stop_event, process_info.hProcess, session_change_event};
      switch (WaitForMultipleObjects(_countof(wait_objects), wait_objects, FALSE, INFINITE)) {
        case WAIT_OBJECT_0 + 2:
          /**
          *  @brief 会话变更事件处理（Session Change Event Handling）
          *  
          *  当session_change_event被触发时（通常由HandlerEx()在检测到会话变更时触发），
          *  需要检查控制台会话ID是否真正改变，以决定是否重启PoixsDesk.exe。
          *  
          *  处理逻辑：
          *  1. 检查当前活动控制台会话ID
          *  2. 如果ID未改变，可能是误触发（如锁屏但会话未改变），继续运行
          *  3. 如果ID已改变，终止PoixsDesk.exe并重新启动
          *  
          *  @note 这解决了锁屏/解锁时可能出现的会话状态不一致问题
          *  @note 如果会话ID未改变，PoixsDesk.exe继续运行，避免不必要的重启
          */
          if (WTSGetActiveConsoleSessionId() == console_session_id) {
            // The active console session didn't actually change. Let PoixsDesk keep running.
            /**
            *  @brief 会话ID未改变的情况
            *  
            *  如果活动控制台会话ID未改变，说明会话变更事件可能是误触发
            *  （例如：锁屏但会话ID未改变）。此时让PoixsDesk.exe继续运行。
            *  
            *  @note 这避免了不必要的重启，提高了稳定性
            *  @note PoixsDesk.exe内部应该能够处理锁屏状态
            */
            still_running = true;
            continue;
          }
          // Fall-through to terminate PoixsDesk.exe and start it again.
          /**
          *  @brief 会话ID已改变，需要重启PoixsDesk.exe
          *  
          *  如果活动控制台会话ID已改变，说明真正的会话变更发生了
          *  （例如：新用户登录、会话切换等）。此时需要终止并重启PoixsDesk.exe。
          *  
          *  处理流程：
          *  1. 会话ID已改变，fall through到下面的终止逻辑
          *  2. 优雅终止PoixsDesk.exe（如果可能）
          *  3. 强制终止（如果优雅终止失败）
          *  4. 在下次循环中重新启动PoixsDesk.exe
          *  
          *  @note 这确保了PoixsDesk.exe在新会话状态下正常运行
          */
        case WAIT_OBJECT_0:
          // The service is shutting down, so try to gracefully terminate PoixsDesk.exe.
          // If it doesn't terminate in 20 seconds, we will forcefully terminate it.
          if (!RunTerminationHelper(console_token, process_info.dwProcessId) ||
              WaitForSingleObject(process_info.hProcess, 20000) != WAIT_OBJECT_0) {
            // If it won't terminate gracefully, kill it now
            TerminateProcess(process_info.hProcess, ERROR_PROCESS_ABORTED);
          }
          still_running = false;
          break;

        case WAIT_OBJECT_0 + 1:
          {
            // PoixsDesk terminated itself.

            DWORD exit_code;
            if (GetExitCodeProcess(process_info.hProcess, &exit_code) && exit_code == ERROR_SHUTDOWN_IN_PROGRESS) {
              // PoixsDesk is asking for us to shut down, so gracefully stop ourselves.
              SetEvent(stop_event);
            }
            still_running = false;
            break;
          }
      }
    } while (still_running);

    CloseHandle(process_info.hThread);
    CloseHandle(process_info.hProcess);
    CloseHandle(console_token);
    CloseHandle(job_handle);
  }

  // Let SCM know we've stopped
  service_status.dwCurrentState = SERVICE_STOPPED;
  SetServiceStatus(service_status_handle, &service_status);
}

// This will run in a child process in the user session
int DoGracefulTermination(DWORD pid) {
  // Attach to PoixsDesk's console
  if (!AttachConsole(pid)) {
    return GetLastError();
  }

  // Disable our own Ctrl-C handling
  SetConsoleCtrlHandler(NULL, TRUE);

  // Send a Ctrl-C event to PoixsDesk
  if (!GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0)) {
    return GetLastError();
  }

  return 0;
}

int main(int argc, char *argv[]) {
  static const SERVICE_TABLE_ENTRY service_table[] = {
    {(LPSTR) SERVICE_NAME, ServiceMain},
    {NULL, NULL}
  };

  // Check if this is a reinvocation of ourselves to send Ctrl-C to PoixsDesk.exe
  if (argc == 3 && strcmp(argv[1], "--terminate") == 0) {
    return DoGracefulTermination(atol(argv[2]));
  }

  // By default, services have their current directory set to %SYSTEMROOT%\System32.
  // We want to use the directory where PoixsDesk.exe is located instead of system32.
  // This requires stripping off 2 path components: the file name and the last folder
  WCHAR module_path[MAX_PATH];
  GetModuleFileNameW(NULL, module_path, _countof(module_path));
  for (auto i = 0; i < 2; i++) {
    auto last_sep = wcsrchr(module_path, '\\');
    if (last_sep) {
      *last_sep = 0;
    }
  }
  SetCurrentDirectoryW(module_path);

  // Trigger our ServiceMain()
  return StartServiceCtrlDispatcher(service_table);
}


#else

#include <windows.h>
#include <iostream>

// 全局变量
SERVICE_STATUS g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;

// 服务名称
const char* SERVICE_NAME = "MyWin32Service";

// 函数声明
VOID WINAPI ServiceMain(DWORD argc, LPSTR* argv);
VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);
void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);

// 服务主函数
VOID WINAPI ServiceMain(DWORD argc, LPSTR* argv)
{
    // 立即注册服务控制处理器
    g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

    if (!g_StatusHandle)
    {
        return;
    }

    // 设置服务状态为启动中
    ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    // 创建服务停止事件
    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL)
    {
        ReportServiceStatus(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }

    // 设置服务状态为运行中
    ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);

    // 创建工作线程执行服务任务
    HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

    // 等待停止信号
    WaitForSingleObject(g_ServiceStopEvent, INFINITE);

    // 设置服务状态为已停止
    ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);

    CloseHandle(g_ServiceStopEvent);
}

// 服务控制处理器
VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
    switch (CtrlCode)
    {
    case SERVICE_CONTROL_STOP:
        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
            break;

        ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

        // 触发停止事件
        SetEvent(g_ServiceStopEvent);
        break;

    case SERVICE_CONTROL_INTERROGATE:
        break;

    default:
        break;
    }
}

// 服务工作线程
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
    // 模拟服务工作
    while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
    {
        // 这里可以执行服务的实际工作
        Sleep(1000);
    }

    return ERROR_SUCCESS;
}

// 报告服务状态
void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // 填充服务状态结构
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = dwCurrentState;
    g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
    g_ServiceStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
    {
        g_ServiceStatus.dwControlsAccepted = 0;
    }
    else
    {
        g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    }

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
    {
        g_ServiceStatus.dwCheckPoint = 0;
    }
    else
    {
        g_ServiceStatus.dwCheckPoint = dwCheckPoint++;
    }

    // 向SCM报告状态
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}

// 程序主函数
int main(int argc, char* argv[])
{
    // 定义服务表
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        { (LPSTR)SERVICE_NAME, ServiceMain },
        { NULL, NULL }
    };

    // 启动服务控制分发器
    if (!StartServiceCtrlDispatcher(ServiceTable))
    {
        std::wcout << L"StartServiceCtrlDispatcher failed: " << GetLastError() << std::endl;
        return 1;
    }

    return 0;
}

#endif //