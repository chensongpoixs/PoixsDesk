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



#define SS_KBE_FLAG_NON_NORMALIZED (0x01)


	thread_local HDESK _lastKnownInputDesktop = nullptr;
 
	void send_input(INPUT &i) {
	retry:
		auto send = SendInput(1, &i, sizeof(INPUT));
		if (send != 1) {
			auto hDesk = syncThreadDesktop();
			if (_lastKnownInputDesktop != hDesk) {
				_lastKnownInputDesktop = hDesk;
				goto retry;
			}
			WARNING_EX_LOG( "Couldn't send input");
		}
	}
	void abs_mouse(FEvent &input,   float x, float y) {
		INPUT i{};

		i.type = INPUT_MOUSE;
		auto &mi = i.mi;

		mi.dwFlags =
			MOUSEEVENTF_MOVE |
			MOUSEEVENTF_ABSOLUTE;// |

			// MOUSEEVENTF_VIRTUALDESK maps to the entirety of the desktop rather than the primary desktop
			//MOUSEEVENTF_VIRTUALDESK;

		//auto scaled_x = std::lround((x + touch_port.offset_x) * ((float)target_touch_port.width / (float)touch_port.width));
		//auto scaled_y = std::lround((y + touch_port.offset_y) * ((float)target_touch_port.height / (float)touch_port.height));
		double fx = x * (65535.0f / 1920);
		double fy = y * (65535.0f / 1080);
		mi.dx = fx;
		mi.dy = fy;
		//mi.dx = x; //scaled_x;
		//mi.dy = y; //scaled_y;

		send_input(i);
	}

	void move_mouse(FEvent &input, int deltaX, int deltaY) {
		INPUT i{};

		i.type = INPUT_MOUSE;
		auto &mi = i.mi;

		mi.dwFlags = MOUSEEVENTF_MOVE;
		//mi.dx = deltaX;
		//mi.dy = deltaY;
		double fx = deltaX * (65535.0f / 1920);
		double fy = deltaY * (65535.0f / 1080);
		mi.dx = fx;
		mi.dy = fy;
		send_input(i);
	}


	void button_mouse(FEvent &input, int32_t posX, int32_t posY, int button, bool release)
	{
		INPUT i{};

		i.type = INPUT_MOUSE;
		auto &mi = i.mi;

		double fx = posX * (65535.0f / 1920);
		double fy = posY * (65535.0f / 1080);
		mi.dx = fx;
		mi.dy = fy;
		if (button == 0) {
			mi.dwFlags = release ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_LEFTDOWN;
		}
		else if (button == 1) {
			mi.dwFlags = release ? MOUSEEVENTF_MIDDLEUP : MOUSEEVENTF_MIDDLEDOWN;
		}
		else if (button == 2) {
			mi.dwFlags = release ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_RIGHTDOWN;
		}
		else if (button == 3) {
			mi.dwFlags = release ? MOUSEEVENTF_XUP : MOUSEEVENTF_XDOWN;
			mi.mouseData = XBUTTON1;
		}
		else {
			mi.dwFlags = release ? MOUSEEVENTF_XUP : MOUSEEVENTF_XDOWN;
			mi.mouseData = XBUTTON2;
		}

		send_input(i);
	}

	void scroll(FEvent &input, int distance)
	{
		INPUT i{};

		i.type = INPUT_MOUSE;
		auto &mi = i.mi;

		mi.dwFlags = MOUSEEVENTF_WHEEL;
		mi.mouseData = distance;

		send_input(i);
	}

	void hscroll(FEvent &input, int distance) {
		INPUT i{};

		i.type = INPUT_MOUSE;
		auto &mi = i.mi;

		mi.dwFlags = MOUSEEVENTF_HWHEEL;
		mi.mouseData = distance;

		send_input(i);
	}



	void keyboard_update(FEvent &input, uint16_t modcode, bool release, uint8_t flags)
	{
		INPUT i{};
		i.type = INPUT_KEYBOARD;
		auto &ki = i.ki;

		// If the client did not normalize this VK code to a US English layout, we can't accurately convert it to a scancode.
		// If we're set to always send scancodes, we will use the current keyboard layout to convert to a scancode. This will
		// assume the client and host have the same keyboard layout, but it's probably better than always using US English.
		if (!(flags & SS_KBE_FLAG_NON_NORMALIZED)) {
			// Mask off the extended key byte
			ki.wScan = VK_TO_SCANCODE_MAP[modcode & 0xFF];
		}
		else if (/*config::input.always_send_scancodes &&*/ modcode != VK_LWIN && modcode != VK_RWIN && modcode != VK_PAUSE) {
			// For some reason, MapVirtualKey(VK_LWIN, MAPVK_VK_TO_VSC) doesn't seem to work :/
			ki.wScan = MapVirtualKey(modcode, MAPVK_VK_TO_VSC);
		}

		// If we can map this to a scancode, send it as a scancode for maximum game compatibility.
		if (ki.wScan) {
			ki.dwFlags = KEYEVENTF_SCANCODE;
		}
		else {
			// If there is no scancode mapping or it's non-normalized, send it as a regular VK event.
			ki.wVk = modcode;
		}

		// https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags
		switch (modcode) {
		case VK_LWIN:
		case VK_RWIN:
		case VK_RMENU:
		case VK_RCONTROL:
		case VK_INSERT:
		case VK_DELETE:
		case VK_HOME:
		case VK_END:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_DIVIDE:
		case VK_APPS:
			ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
			break;
		default:
			break;
		}

		if (release) {
			ki.dwFlags |= KEYEVENTF_KEYUP;
		}

		send_input(i);
	}

}