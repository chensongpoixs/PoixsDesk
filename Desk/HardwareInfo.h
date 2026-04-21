/***********************************************************************************************
					created: 		2026-03-18

					author:			chensong

					purpose:		hardware_info (设备与硬件信息采集接口)
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

#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace chen {

// ----------------------------- 结构体定义 -----------------------------

// 硬盘信息
struct DiskInfo {
    std::string name;      // 盘符或描述
    uint64_t    totalSize; // 总大小（字节）
    uint64_t    freeSize;  // 可用大小（字节）
};

// GPU 信息
struct GpuInfo {
    std::string name;
    std::string driverVersion;
    uint64_t    vramSize;  // 显存大小（字节，可能为 0 表示未知）
};

// CPU 信息
struct CpuInfo {
    std::string name;
    uint32_t    coreCount = 0;              // 物理核心数
    uint32_t    logicalProcessorCount = 0;  // 逻辑处理器数
    float       baseFrequencyMHz = 0.0f;    // 主频
};

// 内存信息
struct MemoryInfo {
    uint64_t totalPhysical = 0;     // 物理内存总量（字节）
    uint64_t availablePhysical = 0; // 可用物理内存（字节）
};

// 主板信息
struct MotherboardInfo {
    std::string manufacturer;
    std::string product;
    std::string serialNumber;
};

// 操作系统信息
struct OsInfo {
    std::string name;        // 如 "Microsoft Windows 10 Pro"
    std::string version;     // 版本号
    std::string buildNumber; // Build 号
};

// 汇总的硬件信息对象（对应 hardware_info 字段）
struct HardwareInfoObject {
    CpuInfo                 cpu;
    std::vector<DiskInfo>  disk;
    GpuInfo                gpu;
    MemoryInfo             memory;
    MotherboardInfo        motherboard;
    OsInfo                 os;
};

// ----------------------------- 设备信息类 -----------------------------
// 对应：
// device_id（必填）
// password（必填）
// ip_address（可选）
// hardware_info（必填，本类中的 hardware 字段）
// mac_address（必填）
class DeviceInfo {
public:
    std::string     deviceId;     // 设备ID（客户端和服务端使用同一个 ID）
    std::string     deviceName;   // 设备名称（本机计算机名称，对应 device_name）
    std::string     password;     // 设备密码
    std::string     ipAddress;    // IP 地址
    std::string     macAddress;   // MAC 地址
    HardwareInfoObject hardware;  // 硬件信息对象

public:
    DeviceInfo(const std::string& devId,
               const std::string& pwd);

    // 统一初始化入口，按需调用
    bool InitAll();

    // 分方法初始化，方便在不同地方单独调用
    bool InitNetworkInfo();      // 初始化 mac_address & ip_address
    bool InitDeviceName();       // 初始化 device_name
    bool InitCpuInfo();
    bool InitDiskInfo();
    bool InitGpuInfo();
    bool InitMemoryInfo();
    bool InitMotherboardInfo();
    bool InitOsInfo();

private:
    // 内部工具函数：仅 cpp 使用
    bool InitMacAddressInternal();
    bool InitIpAddressInternal();

    // WMI 相关辅助
    bool WmiInitCom();
    void WmiUninitCom();
    bool WmiQuerySingleString(const wchar_t* wmiNamespace,
                              const wchar_t* wqlQuery,
                              const wchar_t* propertyName,
                              std::string&  outValue);
};

} // namespace chen


