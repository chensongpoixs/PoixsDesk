/***********************************************************************************************
					created: 		2026-03-18

					author:			chensong

					purpose:		registry_helper (Windows 注册表读写工具类封装)
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
#include <windows.h>
#include <vector>

namespace chen {

// 简单注册表读写工具类：
// - 支持读取/写入字符串 (REG_SZ)
// - 支持读取/写入 DWORD  (REG_DWORD)
// - 扩展：支持删除键/值、读写二进制、列举子键与键值
class RegistryHelper {
public:
	// 读取字符串（REG_SZ）
	// 例如：
	//   std::wstring value;
	//   RegistryHelper::ReadString(HKEY_CURRENT_USER, L"Software\\MyApp", L"MyValue", value);
	static bool ReadString(HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		std::wstring& outValue);

	// 写入字符串（REG_SZ），不存在则创建 Key
	static bool WriteString(HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		const std::wstring& value);

	// 读取 DWORD（REG_DWORD）
	static bool ReadDword(HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		DWORD& outValue);

	// 写入 DWORD（REG_DWORD），不存在则创建 Key
	static bool WriteDword(HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		DWORD value);

	// 读取二进制数据（REG_BINARY）
	static bool ReadBinary(HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		std::vector<BYTE>& outData);

	// 写入二进制数据（REG_BINARY）
	static bool WriteBinary(HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName,
		const std::vector<BYTE>& data);

	// 删除指定值（不删除 Key）
	static bool DeleteValue(HKEY root,
		const std::wstring& subKey,
		const std::wstring& valueName);

	// 删除 Key（不递归，只能删除无子键的 Key）
	static bool DeleteKey(HKEY root,
		const std::wstring& subKey);

	// 列举子键名称
	static bool EnumSubKeys(HKEY root,
		const std::wstring& subKey,
		std::vector<std::wstring>& outSubKeys);

	// 列举当前 Key 下的所有 Value 名称
	static bool EnumValues(HKEY root,
		const std::wstring& subKey,
		std::vector<std::wstring>& outValueNames);
};

} // namespace chen



