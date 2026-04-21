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

#include "HardwareInfo.h"

// 注意：winsock2.h 必须在 windows.h 之前包含，避免 winsock.h 冲突
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>
#include <Wbemidl.h>
#include <comdef.h>

#include <vector>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wbemuuid.lib")

namespace chen {

// ----------------------------- 构造函数 -----------------------------

DeviceInfo::DeviceInfo(const std::string& devId,
                       const std::string& pwd)
    : deviceId(devId), password(pwd) {}

// ----------------------------- 对外接口 -----------------------------

bool DeviceInfo::InitAll() {
    bool ok = true;
    ok = ok && InitDeviceName();
    ok = ok && InitNetworkInfo();
    ok = ok && InitCpuInfo();
    ok = ok && InitDiskInfo();
    ok = ok && InitGpuInfo();
    ok = ok && InitMemoryInfo();
    ok = ok && InitMotherboardInfo();
    ok = ok && InitOsInfo();

    return ok;
}

bool DeviceInfo::InitDeviceName() {
    DWORD size = 1024 + 1;
    char nameBuf[1024 + 1] = { 0 };
    if (!GetComputerNameA(nameBuf, &size)) {
        return false;
    }
    deviceName.assign(nameBuf, size);
    return !deviceName.empty();
}

bool DeviceInfo::InitNetworkInfo() {
    bool ok = true;
    ok = ok && InitMacAddressInternal();
    ok = ok && InitIpAddressInternal();
    return ok;
}

// ----------------------------- MAC / IP -----------------------------

bool DeviceInfo::InitMacAddressInternal() {
    ULONG outBufLen = 15000;
    std::vector<BYTE> buffer(outBufLen);
    IP_ADAPTER_ADDRESSES* pAddresses =
        reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());

    DWORD ret = GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, pAddresses, &outBufLen);
    if (ret == ERROR_BUFFER_OVERFLOW) {
        buffer.resize(outBufLen);
        pAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
        ret = GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, pAddresses, &outBufLen);
    }
    if (ret != NO_ERROR) return false;

    for (IP_ADAPTER_ADDRESSES* aa = pAddresses; aa != nullptr; aa = aa->Next) {
        if (aa->PhysicalAddressLength == 0) continue;

        char macStr[64] = { 0 };
        for (ULONG i = 0; i < aa->PhysicalAddressLength; ++i) {
            sprintf_s(macStr + i * 3, sizeof(macStr) - i * 3,
                      (i == aa->PhysicalAddressLength - 1) ? "%02X" : "%02X-",
                      aa->PhysicalAddress[i]);
        }
        macAddress = macStr;
        return true;
    }
    return false;
}

bool DeviceInfo::InitIpAddressInternal() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        return false;
    }

    ULONG outBufLen = 15000;
    std::vector<BYTE> buffer(outBufLen);
    IP_ADAPTER_ADDRESSES* pAddresses =
        reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());

    DWORD ret = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr,
                                     pAddresses, &outBufLen);
    if (ret == ERROR_BUFFER_OVERFLOW) {
        buffer.resize(outBufLen);
        pAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
        ret = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr,
                                   pAddresses, &outBufLen);
    }
    if (ret != NO_ERROR) {
        WSACleanup();
        return false;
    }

    // 收集所有“真实网卡”的 IPv4 地址（过滤虚拟网卡/回环/隧道等）
    std::vector<std::string> ipList;

    for (IP_ADAPTER_ADDRESSES* aa = pAddresses; aa != nullptr; aa = aa->Next) {
        // 1. 只要处于 up 状态的网卡
        if (aa->OperStatus != IfOperStatusUp) {
            continue;
        }

        // 2. 根据 IfType 粗过滤虚拟/回环/隧道
        switch (aa->IfType) {
        case IF_TYPE_SOFTWARE_LOOPBACK:  // 回环
        case IF_TYPE_TUNNEL:            // 隧道
            continue;
        default:
            break;
        }

        // 3. 根据 FriendlyName / Description 文本再过滤常见虚拟网卡
        auto is_virtual = [](const wchar_t* wstr) -> bool {
            if (!wstr) return false;
            std::wstring s(wstr);
            for (auto& ch : s) ch = towlower(ch);
            if (s.find(L"virtual")    != std::wstring::npos) return true;
            if (s.find(L"vmware")     != std::wstring::npos) return true;
            if (s.find(L"hyper-v")    != std::wstring::npos) return true;
            if (s.find(L"loopback")   != std::wstring::npos) return true;
            if (s.find(L"virtualbox") != std::wstring::npos) return true;
            if (s.find(L"tap-")       != std::wstring::npos) return true;
            if (s.find(L"tun")        != std::wstring::npos) return true;
            return false;
        };

        if (is_virtual(aa->FriendlyName) || is_virtual(aa->Description)) {
            continue;
        }

        // 4. 收集该网卡的 IPv4 地址
        for (IP_ADAPTER_UNICAST_ADDRESS* ua = aa->FirstUnicastAddress;
             ua != nullptr; ua = ua->Next) {
            SOCKADDR* sa = ua->Address.lpSockaddr;
            char buf[128] = { 0 };
            DWORD buflen = sizeof(buf);
            if (sa->sa_family == AF_INET /*|| sa->sa_family == AF_INET6*/) {
                if (WSAAddressToStringA(sa,
                                        (DWORD)ua->Address.iSockaddrLength,
                                        nullptr, buf, &buflen) == 0) {
                    ipList.emplace_back(buf);
                }
            }
        }
    }

    // 组合成一个字符串，使用逗号分隔
    ipAddress.clear();
    for (size_t i = 0; i < ipList.size(); ++i) {
        if (i > 0) {
            ipAddress += ",";
        }
        ipAddress += ipList[i];
    }

    WSACleanup();
    return !ipAddress.empty();
}

