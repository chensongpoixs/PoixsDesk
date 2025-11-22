/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		input_device_event
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

#ifndef _C_INPUT_DEVICE_EVENT_H_
#define _C_INPUT_DEVICE_EVENT_H_
//#include <winnt.h>
#include "cnet_types.h"
#include "api/data_channel_interface.h"
#include <map>
#include "cprotocol.h"
namespace chen {
	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 鼠标按键枚举命名空间（Mouse Buttons Enumeration Namespace）
	*  
	*  EMouseButtons命名空间定义了鼠标按键的枚举类型。用于标识不同的鼠标按键。
	*  
	*  鼠标按键枚举值（Mouse Button Enumeration Values）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  Left = 0: 鼠标左键                                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  Middle = 1: 鼠标中键                                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  Right = 2: 鼠标右键                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  Thumb01 = 3: 鼠标侧键1（拇指键1）                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  Thumb02 = 4: 鼠标侧键2（拇指键2）                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  Invalid = 5: 无效按键                                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  鼠标按键映射（Mouse Button Mapping）：
	*  - Web环境按键映射：0=左键, 1=中键, 2=右键
	*  - Windows消息映射：WM_LBUTTONDOWN, WM_MBUTTONDOWN, WM_RBUTTONDOWN
	*  
	*  @note 按键编号与Web标准兼容
	*  @note 侧键（Thumb01, Thumb02）在某些鼠标上可用
	*  
	*  使用示例：
	*  @code
	*  EMouseButtons::Type button = EMouseButtons::Left;
	*  if (button == EMouseButtons::Left) {
	*      // 处理左键事件
	*  }
	*  @endcode
	*/
	namespace EMouseButtons
	{
		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 鼠标按键类型枚举（Mouse Button Type Enumeration）
		*  
		*  该枚举定义了所有可用的鼠标按键类型。用于标识不同的鼠标按键。
		*  
		*  @note Left = 0: 鼠标左键
		*  @note Middle = 1: 鼠标中键（滚轮按键）
		*  @note Right = 2: 鼠标右键
		*  @note Thumb01 = 3: 鼠标侧键1（拇指键1，部分鼠标支持）
		*  @note Thumb02 = 4: 鼠标侧键2（拇指键2，部分鼠标支持）
		*  @note Invalid = 5: 无效按键，用于错误处理
		*/
		enum Type
		{
			Left = 0,      /**< 鼠标左键 */
			Middle,        /**< 鼠标中键（滚轮按键） */
			Right,         /**< 鼠标右键 */
			Thumb01,       /**< 鼠标侧键1（拇指键1） */
			Thumb02,       /**< 鼠标侧键2（拇指键2） */
			Invalid,       /**< 无效按键 */
		};
	}

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 输入事件类型枚举类（Input Event Type Enumeration Class）
	*  
	*  EventType枚举类定义了所有可以被设备处理的输入事件类型。
	*  包括键盘、鼠标、触摸和游戏手柄等各类输入事件。
	*  
	*  事件类型分类（Event Type Categories）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  键盘事件:                                                      |
	*   |  - KEY_DOWN: 按键按下                                           |
	*   |  - KEY_UP: 按键释放                                             |
	*   |  - KEY_PRESS: 字符输入                                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  鼠标事件:                                                      |
	*   |  - MOUSE_ENTER: 鼠标进入画布                                    |
	*   |  - MOUSE_LEAVE: 鼠标离开画布                                    |
	*   |  - MOUSE_MOVE: 鼠标移动                                         |
	*   |  - MOUSE_DOWN: 鼠标按键按下                                     |
	*   |  - MOUSE_UP: 鼠标按键释放                                       |
	*   |  - MOUSE_WHEEL: 鼠标滚轮滚动                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  触摸事件:                                                      |
	*   |  - TOUCH_START: 手指按下                                        |
	*   |  - TOUCH_END: 手指抬起                                          |
	*   |  - TOUCH_MOVE: 手指移动                                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  游戏手柄事件:                                                  |
	*   |  - GAMEPAD_PRESS: 游戏手柄按键按下                              |
	*   |  - GAMEPAD_RELEASE: 游戏手柄按键释放                            |
	*   |  - GAMEPAD_ANALOG: 游戏手柄模拟摇杆移动                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  其他:                                                          |
	*   |  - UNDEFINED: 未定义事件                                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  事件处理流程（Event Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 接收输入事件（键盘/鼠标/触摸/游戏手柄）                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 确定事件类型（EventType枚举值）                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 创建FEvent结构体，设置EventType                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 根据事件类型填充Data联合体的对应成员                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 调用Set方法设置事件参数                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 调用Get方法获取事件参数                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 处理事件并执行相应操作                                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note 使用枚举类（enum class）确保类型安全
	*  @note 每个事件类型对应FEvent结构体中Data联合体的特定成员
	*  @note UNDEFINED用于表示未初始化的事件
	*  
	*  使用示例：
	*  @code
	*  FEvent event(EventType::KEY_DOWN);
	*  event.SetKeyDown(65, false);  // 按下'A'键
	*  @endcode
	*/
	enum class EventType
	{
		UNDEFINED,		 /**< 未定义事件类型（No value） */
		KEY_DOWN,		 /**< 按键按下事件（A key has been pushed down） */
		KEY_UP,			 /**< 按键释放事件（A key has been released） */
		KEY_PRESS,		 /**< 字符输入事件（A key has been pressed and a character has been input） */
		MOUSE_ENTER,	 /**< 鼠标进入画布事件（The mouse has entered canvas） */
		MOUSE_LEAVE,	 /**< 鼠标离开画布事件（The mouse has left the canvas） */
		MOUSE_MOVE,		 /**< 鼠标移动事件（The mouse has been moved） */
		MOUSE_DOWN,		 /**< 鼠标按键按下事件（A mouse button has been clicked） */
		MOUSE_UP,		 /**< 鼠标按键释放事件（A mouse button has been released） */
		MOUSE_WHEEL,	 /**< 鼠标滚轮滚动事件（The mouse wheel was scrolled） */
		TOUCH_START,	 /**< 触摸开始事件（A finger is put down onto the canvas） */
		TOUCH_END,		 /**< 触摸结束事件（A finger is lifted from the canvas） */
		TOUCH_MOVE,		 /**< 触摸移动事件（A finger is being dragged along the surface of the canvas） */
		GAMEPAD_PRESS,	 /**< 游戏手柄按键按下事件（A gamepad button has been pressed） */
		GAMEPAD_RELEASE, /**< 游戏手柄按键释放事件（A gamepad button has been released） */
		GAMEPAD_ANALOG	 /**< 游戏手柄模拟摇杆移动事件（A gamepad analog stick has been moved） */
	};

