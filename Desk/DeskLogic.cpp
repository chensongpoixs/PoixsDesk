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
#include "DeskLogic.h"

#include "httplib.h"
#include "RegistryHelper.h"
#include "json.hpp"
#include "StringHelper.h"
#include "HardwareInfo.h"
#include <chrono>
#include "clog.h"

// using namespace chen;

// namespace chen {

// 	//static FILE* out_file = ::fopen("desk_logic.log", "wb+");





// 	static const char* kHttpUrl = "http://192.168.9.139:9010";



// 	//DeskLogic g_desk_logic;


		
// 	DeskLogic::DeskLogic()
// 		: m_stoped(false)
// 		, m_device_id("")
// 		, m_device_code("")
// 		, m_password("")
// 	{
// 	}

// 	bool DeskLogic::init()
// 	{
// 		NORMAL_EX_LOG("");
// 		_read_system_data();


// 		m_stoped = false;
// 		m_thread = std::thread(&DeskLogic::_work_thread, this);

		

// 		return true;
// 	}

// 	void DeskLogic::stop()
// 	{
// 		m_stoped = true;
// 	}

// 	void DeskLogic::destory()
// 	{
// 		m_stoped = true;

// 		if (m_thread.joinable())
// 		{
// 			m_thread.join();
// 		}
// 	}

// 	void DeskLogic::_register_device()
// 	{
// 		// 注册 
// 		httplib::Client cli(kHttpUrl);
// 		nlohmann::json data;
// 		{
// 			NORMAL_EX_LOG("");
// 			// TODO: 按你业务实际填入统一的 device_id / password
// 			DeviceInfo dev(m_device_id,  m_password);
// 			NORMAL_EX_LOG("");
// 			// 初始化全部硬件信息（失败时部分字段可能为空）
// 			dev.InitAll();
// 			NORMAL_EX_LOG("");
// 			// 组装 JSONss
			

// 			data["device_id"] = dev.deviceId;
// 			data["password"] = dev.password;
// 			data["ip_address"] = dev.ipAddress;

// 			nlohmann::json hw;
// 			hw["mac_address"] = dev.macAddress;

// 			// cpu
// 			hw["cpu"] = {
// 				{"model",     dev.hardware.cpu.name},
// 				{"cores",     dev.hardware.cpu.coreCount},
// 				{"threads",   dev.hardware.cpu.logicalProcessorCount},
// 				{"frequency", std::to_string(dev.hardware.cpu.baseFrequencyMHz) + " MHz"}
// 			};

// 			// disk 数组
// 			hw["disk"] = nlohmann::json::array();
// 			for (const auto& d : dev.hardware.disk) {
// 				nlohmann::json jd;
// 				jd["device"] = d.name;
// 				jd["total"] = d.totalSize;
// 				jd["available"] = d.freeSize;
// 				jd["filesystem"] = ""; // 暂无文件系统信息，可根据需要填充ss
// 				hw["disk"].push_back(jd);
// 			}

// 			// gpu
// 			hw["gpu"] = {
// 				{"model",          dev.hardware.gpu.name},
// 				{"memory",         dev.hardware.gpu.vramSize},
// 				{"driver_version", dev.hardware.gpu.driverVersion}
// 			};

// 			// memory
// 			hw["memory"] = {
// 				{"total",     dev.hardware.memory.totalPhysical},
// 				{"available", dev.hardware.memory.availablePhysical}
// 			};

// 			// motherboard
// 			hw["motherboard"] = {
// 				{"model",        dev.hardware.motherboard.product},
// 				{"manufacturer", dev.hardware.motherboard.manufacturer}
// 			};

// 			// os
// 			hw["os"] = {
// 				{"type",    "Windows"},
// 				{"version", dev.hardware.os.name.empty() ? dev.hardware.os.version : dev.hardware.os.name},
// 				{"arch",    "x86_64"}
// 			};
// 			NORMAL_EX_LOG("");
// 			data["hardware_info"] = hw;
// 		}
// 		NORMAL_EX_LOG("");
// 		auto res = cli.Post("/api/v1/devices/register", data.dump(), "application/json");
// 		if (res && res->status == 200) {
// 			std::cout << "Response: " << res->body << std::endl;
// 			NORMAL_EX_LOG("register : %s", res->body.c_str());
// 			//NORMAL_EX_LOG("%s", res->body.c_str());ss
// 			nlohmann::json response;
// 			try
// 			{
// 				response = nlohmann::json::parse(res->body);
// 			}
// 			catch (const std::exception&e)
// 			{
// 				//ERROR_EX_LOG("request /rtc/push    [msg = %s] json parse failed !!!", res->body.c_str());
// 				return;
// 			}
// 			//  device_id, device_code, password
// 			m_device_id = response["device_id"];
// 			m_device_code = response["device_code"];
// 			m_password = response["password"];
// 			m_token_device = response["token_device"];