// ----------------------------- CPU / 内存 -----------------------------

bool DeviceInfo::InitCpuInfo() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    hardware.cpu.logicalProcessorCount = si.dwNumberOfProcessors;

    std::string cpuName;
    if (WmiQuerySingleString(
            L"ROOT\\CIMV2",
            L"SELECT Name FROM Win32_Processor",
            L"Name",
            cpuName)) {
        hardware.cpu.name = cpuName;
    }

    std::string coreCountStr;
    if (WmiQuerySingleString(
            L"ROOT\\CIMV2",
            L"SELECT NumberOfCores FROM Win32_Processor",
            L"NumberOfCores",
            coreCountStr)) {
        hardware.cpu.coreCount = static_cast<uint32_t>(std::stoi(coreCountStr));
    }

    std::string freqStr;
    if (WmiQuerySingleString(
            L"ROOT\\CIMV2",
            L"SELECT MaxClockSpeed FROM Win32_Processor",
            L"MaxClockSpeed",
            freqStr)) {
        hardware.cpu.baseFrequencyMHz = static_cast<float>(std::stof(freqStr));
    }

    return true;
}

bool DeviceInfo::InitMemoryInfo() {
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    if (!GlobalMemoryStatusEx(&ms)) return false;

    hardware.memory.totalPhysical = ms.ullTotalPhys;
    hardware.memory.availablePhysical = ms.ullAvailPhys;
    return true;
}

// ----------------------------- 磁盘 -----------------------------