	/**
	*  @author chensong
	*  @date 2022-01-20
	*  @brief 通用输入事件结构体（General Input Event Structure）
	*  
	*  FEvent结构体用于表示各种输入设备事件（键盘、鼠标、触摸、游戏手柄等）。
	*  使用联合体（union）存储不同类型的事件数据，以节省内存空间。
	*  
	*  输入事件结构体架构（Input Event Structure Architecture）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |                        FEvent Structure                         |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  Event: EventType枚举值，标识事件类型                        | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  Data: union联合体，根据Event类型选择对应的成员              | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  Word: 64位整数（通用数据）                              | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  KeyDown: 键盘按下事件数据                                | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  KeyUp: 键盘释放事件数据                                  | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  Character: 字符输入事件数据                              | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  MouseMove: 鼠标移动事件数据                              | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  MouseButton: 鼠标按键事件数据                            | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  MouseWheel: 鼠标滚轮事件数据                             | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  Touch: 触摸事件数据                                      | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  GamepadButton: 游戏手柄按键事件数据                       | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  GamepadAnalog: 游戏手柄模拟摇杆事件数据                  | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  联合体数据布局（Union Data Layout）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  Word (64 bits): 通用数据，可用于直接访问所有数据               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  注意：所有成员共享同一块内存，只能同时使用其中一个             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note 使用联合体节省内存空间，所有事件数据共享64位内存
	*  @note Event成员用于确定当前Data联合体中有效的数据类型
	*  @note 每个事件类型对应Data联合体中的一个特定成员
	*  
	*  使用示例：
	*  @code
	*  // 创建键盘按下事件
	*  FEvent keyEvent(EventType::KEY_DOWN);
	*  keyEvent.SetKeyDown(65, false);  // 按下'A'键
	*  
	*  // 创建鼠标移动事件
	*  FEvent mouseEvent(EventType::MOUSE_MOVE);
	*  mouseEvent.SetMouseDelta(100, 200, 10, 20);
	*  @endcode
	*/
	struct FEvent
	{
		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 事件类型（Event Type）
		*  
		*  该成员变量用于标识当前事件的具体类型。根据事件类型确定Data联合体
		*  中哪个成员包含有效数据。
		*  
		*  @note EventType枚举值，用于确定事件类型
		*  @note 必须与Data联合体中对应的成员匹配
		*  @note UNDEFINED表示事件未初始化
		*/
		EventType Event;

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 事件数据联合体（Event Data Union）
		*  
		*  该联合体用于存储不同类型的事件数据。所有成员共享同一块64位内存，
		*  根据Event类型确定哪个成员包含有效数据。使用联合体可以节省内存空间。
		*  
		*  联合体成员说明（Union Member Description）：
		*  - Word: 64位整数，可用于直接访问所有数据
		*  - KeyDown: 键盘按下事件数据（KEY_DOWN）
		*  - KeyUp: 键盘释放事件数据（KEY_UP）
		*  - Character: 字符输入事件数据（KEY_PRESS）
		*  - MouseMove: 鼠标移动事件数据（MOUSE_MOVE）
		*  - MouseButton: 鼠标按键事件数据（MOUSE_DOWN, MOUSE_UP）
		*  - MouseWheel: 鼠标滚轮事件数据（MOUSE_WHEEL）
		*  - Touch: 触摸事件数据（TOUCH_START, TOUCH_END, TOUCH_MOVE）
		*  - GamepadButton: 游戏手柄按键事件数据（GAMEPAD_PRESS, GAMEPAD_RELEASE）
		*  - GamepadAnalog: 游戏手柄模拟摇杆事件数据（GAMEPAD_ANALOG）
		*  
		*  @note 所有成员共享同一块内存，只能同时使用其中一个
		*  @note 必须根据Event类型访问对应的成员
		*  @note 使用联合体可以节省内存空间（最大64位）
		*/
		union
		{
			/**
			*  @author chensong
			*  @date 2022-01-20
			*  @brief 通用64位整数（General 64-bit Word）
			*  
			*  该成员用于直接访问整个联合体的64位数据。
			*  可用于快速比较或复制事件数据。
			*  
			*  @note 64位整数，可用于直接访问所有数据
			*  @note 与其他成员共享同一块内存
			*/
			uint64 Word;

			/**
			*  @author chensong
			*  @date 2022-01-20
			*  @brief 键盘按下事件数据结构体（Key Down Event Data Structure）
			*  
			*  该结构体用于存储键盘按下事件的数据。当EventType为KEY_DOWN时使用。
			*  
			*  键盘按下事件数据格式（Key Down Event Data Format）：
			*  
			*    0                   1                   2                   3
			*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  KeyCode (8 bits: 虚拟键码)                                  |
			*   |  - 0x41: 'A'键                                                 |
			*   |  - 0x0D: Enter键                                              |
			*   |  - 0x1B: Escape键                                             |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  bIsRepeat (1 bit: 按键重复标志)                              |
			*   |  - false: 首次按下                                             |
			*   |  - true: 按键重复（长按）                                      |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  填充位 (7 bits: 对齐到字节边界)                              |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*  
			*  @note 与Word成员共享内存，占用64位
			*  @note KeyCode: 8位虚拟键码，标识按下的键
			*  @note bIsRepeat: 布尔值，表示按键是否重复（长按）
			*/
			struct   /** KEY_DOWN */
			{
				uint8 KeyCode;      /**< 虚拟键码（Virtual Key Code） */
				bool bIsRepeat;     /**< 按键重复标志（Key Repeat Flag） */
			} KeyDown;

			/**
			*  @author chensong
			*  @date 2022-01-20
			*  @brief 键盘释放事件数据结构体（Key Up Event Data Structure）
			*  
			*  该结构体用于存储键盘释放事件的数据。当EventType为KEY_UP时使用。
			*  
			*  键盘释放事件数据格式（Key Up Event Data Format）：
			*  
			*    0                   1                   2                   3
			*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  KeyCode (8 bits: 虚拟键码)                                  |
			*   |  - 0x41: 'A'键                                                 |
			*   |  - 0x0D: Enter键                                              |
			*   |  - 0x1B: Escape键                                             |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  未使用 (8 bits: 填充)                                        |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*  
			*  @note 与Word成员共享内存，占用64位
			*  @note KeyCode: 8位虚拟键码，标识释放的键
			*/
			struct   /** KEY_UP */
			{
				uint8 KeyCode;      /**< 虚拟键码（Virtual Key Code） */
			} KeyUp;

			/**
			*  @author chensong
			*  @date 2022-01-20
			*  @brief 字符输入事件数据结构体（Character Input Event Data Structure）
			*  
			*  该结构体用于存储字符输入事件的数据。当EventType为KEY_PRESS时使用。
			*  
			*  字符输入事件数据格式（Character Input Event Data Format）：
			*  
			*    0                   1                   2                   3
			*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  Character (16/32 bits: 字符Unicode编码)                      |
			*   |  - 'A': 0x0041                                                  |
			*   |  - '中': 0x4E2D (中文)                                          |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*  
			*  @note 与Word成员共享内存，占用64位
			*  @note Character: TCHAR类型（通常是16位或32位），存储字符的Unicode编码
			*/
			struct   /** KEY_PRESSED */
			{
				TCHAR Character;    /**< 字符Unicode编码（Character Unicode Code） */
			} Character;

			/**
			*  @author chensong
			*  @date 2022-01-20
			*  @brief 鼠标移动事件数据结构体（Mouse Move Event Data Structure）
			*  
			*  该结构体用于存储鼠标移动事件的数据。当EventType为MOUSE_MOVE时使用。
			*  
			*  鼠标移动事件数据格式（Mouse Move Event Data Format）：
			*  
			*    0                   1                   2                   3
			*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  DeltaX (16 bits: 水平偏移，有符号)                            |
			*   |  - 正值: 向右移动                                               |
			*   |  - 负值: 向左移动                                               |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  DeltaY (16 bits: 垂直偏移，有符号)                            |
			*   |  - 正值: 向下移动                                               |
			*   |  - 负值: 向上移动                                               |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  PosX (16 bits: 水平绝对坐标)                                  |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  PosY (16 bits: 垂直绝对坐标)                                  |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*  
			*  @note 与Word成员共享内存，占用64位
			*  @note DeltaX: 16位有符号整数，水平偏移量（像素）
			*  @note DeltaY: 16位有符号整数，垂直偏移量（像素）
			*  @note PosX: 16位无符号整数，水平绝对坐标（像素）
			*  @note PosY: 16位无符号整数，垂直绝对坐标（像素）
			*/
			struct   /** MOUSE_MOVE */
			{
				int16 DeltaX;       /**< 水平偏移量（Horizontal Delta） */
				int16 DeltaY;       /**< 垂直偏移量（Vertical Delta） */
				uint16 PosX;        /**< 水平绝对坐标（Horizontal Position） */
				uint16 PosY;        /**< 垂直绝对坐标（Vertical Position） */
			} MouseMove;

