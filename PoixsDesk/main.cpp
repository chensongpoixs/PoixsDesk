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
************************************************************************************************/

#include <iostream>

#include <codecvt>
#include <csignal>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <minwindef.h>
#include <future>
#include <chrono>
#include "utility.h"
#include "../libcommon/client.h"
#include <Windows.h>
#include <tchar.h>
#include <process.h>
#include <iostream>
#include <ConsoleApi.h>




#ifdef _WIN32
LRESULT CALLBACK SessionMonitorWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_ENDSESSION:
    {
        // Terminate ourselves with a blocking exit call
        std::cout << "Received WM_ENDSESSION" << std::endl;
        std::raise(SIGINT);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
#if 1

BOOL WINAPI  ConsoleCtrlHandler(DWORD type)
{
    if (type == CTRL_CLOSE_EVENT) 
    {
         std::cout <<  "Console closed handler called" << std::endl;
        std::raise(SIGINT);
    }
    return FALSE;
}
#endif //



#endif


int main(int argc, char *argv[])
{

    printf("[argc  = %u][url = %s]\n", argc, argv[1]);

    /*std::unique_ptr<chen::crtc_client>			rtc_client_;
    rtc_client_ = std::make_unique<chen::crtc_client>();
    rtc_client_->init(0);
    std::thread([&]() {
        rtc_client_->Loop(argv[1]);
        }).detach();*/
    s_rtc_client.init(0);
    s_rtc_client.Loop(argv[1]);


    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


    return EXIT_SUCCESS;
#ifdef _WIN32
	setlocale(LC_ALL, "C");
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	// Use UTF-8 conversion for the default C++ locale (used by boost::log)
	std::locale::global(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
#pragma GCC diagnostic pop


	// Wait as long as possible to terminate PoixsDesk.exe during logoff/shutdown
	SetProcessShutdownParameters(0x100, SHUTDOWN_NORETRY);


    // We must create a hidden window to receive shutdown notifications since we load gdi32.dll
    std::promise<HWND> session_monitor_hwnd_promise;
    auto session_monitor_hwnd_future = session_monitor_hwnd_promise.get_future();
    std::promise<void> session_monitor_join_thread_promise;
    auto session_monitor_join_thread_future = session_monitor_join_thread_promise.get_future();

    std::thread session_monitor_thread([&]() {
        session_monitor_join_thread_promise.set_value_at_thread_exit();

        WNDCLASSA wnd_class{};
        wnd_class.lpszClassName = "PoixsDeskSessionMonitorClass";
        wnd_class.lpfnWndProc = SessionMonitorWindowProc;
        if (!RegisterClassA(&wnd_class)) {
            session_monitor_hwnd_promise.set_value(NULL);
            std::cerr <<  "Failed to register session monitor window class" << std::endl;
            return;
        }

        auto wnd = CreateWindowExA(
            0,
            wnd_class.lpszClassName,
            "PoixsDesk Session Monitor Window",
            0,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            nullptr,
            nullptr,
            nullptr,
            nullptr
        );

        session_monitor_hwnd_promise.set_value(wnd);

        if (!wnd) {
            std::cerr << "Failed to create session monitor window" << std::endl;
            return;
        }

        ShowWindow(wnd, SW_HIDE);

        // Run the message loop for our window
        MSG msg{};
        while (GetMessage(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        });


    auto session_monitor_join_thread_guard = chen::fail_guard([&]() {
        if (session_monitor_hwnd_future.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
            if (HWND session_monitor_hwnd = session_monitor_hwnd_future.get()) {
                PostMessage(session_monitor_hwnd, WM_CLOSE, 0, 0);
            }

            if (session_monitor_join_thread_future.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
                session_monitor_thread.join();
                return;
            }
            else {
               std::wcerr << "session_monitor_join_thread_future reached timeout";
            }
        }
        else {
            std::wcerr << "session_monitor_hwnd_future reached timeout";
        }

        session_monitor_thread.detach();
        });

 
    // Terminate gracefully on Windows when console window is closed
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
 

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

	return EXIT_SUCCESS;
}