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
#ifndef _C_WINDOW_DEVICE_H_
#define _C_WINDOW_DEVICE_H_
#include "../libcommon/cinput_device_event.h"
#include "misc.h"

namespace chen
{
	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 发送输入事件（Send Input Event）
	*  
	*  该函数用于向Windows系统发送输入事件。它是对Windows API SendInput的封装，
	*  包含了桌面同步和错误重试机制。
	*  
	*  Windows INPUT 数据结构（Windows INPUT Data Structure）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  type (DWORD, 4 bytes)                                         |
	*   |  INPUT_MOUSE or INPUT_KEYBOARD                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   :                        union                                    :
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi (MOUSEINPUT) - if type == INPUT_MOUSE                    | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | ki (KEYBDINPUT) - if type == INPUT_KEYBOARD                 | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  发送流程（Send Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. Call SendInput(1, &i, sizeof(INPUT))                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. Check if send failed (send != 1)                           |
	*   |     - If yes, sync desktop and retry                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. If desktop changed, update _lastKnownInputDesktop          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. Retry send if desktop changed                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param i Windows INPUT结构体引用，包含要发送的输入事件（鼠标或键盘）
	*  @note 该函数会处理桌面同步，确保输入发送到正确的桌面
	*  @note 如果发送失败，会自动同步桌面并重试
	*  @note 使用thread_local变量_lastKnownInputDesktop跟踪当前桌面
	*  
	*  使用示例：
	*  @code
	*  INPUT input = {};
	*  input.type = INPUT_MOUSE;
	*  // ... 设置鼠标输入 ...
	*  send_input(input);
	*  @endcode
	*/
	void send_input(INPUT &i);

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 绝对鼠标移动（Absolute Mouse Move）
	*  
	*  该函数用于将鼠标移动到屏幕的绝对位置。坐标会被映射到虚拟坐标空间
	*  （0-65535），然后发送给Windows系统。
	*  
	*  绝对鼠标坐标映射（Absolute Mouse Coordinate Mapping）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  INPUT structure:                                               |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | type = INPUT_MOUSE                                          | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE       | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.dx = x * (65535.0 / screen_width)                        | |
	*   |  |       (mapped to virtual coordinate space 0-65535)          | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.dy = y * (65535.0 / screen_height)                       | |
	*   |  |       (mapped to virtual coordinate space 0-65535)          | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  坐标转换公式（Coordinate Conversion Formula）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  virtual_x = x * (65535.0 / 1920)                               |
	*   |  virtual_y = y * (65535.0 / 1080)                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  其中：                                                         |
	*   |  - x, y: 输入坐标（像素）                                        |
	*   |  - 1920, 1080: 屏幕分辨率（硬编码，可能需要动态获取）            |
	*   |  - 65535: Windows虚拟坐标空间的最大值                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用）
	*  @param x 鼠标X坐标（像素），范围取决于屏幕宽度
	*  @param y 鼠标Y坐标（像素），范围取决于屏幕高度
	*  @note 坐标会被映射到Windows虚拟坐标空间（0-65535）
	*  @note 默认屏幕分辨率硬编码为1920x1080，可能需要动态获取
	*  @note MOUSEEVENTF_ABSOLUTE标志表示使用绝对坐标
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  abs_mouse(event, 960.0f, 540.0f);  // 移动到屏幕中心
	*  @endcode
	*/
	void abs_mouse(FEvent &input, float x, float y);

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 相对鼠标移动（Relative Mouse Move）
	*  
	*  该函数用于相对移动鼠标位置。根据deltaX和deltaY的值移动鼠标，
	*  坐标会被映射到虚拟坐标空间。
	*  
	*  相对鼠标移动格式（Relative Mouse Move Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  INPUT structure:                                               |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | type = INPUT_MOUSE                                          | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.dwFlags = MOUSEEVENTF_MOVE                               | |
	*   |  |       (relative move, not absolute)                         | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.dx = deltaX * (65535.0 / 1920)                           | |
	*   |  |       (relative movement in X direction)                    | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.dy = deltaY * (65535.0 / 1080)                           | |
	*   |  |       (relative movement in Y direction)                    | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用）
	*  @param deltaX X方向的相对移动距离（像素），正数向右，负数向左
	*  @param deltaY Y方向的相对移动距离（像素），正数向下，负数向上
	*  @note 相对移动距离会被映射到虚拟坐标空间
	*  @note 使用MOUSEEVENTF_MOVE标志（不包含MOUSEEVENTF_ABSOLUTE）表示相对移动
	*  @note 默认屏幕分辨率硬编码为1920x1080，可能需要动态获取
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  move_mouse(event, 10, -10);  // 向右移动10像素，向上移动10像素
	*  @endcode
	*/
	void move_mouse(FEvent &input, int deltaX, int deltaY);

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 鼠标按钮事件（Mouse Button Event）
	*  
	*  该函数用于发送鼠标按钮按下或释放事件。支持左键、中键、右键和扩展按钮。
	*  
	*  鼠标按钮事件格式（Mouse Button Event Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  INPUT structure:                                               |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | type = INPUT_MOUSE                                          | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.dx = posX * (65535.0 / 1920)                             | |
	*   |  | mi.dy = posY * (65535.0 / 1080)                             | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.dwFlags = button event flags                             | |
	*   |  |       - button 0: MOUSEEVENTF_LEFTDOWN/UP                   | |
	*   |  |       - button 1: MOUSEEVENTF_MIDDLEDOWN/UP                 | |
	*   |  |       - button 2: MOUSEEVENTF_RIGHTDOWN/UP                  | |
	*   |  |       - button 3: MOUSEEVENTF_XDOWN/UP + XBUTTON1           | |
	*   |  |       - button 4+: MOUSEEVENTF_XDOWN/UP + XBUTTON2          | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.mouseData = XBUTTON1 or XBUTTON2 (for button 3+)        | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  按钮映射表（Button Mapping Table）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  button = 0: 左键 (Left Button)                                |
	*   |  button = 1: 中键 (Middle Button)                              |
	*   |  button = 2: 右键 (Right Button)                               |
	*   |  button = 3: 扩展按钮1 (XButton1)                              |
	*   |  button >= 4: 扩展按钮2 (XButton2)                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用）
	*  @param posX 鼠标X坐标（像素），用于设置鼠标位置
	*  @param posY 鼠标Y坐标（像素），用于设置鼠标位置
	*  @param button 按钮编号，0=左键，1=中键，2=右键，3+=扩展按钮
	*  @param release 是否释放按钮，true=释放，false=按下
	*  @note 按钮位置会被映射到虚拟坐标空间
	*  @note 扩展按钮（button 3+）需要设置mouseData字段
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  button_mouse(event, 100, 200, 0, false);  // 在(100,200)位置按下左键
	*  button_mouse(event, 100, 200, 0, true);   // 在(100,200)位置释放左键
	*  @endcode
	*/
	void button_mouse(FEvent &input, int32_t posX, int32_t posY, int button, bool release);

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 垂直滚动（Vertical Scroll）
	*  
	*  该函数用于发送鼠标垂直滚动事件。正数向上滚动，负数向下滚动。
	*  
	*  垂直滚动格式（Vertical Scroll Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  INPUT structure:                                               |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | type = INPUT_MOUSE                                          | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.dwFlags = MOUSEEVENTF_WHEEL                               |
	*   |  |       (vertical scroll wheel event)                         | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.mouseData = distance (scroll distance in WHEEL_DELTA)    | |
	*   |  |       positive: scroll up                                    |
	*   |  |       negative: scroll down                                  |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  滚动距离说明（Scroll Distance Description）：
	*  - WHEEL_DELTA = 120（Windows定义的滚动单位）
	*  - distance > 0: 向上滚动
	*  - distance < 0: 向下滚动
	*  - distance的绝对值表示滚动幅度
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用）
	*  @param distance 滚动距离，正数向上滚动，负数向下滚动，单位为WHEEL_DELTA
	*  @note 滚动距离通常以WHEEL_DELTA（120）为单位
	*  @note 使用MOUSEEVENTF_WHEEL标志表示垂直滚动
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  scroll(event, 120);   // 向上滚动一个单位
	*  scroll(event, -120);  // 向下滚动一个单位
	*  @endcode
	*/
	void scroll(FEvent &input, int distance);

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 水平滚动（Horizontal Scroll）
	*  
	*  该函数用于发送鼠标水平滚动事件。正数向右滚动，负数向左滚动。
	*  
	*  水平滚动格式（Horizontal Scroll Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  INPUT structure:                                               |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | type = INPUT_MOUSE                                          | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.dwFlags = MOUSEEVENTF_HWHEEL                              |
	*   |  |       (horizontal scroll wheel event)                       | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | mi.mouseData = distance (scroll distance in WHEEL_DELTA)    | |
	*   |  |       positive: scroll right                                 |
	*   |  |       negative: scroll left                                  |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  滚动距离说明（Scroll Distance Description）：
	*  - WHEEL_DELTA = 120（Windows定义的滚动单位）
	*  - distance > 0: 向右滚动
	*  - distance < 0: 向左滚动
	*  - distance的绝对值表示滚动幅度
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用）
	*  @param distance 滚动距离，正数向右滚动，负数向左滚动，单位为WHEEL_DELTA
	*  @note 滚动距离通常以WHEEL_DELTA（120）为单位
	*  @note 使用MOUSEEVENTF_HWHEEL标志表示水平滚动
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  hscroll(event, 120);   // 向右滚动一个单位
	*  hscroll(event, -120);  // 向左滚动一个单位
	*  @endcode
	*/
	void hscroll(FEvent &input, int distance);

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 键盘更新（Keyboard Update）
	*  
	*  该函数用于发送键盘按键按下或释放事件。支持虚拟键码（VK）和扫描码（Scan Code）
	*  两种输入方式，优先使用扫描码以提高游戏兼容性。
	*  
	*  键盘输入格式（Keyboard Input Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  INPUT structure:                                               |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | type = INPUT_KEYBOARD                                        |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | ki.wScan (WORD, 2 bytes) - scan code (if available)         |
	*   |  | ki.wVk (WORD, 2 bytes) - virtual key code (if no scan code) | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | ki.dwFlags (DWORD, 4 bytes) - keyboard event flags          |
	*   |  |       KEYEVENTF_SCANCODE: use scan code                      |
	*   |  |       KEYEVENTF_EXTENDEDKEY: extended key flag              |
	*   |  |       KEYEVENTF_KEYUP: key release (if release=true)        |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | ki.time (DWORD) - time stamp                                 |
	*   |  | ki.dwExtraInfo (ULONG_PTR) - extra info                      |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  扫描码转换流程（Scan Code Conversion Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. Check if key is normalized (flags & SS_KBE_FLAG_NON_NORMALIZED) |
	*   |     - If normalized, use VK_TO_SCANCODE_MAP lookup             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. If non-normalized, use MapVirtualKey() to convert          |
	*   |     - Skip for VK_LWIN, VK_RWIN, VK_PAUSE (special cases)      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. If scan code available, use KEYEVENTF_SCANCODE             |
	*   |     - Otherwise, use virtual key code (wVk)                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. Set KEYEVENTF_EXTENDEDKEY for extended keys                |
	*   |     - VK_LWIN, VK_RWIN, VK_RMENU, VK_RCONTROL, etc.            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. Set KEYEVENTF_KEYUP if release=true                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  扩展键列表（Extended Key List）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  Extended keys that require KEYEVENTF_EXTENDEDKEY:             |
	*   |  - VK_LWIN, VK_RWIN (Windows keys)                             |
	*   |  - VK_RMENU, VK_RCONTROL (Right Alt/Ctrl)                      |
	*   |  - VK_INSERT, VK_DELETE, VK_HOME, VK_END                       |
	*   |  - VK_PRIOR, VK_NEXT (Page Up/Down)                            |
	*   |  - VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT (Arrow keys)              |
	*   |  - VK_DIVIDE, VK_APPS (Numpad Divide, Apps key)                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param input FEvent事件引用，包含输入事件信息（当前未使用）
	*  @param modcode 虚拟键码（Virtual Key Code）或扫描码，范围0x00-0xFF
	*  @param release 是否释放按键，true=释放，false=按下
	*  @param flags 键盘事件标志，SS_KBE_FLAG_NON_NORMALIZED表示非规范化键码
	*  @note 优先使用扫描码（KEYEVENTF_SCANCODE）以提高游戏兼容性
	*  @note 如果无法转换为扫描码，则使用虚拟键码（wVk）
	*  @note 某些扩展键需要设置KEYEVENTF_EXTENDEDKEY标志
	*  
	*  使用示例：
	*  @code
	*  FEvent event;
	*  keyboard_update(event, VK_SPACE, false, 0);  // 按下空格键
	*  keyboard_update(event, VK_SPACE, true, 0);   // 释放空格键
	*  @endcode
	*/
	void keyboard_update(FEvent &input, uint16_t modcode, bool release, uint8_t flags);
	 

}


#endif // _C_WINDOW_DEVICE_H_