			/**
			*  @author chensong
			*  @date 2022-01-20
			*  @brief 鼠标按键事件数据结构体（Mouse Button Event Data Structure）
			*  
			*  该结构体用于存储鼠标按键事件的数据。当EventType为MOUSE_DOWN或MOUSE_UP时使用。
			*  
			*  鼠标按键事件数据格式（Mouse Button Event Data Format）：
			*  
			*    0                   1                   2                   3
			*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  Button (8 bits: 鼠标按键标志)                                |
			*   |  - 0: 左键                                                      |
			*   |  - 1: 中键                                                      |
			*   |  - 2: 右键                                                      |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  未使用 (8 bits: 填充)                                        |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  PosX (16 bits: 水平坐标)                                     |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  PosY (16 bits: 垂直坐标)                                     |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*  
			*  @note 与Word成员共享内存，占用64位
			*  @note Button: 8位无符号整数，标识鼠标按键（0=左键, 1=中键, 2=右键）
			*  @note PosX: 16位无符号整数，水平坐标（像素）
			*  @note PosY: 16位无符号整数，垂直坐标（像素）
			*/
			struct   /** MOUSE_DOWN, MOUSE_UP */
			{
				uint8 Button;       /**< 鼠标按键标志（Mouse Button Flag） */
				uint16 PosX;        /**< 水平坐标（Horizontal Position） */
				uint16 PosY;        /**< 垂直坐标（Vertical Position） */
			} MouseButton;

			/**
			*  @author chensong
			*  @date 2022-01-20
			*  @brief 鼠标滚轮事件数据结构体（Mouse Wheel Event Data Structure）
			*  
			*  该结构体用于存储鼠标滚轮滚动事件的数据。当EventType为MOUSE_WHEEL时使用。
			*  
			*  鼠标滚轮事件数据格式（Mouse Wheel Event Data Format）：
			*  
			*    0                   1                   2                   3
			*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  Delta (16 bits: 滚轮增量，有符号)                            |
			*   |  - 正值: 向上滚动（远离用户）                                  |
			*   |  - 负值: 向下滚动（靠近用户）                                  |
			*   |  - 120: 标准滚动单位（Windows）                                |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  PosX (16 bits: 水平坐标)                                     |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  PosY (16 bits: 垂直坐标)                                     |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*  
			*  @note 与Word成员共享内存，占用64位
			*  @note Delta: 16位有符号整数，滚轮滚动增量（120为标准滚动单位）
			*  @note PosX: 16位无符号整数，水平坐标（像素）
			*  @note PosY: 16位无符号整数，垂直坐标（像素）
			*/
			struct   /** MOUSE_WHEEL */
			{
				int16 Delta;        /**< 滚轮增量（Wheel Delta） */
				uint16 PosX;        /**< 水平坐标（Horizontal Position） */
				uint16 PosY;        /**< 垂直坐标（Vertical Position） */
			} MouseWheel;

			/**
			*  @author chensong
			*  @date 2022-01-20
			*  @brief 触摸事件数据结构体（Touch Event Data Structure）
			*  
			*  该结构体用于存储触摸事件的数据。当EventType为TOUCH_START、TOUCH_END或TOUCH_MOVE时使用。
			*  
			*  触摸事件数据格式（Touch Event Data Format）：
			*  
			*    0                   1                   2                   3
			*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  TouchIndex (8 bits: 触摸索引，多触摸支持)                    |
			*   |  - 0: 第一个手指                                                |
			*   |  - 1: 第二个手指                                                |
			*   |  - ...                                                          |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  PosX (16 bits: 水平坐标)                                     |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  PosY (16 bits: 垂直坐标)                                     |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  Force (8 bits: 压力值)                                       |
			*   |  - 0-255: 压力级别                                            |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*  
			*  @note 与Word成员共享内存，占用64位
			*  @note TouchIndex: 8位无符号整数，触摸索引（多触摸支持）
			*  @note PosX: 16位无符号整数，水平坐标（像素）
			*  @note PosY: 16位无符号整数，垂直坐标（像素）
			*  @note Force: 8位无符号整数，压力值（0-255）
			*/
			struct   /** TOUCH_START, TOUCH_END, TOUCH_MOVE */
			{
				uint8 TouchIndex;   /**< 触摸索引（Touch Index） */
				uint16 PosX;        /**< 水平坐标（Horizontal Position） */
				uint16 PosY;        /**< 垂直坐标（Vertical Position） */
				uint8 Force;        /**< 压力值（Force/Pressure） */
			} Touch;

			/**
			*  @author chensong
			*  @date 2022-01-20
			*  @brief 游戏手柄按键事件数据结构体（Gamepad Button Event Data Structure）
			*  
			*  该结构体用于存储游戏手柄按键事件的数据。当EventType为GAMEPAD_PRESS或GAMEPAD_RELEASE时使用。
			*  
			*  游戏手柄按键事件数据格式（Gamepad Button Event Data Format）：
			*  
			*    0                   1                   2                   3
			*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  ControllerIndex (8 bits: 控制器索引)                         |
			*   |  - 0: 第一个控制器                                             |
			*   |  - 1: 第二个控制器                                             |
			*   |  - ...                                                          |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  ButtonIndex (8 bits: 按键索引)                               |
			*   |  - 0-15: 标准游戏手柄按键                                      |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  bIsRepeat (1 bit: 按键重复标志)                              |
			*   |  - false: 首次按下                                             |
			*   |  - true: 按键重复（长按）                                      |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  填充位 (7 bits: 对齐到字节边界)                              |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*  
			*  @note 与Word成员共享内存，占用64位
			*  @note ControllerIndex: 8位无符号整数，控制器索引（多控制器支持）
			*  @note ButtonIndex: 8位无符号整数，按键索引（0-15为标准按键）
			*  @note bIsRepeat: 布尔值，表示按键是否重复（长按）
			*/
			struct   /** GAMEPAD_PRESSED, GAMEPAD_RELEASED */
			{
				uint8 ControllerIndex;  /**< 控制器索引（Controller Index） */
				uint8 ButtonIndex;      /**< 按键索引（Button Index） */
				bool bIsRepeat;         /**< 按键重复标志（Button Repeat Flag） */
			} GamepadButton;