// 			_write_system_data();
// 		}
// 		else 
// 		{
// 			std::cout << "Error in response" << std::endl;
// 		}
// 	}

// 	void DeskLogic::_heartbeat_device()
// 	{
// 		// 注册 
// 		httplib::Client cli(kHttpUrl);
// 		nlohmann::json j = { {"device_id", m_device_id}};
		

// 		auto res = cli.Post("/api/v1/devices/heartbeat", j.dump(), "application/json");
// 		if (res && res->status == 200) {
// 			std::cout << "Response: " << res->body << std::endl;
// 			NORMAL_EX_LOG("heartbeat :  %s", res->body.c_str());
// 			nlohmann::json response;
// 			try
// 			{
// 				response = nlohmann::json::parse(res->body);
// 			}
// 			catch (const std::exception& e)
// 			{
// 				//ERROR_EX_LOG("request /rtc/push    [msg = %s] json parse failed !!!", res->body.c_str());
// 				return;
// 			}
			
// 		}
// 		else
// 		{
// 			std::cout << "Error in response" << std::endl;
// 		}
// 	}

// 	void DeskLogic::_read_system_data()
// 	{
// 		// 将 JSON 字符串写入注册表（示例路径）
// 		std::wstring subKey = L"Software\\DeskServiceCRTC";
// 		std::wstring valueName = L"CRTCInfoJson";

// 		//std::string cc = j.dump(4); 
// 		/*if (out_file)
// 		{
// 			fprintf(out_file, "[%s][%d]======\n", __FUNCTION__, __LINE__);
// 			fflush(out_file);
// 		}*/

		

// 		std::wstring jsonW;
// 		NORMAL_EX_LOG("");
// 		RegistryHelper::ReadString(HKEY_LOCAL_MACHINE, subKey, valueName, jsonW);

// 		NORMAL_EX_LOG("");


// 		std::string  data =   StringHelper::WStringToAnsi(jsonW);


// 		nlohmann::json response;
// 		try
// 		{
// 			response = nlohmann::json::parse(data);
// 		}
// 		catch (const std::exception&)
// 		{
// 			//ERROR_EX_LOG("request /rtc/push    [msg = %s] json parse failed !!!", res->body.c_str());
// 			return;
// 		}

		
// 		m_device_id = response["device_id"];
// 		m_device_code = response["device_code"];
// 		m_password = response["password"];
// 		NORMAL_EX_LOG("");
// 	}

// 	void DeskLogic::_write_system_data()
// 	{
// 		// 将 JSON 字符串写入注册表（示例路径）
// 		std::wstring subKey = L"Software\\DeskServiceCRTC";
// 		std::wstring valueName = L"CRTCInfoJson";

// 		//std::string cc = j.dump(4); 
// 		/*if (out_file)
// 		{
// 			fprintf(out_file, "[%s][%d]======\n", __FUNCTION__, __LINE__);
// 			fflush(out_file);
// 		}*/

// 		nlohmann::json  jsonStr = {
			
// 			{"device_id", m_device_id},
// 			{"device_code", m_device_code},
// 			{"password", m_password}
								
// 		};


// 		std::wstring jsonW(jsonStr.begin(), jsonStr.end());
// 		RegistryHelper::WriteString(HKEY_LOCAL_MACHINE, subKey, valueName, jsonW);
// 	}

// 	void DeskLogic::_work_thread()
// 	{
// 		NORMAL_EX_LOG("");
// 		_register_device();
// 		NORMAL_EX_LOG("");
// 		while (!m_stoped)
// 		{
// 			NORMAL_EX_LOG("");
// 			_heartbeat_device();
// 			std::this_thread::sleep_for(std::chrono::seconds(300));



// 		}
// 	}
	
// 	DeskLogic::~DeskLogic()
// 	{
// 	}

// }
 
#include "DeskLogic.h"
#include "httplib.h"
#include "RegistryHelper.h"
#include "json.hpp"
#include "StringHelper.h"
#include "HardwareInfo.h"
#include <chrono>
#include "clog.h"
#include <wininet.h>
#include <ws2tcpip.h>
#include "libcrtc.h"
#include <iostream>