bool DeviceInfo::InitDiskInfo() {
    hardware.disk.clear();
    DWORD drives = GetLogicalDrives();
    if (drives == 0) return false;

    for (char c = 'A'; c <= 'Z'; ++c) {
        if (!(drives & (1 << (c - 'A')))) continue;

        char rootPath[4] = { c, ':', '\\', 0 };
        ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
        if (GetDiskFreeSpaceExA(rootPath, &freeBytesAvailable,
                                &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
            DiskInfo d;
            d.name = rootPath;
            d.totalSize = totalNumberOfBytes.QuadPart;
            d.freeSize = totalNumberOfFreeBytes.QuadPart;
            hardware.disk.push_back(d);
        }
    }
    return true;
}

// ----------------------------- 主板 / GPU / OS （WMI） -----------------------------

bool DeviceInfo::InitMotherboardInfo() {
    std::string manufacturer, product, serial;
    if (WmiQuerySingleString(
            L"ROOT\\CIMV2",
            L"SELECT Manufacturer FROM Win32_BaseBoard",
            L"Manufacturer",
            manufacturer)) {
        hardware.motherboard.manufacturer = manufacturer;
    }

    if (WmiQuerySingleString(
            L"ROOT\\CIMV2",
            L"SELECT Product FROM Win32_BaseBoard",
            L"Product",
            product)) {
        hardware.motherboard.product = product;
    }

    if (WmiQuerySingleString(
            L"ROOT\\CIMV2",
            L"SELECT SerialNumber FROM Win32_BaseBoard",
            L"SerialNumber",
            serial)) {
        hardware.motherboard.serialNumber = serial;
    }

    return true;
}

bool DeviceInfo::InitGpuInfo() {
    std::string name, driverVersion, adapterRam;

    WmiQuerySingleString(
        L"ROOT\\CIMV2",
        L"SELECT Name FROM Win32_VideoController",
        L"Name",
        name);

    WmiQuerySingleString(
        L"ROOT\\CIMV2",
        L"SELECT DriverVersion FROM Win32_VideoController",
        L"DriverVersion",
        driverVersion);

    WmiQuerySingleString(
        L"ROOT\\CIMV2",
        L"SELECT AdapterRAM FROM Win32_VideoController",
        L"AdapterRAM",
        adapterRam);

    hardware.gpu.name = name;
    hardware.gpu.driverVersion = driverVersion;
    if (!adapterRam.empty()) {
        hardware.gpu.vramSize = static_cast<uint64_t>(_strtoui64(adapterRam.c_str(), nullptr, 10));
    } else {
        hardware.gpu.vramSize = 0;
    }

    return true;
}

bool DeviceInfo::InitOsInfo() {
    std::string caption, version, buildNumber;

    WmiQuerySingleString(
        L"ROOT\\CIMV2",
        L"SELECT Caption FROM Win32_OperatingSystem",
        L"Caption",
        caption);
    WmiQuerySingleString(
        L"ROOT\\CIMV2",
        L"SELECT Version FROM Win32_OperatingSystem",
        L"Version",
        version);
    WmiQuerySingleString(
        L"ROOT\\CIMV2",
        L"SELECT BuildNumber FROM Win32_OperatingSystem",
        L"BuildNumber",
        buildNumber);

    hardware.os.name = caption;
    hardware.os.version = version;
    hardware.os.buildNumber = buildNumber;

    return true;
}

// ----------------------------- WMI 辅助 -----------------------------

bool DeviceInfo::WmiInitCom() {
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        return false;
    }

    hr = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL);

    if (FAILED(hr) && hr != RPC_E_TOO_LATE) {
        CoUninitialize();
        return false;
    }
    return true;
}

void DeviceInfo::WmiUninitCom() {
    CoUninitialize();
}

bool DeviceInfo::WmiQuerySingleString(const wchar_t* wmiNamespace,
                                      const wchar_t* wqlQuery,
                                      const wchar_t* propertyName,
                                      std::string&  outValue) {
    outValue.clear();
    if (!WmiInitCom()) {
        return false;
    }

    HRESULT hr;
    IWbemLocator* pLoc = nullptr;
    IWbemServices* pSvc = nullptr;
    IEnumWbemClassObject* pEnumerator = nullptr;
    IWbemClassObject* pclsObj = nullptr;

    bool success = false;

    do {
        hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                              IID_IWbemLocator, (LPVOID*)&pLoc);
        if (FAILED(hr) || !pLoc) break;

        hr = pLoc->ConnectServer(
            _bstr_t(wmiNamespace),
            NULL,
            NULL,
            0,
            NULL,
            0,
            0,
            &pSvc
        );
        if (FAILED(hr) || !pSvc) break;

        hr = CoSetProxyBlanket(
            pSvc,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            NULL,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE
        );
        if (FAILED(hr)) break;

        hr = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t(wqlQuery),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator
        );
        if (FAILED(hr) || !pEnumerator) break;

        ULONG uReturn = 0;
        hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (0 == uReturn || FAILED(hr) || !pclsObj) break;

        VARIANT vtProp;
        VariantInit(&vtProp);
        hr = pclsObj->Get(propertyName, 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR) {
            // 修复：将 BSTR (UTF-16) 正确转换为 UTF-8
            BSTR bstr = vtProp.bstrVal;
            if (bstr != nullptr) {
                int len = WideCharToMultiByte(CP_UTF8, 0, bstr, -1, nullptr, 0, nullptr, nullptr);
                if (len > 0) {
                    std::vector<char> buffer(len);
                    WideCharToMultiByte(CP_UTF8, 0, bstr, -1, buffer.data(), len, nullptr, nullptr);
                    outValue = buffer.data();
                }
            }
            success = true;
        } else if (SUCCEEDED(hr) && (vtProp.vt == VT_I4 || vtProp.vt == VT_I2)) {
            outValue = std::to_string(vtProp.lVal);
            success = true;
        } else if (SUCCEEDED(hr) && vtProp.vt == VT_UI4) {
            outValue = std::to_string(vtProp.ulVal);
            success = true;
        }

        VariantClear(&vtProp);

    } while (false);

    if (pclsObj) pclsObj->Release();
    if (pEnumerator) pEnumerator->Release();
    if (pSvc) pSvc->Release();
    if (pLoc) pLoc->Release();

    WmiUninitCom();
    return success;
}

} // namespace chen