			/**
			*  @author chensong
			*  @date 2022-01-20
			*  @brief 游戏手柄模拟摇杆事件数据结构体（Gamepad Analog Stick Event Data Structure）
			*  
			*  该结构体用于存储游戏手柄模拟摇杆移动事件的数据。当EventType为GAMEPAD_ANALOG时使用。
			*  
			*  游戏手柄模拟摇杆事件数据格式（Gamepad Analog Stick Event Data Format）：
			*  
			*    0                   1                   2                   3
			*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  ControllerIndex (8 bits: 控制器索引)                         |
			*   |  - 0: 第一个控制器                                             |
			*   |  - 1: 第二个控制器                                             |
			*   |  - ...                                                          |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  AxisIndex (8 bits: 轴索引)                                   |
			*   |  - 0: 左摇杆X轴                                               |
			*   |  - 1: 左摇杆Y轴                                               |
			*   |  - 2: 右摇杆X轴                                               |
			*   |  - 3: 右摇杆Y轴                                               |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*   |  AnalogValue (32 bits: 模拟值，浮点数)                        |
			*   |  - -1.0 到 1.0: 摇杆位置                                      |
			*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			*  
			*  @note 与Word成员共享内存，占用64位
			*  @note ControllerIndex: 8位无符号整数，控制器索引（多控制器支持）
			*  @note AxisIndex: 8位无符号整数，轴索引（0-3为标准轴）
			*  @note AnalogValue: 32位浮点数，模拟值（-1.0到1.0）
			*/
			struct   /** GAMEPAD_ANALOG */
			{
				uint8 ControllerIndex;  /**< 控制器索引（Controller Index） */
				float AnalogValue;      /**< 模拟值（Analog Value，-1.0到1.0） */
				uint8 AxisIndex;        /**< 轴索引（Axis Index） */
			} GamepadAnalog;
		} Data;

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 默认构造函数（Default Constructor）
		*  
		*  该构造函数用于创建一个完全空的事件。事件类型被设置为UNDEFINED。
		*  
		*  初始化说明（Initialization Description）：
		*  - Event: 初始化为EventType::UNDEFINED，表示未定义事件
		*  - Data: 联合体数据未初始化，需要先设置Event类型
		*  
		*  @note 创建的事件类型为UNDEFINED，表示事件未初始化
		*  @note 在使用前需要先设置Event类型和Data数据
		*  
		*  使用示例：
		*  @code
		*  FEvent event;
		*  // event.Event == EventType::UNDEFINED
		*  @endcode
		*/
		FEvent()
			: Event(EventType::UNDEFINED)
		{
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于创建一个指定类型的事件。事件类型由参数指定。
		*  
		*  初始化说明（Initialization Description）：
		*  - Event: 设置为参数指定的EventType值
		*  - Data: 联合体数据未初始化，需要调用相应的Set方法设置数据
		*  
		*  @param InEvent 事件类型（EventType枚举值）
		*  @note 创建的事件类型由参数指定
		*  @note 在使用前需要调用相应的Set方法设置Data数据
		*  
		*  使用示例：
		*  @code
		*  // 创建键盘按下事件
		*  FEvent keyEvent(EventType::KEY_DOWN);
		*  keyEvent.SetKeyDown(65, false);  // 按下'A'键
		*  
		*  // 创建鼠标移动事件
		*  FEvent mouseEvent(EventType::MOUSE_MOVE);
		*  mouseEvent.SetMouseDelta(100, 200, 10, 20);
		*  @endcode
		*/
		FEvent(EventType InEvent)
			: Event(InEvent)
		{
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 设置游戏手柄模拟摇杆事件数据（Set Gamepad Analog Stick Event Data）
		*  
		*  该方法用于设置游戏手柄模拟摇杆移动事件的数据。当EventType为GAMEPAD_ANALOG时使用。
		*  
		*  游戏手柄模拟摇杆数据设置（Gamepad Analog Stick Data Setting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为GAMEPAD_ANALOG                          |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 设置ControllerIndex: 控制器索引                            |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 设置AxisIndex: 轴索引（0-3为标准轴）                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. 设置AnalogValue: 模拟值（-1.0到1.0）                       |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  轴索引说明（Axis Index Description）：
		*  - 0: 左摇杆X轴
		*  - 1: 左摇杆Y轴
		*  - 2: 右摇杆X轴
		*  - 3: 右摇杆Y轴
		*  
		*  @param InControllerIndex 控制器索引，标识是哪个控制器（0为第一个控制器）
		*  @param InAxisIndex 轴索引，标识是哪个轴（0-3为标准轴）
		*  @param InAnalogValue 模拟值，摇杆位置（-1.0到1.0，0.0为中心位置）
		*  @note Event类型必须为GAMEPAD_ANALOG
		*  @note 模拟值范围：-1.0（最小值）到1.0（最大值）
		*  @note 0.0表示摇杆在中心位置
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::GAMEPAD_ANALOG);
		*  event.SetGamepadAnalog(0, 0, 0.5f);  // 第一个控制器，左摇杆X轴，向右移动50%
		*  @endcode
		*/
		void SetGamepadAnalog(uint8 InControllerIndex, uint8 InAxisIndex, float InAnalogValue)
		{
			check(Event == EventType::GAMEPAD_ANALOG);
			Data.GamepadAnalog.ControllerIndex = InControllerIndex;
			Data.GamepadAnalog.AxisIndex = InAxisIndex;
			Data.GamepadAnalog.AnalogValue = InAnalogValue;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 设置游戏手柄按键按下事件数据（Set Gamepad Button Pressed Event Data）
		*  
		*  该方法用于设置游戏手柄按键按下事件的数据。当EventType为GAMEPAD_PRESS时使用。
		*  
		*  游戏手柄按键按下数据设置（Gamepad Button Pressed Data Setting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为GAMEPAD_PRESS                           |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 设置ControllerIndex: 控制器索引                            |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 设置ButtonIndex: 按键索引（0-15为标准按键）                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. 设置bIsRepeat: 按键重复标志                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  按键索引说明（Button Index Description）：
		*  - 0: A按钮
		*  - 1: B按钮
		*  - 2: X按钮
		*  - 3: Y按钮
		*  - 4-15: 其他标准按钮
		*  
		*  @param InControllerIndex 控制器索引，标识是哪个控制器（0为第一个控制器）
		*  @param InButtonIndex 按键索引，标识是哪个按键（0-15为标准按键）
		*  @param InIsRepeat 按键重复标志，true表示按键重复（长按），false表示首次按下
		*  @note Event类型必须为GAMEPAD_PRESS
		*  @note bIsRepeat用于区分首次按下和按键重复（长按）
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::GAMEPAD_PRESS);
		*  event.SetGamepadButtonPressed(0, 0, false);  // 第一个控制器，A按钮，首次按下
		*  @endcode
		*/
		void SetGamepadButtonPressed(uint8 InControllerIndex, uint8 InButtonIndex, bool InIsRepeat)
		{
			check(Event == EventType::GAMEPAD_PRESS);
			Data.GamepadButton.ControllerIndex = InControllerIndex;
			Data.GamepadButton.ButtonIndex = InButtonIndex;
			Data.GamepadButton.bIsRepeat = InIsRepeat;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 设置游戏手柄按键释放事件数据（Set Gamepad Button Released Event Data）
		*  
		*  该方法用于设置游戏手柄按键释放事件的数据。当EventType为GAMEPAD_RELEASE时使用。
		*  
		*  游戏手柄按键释放数据设置（Gamepad Button Released Data Setting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为GAMEPAD_RELEASE                         |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 设置ControllerIndex: 控制器索引                            |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 设置ButtonIndex: 按键索引（0-15为标准按键）                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param InControllerIndex 控制器索引，标识是哪个控制器（0为第一个控制器）
		*  @param InButtonIndex 按键索引，标识是哪个按键（0-15为标准按键）
		*  @note Event类型必须为GAMEPAD_RELEASE
		*  @note 按键释放事件不需要重复标志
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::GAMEPAD_RELEASE);
		*  event.SetGamepadButtonReleased(0, 0);  // 第一个控制器，A按钮，释放
		*  @endcode
		*/
		void SetGamepadButtonReleased(uint8 InControllerIndex, uint8 InButtonIndex)
		{
			check(Event == EventType::GAMEPAD_RELEASE);
			Data.GamepadButton.ControllerIndex = InControllerIndex;
			Data.GamepadButton.ButtonIndex = InButtonIndex;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 设置键盘按键按下事件数据（Set Key Down Event Data）
		*  
		*  该方法用于设置键盘按键按下事件的数据。当EventType为KEY_DOWN时使用。
		*  
		*  键盘按键按下数据设置（Key Down Data Setting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为KEY_DOWN                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 设置KeyCode: 虚拟键码（8位）                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 设置bIsRepeat: 按键重复标志                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  虚拟键码示例（Virtual Key Code Examples）：
		*  - 0x41: 'A'键
		*  - 0x0D: Enter键
		*  - 0x1B: Escape键
		*  - 0x20: 空格键
		*  
		*  @param InKeyCode 虚拟键码，标识按下的键（8位无符号整数）
		*  @param InIsRepeat 按键重复标志，true表示按键重复（长按），false表示首次按下
		*  @note Event类型必须为KEY_DOWN
		*  @note bIsRepeat用于区分首次按下和按键重复（长按）
		*  @note keydown事件在所有键按下时都会触发
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::KEY_DOWN);
		*  event.SetKeyDown(65, false);  // 按下'A'键，首次按下
		*  @endcode
		*/
		void SetKeyDown(uint8 InKeyCode, bool InIsRepeat)
		{
			check(Event == EventType::KEY_DOWN);
			Data.KeyDown.KeyCode = InKeyCode;
			Data.KeyDown.bIsRepeat = InIsRepeat;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 设置键盘按键释放事件数据（Set Key Up Event Data）
		*  
		*  该方法用于设置键盘按键释放事件的数据。当EventType为KEY_UP时使用。
		*  
		*  键盘按键释放数据设置（Key Up Data Setting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为KEY_UP                                  |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 设置KeyCode: 虚拟键码（8位）                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  虚拟键码示例（Virtual Key Code Examples）：
		*  - 0x41: 'A'键
		*  - 0x0D: Enter键
		*  - 0x1B: Escape键
		*  
		*  @param InKeyCode 虚拟键码，标识释放的键（8位无符号整数）
		*  @note Event类型必须为KEY_UP
		*  @note keyup事件在键释放时触发
		*  @note 需要与keydown事件配对使用
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::KEY_UP);
		*  event.SetKeyUp(65);  // 释放'A'键
		*  @endcode
		*/
		void SetKeyUp(uint8 InKeyCode)
		{
			check(Event == EventType::KEY_UP);
			Data.KeyUp.KeyCode = InKeyCode;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 设置字符输入事件数据（Set Character Input Event Data）
		*  
		*  该方法用于设置字符输入事件的数据。当EventType为KEY_PRESS时使用。
		*  keypress事件只在按下字符键时触发，与keydown不同。
		*  
		*  字符输入数据设置（Character Input Data Setting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为KEY_PRESS                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 设置Character: 字符Unicode编码                              |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  字符编码示例（Character Code Examples）：
		*  - 'A': 0x0041 (ASCII)
		*  - '中': 0x4E2D (中文Unicode)
		*  - '1': 0x0031 (数字)
		*  
		*  @param InCharacter 字符Unicode编码，标识输入的字符（TCHAR类型）
		*  @note Event类型必须为KEY_PRESS
		*  @note keypress只在字符键按下时触发
		*  @note 功能键（如F1-F12）不会触发keypress事件
		*  @note Character使用TCHAR类型（通常是16位或32位Unicode编码）
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::KEY_PRESS);
		*  event.SetCharCode('A');  // 输入字符'A'
		*  @endcode
		*/
		void SetCharCode(TCHAR InCharacter)
		{
			check(Event == EventType::KEY_PRESS);
			Data.Character.Character = InCharacter;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 设置鼠标移动事件数据（Set Mouse Move Event Data）
		*  
		*  该方法用于设置鼠标移动事件的数据。当EventType为MOUSE_MOVE时使用。
	 *  支持绝对坐标和相对偏移两种模式。
	*  
	*  鼠标移动数据设置（Mouse Move Data Setting）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 验证Event类型是否为MOUSE_MOVE                                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 设置PosX: 水平绝对坐标（16位无符号整数）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 设置PosY: 垂直绝对坐标（16位无符号整数）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 设置DeltaX: 水平偏移量（16位有符号整数）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 设置DeltaY: 垂直偏移量（16位有符号整数）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  坐标说明（Coordinate Description）：
	*  - PosX/PosY: 绝对坐标（像素），表示鼠标的当前位置
	*  - DeltaX/DeltaY: 相对偏移（像素），表示相对于上一次位置的偏移
	*  - DeltaX > 0: 向右移动
	*  - DeltaX < 0: 向左移动
	*  - DeltaY > 0: 向下移动
	*  - DeltaY < 0: 向上移动
	*  
	*  @param InPosX 水平绝对坐标，鼠标指针的水平位置（16位无符号整数，单位：像素）
	*  @param InPosY 垂直绝对坐标，鼠标指针的垂直位置（16位无符号整数，单位：像素）
	*  @param InDeltaX 水平偏移量，鼠标指针的水平偏移（16位有符号整数，单位：像素）
	*  @param InDeltaY 垂直偏移量，鼠标指针的垂直偏移（16位有符号整数，单位：像素）
	*  @note Event类型必须为MOUSE_MOVE
	*  @note 支持移动时按下按键的情况（拖拽操作）
	*  @note DeltaX和DeltaY表示相对偏移，可以为负值
	*  
	*  使用示例：
	*  @code
	*  FEvent event(EventType::MOUSE_MOVE);
	*  event.SetMouseDelta(100, 200, 10, 20);  // 绝对坐标(100,200)，偏移(10,20)
	*  @endcode
		*/
		void SetMouseDelta(uint16 InPosX, uint16 InPosY, int16 InDeltaX, int16 InDeltaY)
		{
			check(Event == EventType::MOUSE_MOVE);
			Data.MouseMove.DeltaX = InDeltaX;
			Data.MouseMove.DeltaY = InDeltaY;
			Data.MouseMove.PosX = InPosX;
			Data.MouseMove.PosY = InPosY;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 设置鼠标按键事件数据（Set Mouse Button Event Data）
		*  
		*  该方法用于设置鼠标按键事件的数据。当EventType为MOUSE_DOWN或MOUSE_UP时使用。
		*  
		*  鼠标按键数据设置（Mouse Button Data Setting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为MOUSE_DOWN或MOUSE_UP                    |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 设置Button: 鼠标按键标志（8位无符号整数）                    |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 设置PosX: 水平坐标（16位无符号整数）                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. 设置PosY: 垂直坐标（16位无符号整数）                        |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  鼠标按键标志说明（Mouse Button Flag Description）：
	*  - 0: 左键（Left Button）
	*  - 1: 中键（Middle Button，滚轮按键）
	*  - 2: 右键（Right Button）
	*  - 3-4: 侧键（Thumb01, Thumb02，部分鼠标支持）
	*  
	*  @param InButton 鼠标按键标志，标识按下的鼠标按键（8位无符号整数，0=左键, 1=中键, 2=右键）
	*  @param InPosX 水平坐标，鼠标指针的水平位置（16位无符号整数，单位：像素）
	*  @param InPosY 垂直坐标，鼠标指针的垂直位置（16位无符号整数，单位：像素）
	*  @note Event类型必须为MOUSE_DOWN或MOUSE_UP
	*  @note 在Web环境中，左右键可能互换（right <-> left）
	*  @note PosX和PosY表示鼠标按键按下/释放时的位置
	*  
	*  使用示例：
	*  @code
	*  // 鼠标按下事件
	*  FEvent downEvent(EventType::MOUSE_DOWN);
	*  downEvent.SetMouseClick(0, 100, 200);  // 左键按下，位置(100,200)
	*  
	*  // 鼠标释放事件
	*  FEvent upEvent(EventType::MOUSE_UP);
	*  upEvent.SetMouseClick(0, 100, 200);  // 左键释放，位置(100,200)
	*  @endcode
		*/
		void SetMouseClick(uint8 InButton, uint16 InPosX, uint16 InPosY)
		{
			check(Event == EventType::MOUSE_DOWN || Event == EventType::MOUSE_UP);
			Data.MouseButton.Button = InButton;
			Data.MouseButton.PosX = InPosX;
			Data.MouseButton.PosY = InPosY;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 设置鼠标滚轮滚动事件数据（Set Mouse Wheel Event Data）
		*  
		*  该方法用于设置鼠标滚轮滚动事件的数据。当EventType为MOUSE_WHEEL时使用。
	*  
	*  鼠标滚轮数据设置（Mouse Wheel Data Setting）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 验证Event类型是否为MOUSE_WHEEL                                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 设置Delta: 滚轮增量（16位有符号整数）                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 设置PosX: 水平坐标（16位无符号整数）                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 设置PosY: 垂直坐标（16位无符号整数）                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  滚轮增量说明（Wheel Delta Description）：
	*  - Delta > 0: 向上滚动（远离用户）
	*  - Delta < 0: 向下滚动（靠近用户）
	*  - Delta = 120: 标准滚动单位（Windows）
	*  - Delta = -120: 标准滚动单位（向下滚动）
	*  - Ctrl+滚轮: 通常用于缩放操作
	*  
	*  @param InDelta 滚轮增量，鼠标滚轮滚动的距离（16位有符号整数，120为标准滚动单位）
	*  @param InPosX 水平坐标，鼠标指针的水平位置（16位无符号整数，单位：像素）
	*  @param InPosY 垂直坐标，鼠标指针的垂直位置（16位无符号整数，单位：像素）
	*  @note Event类型必须为MOUSE_WHEEL
	*  @note Delta值表示滚动方向和距离
	*  @note PosX和PosY表示滚轮滚动时鼠标的位置
	*  @note 120是Windows的标准滚动单位
	*  
	*  使用示例：
	*  @code
	*  FEvent event(EventType::MOUSE_WHEEL);
	*  event.SetMouseWheel(120, 100, 200);  // 向上滚动，位置(100,200)
	*  @endcode
		*/
		void SetMouseWheel(int16 InDelta, uint16 InPosX, uint16 InPosY)
		{
			check(Event == EventType::MOUSE_WHEEL);
			Data.MouseWheel.Delta = InDelta;
			Data.MouseWheel.PosX = InPosX;
			Data.MouseWheel.PosY = InPosY;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 设置触摸事件数据（Set Touch Event Data）
		*  
		*  该方法用于设置触摸事件的数据。当EventType为TOUCH_START、TOUCH_END或TOUCH_MOVE时使用。
		*  支持多触摸操作。
	*  
	*  触摸事件数据设置（Touch Event Data Setting）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 验证Event类型是否为TOUCH_START/TOUCH_END/TOUCH_MOVE          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 设置TouchIndex: 触摸索引（8位无符号整数）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 设置PosX: 水平坐标（16位无符号整数）                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 设置PosY: 垂直坐标（16位无符号整数）                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 设置Force: 压力值（8位无符号整数，0-255）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  触摸索引说明（Touch Index Description）：
	*  - 0: 第一个手指（主手指）
	*  - 1: 第二个手指
	*  - 2: 第三个手指
	*  - ...: 更多手指（多触摸支持）
	*  
	*  压力值说明（Force Description）：
	*  - 0-255: 压力级别（0=无压力，255=最大压力）
	*  - 某些触摸设备可能不支持压力检测
	*  
	*  @param InTouchIndex 触摸索引，标识是哪个手指（8位无符号整数，0为第一个手指）
	*  @param InPosX 水平坐标，手指的水平位置（16位无符号整数，单位：像素）
	*  @param InPosY 垂直坐标，手指的垂直位置（16位无符号整数，单位：像素）
	*  @param InForce 压力值，手指施加的压力（8位无符号整数，0-255）
	*  @note Event类型必须为TOUCH_START、TOUCH_END或TOUCH_MOVE
	*  @note 支持多触摸操作（多个手指同时触摸）
	*  @note Force值表示压力大小，某些设备可能不支持
	*  
	*  使用示例：
	*  @code
	*  // 触摸开始事件
	*  FEvent touchEvent(EventType::TOUCH_START);
	*  touchEvent.SetTouch(0, 100, 200, 128);  // 第一个手指，位置(100,200)，压力128
	*  @endcode
		*/
		void SetTouch(uint8 InTouchIndex, uint16 InPosX, uint16 InPosY, uint8 InForce)
		{
			check(Event == EventType::TOUCH_START || Event == EventType::TOUCH_END || Event == EventType::TOUCH_MOVE);
			Data.Touch.TouchIndex = InTouchIndex;
			Data.Touch.PosX = InPosX;
			Data.Touch.PosY = InPosY;
			Data.Touch.Force = InForce;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 获取游戏手柄模拟摇杆事件数据（Get Gamepad Analog Stick Event Data）
		*  
		*  该方法用于获取游戏手柄模拟摇杆移动事件的数据。当EventType为GAMEPAD_ANALOG时使用。
		*  
		*  游戏手柄模拟摇杆数据获取（Gamepad Analog Stick Data Getting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为GAMEPAD_ANALOG                          |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 获取ControllerIndex: 控制器索引                            |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 获取AxisIndex: 轴索引                                      |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. 获取AnalogValue: 模拟值                                    |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param OutControllerIndex 输出参数，控制器索引，标识是哪个控制器
		*  @param OutAxisIndex 输出参数，轴索引，标识是哪个轴（0-3为标准轴）
		*  @param OutAnalogValue 输出参数，模拟值，摇杆位置（-1.0到1.0）
		*  @note Event类型必须为GAMEPAD_ANALOG
		*  @note 模拟值范围：-1.0（最小值）到1.0（最大值）
		*  @note 0.0表示摇杆在中心位置
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::GAMEPAD_ANALOG);
		*  event.SetGamepadAnalog(0, 0, 0.5f);
		*  uint8 controllerIndex, axisIndex;
		*  float analogValue;
		*  event.GetGamepadAnalog(controllerIndex, axisIndex, analogValue);
		*  // controllerIndex = 0, axisIndex = 0, analogValue = 0.5f
		*  @endcode
		*/
		void GetGamepadAnalog(uint8& OutControllerIndex, uint8& OutAxisIndex, float& OutAnalogValue)
		{
			check(Event == EventType::GAMEPAD_ANALOG);
			OutControllerIndex = Data.GamepadAnalog.ControllerIndex;
			OutAxisIndex = Data.GamepadAnalog.AxisIndex;
			OutAnalogValue = Data.GamepadAnalog.AnalogValue;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 获取游戏手柄按键按下事件数据（Get Gamepad Button Pressed Event Data）
		*  
		*  该方法用于获取游戏手柄按键按下事件的数据。当EventType为GAMEPAD_PRESS时使用。
		*  
		*  游戏手柄按键按下数据获取（Gamepad Button Pressed Data Getting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为GAMEPAD_PRESS                           |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 获取ControllerIndex: 控制器索引                            |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 获取ButtonIndex: 按键索引                                  |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  4. 获取bIsRepeat: 按键重复标志                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param OutControllerIndex 输出参数，控制器索引，标识是哪个控制器
		*  @param OutButtonIndex 输出参数，按键索引，标识是哪个按键（0-15为标准按键）
		*  @param OutIsRepeat 输出参数，按键重复标志，true表示按键重复（长按），false表示首次按下
		*  @note Event类型必须为GAMEPAD_PRESS
		*  @note bIsRepeat用于区分首次按下和按键重复（长按）
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::GAMEPAD_PRESS);
		*  event.SetGamepadButtonPressed(0, 0, false);
		*  uint8 controllerIndex, buttonIndex;
		*  bool isRepeat;
		*  event.GetGamepadButtonPressed(controllerIndex, buttonIndex, isRepeat);
		*  // controllerIndex = 0, buttonIndex = 0, isRepeat = false
		*  @endcode
		*/
		void GetGamepadButtonPressed(uint8& OutControllerIndex, uint8& OutButtonIndex, bool& OutIsRepeat)
		{
			check(Event == EventType::GAMEPAD_PRESS);
			OutControllerIndex = Data.GamepadButton.ControllerIndex;
			OutButtonIndex = Data.GamepadButton.ButtonIndex;
			OutIsRepeat = Data.GamepadButton.bIsRepeat;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 获取游戏手柄按键释放事件数据（Get Gamepad Button Released Event Data）
		*  
		*  该方法用于获取游戏手柄按键释放事件的数据。当EventType为GAMEPAD_RELEASE时使用。
		*  
		*  游戏手柄按键释放数据获取（Gamepad Button Released Data Getting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为GAMEPAD_RELEASE                         |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 获取ControllerIndex: 控制器索引                            |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 获取ButtonIndex: 按键索引                                  |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param OutControllerIndex 输出参数，控制器索引，标识是哪个控制器
		*  @param OutButtonIndex 输出参数，按键索引，标识是哪个按键（0-15为标准按键）
		*  @note Event类型必须为GAMEPAD_RELEASE
		*  @note 按键释放事件不需要重复标志
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::GAMEPAD_RELEASE);
		*  event.SetGamepadButtonReleased(0, 0);
		*  uint8 controllerIndex, buttonIndex;
		*  event.GetGamepadButtonReleased(controllerIndex, buttonIndex);
		*  // controllerIndex = 0, buttonIndex = 0
		*  @endcode
		*/
		void GetGamepadButtonReleased(uint8& OutControllerIndex, uint8& OutButtonIndex)
		{
			check(Event == EventType::GAMEPAD_RELEASE);
			OutControllerIndex = Data.GamepadButton.ControllerIndex;
			OutButtonIndex = Data.GamepadButton.ButtonIndex;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 获取键盘按键按下事件数据（Get Key Down Event Data）
		*  
		*  该方法用于获取键盘按键按下事件的数据。当EventType为KEY_DOWN时使用。
		*  
		*  键盘按键按下数据获取（Key Down Data Getting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为KEY_DOWN                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 获取KeyCode: 虚拟键码（8位无符号整数）                      |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  3. 获取bIsRepeat: 按键重复标志（布尔值）                      |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param OutKeyCode 输出参数，虚拟键码，标识按下的键（8位无符号整数）
		*  @param OutIsRepeat 输出参数，按键重复标志，true表示按键重复（长按），false表示首次按下
		*  @note Event类型必须为KEY_DOWN
		*  @note bIsRepeat用于区分首次按下和按键重复（长按）
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::KEY_DOWN);
		*  event.SetKeyDown(65, false);
		*  uint8 keyCode;
		*  bool isRepeat;
		*  event.GetKeyDown(keyCode, isRepeat);
		*  // keyCode = 65, isRepeat = false
		*  @endcode
		*/
		void GetKeyDown(uint8& OutKeyCode, bool& OutIsRepeat)
		{
			check(Event == EventType::KEY_DOWN);
			OutKeyCode = Data.KeyDown.KeyCode;
			OutIsRepeat = Data.KeyDown.bIsRepeat;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 获取键盘按键释放事件数据（Get Key Up Event Data）
		*  
		*  该方法用于获取键盘按键释放事件的数据。当EventType为KEY_UP时使用。
		*  
		*  键盘按键释放数据获取（Key Up Data Getting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为KEY_UP                                  |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 获取KeyCode: 虚拟键码（8位无符号整数）                      |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param OutKeyCode 输出参数，虚拟键码，标识释放的键（8位无符号整数）
		*  @note Event类型必须为KEY_UP
		*  @note keyup事件在键释放时触发
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::KEY_UP);
		*  event.SetKeyUp(65);
		*  uint8 keyCode;
		*  event.GetKeyUp(keyCode);
		*  // keyCode = 65
		*  @endcode
		*/
		void GetKeyUp(uint8& OutKeyCode)
		{
			check(Event == EventType::KEY_UP);
			OutKeyCode = Data.KeyUp.KeyCode;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 获取字符输入事件数据（Get Character Input Event Data）
		*  
		*  该方法用于获取字符输入事件的数据。当EventType为KEY_PRESS时使用。
		*  keypress事件只在按下字符键时触发。
		*  
		*  字符输入数据获取（Character Input Data Getting）：
		*  
		*    0                   1                   2                   3
		*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  1. 验证Event类型是否为KEY_PRESS                                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*   |  2. 获取Character: 字符Unicode编码（TCHAR类型）                |
		*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  
		*  @param OutCharacter 输出参数，字符Unicode编码，标识输入的字符（TCHAR类型）
		*  @note Event类型必须为KEY_PRESS
		*  @note keypress只在字符键按下时触发
		*  @note 功能键（如F1-F12）不会触发keypress事件
		*  
		*  使用示例：
		*  @code
		*  FEvent event(EventType::KEY_PRESS);
		*  event.SetCharCode('A');
		*  TCHAR character;
		*  event.GetCharacterCode(character);
		*  // character = 'A'
		*  @endcode
		*/
		void GetCharacterCode(TCHAR& OutCharacter)
		{
			check(Event == EventType::KEY_PRESS);
			OutCharacter = Data.Character.Character;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 获取鼠标移动事件数据（Get Mouse Move Event Data）
		*  
		*  该方法用于获取鼠标移动事件的数据。当EventType为MOUSE_MOVE时使用。
		*  支持绝对坐标和相对偏移两种模式。
	*  
	*  鼠标移动数据获取（Mouse Move Data Getting）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 验证Event类型是否为MOUSE_MOVE                                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 获取PosX: 水平绝对坐标（16位无符号整数）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 获取PosY: 垂直绝对坐标（16位无符号整数）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 获取DeltaX: 水平偏移量（16位有符号整数）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 获取DeltaY: 垂直偏移量（16位有符号整数）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param OutPosX 输出参数，水平绝对坐标，鼠标指针的水平位置（16位无符号整数，单位：像素）
	*  @param OutPosY 输出参数，垂直绝对坐标，鼠标指针的垂直位置（16位无符号整数，单位：像素）
	*  @param OutDeltaX 输出参数，水平偏移量，鼠标指针的水平偏移（16位有符号整数，单位：像素）
	*  @param OutDeltaY 输出参数，垂直偏移量，鼠标指针的垂直偏移（16位有符号整数，单位：像素）
	*  @note Event类型必须为MOUSE_MOVE
	*  @note DeltaX和DeltaY表示相对偏移，可以为负值
	*  
	*  使用示例：
	*  @code
	*  FEvent event(EventType::MOUSE_MOVE);
	*  event.SetMouseDelta(100, 200, 10, 20);
	*  uint16 posX, posY;
	*  int16 deltaX, deltaY;
	*  event.GetMouseDelta(posX, posY, deltaX, deltaY);
	*  // posX = 100, posY = 200, deltaX = 10, deltaY = 20
	*  @endcode
		*/
		void GetMouseDelta(uint16& OutPosX, uint16& OutPosY, int16& OutDeltaX, int16& OutDeltaY)
		{
			check(Event == EventType::MOUSE_MOVE);
			OutPosX = Data.MouseMove.PosX;
			OutPosY = Data.MouseMove.PosY;
			OutDeltaX = Data.MouseMove.DeltaX;
			OutDeltaY = Data.MouseMove.DeltaY;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 获取鼠标按键事件数据（Get Mouse Button Event Data）
		*  
		*  该方法用于获取鼠标按键事件的数据。当EventType为MOUSE_DOWN或MOUSE_UP时使用。
		*  该方法会将Web环境的按键编号转换为Windows消息代码。
	*  
	*  鼠标按键数据获取（Mouse Button Data Getting）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 验证Event类型是否为MOUSE_DOWN或MOUSE_UP                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 从Data.MouseButton.Button获取按键编号                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 根据按键编号和事件类型转换为Windows消息代码                 |
	*   |     - 0: WM_LBUTTONDOWN/WM_LBUTTONUP (左键)                      |
	*   |     - 1: WM_MBUTTONDOWN/WM_MBUTTONUP (中键)                      |
	*   |     - 2: WM_RBUTTONDOWN/WM_RBUTTONUP (右键)                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 获取PosX: 水平坐标                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 获取PosY: 垂直坐标                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  按键编号到Windows消息映射（Button Number to Windows Message Mapping）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  按键编号 0 (左键):                                                |
	*   |  - MOUSE_DOWN -> WM_LBUTTONDOWN                                 |
	*   |  - MOUSE_UP -> WM_LBUTTONUP                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  按键编号 1 (中键):                                                |
	*   |  - MOUSE_DOWN -> WM_MBUTTONDOWN                                 |
	*   |  - MOUSE_UP -> WM_MBUTTONUP                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  按键编号 2 (右键):                                                |
	*   |  - MOUSE_DOWN -> WM_RBUTTONDOWN                                 |
	*   |  - MOUSE_UP -> WM_RBUTTONUP                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param OutButton 输出参数，Windows消息代码，标识鼠标按键和事件类型（32位无符号整数）
	*  @param OutPosX 输出参数，水平坐标，鼠标指针的水平位置（16位无符号整数，单位：像素）
	*  @param OutPosY 输出参数，垂直坐标，鼠标指针的垂直位置（16位无符号整数，单位：像素）
	*  @note Event类型必须为MOUSE_DOWN或MOUSE_UP
	*  @note OutButton会被设置为Windows消息代码（仅Windows平台）
	*  @note 按键编号与Web标准兼容（0=左键, 1=中键, 2=右键）
	*  @note 侧键（Thumb01, Thumb02）当前不支持Windows消息映射
	*  
	*  使用示例：
	*  @code
	*  // 鼠标按下事件
	*  FEvent downEvent(EventType::MOUSE_DOWN);
	*  downEvent.SetMouseClick(0, 100, 200);
	*  uint32 button;
	*  uint16 posX, posY;
	*  downEvent.GetMouseClick(button, posX, posY);
	*  // button = WM_LBUTTONDOWN, posX = 100, posY = 200
	*  @endcode
		*/
		void GetMouseClick(uint32 &OutButton/*EMouseButtons::Type& OutButton*/, uint16& OutPosX, uint16& OutPosY)
		{
			check(Event == EventType::MOUSE_DOWN || Event == EventType::MOUSE_UP);
			// Web标准鼠标按键映射：https://developer.mozilla.org/en-US/docs/Web/Events/mousedown
			// 获取原始按键编号
			uint8 Button = Data.MouseButton.Button;
			
			#if defined(_MSC_VER)
			// Windows平台：将Web按键编号转换为Windows消息代码
			switch (Button)
			{
			case 0:  // 左键
			{
				OutButton = Event == EventType::MOUSE_DOWN ? WM_LBUTTONDOWN : WM_LBUTTONUP;
			}
			break;
			case 1:  // 中键
			{
				OutButton = Event == EventType::MOUSE_DOWN ? WM_MBUTTONDOWN : WM_MBUTTONUP;
			}
			break;
			case 2:  // 右键
			{
				OutButton = Event == EventType::MOUSE_DOWN ? WM_RBUTTONDOWN : WM_RBUTTONUP;
			}
			break;
			case 3:  // 侧键1（Thumb01）
			{
				// TODO: 处理侧键1的Windows消息映射
				// OutButton = EMouseButtons::Thumb01;
			}
			break;
			case 4:  // 侧键2（Thumb02）
			{
				// TODO: 处理侧键2的Windows消息映射
				// OutButton = EMouseButtons::Thumb02;
			}
			break;
			default:
			{
				// TODO: 记录错误日志：未知的鼠标按键编号
				// UE_LOG(PixelStreamerInputDevice, Error, TEXT("Unknown Pixel Streaming mouse click with button %d and word 0x%016llx"), Button, Data.Word);
			}
			break;
			}
			#endif // #if defined(_MSC_VER)
			
			// 获取鼠标坐标
			OutPosX = Data.MouseButton.PosX;
			OutPosY = Data.MouseButton.PosY;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 获取鼠标滚轮滚动事件数据（Get Mouse Wheel Event Data）
		*  
		*  该方法用于获取鼠标滚轮滚动事件的数据。当EventType为MOUSE_WHEEL时使用。
	*  
	*  鼠标滚轮数据获取（Mouse Wheel Data Getting）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 验证Event类型是否为MOUSE_WHEEL                                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 获取Delta: 滚轮增量（16位有符号整数）                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 获取PosX: 水平坐标（16位无符号整数）                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 获取PosY: 垂直坐标（16位无符号整数）                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param OutDelta 输出参数，滚轮增量，鼠标滚轮滚动的距离（16位有符号整数，120为标准滚动单位）
	*  @param OutPosX 输出参数，水平坐标，鼠标指针的水平位置（16位无符号整数，单位：像素）
	*  @param OutPosY 输出参数，垂直坐标，鼠标指针的垂直位置（16位无符号整数，单位：像素）
	*  @note Event类型必须为MOUSE_WHEEL
	*  @note Delta值表示滚动方向和距离
	*  @note PosX和PosY表示滚轮滚动时鼠标的位置
	*  
	*  使用示例：
	*  @code
	*  FEvent event(EventType::MOUSE_WHEEL);
	*  event.SetMouseWheel(120, 100, 200);
	*  int16 delta;
	*  uint16 posX, posY;
	*  event.GetMouseWheel(delta, posX, posY);
	*  // delta = 120, posX = 100, posY = 200
	*  @endcode
		*/
		void GetMouseWheel(int16& OutDelta, uint16& OutPosX, uint16& OutPosY)
		{
			check(Event == EventType::MOUSE_WHEEL);
			OutDelta = Data.MouseWheel.Delta;
			OutPosX = Data.MouseWheel.PosX;
			OutPosY = Data.MouseWheel.PosY;
		}

		/**
		*  @author chensong
		*  @date 2022-01-20
		*  @brief 获取触摸事件数据（Get Touch Event Data）
		*  
		*  该方法用于获取触摸事件的数据。当EventType为TOUCH_START、TOUCH_END或TOUCH_MOVE时使用。
		*  支持多触摸操作。
	*  
	*  触摸事件数据获取（Touch Event Data Getting）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 验证Event类型是否为TOUCH_START/TOUCH_END/TOUCH_MOVE          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 获取TouchIndex: 触摸索引（8位无符号整数）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 获取PosX: 水平坐标（16位无符号整数）                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 获取PosY: 垂直坐标（16位无符号整数）                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 获取Force: 压力值（8位无符号整数，0-255）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param OutTouchIndex 输出参数，触摸索引，标识是哪个手指（8位无符号整数，0为第一个手指）
	*  @param OutPosX 输出参数，水平坐标，手指的水平位置（16位无符号整数，单位：像素）
	*  @param OutPosY 输出参数，垂直坐标，手指的垂直位置（16位无符号整数，单位：像素）
	*  @param OutForce 输出参数，压力值，手指施加的压力（8位无符号整数，0-255）
	*  @note Event类型必须为TOUCH_START、TOUCH_END或TOUCH_MOVE
	*  @note 支持多触摸操作（多个手指同时触摸）
	*  @note Force值表示压力大小，某些设备可能不支持
	*  
	*  使用示例：
	*  @code
	*  FEvent event(EventType::TOUCH_START);
	*  event.SetTouch(0, 100, 200, 128);
	*  uint8 touchIndex, force;
	*  uint16 posX, posY;
	*  event.GetTouch(touchIndex, posX, posY, force);
	*  // touchIndex = 0, posX = 100, posY = 200, force = 128
	*  @endcode
		*/
		void GetTouch(uint8& OutTouchIndex, uint16& OutPosX, uint16& OutPosY, uint8& OutForce)
		{
			check(Event == EventType::TOUCH_START || Event == EventType::TOUCH_END || Event == EventType::TOUCH_MOVE);
			OutTouchIndex = Data.Touch.TouchIndex;
			OutPosX = Data.Touch.PosX;
			OutPosY = Data.Touch.PosY;
			OutForce = Data.Touch.Force;
		}
	};

}
#endif // _C_INPUT_DEVICE_EVENT_H_