#include <functional>


#pragma comment(lib, "wininet.lib")


 
namespace chen {

#define  TEST_RTC_IP_PORT (1)

#define  OFFICIAL_SERVER  (1)

#if 0
    static const char* kIp = "192.168.9.139";
    static const uint16_t kPort = 9010;


static const char* kHttpUrl = "http://192.168.9.139:9010";
#elif OFFICIAL_SERVER
    

    static const char* kIp = "112.80.31.194";
    static const uint16_t kPort = 20708;


    static const char* kHttpUrl = "http://112.80.31.194:20913";
#else 
    static const char* kIp = "112.80.31.194";
    static const uint16_t kPort = 20913;


    static const char* kHttpUrl = "http://112.80.31.194:20913";
#endif 



static bool  g_g_pushing = false;

static void librtc_status_callback(ECrtcType type)
{
    switch (type)
    {
    case chen::ECrtcInit:
        NORMAL_EX_LOG("rtc_status callbac init !!!");
        break;
    case chen::ECrtcConnecting:
        NORMAL_EX_LOG("rtc_status callbac ECrtcConnecting !!!");
        break;
    case chen::ECrtcConnected:
        NORMAL_EX_LOG("rtc_status callbac ECrtcConnected !!!");
        break;
    case chen::ECrtcConnectFailed:
        WARNING_EX_LOG("rtc_status callbac ECrtcConnectFailed !!!");
        g_g_pushing = false;
        break;
    case chen::ECrtcDes:
        WARNING_EX_LOG("rtc_status callbac ECrtcDes !!!");
        g_g_pushing = false;
        break;
    default:
        NORMAL_EX_LOG("rtc_status callbac init !!!");
        break;
    }
    }

 	DeskLogic::DeskLogic()
 		: m_stoped(false)
 		, m_device_id("test898989")
 		, m_device_code("")
 		, m_password("")
       // , m_pushing(false)
 	{
 	}

    DeskLogic::~DeskLogic()
    {
    }

 	bool DeskLogic::init()
 	{
 		NORMAL_EX_LOG("");
 		_read_system_data();

        libcrtc_device_info_callback(nullptr);
        libcrtc_rtc_status_callback( &librtc_status_callback
        /*std::bind(&DeskLogic::crtc_rtc_status_callback, this, std::placeholders::_1)*/);
       
 		m_stoped = false;
       // m_pushing = false;

        g_g_pushing = false;
 		m_thread = std::thread(&DeskLogic::_work_thread, this);



 		return true;
 	}

 	void DeskLogic::stop()
 	{
 		m_stoped = true;
 	}

 	void DeskLogic::destory()
 	{
 		m_stoped = true;

 		if (m_thread.joinable())
 		{
 			m_thread.join();
 		}
 	}

    void DeskLogic::crtc_rtc_status_callback(ECrtcType type)
    {
        switch (type)
        {
        case chen::ECrtcInit:
            NORMAL_EX_LOG("rtc_status callbac init !!!");
            break;
        case chen::ECrtcConnecting:
            NORMAL_EX_LOG("rtc_status callbac ECrtcConnecting !!!");
            break;
        case chen::ECrtcConnected:
            NORMAL_EX_LOG("rtc_status callbac ECrtcConnected !!!");
            break;
        case chen::ECrtcConnectFailed:
            WARNING_EX_LOG("rtc_status callbac ECrtcConnectFailed !!!");
          //  m_pushing = false;
            break;
        case chen::ECrtcDes:
            WARNING_EX_LOG("rtc_status callbac ECrtcDes !!!");
          //  m_pushing = false;
            break;
        default:
            NORMAL_EX_LOG("rtc_status callbac init !!!");
            break;
        }
    }

     	void DeskLogic::_read_system_data()
     	{
     		// 将 JSON 字符串写入注册表（示例路径）
     		std::wstring subKey = L"Software\\DeskServiceCRTC";
     		std::wstring valueName = L"CRTCInfoJson";

     		//std::string cc = j.dump(4); 
     		/*if (out_file)
     		{
     			fprintf(out_file, "[%s][%d]======\n", __FUNCTION__, __LINE__);
     			fflush(out_file);
     		}*/



     		std::wstring jsonW;
     		NORMAL_EX_LOG("");
            if (!RegistryHelper::ReadString(HKEY_LOCAL_MACHINE, subKey, valueName, jsonW))
            {
                WARNING_EX_LOG("read registry ... failed !!!");
                return  ;
            }

     		NORMAL_EX_LOG("");


     		std::string  data =   StringHelper::WStringToAnsi(jsonW);


     		nlohmann::json response;
     		try
     		{
     			response = nlohmann::json::parse(data);
     		}
     		catch (const std::exception&)
     		{
     			//ERROR_EX_LOG("request /rtc/push    [msg = %s] json parse failed !!!", res->body.c_str());
     			return;
     		}


     		m_device_id = response["device_id"];
     		m_device_code = response["device_code"];
     		m_password = response["password"];
     		NORMAL_EX_LOG("");
     	}

