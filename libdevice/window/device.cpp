/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
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
#include "window/device.h"
#define WINVER 0x0A00

// platform includes
#include <windows.h>

// standard includes
#include <cmath>
#include <thread>

 
// local includes
#include "keylayout.h"
#include "../libcommon/clog.h"
#include "misc.h"

namespace chen {
	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 键盘事件标志：非规范化键码（Keyboard Event Flag: Non-Normalized Key Code）
	*  
	*  该标志用于标识虚拟键码（VK）是否已经规范化到US English布局。
	*  如果未设置此标志，表示键码已经规范化，可以直接使用VK_TO_SCANCODE_MAP查找扫描码。
	*  如果设置此标志，表示键码未规范化，需要使用MapVirtualKey()进行转换。
	*  
	*  标志位说明（Flag Bit Description）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  Bit 0 (0x01): SS_KBE_FLAG_NON_NORMALIZED                      |
	*   |  - 0: 键码已规范化到US English布局，可直接查找扫描码            |
	*   |  - 1: 键码未规范化，需要使用MapVirtualKey()转换                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  Bit 1-7: 保留位，用于未来扩展                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note 该标志用于区分规范化和非规范化的虚拟键码
	*  @note 规范化键码可以提高扫描码转换的准确性
	*  
	*  使用示例：
	*  @code
	*  uint8_t flags = 0;  // 规范化键码
	*  if (!normalized) {
	*      flags |= SS_KBE_FLAG_NON_NORMALIZED;  // 非规范化键码
	*  }
	*  @endcode
	*/
	#define SS_KBE_FLAG_NON_NORMALIZED (0x01)

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 线程局部变量：最后已知的输入桌面句柄（Thread-Local Variable: Last Known Input Desktop Handle）
	*  
	*  该变量用于跟踪当前线程最后已知的输入桌面。当桌面切换时，需要重新同步桌面
	*  以确保输入事件发送到正确的桌面。使用thread_local确保每个线程有独立的副本。
	*  
	*  桌面切换检测（Desktop Switching Detection）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 发送输入事件失败（SendInput返回失败）                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 调用syncThreadDesktop()同步桌面                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 比较当前桌面与_lastKnownInputDesktop                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 如果桌面改变，更新_lastKnownInputDesktop并重试               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note 使用thread_local确保每个线程有独立的桌面跟踪
	*  @note 初始值为nullptr，表示尚未初始化
	*  @note 当桌面切换时，需要重新同步并重试发送输入
	*  
	*  桌面句柄说明（Desktop Handle Description）：
	*  - HDESK: Windows桌面对象句柄
	*  - nullptr: 表示未初始化或无效桌面
	*  - 有效句柄: 指向当前输入桌面的句柄
	*/
	thread_local HDESK _lastKnownInputDesktop = nullptr;
 
	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 发送输入事件实现（Send Input Event Implementation）
	*  
	*  该函数是send_input的实现，用于向Windows系统发送输入事件（鼠标或键盘）。
	*  包含了桌面同步和错误重试机制，确保输入事件能够正确发送。
	*  
	*  发送输入事件流程（Send Input Event Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 调用SendInput(1, &i, sizeof(INPUT))发送输入事件              |
	*   |     - 参数1: 输入事件数量（1个）                                 |
	*   |     - 参数2: INPUT结构体指针                                    |
	*   |     - 参数3: INPUT结构体大小                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 检查返回值（send == 1表示成功）                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 如果失败（send != 1）：                                      |
	*   |     a. 调用syncThreadDesktop()同步当前桌面                       |
	*   |     b. 获取当前桌面句柄hDesk                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 比较当前桌面与_lastKnownInputDesktop                        |
	*   |     - 如果不同，表示桌面已切换                                   |
	*   |     - 更新_lastKnownInputDesktop = hDesk                         |
	*   |     - 跳转到retry标签重试发送                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 如果桌面相同但仍然失败，记录警告日志                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  错误处理（Error Handling）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  情况1: 桌面切换导致失败                                          |
	*   |  - 自动同步桌面并重试                                             |
	*   |  - 最多重试一次（防止无限循环）                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  情况2: 其他原因导致失败                                          |
	*   |  - 记录警告日志：WARNING_EX_LOG("Couldn't send input")         |
	*   |  - 不抛出异常，继续执行                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param i Windows INPUT结构体引用，包含要发送的输入事件（鼠标或键盘）
	*  @note 该函数会处理桌面同步，确保输入发送到正确的桌面
	*  @note 如果桌面切换导致失败，会自动重试一次
	*  @note 使用goto retry实现重试逻辑（避免无限循环）
	*  @note SendInput返回成功发送的输入事件数量，应该为1
	*  
	*  使用示例：
	*  @code
	*  INPUT input = {};
	*  input.type = INPUT_MOUSE;
	*  input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	*  input.mi.dx = 100;
	*  input.mi.dy = 200;
	*  send_input(input);  // 发送鼠标左键按下事件
	*  @endcode
	*  
	*  @see syncThreadDesktop() - 同步线程桌面函数
	*  @see SendInput() - Windows API函数
	*/
	void send_input(INPUT &i) {
	retry:
		// 发送输入事件，期望返回1（成功发送1个输入事件）
		auto send = SendInput(1, &i, sizeof(INPUT));
		if (send != 1) {
			// 发送失败，同步桌面并检查是否桌面已切换
			auto hDesk = syncThreadDesktop();
			if (_lastKnownInputDesktop != hDesk) {
				// 桌面已切换，更新桌面句柄并重试
				_lastKnownInputDesktop = hDesk;
				goto retry;  // 重试发送
			}
			// 桌面未切换但仍然失败，记录警告日志
			WARNING_EX_LOG("Couldn't send input");
		}
	}
	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 绝对鼠标移动实现（Absolute Mouse Move Implementation）
	*  
	*  该函数用于将鼠标移动到屏幕的绝对位置。坐标会被映射到Windows虚拟坐标空间
	*  （0-65535），然后发送给Windows系统。
	*  
	*  绝对鼠标移动实现流程（Absolute Mouse Move Implementation Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 初始化INPUT结构体：i = {}                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 设置输入类型：i.type = INPUT_MOUSE                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 设置鼠标事件标志：                                            |
	*   |     mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE        |
	*   |     - MOUSEEVENTF_MOVE: 移动鼠标                                 |
	*   |     - MOUSEEVENTF_ABSOLUTE: 使用绝对坐标                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 坐标映射到虚拟坐标空间：                                      |
	*   |     fx = x * (65535.0 / 1920)  // 水平坐标映射                   |
	*   |     fy = y * (65535.0 / 1080)  // 垂直坐标映射                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 设置鼠标坐标：                                               |
	*   |     mi.dx = fx  // 水平虚拟坐标（0-65535）                       |
	*   |     mi.dy = fy  // 垂直虚拟坐标（0-65535）                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 发送输入事件：send_input(i)                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  坐标映射说明（Coordinate Mapping Description）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  屏幕坐标 -> 虚拟坐标映射：                                        |
	*   |  - 屏幕分辨率: 1920x1080（硬编码）                                |
	*   |  - 虚拟坐标范围: 0-65535                                          |
	*   |  - 映射公式: virtual = pixel * (65535 / screen_dimension)       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  示例：                                                           |
	*   |  - 屏幕中心(960, 540) -> 虚拟坐标(32767, 32767)                  |
	*   |  - 屏幕左上角(0, 0) -> 虚拟坐标(0, 0)                            |
	*   |  - 屏幕右下角(1919, 1079) -> 虚拟坐标(65534, 65534)             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  注意事项（Notes）：
	*  - MOUSEEVENTF_VIRTUALDESK标志被注释掉，因为它是将坐标映射到整个虚拟桌面，
	*    而不是主桌面。如果需要多显示器支持，可以启用此标志。
	*  - 当前实现使用硬编码的1920x1080分辨率，实际应用中应该动态获取屏幕分辨率。
	*  - 虚拟坐标空间范围是0-65535，对应屏幕的0到screen_dimension-1像素。
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用，保留用于未来扩展）
	*  @param x 鼠标X坐标（像素），范围0到屏幕宽度-1，当前假设屏幕宽度为1920
	*  @param y 鼠标Y坐标（像素），范围0到屏幕高度-1，当前假设屏幕高度为1080
	*  @note 坐标会被映射到Windows虚拟坐标空间（0-65535）
	*  @note 默认屏幕分辨率硬编码为1920x1080，可能需要动态获取
	*  @note MOUSEEVENTF_ABSOLUTE标志表示使用绝对坐标
	*  @note MOUSEEVENTF_VIRTUALDESK标志被注释，可启用以支持多显示器
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  abs_mouse(event, 960.0f, 540.0f);  // 移动到屏幕中心
	*  abs_mouse(event, 0.0f, 0.0f);      // 移动到屏幕左上角
	*  @endcode
	*  
	*  @see send_input() - 发送输入事件函数
	*  @see move_mouse() - 相对鼠标移动函数
	*/
	void abs_mouse(FEvent &input,   float x, float y) {
		// 初始化INPUT结构体
		INPUT i{};

		// 设置输入类型为鼠标输入
		i.type = INPUT_MOUSE;
		auto &mi = i.mi;

		// 设置鼠标事件标志：移动鼠标 + 使用绝对坐标
		mi.dwFlags =
			MOUSEEVENTF_MOVE |          // 移动鼠标
			MOUSEEVENTF_ABSOLUTE;       // 使用绝对坐标

		// 注意：MOUSEEVENTF_VIRTUALDESK标志被注释掉
		// MOUSEEVENTF_VIRTUALDESK会将坐标映射到整个虚拟桌面，而不是主桌面
		// 如果需要多显示器支持，可以启用此标志
		// MOUSEEVENTF_VIRTUALDESK;

		// 坐标映射到虚拟坐标空间（0-65535）
		// 注意：当前使用硬编码的1920x1080分辨率，实际应用中应该动态获取
		// TODO: 动态获取屏幕分辨率而不是硬编码
		double fx = x * (65535.0f / input.Data.MouseMove.PosX);  // 水平坐标映射：像素 -> 虚拟坐标
		double fy = y * (65535.0f / input.Data.MouseMove.PosY);  // 垂直坐标映射：像素 -> 虚拟坐标
		mi.dx = fx;  // 设置水平虚拟坐标
		mi.dy = fy;  // 设置垂直虚拟坐标

		// 发送输入事件
		send_input(i);
	}

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 相对鼠标移动实现（Relative Mouse Move Implementation）
	*  
	*  该函数用于相对移动鼠标位置。根据deltaX和deltaY的值移动鼠标，
	*  相对移动距离会被映射到虚拟坐标空间。
	*  
	*  相对鼠标移动实现流程（Relative Mouse Move Implementation Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 初始化INPUT结构体：i = {}                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 设置输入类型：i.type = INPUT_MOUSE                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 设置鼠标事件标志：                                            |
	*   |     mi.dwFlags = MOUSEEVENTF_MOVE                                |
	*   |     - 注意：不包含MOUSEEVENTF_ABSOLUTE，表示相对移动             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 相对移动距离映射到虚拟坐标空间：                              |
	*   |     fx = deltaX * (65535.0 / 1920)  // 水平相对移动映射          |
	*   |     fy = deltaY * (65535.0 / 1080)  // 垂直相对移动映射          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 设置鼠标相对移动距离：                                        |
	*   |     mi.dx = fx  // 水平相对移动（虚拟坐标单位）                    |
	*   |     mi.dy = fy  // 垂直相对移动（虚拟坐标单位）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 发送输入事件：send_input(i)                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  相对移动说明（Relative Move Description）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  相对移动方向：                                                  |
	*   |  - deltaX > 0: 向右移动                                           |
	*   |  - deltaX < 0: 向左移动                                           |
	*   |  - deltaY > 0: 向下移动                                           |
	*   |  - deltaY < 0: 向上移动                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  相对移动距离映射：                                              |
	*   |  - 屏幕分辨率: 1920x1080（硬编码）                                |
	*   |  - 虚拟坐标范围: 0-65535                                          |
	*   |  - 映射公式: virtual_delta = pixel_delta * (65535 / screen_dim)  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  示例：                                                           |
	*   |  - move_mouse(event, 10, 0): 向右移动10像素                      |
	*   |  - move_mouse(event, 0, -10): 向上移动10像素                     |
	*   |  - move_mouse(event, 10, 10): 向右下移动                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用，保留用于未来扩展）
	*  @param deltaX X方向的相对移动距离（像素），正数向右，负数向左
	*  @param deltaY Y方向的相对移动距离（像素），正数向下，负数向上
	*  @note 相对移动距离会被映射到虚拟坐标空间
	*  @note 使用MOUSEEVENTF_MOVE标志（不包含MOUSEEVENTF_ABSOLUTE）表示相对移动
	*  @note 默认屏幕分辨率硬编码为1920x1080，可能需要动态获取
	*  @note 相对移动不会改变鼠标的绝对位置，只是相对于当前位置移动
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  move_mouse(event, 10, -10);   // 向右移动10像素，向上移动10像素
	*  move_mouse(event, -5, 5);     // 向左移动5像素，向下移动5像素
	*  @endcode
	*  
	*  @see send_input() - 发送输入事件函数
	*  @see abs_mouse() - 绝对鼠标移动函数
	*/
	void move_mouse(FEvent &input, int deltaX, int deltaY) {
		// 初始化INPUT结构体
		INPUT i{};

		// 设置输入类型为鼠标输入
		i.type = INPUT_MOUSE;
		auto &mi = i.mi;

		// 设置鼠标事件标志：相对移动（不包含MOUSEEVENTF_ABSOLUTE）
		mi.dwFlags = MOUSEEVENTF_MOVE;  // 相对移动，不使用绝对坐标

		// 相对移动距离映射到虚拟坐标空间
		// 注意：当前使用硬编码的1920x1080分辨率，实际应用中应该动态获取
		// TODO: 动态获取屏幕分辨率而不是硬编码
		double fx = deltaX * (65535.0f / input.Data.MouseMove.PosX);  // 水平相对移动映射：像素 -> 虚拟坐标
		double fy = deltaY * (65535.0f / input.Data.MouseMove.PosY);  // 垂直相对移动映射：像素 -> 虚拟坐标
		mi.dx = fx;  // 设置水平相对移动距离（虚拟坐标单位）
		mi.dy = fy;  // 设置垂直相对移动距离（虚拟坐标单位）

		// 发送输入事件
		send_input(i);
	}


	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 鼠标按钮事件实现（Mouse Button Event Implementation）
	*  
	*  该函数用于发送鼠标按钮按下或释放事件。支持左键、中键、右键和扩展按钮（XButton1、XButton2）。
	*  按钮位置会被映射到虚拟坐标空间，然后发送给Windows系统。
	*  
	*  鼠标按钮事件实现流程（Mouse Button Event Implementation Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 初始化INPUT结构体：i = {}                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 设置输入类型：i.type = INPUT_MOUSE                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 坐标映射到虚拟坐标空间：                                      |
	*   |     fx = posX * (65535.0 / 1920)  // 水平坐标映射                 |
	*   |     fy = posY * (65535.0 / 1080)  // 垂直坐标映射                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 设置鼠标位置：                                               |
	*   |     mi.dx = fx  // 水平虚拟坐标                                   |
	*   |     mi.dy = fy  // 垂直虚拟坐标                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 根据按钮编号和释放标志设置事件标志：                          |
	*   |     - button 0: MOUSEEVENTF_LEFTDOWN/UP (左键)                  |
	*   |     - button 1: MOUSEEVENTF_MIDDLEDOWN/UP (中键)                |
	*   |     - button 2: MOUSEEVENTF_RIGHTDOWN/UP (右键)                 |
	*   |     - button 3: MOUSEEVENTF_XDOWN/UP + XBUTTON1 (扩展按钮1)    |
	*   |     - button 4+: MOUSEEVENTF_XDOWN/UP + XBUTTON2 (扩展按钮2)   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 对于扩展按钮（button 3+），设置mouseData字段：              |
	*   |     - button 3: mi.mouseData = XBUTTON1                         |
	*   |     - button 4+: mi.mouseData = XBUTTON2                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 发送输入事件：send_input(i)                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  按钮映射表（Button Mapping Table）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  button = 0: 左键 (Left Button)                                  |
	*   |  - 按下: MOUSEEVENTF_LEFTDOWN                                    |
	*   |  - 释放: MOUSEEVENTF_LEFTUP                                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  button = 1: 中键 (Middle Button, 滚轮按键)                      |
	*   |  - 按下: MOUSEEVENTF_MIDDLEDOWN                                  |
	*   |  - 释放: MOUSEEVENTF_MIDDLEUP                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  button = 2: 右键 (Right Button)                                 |
	*   |  - 按下: MOUSEEVENTF_RIGHTDOWN                                   |
	*   |  - 释放: MOUSEEVENTF_RIGHTUP                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  button = 3: 扩展按钮1 (XButton1, 侧键1)                         |
	*   |  - 按下: MOUSEEVENTF_XDOWN                                       |
	*   |  - 释放: MOUSEEVENTF_XUP                                         |
	*   |  - mouseData: XBUTTON1                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  button >= 4: 扩展按钮2 (XButton2, 侧键2)                        |
	*   |  - 按下: MOUSEEVENTF_XDOWN                                       |
	*   |  - 释放: MOUSEEVENTF_XUP                                         |
	*   |  - mouseData: XBUTTON2                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用，保留用于未来扩展）
	*  @param posX 鼠标X坐标（像素），用于设置鼠标位置，当前假设屏幕宽度为1920
	*  @param posY 鼠标Y坐标（像素），用于设置鼠标位置，当前假设屏幕高度为1080
	*  @param button 按钮编号，0=左键，1=中键，2=右键，3=扩展按钮1，4+=扩展按钮2
	*  @param release 是否释放按钮，true=释放，false=按下
	*  @note 按钮位置会被映射到虚拟坐标空间
	*  @note 扩展按钮（button 3+）需要设置mouseData字段
	*  @note 默认屏幕分辨率硬编码为1920x1080，可能需要动态获取
	*  @note 按钮编号与Web标准兼容（0=左键, 1=中键, 2=右键）
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  button_mouse(event, 100, 200, 0, false);  // 在(100,200)位置按下左键
	*  button_mouse(event, 100, 200, 0, true);   // 在(100,200)位置释放左键
	*  button_mouse(event, 100, 200, 2, false);  // 在(100,200)位置按下右键
	*  button_mouse(event, 100, 200, 3, false);  // 在(100,200)位置按下扩展按钮1
	*  @endcode
	*  
	*  @see send_input() - 发送输入事件函数
	*  @see abs_mouse() - 绝对鼠标移动函数
	*/
	void button_mouse(FEvent &input, int32_t posX, int32_t posY, int button, bool release)
	{
		// 初始化INPUT结构体
		INPUT i{};

		// 设置输入类型为鼠标输入
		i.type = INPUT_MOUSE;
		auto &mi = i.mi;

		// 坐标映射到虚拟坐标空间（0-65535）
		// 注意：当前使用硬编码的1920x1080分辨率，实际应用中应该动态获取
		// TODO: 动态获取屏幕分辨率而不是硬编码
		double fx = posX * (65535.0f / 1920);  // 水平坐标映射：像素 -> 虚拟坐标
		double fy = posY * (65535.0f / 1080);  // 垂直坐标映射：像素 -> 虚拟坐标
		mi.dx = fx;  // 设置水平虚拟坐标
		mi.dy = fy;  // 设置垂直虚拟坐标

		// 根据按钮编号和释放标志设置事件标志
		if (button == 0) {
			// 左键：按下或释放
			mi.dwFlags = release ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_LEFTDOWN;
		}
		else if (button == 1) {
			// 中键（滚轮按键）：按下或释放
			mi.dwFlags = release ? MOUSEEVENTF_MIDDLEUP : MOUSEEVENTF_MIDDLEDOWN;
		}
		else if (button == 2) {
			// 右键：按下或释放
			mi.dwFlags = release ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_RIGHTDOWN;
		}
		else if (button == 3) {
			// 扩展按钮1（XButton1，侧键1）：按下或释放
			mi.dwFlags = release ? MOUSEEVENTF_XUP : MOUSEEVENTF_XDOWN;
			mi.mouseData = XBUTTON1;  // 标识为扩展按钮1
		}
		else {
			// 扩展按钮2（XButton2，侧键2）：按下或释放
			mi.dwFlags = release ? MOUSEEVENTF_XUP : MOUSEEVENTF_XDOWN;
			mi.mouseData = XBUTTON2;  // 标识为扩展按钮2
		}

		// 发送输入事件
		send_input(i);
	}

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 垂直滚动实现（Vertical Scroll Implementation）
	*  
	*  该函数用于发送鼠标垂直滚动事件。正数向上滚动，负数向下滚动。
	*  滚动距离以WHEEL_DELTA（120）为单位。
	*  
	*  垂直滚动实现流程（Vertical Scroll Implementation Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 初始化INPUT结构体：i = {}                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 设置输入类型：i.type = INPUT_MOUSE                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 设置鼠标事件标志：                                            |
	*   |     mi.dwFlags = MOUSEEVENTF_WHEEL                               |
	*   |     - 表示垂直滚动事件                                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 设置滚动距离：                                                |
	*   |     mi.mouseData = distance                                       |
	*   |     - distance > 0: 向上滚动                                      |
	*   |     - distance < 0: 向下滚动                                      |
	*   |     - 通常以WHEEL_DELTA（120）为单位                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 发送输入事件：send_input(i)                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  滚动距离说明（Scroll Distance Description）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  滚动方向和距离：                                                  |
	*   |  - WHEEL_DELTA = 120（Windows定义的滚动单位）                    |
	*   |  - distance > 0: 向上滚动（远离用户）                              |
	*   |  - distance < 0: 向下滚动（靠近用户）                              |
	*   |  - |distance| 表示滚动幅度                                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  滚动单位说明：                                                    |
	*   |  - 120: 一个标准滚动单位（向上滚动）                                |
	*   |  - -120: 一个标准滚动单位（向下滚动）                              |
	*   |  - 240: 两个标准滚动单位（向上滚动）                                |
	*   |  - -240: 两个标准滚动单位（向下滚动）                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用，保留用于未来扩展）
	*  @param distance 滚动距离，正数向上滚动，负数向下滚动，单位为WHEEL_DELTA（120）
	*  @note 滚动距离通常以WHEEL_DELTA（120）为单位
	*  @note 使用MOUSEEVENTF_WHEEL标志表示垂直滚动
	*  @note 正数表示向上滚动（远离用户），负数表示向下滚动（靠近用户）
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  scroll(event, 120);   // 向上滚动一个单位
	*  scroll(event, -120);  // 向下滚动一个单位
	*  scroll(event, 240);   // 向上滚动两个单位
	*  @endcode
	*  
	*  @see send_input() - 发送输入事件函数
	*  @see hscroll() - 水平滚动函数
	*/
	void scroll(FEvent &input, int distance)
	{
		// 初始化INPUT结构体
		INPUT i{};

		// 设置输入类型为鼠标输入
		i.type = INPUT_MOUSE;
		auto &mi = i.mi;

		// 设置鼠标事件标志：垂直滚动
		mi.dwFlags = MOUSEEVENTF_WHEEL;  // 垂直滚动事件

		// 设置滚动距离（正数向上滚动，负数向下滚动，通常以WHEEL_DELTA=120为单位）
		mi.mouseData = distance;

		// 发送输入事件
		send_input(i);
	}

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 水平滚动实现（Horizontal Scroll Implementation）
	*  
	*  该函数用于发送鼠标水平滚动事件。正数向右滚动，负数向左滚动。
	*  滚动距离以WHEEL_DELTA（120）为单位。
	*  
	*  水平滚动实现流程（Horizontal Scroll Implementation Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 初始化INPUT结构体：i = {}                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 设置输入类型：i.type = INPUT_MOUSE                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 设置鼠标事件标志：                                            |
	*   |     mi.dwFlags = MOUSEEVENTF_HWHEEL                              |
	*   |     - 表示水平滚动事件                                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 设置滚动距离：                                                |
	*   |     mi.mouseData = distance                                       |
	*   |     - distance > 0: 向右滚动                                      |
	*   |     - distance < 0: 向左滚动                                      |
	*   |     - 通常以WHEEL_DELTA（120）为单位                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 发送输入事件：send_input(i)                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  滚动距离说明（Scroll Distance Description）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  滚动方向和距离：                                                  |
	*   |  - WHEEL_DELTA = 120（Windows定义的滚动单位）                    |
	*   |  - distance > 0: 向右滚动                                         |
	*   |  - distance < 0: 向左滚动                                         |
	*   |  - |distance| 表示滚动幅度                                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  滚动单位说明：                                                    |
	*   |  - 120: 一个标准滚动单位（向右滚动）                                |
	*   |  - -120: 一个标准滚动单位（向左滚动）                              |
	*   |  - 240: 两个标准滚动单位（向右滚动）                                |
	*   |  - -240: 两个标准滚动单位（向左滚动）                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用，保留用于未来扩展）
	*  @param distance 滚动距离，正数向右滚动，负数向左滚动，单位为WHEEL_DELTA（120）
	*  @note 滚动距离通常以WHEEL_DELTA（120）为单位
	*  @note 使用MOUSEEVENTF_HWHEEL标志表示水平滚动
	*  @note 正数表示向右滚动，负数表示向左滚动
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  hscroll(event, 120);   // 向右滚动一个单位
	*  hscroll(event, -120);  // 向左滚动一个单位
	*  hscroll(event, 240);   // 向右滚动两个单位
	*  @endcode
	*  
	*  @see send_input() - 发送输入事件函数
	*  @see scroll() - 垂直滚动函数
	*/
	void hscroll(FEvent &input, int distance) {
		// 初始化INPUT结构体
		INPUT i{};

		// 设置输入类型为鼠标输入
		i.type = INPUT_MOUSE;
		auto &mi = i.mi;

		// 设置鼠标事件标志：水平滚动
		mi.dwFlags = MOUSEEVENTF_HWHEEL;  // 水平滚动事件

		// 设置滚动距离（正数向右滚动，负数向左滚动，通常以WHEEL_DELTA=120为单位）
		mi.mouseData = distance;

		// 发送输入事件
		send_input(i);
	}



	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 键盘更新实现（Keyboard Update Implementation）
	*  
	*  该函数用于发送键盘按键按下或释放事件。支持虚拟键码（VK）和扫描码（Scan Code）
	*  两种输入方式，优先使用扫描码以提高游戏兼容性。还支持扩展键标志设置。
	*  
	*  键盘更新实现流程（Keyboard Update Implementation Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 初始化INPUT结构体：i = {}                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 设置输入类型：i.type = INPUT_KEYBOARD                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 扫描码转换逻辑：                                              |
	*   |     a. 如果键码已规范化（!flags & SS_KBE_FLAG_NON_NORMALIZED）   |
	*   |        - 使用VK_TO_SCANCODE_MAP查找扫描码                         |
	*   |     b. 如果键码未规范化，且非特殊键（VK_LWIN/RWIN/PAUSE除外）    |
	*   |        - 使用MapVirtualKey()转换扫描码                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 根据扫描码可用性设置输入方式：                                |
	*   |     - 如果有扫描码：使用KEYEVENTF_SCANCODE标志                    |
	*   |     - 如果没有扫描码：使用虚拟键码（ki.wVk = modcode）          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 设置扩展键标志（如果适用）：                                  |
	*   |     - 对于扩展键（VK_LWIN, VK_RWIN, VK_RMENU等）                |
	*   |     - 设置KEYEVENTF_EXTENDEDKEY标志                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 设置按键释放标志（如果release=true）：                       |
	*   |     - 设置KEYEVENTF_KEYUP标志                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 发送输入事件：send_input(i)                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  扫描码转换逻辑（Scan Code Conversion Logic）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  情况1: 规范化键码（normalized VK code）                         |
	*   |  - 检查：!(flags & SS_KBE_FLAG_NON_NORMALIZED)                  |
	*   |  - 操作：使用VK_TO_SCANCODE_MAP[modcode & 0xFF]查找扫描码       |
	*   |  - 优点：快速、准确（基于US English布局）                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  情况2: 非规范化键码（non-normalized VK code）                  |
	*   |  - 检查：flags & SS_KBE_FLAG_NON_NORMALIZED                     |
	*   |  - 排除：VK_LWIN, VK_RWIN, VK_PAUSE（特殊键，MapVirtualKey失效） |
	*   |  - 操作：使用MapVirtualKey(modcode, MAPVK_VK_TO_VSC)转换       |
	*   |  - 前提：客户端和主机使用相同的键盘布局                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  情况3: 无扫描码映射                                              |
	*   |  - 操作：使用虚拟键码（ki.wVk = modcode）                        |
	*   |  - 标志：不使用KEYEVENTF_SCANCODE                                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  扩展键列表（Extended Key List）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  需要设置KEYEVENTF_EXTENDEDKEY标志的扩展键：                     |
	*   |  - VK_LWIN, VK_RWIN (Windows键)                                 |
	*   |  - VK_RMENU (右Alt键)                                           |
	*   |  - VK_RCONTROL (右Ctrl键)                                       |
	*   |  - VK_INSERT, VK_DELETE (Insert, Delete键)                      |
	*   |  - VK_HOME, VK_END (Home, End键)                                |
	*   |  - VK_PRIOR, VK_NEXT (Page Up, Page Down键)                    |
	*   |  - VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT (方向键)                  |
	*   |  - VK_DIVIDE (数字键盘除号键)                                    |
	*   |  - VK_APPS (应用程序键)                                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  特殊键说明（Special Key Description）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  VK_LWIN, VK_RWIN, VK_PAUSE：                                   |
	*   |  - MapVirtualKey()无法正确转换这些键                             |
	*   |  - 需要特殊处理或使用虚拟键码                                    |
	*   |  - 原因：这些键在扫描码转换时存在问题                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  输入方式选择（Input Method Selection）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  扫描码方式（首选，游戏兼容性更好）：                             |
	*   |  - 使用KEYEVENTF_SCANCODE标志                                    |
	*   |  - ki.wScan = 扫描码值                                           |
	*   |  - 优点：不依赖于键盘布局，兼容性好                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  虚拟键码方式（备用）：                                           |
	*   |  - 不使用KEYEVENTF_SCANCODE标志                                  |
	*   |  - ki.wVk = 虚拟键码值                                           |
	*   |  - 优点：简单直接                                                 |
	*   |  - 缺点：依赖于键盘布局                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用，保留用于未来扩展）
	*  @param modcode 虚拟键码（Virtual Key Code）或扫描码，范围0x00-0xFF
	*  @param release 是否释放按键，true=释放，false=按下
	*  @param flags 键盘事件标志，SS_KBE_FLAG_NON_NORMALIZED表示非规范化键码
	*  @note 优先使用扫描码（KEYEVENTF_SCANCODE）以提高游戏兼容性
	*  @note 如果无法转换为扫描码，则使用虚拟键码（wVk）
	*  @note 某些扩展键需要设置KEYEVENTF_EXTENDEDKEY标志
	*  @note VK_LWIN, VK_RWIN, VK_PAUSE不能使用MapVirtualKey()转换
	*  @note 规范化键码使用VK_TO_SCANCODE_MAP查找，基于US English布局
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  keyboard_update(event, VK_SPACE, false, 0);  // 按下空格键（规范化）
	*  keyboard_update(event, VK_SPACE, true, 0);   // 释放空格键（规范化）
	*  keyboard_update(event, VK_A, false, SS_KBE_FLAG_NON_NORMALIZED);  // 按下A键（非规范化）
	*  keyboard_update(event, VK_LWIN, false, 0);  // 按下左Windows键（特殊键）
	*  @endcode
	*  
	*  @see send_input() - 发送输入事件函数
	*  @see VK_TO_SCANCODE_MAP - 虚拟键码到扫描码映射表
	*  @see MapVirtualKey() - Windows API函数，用于键码转换
	*  
	*  参考文档：
	*  @see https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags
	*/
	void keyboard_update(FEvent &input, uint16_t modcode, bool release, uint8_t flags)
	{
		// 初始化INPUT结构体
		INPUT i{};
		i.type = INPUT_KEYBOARD;  // 设置输入类型为键盘输入
		auto &ki = i.ki;

		// 扫描码转换逻辑
		// 如果客户端已经将VK码规范化到US English布局，可以直接使用映射表查找扫描码
		// 如果未规范化，需要使用当前键盘布局转换（假设客户端和主机使用相同布局）
		if (!(flags & SS_KBE_FLAG_NON_NORMALIZED)) {
			// 规范化键码：使用VK_TO_SCANCODE_MAP查找扫描码
			// 掩码掉扩展键字节，只使用低8位
			ki.wScan = VK_TO_SCANCODE_MAP[modcode & 0xFF];
		}
		else if (/*config::input.always_send_scancodes &&*/ modcode != VK_LWIN && modcode != VK_RWIN && modcode != VK_PAUSE) {
			// 非规范化键码：使用MapVirtualKey()转换扫描码
			// 注意：VK_LWIN, VK_RWIN, VK_PAUSE不能使用MapVirtualKey()转换（存在已知问题）
			// 参考：https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-mapvirtualkeya
			ki.wScan = MapVirtualKey(modcode, MAPVK_VK_TO_VSC);
		}

		// 根据扫描码可用性选择输入方式
		if (ki.wScan) {
			// 有扫描码映射：使用扫描码方式（优先选择，游戏兼容性更好）
			ki.dwFlags = KEYEVENTF_SCANCODE;
		}
		else {
			// 无扫描码映射或非规范化键码：使用虚拟键码方式（备用方案）
			ki.wVk = modcode;
		}

		// 设置扩展键标志（如果需要）
		// 参考：https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags
		switch (modcode) {
		case VK_LWIN:      // 左Windows键
		case VK_RWIN:      // 右Windows键
		case VK_RMENU:     // 右Alt键
		case VK_RCONTROL:  // 右Ctrl键
		case VK_INSERT:    // Insert键
		case VK_DELETE:    // Delete键
		case VK_HOME:      // Home键
		case VK_END:       // End键
		case VK_PRIOR:     // Page Up键
		case VK_NEXT:      // Page Down键
		case VK_UP:        // 上方向键
		case VK_DOWN:      // 下方向键
		case VK_LEFT:      // 左方向键
		case VK_RIGHT:     // 右方向键
		case VK_DIVIDE:    // 数字键盘除号键
		case VK_APPS:      // 应用程序键
			// 这些键需要设置扩展键标志
			ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
			break;
		default:
			break;
		}

		// 设置按键释放标志（如果需要）
		if (release) {
			ki.dwFlags |= KEYEVENTF_KEYUP;  // 按键释放
		}

		// 发送输入事件
		send_input(i);
	}

}