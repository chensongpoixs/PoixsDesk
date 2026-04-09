/***********************************************************************************************
                    created: 		2026-03-18

                    author:			chensong

                    purpose:		hardware_info (设备与硬件信息采集实现)
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


#ifndef  _C_PIPE_SERVER_H_
#define  _C_PIPE_SERVER_H_
#include <windows.h>
#include <iostream>
#include <string>
#include "clog.h"
namespace chen {


    class NamedPipeServer {
    public:
        NamedPipeServer(const std::string& pipeName)
            : m_pipeName("\\\\.\\pipe\\" + pipeName), m_hPipe(INVALID_HANDLE_VALUE) {
        }

        ~NamedPipeServer() { Close(); }
         
        bool Start() { 
            m_hPipe = CreateNamedPipe(
                m_pipeName.c_str(),             // 管道名称
                PIPE_ACCESS_DUPLEX,              // 双向通信
                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                PIPE_UNLIMITED_INSTANCES,        // 最大实例数
                4096,                            // 输出缓冲区大小
                4096,                            // 输入缓冲区大小
                0,                               // 默认超时
                NULL                             // 安全属性
            );

            if (m_hPipe == INVALID_HANDLE_VALUE) {
                std::cerr << "CreateNamedPipe failed: " << GetLastError() << std::endl;

                WARNING_EX_LOG("CreateNamedPipe failed: %u", GetLastError());
                return false;
            }
             
            NORMAL_EX_LOG("wait client connect ...");
            
            if (!ConnectNamedPipe(m_hPipe, NULL)) {
                std::cerr << "ConnectNamedPipe failed: " << GetLastError() << std::endl;
                WARNING_EX_LOG("CreateNamedPipe failed: %u", GetLastError());
                CloseHandle(m_hPipe);
                m_hPipe = INVALID_HANDLE_VALUE;
                return false;
            } 
            NORMAL_EX_LOG("  client connected ");
            return true;
        }
         
        bool SendMessage(const std::string& message) {
            DWORD bytesWritten;
            BOOL result = WriteFile(
                m_hPipe,
                message.c_str(),
                static_cast<DWORD>(message.length()),
                &bytesWritten,
                NULL
            );

            if (!result || bytesWritten != message.length()) {
                std::cerr << "WriteFile failed: " << GetLastError() << std::endl;
                WARNING_EX_LOG("CreateNamedPipe failed: %u", GetLastError());
                return false;
            }
            return true;
        }
         
        std::string ReceiveMessage() {
            char buffer[4096];
            DWORD bytesRead;

            BOOL result = ReadFile(
                m_hPipe,
                buffer,
                sizeof(buffer) - 1,
                &bytesRead,
                NULL
            );

            if (!result) {
                DWORD err = GetLastError();
                if (err != ERROR_BROKEN_PIPE && err != ERROR_PIPE_NOT_CONNECTED) {
                    std::cerr << "ReadFile failed: " << err << std::endl;
                    WARNING_EX_LOG("ReadFile failed: %u", err);
                }
                return "";
            }

            buffer[bytesRead] = '\0';
            return std::string(buffer);
        }
         
        void Close() {
            if (m_hPipe != INVALID_HANDLE_VALUE) {
                FlushFileBuffers(m_hPipe);
                DisconnectNamedPipe(m_hPipe);
                CloseHandle(m_hPipe);
                m_hPipe = INVALID_HANDLE_VALUE;
            }
        }

    private:
        std::string m_pipeName;
        HANDLE m_hPipe;
    };

   
}
#endif // _C_PIPE_SERVER_H_