     	void DeskLogic::_write_system_data()
     	{
     		// 将 JSON 字符串写入注册表（示例路径）
     		std::wstring subKey = L"Software\\DeskServiceCRTC";
     		std::wstring valueName = L"CRTCInfoJson";

     		// 创建 JSON 对象
     		nlohmann::json jsonData = {
     			{"device_id", m_device_id},
     			{"device_code", m_device_code},
     			{"password", m_password}
     		};

     		// 正确的转换流程：
     		// 1. JSON 对象 → UTF-8 字符串
     		std::string jsonStr = jsonData.dump();
     		
     		// 2. UTF-8 字符串 → UTF-16 宽字符串
     		std::wstring jsonW = StringHelper::StringToWString(jsonStr);
     		
     		// 3. 写入注册表
     		RegistryHelper::WriteString(HKEY_LOCAL_MACHINE, subKey, valueName, jsonW);
     	}

//   添加：检查网络是否可用
bool DeskLogic::_check_network_available()
{
    // 方法1: 使用 Windows API 检查
    DWORD flags;
    if (InternetGetConnectedState(&flags, 0))
    {
        return true;
    }
    
    // 方法2: 尝试 DNS 解析
    struct addrinfo hints = {0}, *result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    int ret = getaddrinfo(kIp, "9010", &hints, &result);
    if (ret == 0)
    {
        freeaddrinfo(result);
        return true;
    }
    
    return false;
}

//   添加：等待网络可用
bool DeskLogic::_wait_for_network(int timeout_seconds)
{
    NORMAL_EX_LOG("Waiting for network to be available (timeout: %d seconds)...", timeout_seconds);
    
    for (int i = 0; i < timeout_seconds; ++i)
    {
        if (m_stoped)
        {
            NORMAL_EX_LOG("Stopped while waiting for network");
            return false;
        }
        
        if (_check_network_available())
        {
            WARNING_EX_LOG("Network is available");
            return true;
        }
        
        if (i % 10 == 0 && i > 0)
        {
            NORMAL_EX_LOG("Still waiting for network... (%d/%d seconds)", i, timeout_seconds);
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    ERROR_EX_LOG("Network not available after %d seconds", timeout_seconds);
    return false;
}

//  修复：添加完整的异常处理
void DeskLogic::_register_device()
{
    try
    {
        NORMAL_EX_LOG("Starting device registration...");
        
        // 检查网络
        if (!_check_network_available())
        {
            WARNING_EX_LOG("Network not available, skipping registration");
            return;
        }
        
        // 创建 HTTP 客户端
        httplib::Client cli(kHttpUrl);
        
        // 设置超时
        cli.set_connection_timeout(10, 0);  // 10秒连接超时
        cli.set_read_timeout(30, 0);        // 30秒读取超时
        cli.set_write_timeout(30, 0);       // 30秒写入超时
        
        // 准备数据
        nlohmann::json data;
        {
            NORMAL_EX_LOG("Collecting hardware information...");
            DeviceInfo dev(m_device_id, m_password);
            dev.InitAll();
            
            // 辅助函数：确保字符串是有效的 UTF-8
            // 如果字符串已经是 UTF-8，直接返回；否则尝试从 GBK 转换
            auto ensure_utf8 = [](const std::string& str) -> std::string {
                if (str.empty()) return str;
                
                // 检查是否已经是有效的 UTF-8
                bool is_valid_utf8 = true;
                for (size_t i = 0; i < str.size(); ) {
                    unsigned char c = str[i];
                    if (c < 0x80) {
                        i++;
                    } else if ((c & 0xE0) == 0xC0) {
                        if (i + 1 >= str.size() || (str[i+1] & 0xC0) != 0x80) {
                            is_valid_utf8 = false;
                            break;
                        }
                        i += 2;
                    } else if ((c & 0xF0) == 0xE0) {
                        if (i + 2 >= str.size() || (str[i+1] & 0xC0) != 0x80 || (str[i+2] & 0xC0) != 0x80) {
                            is_valid_utf8 = false;
                            break;
                        }
                        i += 3;
                    } else if ((c & 0xF8) == 0xF0) {
                        if (i + 3 >= str.size() || (str[i+1] & 0xC0) != 0x80 || (str[i+2] & 0xC0) != 0x80 || (str[i+3] & 0xC0) != 0x80) {
                            is_valid_utf8 = false;
                            break;
                        }
                        i += 4;
                    } else {
                        is_valid_utf8 = false;
                        break;
                    }
                }
                
                if (is_valid_utf8) {
                    return str;
                }
                
                // 不是有效的 UTF-8，尝试从 GBK/ANSI 转换
                std::wstring wstr = StringHelper::AnsiToWString(str);
                return StringHelper::WStringToString(wstr);
            };
            std::string current_device_id = dev.deviceId.empty() ? "chensong": ensure_utf8(dev.deviceId);
            std::string current_password = dev.password.empty() ? "123456": ensure_utf8(dev.password);

            data["device_id"] = current_device_id;
            data["password"] = current_password ;
            data["ip_address"] = ensure_utf8(dev.ipAddress);

            nlohmann::json hw;
            hw["mac_address"] = ensure_utf8(dev.macAddress);
            hw["cpu"] = {
                {"model", ensure_utf8(dev.hardware.cpu.name)},
                {"cores", dev.hardware.cpu.coreCount},
                {"threads", dev.hardware.cpu.logicalProcessorCount},
                {"frequency", std::to_string(dev.hardware.cpu.baseFrequencyMHz) + " MHz"}
            };

            hw["disk"] = nlohmann::json::array();
            for (const auto& d : dev.hardware.disk) {
                nlohmann::json jd;
                jd["device"] = ensure_utf8(d.name);
                jd["total"] = d.totalSize;
                jd["available"] = d.freeSize;
                jd["filesystem"] = "";
                hw["disk"].push_back(jd);
            }

            hw["gpu"] = {
                {"model", ensure_utf8(dev.hardware.gpu.name)},
                {"memory", dev.hardware.gpu.vramSize},
                {"driver_version", ensure_utf8(dev.hardware.gpu.driverVersion)}
            };

            hw["memory"] = {
                {"total", dev.hardware.memory.totalPhysical},
                {"available", dev.hardware.memory.availablePhysical}
            };

            hw["motherboard"] = {
                {"model", ensure_utf8(dev.hardware.motherboard.product)},
                {"manufacturer", ensure_utf8(dev.hardware.motherboard.manufacturer)}
            };

            hw["os"] = {
                {"type", "Windows"},
                {"version", ensure_utf8(dev.hardware.os.name.empty() ? dev.hardware.os.version : dev.hardware.os.name)},
                {"arch", "x86_64"}
            };

            data["hardware_info"] = hw;
        }
        
        NORMAL_EX_LOG("Sending registration request to %s...", kHttpUrl);
        
        // 发送请求
        auto res = cli.Post("/api/v1/devices/register", data.dump(), "application/json");
        
        //  检查响应是否有效
        if (!res)
        {
            ERROR_EX_LOG("Registration failed: No response (network error or timeout)");
            return;
        }
        
        NORMAL_EX_LOG("Received response, HTTP status: %d", res->status);
        
        //   检查 HTTP 状态码
        if (res->status != 200)
        {
            ERROR_EX_LOG("Registration failed: HTTP %d", res->status);
            if (!res->body.empty())
            {
                ERROR_EX_LOG("Response body: %s", res->body.c_str());
            }
            return;
        }
        
        //   解析响应
        try
        {
            nlohmann::json response = nlohmann::json::parse(res->body);
            
            // 辅助函数：安全地从 JSON 中提取字符串（处理数字类型）
            auto get_string_value = [](const nlohmann::json& j, const std::string& key) -> std::string {
                if (!j.contains(key)) {
                    return "";
                }
                
                const auto& value = j[key];
                if (value.is_string()) {
                    return value.get<std::string>();
                } else if (value.is_number_integer()) {
                    return std::to_string(value.get<int64_t>());
                } else if (value.is_number_unsigned()) {
                    return std::to_string(value.get<uint64_t>());
                } else if (value.is_number_float()) {
                    return std::to_string(value.get<double>());
                } else if (value.is_boolean()) {
                    return value.get<bool>() ? "true" : "false";
                } else if (value.is_null()) {
                    return "";
                }
                
                // 其他类型尝试转换为字符串
                return value.dump();
            };
            
            // 检查响应结构：服务器返回 {code, message, data}
            if (response.contains("code") && response["code"].is_number())
            {
                int code = response["code"].get<int>();
                if (code == 0 && response.contains("data") && response["data"].is_object())
                {
                    // 成功响应，从 data 字段中提取数据
                    const auto& data = response["data"];
                    
                    m_device_id = get_string_value(data, "device_id");
                    m_device_code = get_string_value(data, "device_code");
                    m_password = get_string_value(data, "password");
                    m_token_device = get_string_value(data, "token_device");
                    
                    _write_system_data();
                    
                    NORMAL_EX_LOG("Device registered successfully, device_id: %s, device_code: %s", 
                                  m_device_id.c_str(), m_device_code.c_str());
                }
                else
                {
                    // 错误响应
                    std::string message = get_string_value(response, "message");
                    ERROR_EX_LOG("Registration failed with code %d: %s", code, message.c_str());
                }
            }
            else
            {
                // 旧格式兼容：直接从根对象提取
                m_device_id = get_string_value(response, "device_id");
                m_device_code = get_string_value(response, "device_code");
                m_password = get_string_value(response, "password");
                m_token_device = get_string_value(response, "token_device");
                
                _write_system_data();
                
                NORMAL_EX_LOG("Device registered successfully (legacy format), device_id: %s", m_device_id.c_str());
            }
        }
        catch (const nlohmann::json::exception& e)
        {
            ERROR_EX_LOG("JSON parse error: %s", e.what());
            ERROR_EX_LOG("Response body: %s", res->body.c_str());
        }
    }
    catch (const std::exception& e)
    {
        ERROR_EX_LOG("Registration exception: %s", e.what());
    }
    catch (...)
    {
        ERROR_EX_LOG("Unknown exception during registration");
    }
}

//   修复：添加完整的异常处理
void DeskLogic::_heartbeat_device()
{
    try
    {
        // 检查网络
        if (!_check_network_available())
        {
            WARNING_EX_LOG("Network not available, skipping heartbeat");
            return;
        }
        
        // 创建 HTTP 客户端
        httplib::Client cli(kHttpUrl);
        
        // 设置超时
        cli.set_connection_timeout(10, 0);
        cli.set_read_timeout(30, 0);
        cli.set_write_timeout(30, 0);
        
        nlohmann::json j = {{"device_id", m_device_id}};
        
        NORMAL_EX_LOG("Sending heartbeat for device: %s", m_device_id.c_str());
        
        auto res = cli.Post("/api/v1/devices/heartbeat", j.dump(), "application/json");
        
        //  检查响应
        if (!res)
        {
            WARNING_EX_LOG("Heartbeat failed: No response");
            return;
        }
        
        if (res->status != 200)
        {
            WARNING_EX_LOG("Heartbeat failed: HTTP %d", res->status);
            return;
        }
        
        NORMAL_EX_LOG("Heartbeat successful");
    }
    catch (const std::exception& e)
    {
        ERROR_EX_LOG("Heartbeat exception: %s", e.what());
    }
    catch (...)
    {
        ERROR_EX_LOG("Unknown exception during heartbeat");
    }
}


void DeskLogic::_work_thread()
{
    NORMAL_EX_LOG("DeskLogic work thread started");
    
    try
    {
        
        
        // 心跳循环
        while (!m_stoped)
        {
            NORMAL_EX_LOG("Waiting 5 minutes before next heartbeat...");
            
            // 等待5分钟或停止信号
            for (int i = 0; i < 100 && !m_stoped; ++i)
            {
                if (!g_g_pushing)
                {
                    // push -->
                    g_g_pushing = true;
                    // 等待网络可用（最多60秒）
                    if (_wait_for_network(60))
                    {
                        // 尝试注册设备
                        _register_device();
                    }
                    else
                    {
                        ERROR_EX_LOG("Network not available, device registration skipped");
                    }
                    libcrtc_init(kIp, kPort, m_token_device.c_str());
                }
                std::this_thread::sleep_for(std::chrono::seconds(3));
                
            }
            
            if (m_stoped)
            {
                break;
            }
            
            // 发送心跳
            _heartbeat_device();
        }
    }
    catch (const std::exception& e)
    {
        ERROR_EX_LOG("Work thread exception: %s", e.what());
    }
    catch (...)
    {
        ERROR_EX_LOG("Unknown exception in work thread");
    }
    
    NORMAL_EX_LOG("DeskLogic work thread stopped");
}

} // namespace chen
