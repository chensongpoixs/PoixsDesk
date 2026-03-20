/***********************************************************************************************
					created: 		2026-03-18

					author:			chensong

					purpose:		registry_helper (Windows 注册表读写工具类实现)
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

#include "RegistryHelper.h"

namespace chen {

bool RegistryHelper::ReadString(HKEY root,
	const std::wstring& subKey,
	const std::wstring& valueName,
	std::wstring& outValue)
{
	outValue.clear();

	HKEY hKey = nullptr;
	LONG ret = RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ, &hKey);
	if (ret != ERROR_SUCCESS)
	{
		return false;
	}

	DWORD type = 0;
	DWORD cbData = 0;

	// 先查询需要的缓冲区大小
	ret = RegQueryValueExW(hKey, valueName.c_str(), nullptr, &type, nullptr, &cbData);
	if (ret != ERROR_SUCCESS || type != REG_SZ)
	{
		RegCloseKey(hKey);
		return false;
	}

	std::wstring buffer(cbData / sizeof(wchar_t), L'\0');
	ret = RegQueryValueExW(hKey, valueName.c_str(), nullptr, &type,
		reinterpret_cast<LPBYTE>(&buffer[0]), &cbData);

	RegCloseKey(hKey);

	if (ret != ERROR_SUCCESS || type != REG_SZ)
	{
		return false;
	}

	// 去掉多余的 '\0'
	if (!buffer.empty() && buffer.back() == L'\0')
	{
		buffer.pop_back();
	}

	outValue.swap(buffer);
	return true;
}

bool RegistryHelper::WriteString(HKEY root,
	const std::wstring& subKey,
	const std::wstring& valueName,
	const std::wstring& value)
{
	HKEY hKey = nullptr;
	DWORD dwDisp = 0;
	LONG ret = RegCreateKeyExW(root,
		subKey.c_str(),
		0,
		nullptr,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		nullptr,
		&hKey,
		&dwDisp);
	if (ret != ERROR_SUCCESS)
	{
		return false;
	}

	const wchar_t* data = value.c_str();
	DWORD cbData = static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t));

	ret = RegSetValueExW(hKey,
		valueName.c_str(),
		0,
		REG_SZ,
		reinterpret_cast<const BYTE*>(data),
		cbData);

	RegCloseKey(hKey);
	return (ret == ERROR_SUCCESS);
}

bool RegistryHelper::ReadDword(HKEY root,
	const std::wstring& subKey,
	const std::wstring& valueName,
	DWORD& outValue)
{
	outValue = 0;

	HKEY hKey = nullptr;
	LONG ret = RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ, &hKey);
	if (ret != ERROR_SUCCESS)
	{
		return false;
	}

	DWORD type = 0;
	DWORD cbData = sizeof(DWORD);

	ret = RegQueryValueExW(hKey,
		valueName.c_str(),
		nullptr,
		&type,
		reinterpret_cast<LPBYTE>(&outValue),
		&cbData);

	RegCloseKey(hKey);

	return (ret == ERROR_SUCCESS && type == REG_DWORD);
}

bool RegistryHelper::WriteDword(HKEY root,
	const std::wstring& subKey,
	const std::wstring& valueName,
	DWORD value)
{
	HKEY hKey = nullptr;
	DWORD dwDisp = 0;
	LONG ret = RegCreateKeyExW(root,
		subKey.c_str(),
		0,
		nullptr,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		nullptr,
		&hKey,
		&dwDisp);
	if (ret != ERROR_SUCCESS)
	{
		return false;
	}

	ret = RegSetValueExW(hKey,
		valueName.c_str(),
		0,
		REG_DWORD,
		reinterpret_cast<const BYTE*>(&value),
		sizeof(DWORD));

	RegCloseKey(hKey);
	return (ret == ERROR_SUCCESS);
}

bool RegistryHelper::ReadBinary(HKEY root,
	const std::wstring& subKey,
	const std::wstring& valueName,
	std::vector<BYTE>& outData)
{
	outData.clear();

	HKEY hKey = nullptr;
	LONG ret = RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ, &hKey);
	if (ret != ERROR_SUCCESS)
	{
		return false;
	}

	DWORD type = 0;
	DWORD cbData = 0;

	// 先获取大小
	ret = RegQueryValueExW(hKey,
		valueName.c_str(),
		nullptr,
		&type,
		nullptr,
		&cbData);
	if (ret != ERROR_SUCCESS || type != REG_BINARY || cbData == 0)
	{
		RegCloseKey(hKey);
		return false;
	}

	outData.resize(cbData);
	ret = RegQueryValueExW(hKey,
		valueName.c_str(),
		nullptr,
		&type,
		outData.data(),
		&cbData);

	RegCloseKey(hKey);
	return (ret == ERROR_SUCCESS && type == REG_BINARY);
}

bool RegistryHelper::WriteBinary(HKEY root,
	const std::wstring& subKey,
	const std::wstring& valueName,
	const std::vector<BYTE>& data)
{
	HKEY hKey = nullptr;
	DWORD dwDisp = 0;
	LONG ret = RegCreateKeyExW(root,
		subKey.c_str(),
		0,
		nullptr,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		nullptr,
		&hKey,
		&dwDisp);
	if (ret != ERROR_SUCCESS)
	{
		return false;
	}

	ret = RegSetValueExW(hKey,
		valueName.c_str(),
		0,
		REG_BINARY,
		data.empty() ? nullptr : data.data(),
		static_cast<DWORD>(data.size()));

	RegCloseKey(hKey);
	return (ret == ERROR_SUCCESS);
}

bool RegistryHelper::DeleteValue(HKEY root,
	const std::wstring& subKey,
	const std::wstring& valueName)
{
	HKEY hKey = nullptr;
	LONG ret = RegOpenKeyExW(root, subKey.c_str(), 0, KEY_SET_VALUE, &hKey);
	if (ret != ERROR_SUCCESS)
	{
		return false;
	}

	ret = RegDeleteValueW(hKey, valueName.c_str());
	RegCloseKey(hKey);
	return (ret == ERROR_SUCCESS);
}

bool RegistryHelper::DeleteKey(HKEY root,
	const std::wstring& subKey)
{
	// 非递归删除（要求该 Key 没有子项）
	LONG ret = RegDeleteKeyW(root, subKey.c_str());
	return (ret == ERROR_SUCCESS);
}

bool RegistryHelper::EnumSubKeys(HKEY root,
	const std::wstring& subKey,
	std::vector<std::wstring>& outSubKeys)
{
	outSubKeys.clear();

	HKEY hKey = nullptr;
	LONG ret = RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ, &hKey);
	if (ret != ERROR_SUCCESS)
	{
		return false;
	}

	DWORD index = 0;
	wchar_t nameBuf[256];
	DWORD nameLen = 0;

	while (true)
	{
		nameLen = static_cast<DWORD>(sizeof(nameBuf) / sizeof(wchar_t));
		FILETIME ft = {};
		ret = RegEnumKeyExW(hKey,
			index,
			nameBuf,
			&nameLen,
			nullptr,
			nullptr,
			nullptr,
			&ft);

		if (ret == ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		else if (ret != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return false;
		}

		outSubKeys.emplace_back(nameBuf, nameLen);
		++index;
	}

	RegCloseKey(hKey);
	return true;
}

bool RegistryHelper::EnumValues(HKEY root,
	const std::wstring& subKey,
	std::vector<std::wstring>& outValueNames)
{
	outValueNames.clear();

	HKEY hKey = nullptr;
	LONG ret = RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ, &hKey);
	if (ret != ERROR_SUCCESS)
	{
		return false;
	}

	DWORD index = 0;
	wchar_t nameBuf[256];
	DWORD nameLen = 0;

	while (true)
	{
		nameLen = static_cast<DWORD>(sizeof(nameBuf) / sizeof(wchar_t));
		ret = RegEnumValueW(hKey,
			index,
			nameBuf,
			&nameLen,
			nullptr,
			nullptr,
			nullptr,
			nullptr);

		if (ret == ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		else if (ret != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return false;
		}

		outValueNames.emplace_back(nameBuf, nameLen);
		++index;
	}

	RegCloseKey(hKey);
	return true;
}

} // namespace chen



