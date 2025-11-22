/***********************************************************************************************
created: 		2022-01-19

author:			chensong

purpose:		input_device
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

#include "cinput_device.h"
 
#include "cprotocol.h"
#include "cinput_device_event.h"
#include "rtc_base/logging.h"
#include "clog.h"
#if defined(_MSC_VER)
// TODO@chensong 20220711  UE 鼠标控制权问题
#define _WIN32_WINNT 0x0400 
#include <Windows.h>
#include <WinUser.h>
#include <UserEnv.h>
#include <mutex>
 
#include <shellapi.h>
#include "../libdevice/window/device.h"
//void CallMessage(HWND hwnd, int nMsgId, int wParam, int lParam)
//
//{
//
//	WNDPROC fWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC); //获取窗口wndproc
//
//	if (fWndProc != NULL)
//
//	{
//
//		fWndProc(hwnd, nMsgId, wParam, lParam);
//
//	}
//
//}
#endif // WIN
namespace chen {
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 全局屏幕宽度（Global Screen Width）
	*  
	*  该全局变量用于存储当前屏幕的宽度（像素）。用于坐标转换。
	*  
	*  @note 初始化为0，表示屏幕宽度未知
	*  @note 在鼠标事件处理时更新
	*/
	int32_t  g_width = 0;
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 全局屏幕高度（Global Screen Height）
	*  
	*  该全局变量用于存储当前屏幕的高度（像素）。用于坐标转换。
	*  
	*  @note 初始化为0，表示屏幕高度未知
	*  @note 在鼠标事件处理时更新
	*/
	int32_t  g_height = 0;
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 类型别名定义（Type Alias Definitions）
	*  
	*  这些类型别名用于统一输入事件的数据类型，提高代码可读性。
	*  
	*  类型别名说明（Type Alias Description）：
	*  - FKeyCodeType: 按键代码类型 (uint8)
	*  - FCharacterType: 字符类型 (TCHAR)
	*  - FRepeatType: 按键重复类型 (uint8)
	*  - FButtonType: 鼠标按键类型 (uint8)
	*  - FPosType: 坐标位置类型 (uint16，归一化坐标)
	*  - FDeltaType: 坐标偏移类型 (int16，归一化偏移)
	*  - FControllerIndex: 控制器索引类型 (uint8)
	*  - FControllerButtonIndex: 控制器按键索引类型 (uint8)
	*  - FControllerAnalog: 控制器模拟值类型 (double)
	*  - FControllerAxis: 控制器轴类型 (uint8)
	*  
	*  @note 类型别名用于统一数据类型，便于修改和维护
	*/
	using FKeyCodeType = uint8;
	using FCharacterType = TCHAR;
	using FRepeatType = uint8;
	using FButtonType = uint8;
	using FPosType = uint16;
	using FDeltaType = int16;
	using FControllerIndex = uint8;
	using FControllerButtonIndex = uint8;
	using FControllerAnalog = double;
	using FControllerAxis = uint8;

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 注册输入设备处理器宏（Register Input Device Handler Macro）
	*  
	*  该宏用于将二进制消息类型和对应的处理器函数注册到映射表中。
	*  如果注册失败（键已存在），会记录错误日志并返回false。
	*  
	*  注册流程（Registration Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 尝试插入(type, handler_ptr)到m_input_device映射表         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. insert()返回pair<iterator, bool>                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 检查second字段，如果为false表示键已存在                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 如果注册失败，记录错误日志并返回false                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 如果注册成功，继续执行                                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param type 消息类型（EToStreamMsg枚举值）
	*  @param handler_ptr 处理器函数指针（成员函数指针）
	*  @note 宏会在注册失败时记录错误并返回false
	*  @note 用于init()方法中批量注册处理器
	*/
	#define REGISTER_INPUT_DEVICE(type, handler_ptr) \
		if (false == m_input_device.insert(std::make_pair(type, handler_ptr)).second) { \
			ERROR_EX_LOG("[type = %s][handler_ptr = %s]", #type, #handler_ptr); \
			return false; \
		}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 注册RTC输入设备处理器宏（Register RTC Input Device Handler Macro）
	*  
	*  该宏用于将JSON消息类型和对应的处理器函数注册到映射表中。
	*  如果注册失败（键已存在），会记录错误日志并返回false。
	*  
	*  注册流程（Registration Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 尝试插入(type, handler_ptr)到m_rtc_input_device映射表     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. insert()返回pair<iterator, bool>                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 检查second字段，如果为false表示键已存在                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 如果注册失败，记录错误日志并返回false                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 如果注册成功，继续执行                                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param type 消息类型（EToStreamMsg枚举值）
	*  @param handler_ptr 处理器函数指针（RTC成员函数指针）
	*  @note 宏会在注册失败时记录错误并返回false
	*  @note 用于init()方法中批量注册JSON处理器
	*/
	#define REGISTER_RTC_INPUT_DEVICE(type, handler_ptr) \
		if (false == m_rtc_input_device.insert(std::make_pair(type, handler_ptr)).second) { \
			ERROR_EX_LOG("[type = %s][handler_ptr = %s]", #type, #handler_ptr); \
			return false; \
		}



//struct c_init_input_device
//{
//	c_init_input_device()
//	{
//		s_input_device.init();
//	}
//};

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 全局主窗口句柄（Global Main Window Handle）- Windows平台
	*  
	*  该静态全局变量用于存储主窗口句柄（仅Windows平台）。
	*  用于鼠标按键事件的发送。
	*  
	*  @note 仅在Windows平台（_MSC_VER）下使用
	*  @note 初始化为NULL，表示主窗口句柄未设置
	*/
	static HWND g_main_mouse_down_up = NULL;
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 全局子窗口句柄（Global Child Window Handle）- Windows平台
	*  
	*  该静态全局变量用于存储子窗口句柄（仅Windows平台）。
	*  用于鼠标按键事件的发送。
	*  
	*  @note 仅在Windows平台（_MSC_VER）下使用
	*  @note 初始化为NULL，表示子窗口句柄未设置
	*/
	static HWND g_child_mouse_down_up = NULL;
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 构造函数实现（Constructor Implementation）
	*  
	*  该构造函数用于初始化输入设备管理器实例。初始化所有成员变量为默认值。
	*  
	*  初始化说明（Initialization Description）：
	*  - m_stoped: 初始化为false，表示未停止
	*  - m_input_device: 初始化输入设备处理器映射表（空）
	*  - m_rtc_input_device: 初始化RTC输入设备处理器映射表（空）
	*  - m_int_point: 初始化坐标点（0, 0）
	*  - m_all_consumer: 初始化消费者鼠标信息映射表（空）
	*  - m_mouse_id: 初始化为空字符串
	*  - m_input_list: 初始化消息队列（空）
	*  - m_init: 初始化为false，表示未初始化
	*  - m_main_win: 初始化为NULL（Windows平台）
	*  
	*  @note 构造函数初始化映射表为空，需要在init()中注册处理器
	*  @note 所有成员变量使用初始化列表初始化
	*/
	cinput_device::cinput_device() 
		: m_stoped(false)
		, m_input_device()
		,  m_int_point()
		, m_all_consumer()
		, m_mouse_id("")
		, m_input_list()
		, m_init(false)
#if defined(_MSC_VER)
		, m_main_win(NULL)
#endif //#if defined(_MSC_VER)
	{}
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 析构函数实现（Destructor Implementation）
	*  
	*  该析构函数用于清理输入设备管理器实例。使用默认析构函数，
	*  智能指针和容器会自动清理。
	*  
	*  @note 使用默认析构函数，容器和智能指针自动释放
	*  @note 应该在析构前调用Destroy()确保工作线程已停止
	*/
	cinput_device::~cinput_device() {}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 初始化输入设备管理器实现（Initialize Input Device Manager Implementation）
	*  
	*  该方法用于初始化输入设备管理器。注册所有输入事件处理器，创建工作线程。
	*  
	*  初始化流程（Initialization Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 检查是否已初始化（m_init标志）                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 如果已初始化，直接返回true                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 设置m_init标志为true                                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 注册二进制消息处理器到m_input_device映射表                  |
	*   |     - RequestQualityControl, KeyDown, KeyUp, KeyPress          |
	*   |     - MouseEnter, MouseLeave, MouseDown, MouseUp, MouseMove    |
	*   |     - MouseWheel, MouseDoubleClick                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 注册JSON消息处理器到m_rtc_input_device映射表                |
	*   |     - OnRtcKeyChar, OnRtcKeyDown, OnRtcKeyUp, OnRtcKeyPress    |
	*   |     - OnRtcMouseEnter, OnRtcMouseLeave, OnRtcMouseDown等       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 创建工作线程m_thread，执行_work_pthread()方法               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 返回true表示初始化成功                                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  处理器注册格式（Handler Registration Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  二进制消息处理器映射表:                                        |
	*   |  EToStreamMsg::KeyDown -> &cinput_device::OnKeyDown            |
	*   |  EToStreamMsg::KeyUp -> &cinput_device::OnKeyUp                |
	*   |  EToStreamMsg::MouseDown -> &cinput_device::OnMouseDown        |
	*   |  ...                                                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  JSON消息处理器映射表:                                          |
	*   |  EToStreamMsg::KeyDown -> &cinput_device::OnRtcKeyDown         |
	*   |  EToStreamMsg::KeyUp -> &cinput_device::OnRtcKeyUp             |
	*   |  ...                                                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @return 返回true表示初始化成功，false表示初始化失败
	*  @note 该方法保证只初始化一次（检查m_init标志）
	*  @note 如果注册处理器失败，会记录错误并返回false
	*  @note 创建的工作线程会在Destroy()中停止
	*  
	*  使用示例：
	*  @code
	*  if (s_input_device.init()) {
	*      s_input_device.startup();
	*  }
	*  @endcode
	*/
	bool cinput_device::init()
	{
		if (m_init)
		{
			return true;
		}
		m_init = true;
		
		// 注册二进制消息处理器
		REGISTER_INPUT_DEVICE(RequestQualityControl, &cinput_device::OnKeyChar);
		REGISTER_INPUT_DEVICE(KeyDown, &cinput_device::OnKeyDown);
		REGISTER_INPUT_DEVICE(KeyUp, &cinput_device::OnKeyUp);
		REGISTER_INPUT_DEVICE(KeyPress, &cinput_device::OnKeyPress);
		REGISTER_INPUT_DEVICE(MouseEnter, &cinput_device::OnMouseEnter);
		REGISTER_INPUT_DEVICE(MouseLeave, &cinput_device::OnMouseLeave);
		REGISTER_INPUT_DEVICE(MouseDown, &cinput_device::OnMouseDown);
		REGISTER_INPUT_DEVICE(MouseUp, &cinput_device::OnMouseUp);
		REGISTER_INPUT_DEVICE(MouseMove, &cinput_device::OnMouseMove);
		REGISTER_INPUT_DEVICE(MouseWheel, &cinput_device::OnMouseWheel);
		REGISTER_INPUT_DEVICE(MouseDoubleClick, &cinput_device::OnMouseDoubleClick);

		// 注册JSON消息处理器
		REGISTER_RTC_INPUT_DEVICE(RequestQualityControl, &cinput_device::OnRtcKeyChar);
		REGISTER_RTC_INPUT_DEVICE(KeyDown, &cinput_device::OnRtcKeyDown);
		REGISTER_RTC_INPUT_DEVICE(KeyUp, &cinput_device::OnRtcKeyUp);
		REGISTER_RTC_INPUT_DEVICE(KeyPress, &cinput_device::OnRtcKeyPress);
		REGISTER_RTC_INPUT_DEVICE(MouseEnter, &cinput_device::OnRtcMouseEnter);
		REGISTER_RTC_INPUT_DEVICE(MouseLeave, &cinput_device::OnRtcMouseLeave);
		REGISTER_RTC_INPUT_DEVICE(MouseDown, &cinput_device::OnRtcMouseDown);
		REGISTER_RTC_INPUT_DEVICE(MouseUp, &cinput_device::OnRtcMouseUp);
		REGISTER_RTC_INPUT_DEVICE(MouseMove, &cinput_device::OnRtcMouseMove);
		REGISTER_RTC_INPUT_DEVICE(MouseWheel, &cinput_device::OnRtcMouseWheel);
		REGISTER_RTC_INPUT_DEVICE(MouseDoubleClick, &cinput_device::OnRtcMouseDoubleClick);

		// 创建工作线程处理消息队列
		std::thread td(&cinput_device::_work_pthread, this);
		m_thread.swap(td);
		return true;
	}
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 销毁输入设备管理器实现（Destroy Input Device Manager Implementation）
	*  
	*  该方法用于销毁输入设备管理器。停止工作线程并清理所有资源。
	*  
	*  销毁流程（Destruction Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 设置m_stoped标志为true，通知工作线程退出                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 如果工作线程可加入，等待线程结束                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 清理输入设备处理器映射表                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 清理RTC输入设备处理器映射表                                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 清理所有消费者的鼠标信息                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 加锁清理消息队列                                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note 该方法会停止工作线程并清理所有资源
	*  @note 使用互斥锁保护消息队列清理操作
	*  @note 销毁后对象不能再使用
	*  
	*  使用示例：
	*  @code
	*  s_input_device.Destroy();
	*  @endcode
	*/
	void cinput_device::Destroy()
	{
		m_stoped = true;
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		m_input_device.clear();
		m_rtc_input_device.clear();
		m_all_consumer.clear();
		
		{
			std::lock_guard<std::mutex>  lock(m_input_mutex);
			m_input_list.clear();
		}
	}
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 启动输入设备管理器实现（Startup Input Device Manager Implementation）
	*  
	*  该方法用于启动输入设备管理器。目前为空实现，预留扩展接口。
	*  
	*  @note 该方法目前为空实现，可以在将来添加启动逻辑
	*  @note 工作线程在init()中创建，不需要在startup()中创建
	*  
	*  使用示例：
	*  @code
	*  s_input_device.init();
	*  s_input_device.startup();
	*  @endcode
	*/
	void cinput_device::startup()
	{
		// 预留扩展接口，可以在将来添加启动逻辑
	}
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 设置坐标点实现（Set Point Implementation）
	*  
	*  该方法用于设置鼠标的初始坐标点。坐标点用于坐标转换和反量化。
	*  
	*  坐标点格式（Point Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  x: 水平坐标 (32位，像素单位)                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  y: 垂直坐标 (32位，像素单位)                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param x 水平坐标，单位为像素
	*  @param y 垂直坐标，单位为像素
	*  @return 返回true表示设置成功
	*  @note 坐标点用于坐标转换和反量化操作
	*  @note 坐标点会存储在m_int_point成员变量中
	*  
	*  使用示例：
	*  @code
	*  s_input_device.set_point(1920, 1080);  // 设置屏幕分辨率为1920x1080
	*  @endcode
	*/
	bool cinput_device::set_point(uint32 x, uint32 y)
	{
		m_int_point.X = x;
		m_int_point.Y = y;
		return true;
	}
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理数据通道消息实现 - DataBuffer版本（Handle Data Channel Message Implementation - DataBuffer Version）
	*  
	*  该方法用于处理来自数据通道的二进制消息。解析消息类型，查找对应的处理器函数，
	*  并调用处理器函数处理消息。同时会统计处理时间，如果超过900微秒会记录警告日志。
	*  
	*  消息处理流程（Message Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 设置当前操作的鼠标ID（m_mouse_id = consumer_id）            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 从DataBuffer中获取消息数据指针和大小                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 解析消息类型（EToStreamMsg枚举，第一个字节）                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 在m_input_device映射表中查找对应的处理器函数                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 如果未找到处理器，记录错误并返回false                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 记录处理开始时间                                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 调用处理器函数处理消息（成员函数指针调用）                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  8. 记录处理结束时间，计算处理耗时                                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  9. 如果处理耗时超过900微秒，记录警告日志                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  10. 返回true表示处理成功                                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  二进制消息格式（Binary Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type (8 bits: EToStreamMsg枚举)                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  payload_data (可变长度，根据消息类型不同)                        |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | 键盘消息: key_code (8 bits), repeat (8 bits)                  | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | 鼠标消息: x (16 bits), y (16 bits), button (8 bits)          | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | 滚轮消息: delta (16 bits)                                    | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  性能监控（Performance Monitoring）：
	*  - 处理耗时超过900微秒会记录警告日志
	*  - 用于监控输入事件处理的性能
	*  
	*  @param consumer_id 消费者ID，标识消息来源的消费者
	*  @param Buffer 数据缓冲区，包含二进制消息数据
	*  @return 返回true表示处理成功，false表示处理失败
	*  @note 该方法会立即处理消息，不放入队列
	*  @note 使用成员函数指针调用处理器函数
	*  @note 处理耗时超过900微秒会记录警告日志
	*  
	*  使用示例：
	*  @code
	*  webrtc::DataBuffer buffer = ...;
	*  s_input_device.OnMessage("consumer_1", buffer);
	*  @endcode
	*/
	bool cinput_device::OnMessage(const std::string & consumer_id, const webrtc::DataBuffer& Buffer)
	{
		m_mouse_id = consumer_id;
		const uint8* Data = Buffer.data.data();
		uint32 Size = static_cast<uint32>(Buffer.data.size());

		// 解析消息类型（第一个字节）
		GET(EToStreamMsg, MsgType);
		
		// 查找对应的处理器函数
		M_INPUT_DEVICE_MAP::iterator iter =  m_input_device.find(MsgType);
		if (iter == m_input_device.end())
		{ 
			ERROR_EX_LOG("input_device msg_type = %d not find failed !!!", MsgType);
			return false;
		}
		 
		// 记录处理开始时间，用于性能监控
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::microseconds microseconds;
		uint32_t elapse = 0;

		// 调用处理器函数处理消息（成员函数指针调用）
		(this->*(iter->second))(Data, Size);
		
		// 计算处理耗时
		cur_time_ms = std::chrono::steady_clock::now();
		dur = cur_time_ms - pre_time;
		microseconds = std::chrono::duration_cast<std::chrono::microseconds>(dur);
		elapse = static_cast<uint32_t>(microseconds.count());
		
		// 如果处理耗时超过900微秒，记录警告日志
		if (elapse > 900)
		{
			WARNING_EX_LOG("input_device  microseconds = %lu", microseconds.count());
		}
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理数据通道消息实现 - JSON版本（Handle Data Channel Message Implementation - JSON Version）
	*  
	*  该方法用于处理来自数据通道的JSON消息。解析JSON格式，提取消息类型，
	*  查找对应的处理器函数，并调用处理器函数处理消息。同时会统计处理时间，
	*  如果超过900微秒会记录警告日志。
	*  
	*  消息处理流程（Message Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 检查JSON消息中是否存在"type"字段                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 如果不存在"type"字段，记录警告并返回false                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 检查"type"字段是否为数字类型                                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 如果"type"不是数字类型，记录警告并返回false                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 从JSON中提取消息类型（EToStreamMsg枚举）                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 在m_rtc_input_device映射表中查找对应的处理器函数           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 如果未找到处理器，记录错误并返回false                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  8. 记录处理开始时间                                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  9. 调用处理器函数处理JSON消息（成员函数指针调用）               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  10. 记录处理结束时间，计算处理耗时                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  11. 如果处理耗时超过900微秒，记录警告日志                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  12. 返回true表示处理成功                                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  JSON消息格式（JSON Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  {                                                               |
	*   |    "type": EToStreamMsg枚举值 (数字类型)                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "keyCode": 键码 (数字类型，键盘事件)                           |
	*   |    "x": 水平坐标 (数字类型，鼠标事件)                             |
	*   |    "y": 垂直坐标 (数字类型，鼠标事件)                             |
	*   |    "button": 鼠标按键 (数字类型，鼠标事件)                        |
	*   |    "delta": 滚轮增量 (数字类型，滚轮事件)                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  }                                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param datachannel JSON对象，包含输入事件数据
	*  @return 返回true表示处理成功，false表示处理失败
	*  @note 该方法会立即处理消息，不放入队列
	*  @note 使用成员函数指针调用处理器函数
	*  @note 处理耗时超过900微秒会记录警告日志
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::KeyDown}, {"keyCode", 65}};
	*  s_input_device.OnMessage(msg);
	*  @endcode
	*/
	bool cinput_device::OnMessage(const nlohmann::json & datachannel)
	{
		// 检查JSON消息中是否存在"type"字段
		if (datachannel.find("type") == datachannel.end())
		{
			WARNING_EX_LOG("not find msg_type [data = %s]", datachannel.dump().c_str());
			return false;
		}
		
		// 检查"type"字段是否为数字类型
		if (!datachannel["type"].is_number() )
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", datachannel.dump().c_str());
			return false;
		}
		
		// 从JSON中提取消息类型
		EToStreamMsg msg_type = datachannel["type"].get<EToStreamMsg>();
		
		// 查找对应的处理器函数
		M_RTC_INPUT_DEVICE_MAP::iterator iter = m_rtc_input_device.find(msg_type);
		if (iter == m_rtc_input_device.end())
		{
			ERROR_EX_LOG("rtc input_device msg_type = %d not find failed !!!", msg_type);
			return false;
		}

		// 记录处理开始时间，用于性能监控
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::microseconds microseconds;
		uint32_t elapse = 0;

		// 调用处理器函数处理JSON消息（成员函数指针调用）
		(this->*(iter->second))(datachannel);
		
		// 计算处理耗时
		cur_time_ms = std::chrono::steady_clock::now();
		dur = cur_time_ms - pre_time;
		microseconds = std::chrono::duration_cast<std::chrono::microseconds>(dur);
		elapse = static_cast<uint32_t>(microseconds.count());
		
		// 如果处理耗时超过900微秒，记录警告日志
		if (elapse > 900)
		{
			WARNING_EX_LOG("input_device  microseconds = %lu", microseconds.count());
		}
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 插入消息到队列实现（Insert Message to Queue Implementation）
	*  
	*  该方法用于将消息插入到处理队列。工作线程会从队列中取出消息并处理。
	*  使用互斥锁保护队列，确保线程安全。
	*  
	*  消息插入流程（Message Insertion Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 获取m_input_mutex互斥锁                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 将Buffer添加到m_input_list队列末尾                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 释放互斥锁（lock_guard自动释放）                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 工作线程从队列中取出消息并处理                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  消息队列操作（Message Queue Operation）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  m_input_list (FIFO队列)                                       |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | [Buffer 1] -> [Buffer 2] -> [Buffer 3] -> ...            | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  ^                                                              |
	*   |  队列头部（工作线程从此处取出）                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  push_back(): 将消息添加到队列末尾                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param Buffer 数据缓冲区，包含要处理的消息数据
	*  @note 该方法使用互斥锁保护队列，线程安全
	*  @note 消息会在工作线程中异步处理
	*  @note 使用lock_guard自动管理互斥锁，避免死锁
	*  
	*  使用示例：
	*  @code
	*  webrtc::DataBuffer buffer = ...;
	*  s_input_device.insert_message(buffer);
	*  @endcode
	*/
	void cinput_device::insert_message(const webrtc::DataBuffer & Buffer)
	{
		{
			std::lock_guard<std::mutex> lock(m_input_mutex);
			m_input_list.push_back(Buffer);
		}
	}


	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 请求质量控制实现（Request Quality Control Implementation）
	*  
	*  该方法用于处理质量控制请求消息。用于控制视频质量参数。
	*  目前为占位实现，预留扩展接口。
	*  
	*  质量控制消息格式（Quality Control Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::RequestQualityControl             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  quality_level (8 bits: 质量级别)                                |
	*   |  - 0: 低质量                                                     |
	*   |  - 1: 中等质量                                                   |
	*   |  - 2: 高质量                                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 该方法目前为占位实现，需要在将来添加质量控制逻辑
	*  @note 用于远程控制视频质量
	*  
	*  使用示例：
	*  @code
	*  // 目前为占位实现，将在将来添加质量控制逻辑
	*  @endcode
	*/
	bool cinput_device::OnRequestQualityControl(const uint8*& Data, uint32 size)
	{
		// TODO: 这边需要启动引擎推送视频流，控制视频质量参数
		return true;
	}
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理字符输入事件实现 - 二进制版本（Handle Key Character Event Implementation - Binary Version）
	*  
	*  该方法用于处理字符输入事件。当用户输入字符时，会将字符转换为本地输入操作。
	*  目前为占位实现，预留扩展接口。
	*  
	*  字符输入消息格式（Key Character Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::KeyChar                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  char_code (16 bits: 字符Unicode编码)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  modifiers (8 bits: 修饰键标志)                                  |
	*   |  - Bit 0: Ctrl                                                   |
	*   |  - Bit 1: Shift                                                  |
	*   |  - Bit 2: Alt                                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 该方法目前为占位实现，需要在将来添加字符输入逻辑
	*  @note 字符输入会触发本地字符输入事件
	*  @note 用于文本输入场景
	*  
	*  使用示例：
	*  @code
	*  // 目前为占位实现，将在将来添加字符输入逻辑
	*  @endcode
	*/
	bool cinput_device::OnKeyChar(const uint8*& Data,   uint32 size)
	{
		NORMAL_LOG("KeyChar");
		// TODO: 实现字符输入逻辑
		// 可以调用 Windows API: PostMessageW(hwnd, WM_CHAR, Character, 1)
		return true;
	}
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理字符输入事件实现 - JSON版本（Handle Key Character Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的字符输入事件。目前为占位实现，预留扩展接口。
	*  
	*  JSON消息格式：
	*  {
	*    "type": "keyChar",
	*    "char": "A",
	*    "code": 65,
	*    "modifiers": {"ctrl": false, "shift": true, "alt": false}
	*  }
	*  
	*  @param data JSON对象，包含字符输入数据
	*  @return 返回false表示未实现
	*  @note 该方法目前为占位实现，需要在将来添加JSON字符输入逻辑
	*  @note 用于处理JSON格式的字符输入事件
	*  
	*  使用示例：
	*  @code
	*  // 目前为占位实现，将在将来添加JSON字符输入逻辑
	*  @endcode
	*/
	bool cinput_device::OnRtcKeyChar(const nlohmann::json & data)
	{
		// TODO: 实现JSON格式的字符输入逻辑
		return false;
	}
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理按键按下事件实现 - 二进制版本（Handle Key Down Event Implementation - Binary Version）
	*  
	*  该方法用于处理按键按下事件。将远程按键按下操作转换为本地按键事件。
	*  解析消息数据，创建按键事件，并触发本地按键事件。
	*  
	*  按键按下消息格式（Key Down Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::KeyDown                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  key_code (8 bits: 虚拟键码)                                    |
	*   |  - 0x41: 'A'键                                                  |
	*   |  - 0x0D: Enter键                                                |
	*   |  - 0x1B: Escape键                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  repeat (8 bits: 按键重复标志)                                  |
	*   |  - 0: 首次按下                                                   |
	*   |  - 1: 按键重复                                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  按键事件处理流程（Key Event Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 解析消息数据：提取key_code和repeat                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证消息数据大小（Size == 0表示解析完成）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 创建按键事件：FEvent(EventType::KEY_DOWN)                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 设置按键事件参数：SetKeyDown(KeyCode, Repeat != 0)         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 记录日志：NORMAL_LOG("OnKeyDown==KeyCode = %u, Repeat = %u") |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 触发本地按键事件（ProcessEvent）                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param Size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 按键按下会触发本地按键事件
	*  @note keydown事件在所有键按下时都会触发
	*  @note repeat标志表示按键是否重复（长按）
	*  
	*  使用示例：
	*  @code
	*  // 由OnMessage()方法调用，无需手动调用
	*  @endcode
	*/
	bool cinput_device::OnKeyDown(const uint8*& Data,  uint32 Size)
	{
		// 解析消息数据：提取按键代码和重复标志
		GET(FKeyCodeType, KeyCode);
		GET(FRepeatType, Repeat);
		
		// 验证消息数据大小（Size == 0表示解析完成）
		checkf(Size == 0, TEXT("%d"), Size);
		
		// 创建按键事件并设置参数
		FEvent KeyDownEvent(EventType::KEY_DOWN);
		KeyDownEvent.SetKeyDown(KeyCode, Repeat != 0);
		NORMAL_LOG("OnKeyDown==KeyCode = %u, Repeat = %u", KeyCode, Repeat);
		keyboard_update(event, VK_SPACE, false, 0);
		// TODO: 触发本地按键事件（ProcessEvent）
		// KeyDownEvent.GetKeyDown();
		// ProcessEvent(KeyDownEvent);
		
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理按键按下事件实现 - JSON版本（Handle Key Down Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的按键按下事件。解析JSON数据，创建按键事件，并触发本地按键事件。
	*  
	*  JSON消息格式（JSON Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  {                                                               |
	*   |    "type": EToStreamMsg::KeyDown (数字类型)                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "keyCode": 键码 (数字类型，如65表示'A')                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "repeat": 重复标志 (数字类型，0或1)                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  }                                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  JSON消息处理流程（JSON Message Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 检查JSON中是否存在"keyCode"和"repeat"字段                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证字段类型是否为数字类型                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 从JSON中提取keyCode和repeat值                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 创建按键事件：FEvent(EventType::KEY_DOWN)                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 设置按键事件参数：SetKeyDown(KeyCode, Repeat != 0)         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 记录日志：NORMAL_LOG("OnKeyDown==KeyCode = %u, Repeat = %u") |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param data JSON对象，包含按键按下数据
	*  @return 返回true表示处理成功，false表示处理失败
	*  @note 用于处理JSON格式的按键按下事件
	*  @note 如果JSON中缺少必要字段或类型不正确，会记录警告并返回false
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::KeyDown}, {"keyCode", 65}, {"repeat", 0}};
	*  s_input_device.OnRtcKeyDown(msg);
	*  @endcode
	*/
	bool cinput_device::OnRtcKeyDown(const nlohmann::json & data)
	{
		//GET(FKeyCodeType, KeyCode);
		//GET(FRepeatType, Repeat);
		 
		// 检查JSON中是否存在"keyCode"和"repeat"字段
		if (data.find("keyCode") == data.end() || data.find("repeat") == data.end())
		{
			WARNING_EX_LOG("not find 'keycode' or 'repeat' [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 验证字段类型是否为数字类型
		if (!data["keyCode"].is_number() || !data["repeat"].is_number())
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 从JSON中提取keyCode和repeat值
		FKeyCodeType KeyCode = data["keyCode"].get<FKeyCodeType>();
		FRepeatType  Repeat = data["repeat"].get<FKeyCodeType>();
		
		// 创建按键事件并设置参数
		FEvent KeyDownEvent(EventType::KEY_DOWN);
		KeyDownEvent.SetKeyDown(KeyCode, Repeat != 0);
		NORMAL_LOG("OnKeyDown==KeyCode = %u, Repeat = %u", KeyCode, Repeat);
 
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理按键释放事件实现 - 二进制版本（Handle Key Up Event Implementation - Binary Version）
	*  
	*  该方法用于处理按键释放事件。将远程按键释放操作转换为本地按键事件。
	*  解析消息数据，创建按键释放事件，并触发本地按键事件。
	*  
	*  按键释放消息格式（Key Up Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::KeyUp                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  key_code (8 bits: 虚拟键码)                                    |
	*   |  - 0x41: 'A'键                                                  |
	*   |  - 0x0D: Enter键                                                |
	*   |  - 0x1B: Escape键                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  按键释放事件处理流程（Key Up Event Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 解析消息数据：提取key_code                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证消息数据大小（Size == 0表示解析完成）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 创建按键释放事件：FEvent(EventType::KEY_UP)                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 设置按键释放事件参数：SetKeyUp(KeyCode)                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 记录日志：NORMAL_LOG("OnKeyUp==KeyCode = %u")                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 触发本地按键释放事件（ProcessEvent）                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param Size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 按键释放会触发本地按键释放事件
	*  @note keyup事件在键释放时触发
	*  @note 需要与keydown事件配对使用
	*  
	*  使用示例：
	*  @code
	*  // 由OnMessage()方法调用，无需手动调用
	*  @endcode
	*/
	bool cinput_device::OnKeyUp(const uint8*& Data,   uint32 Size)
	{
		// 解析消息数据：提取按键代码
		GET(FKeyCodeType, KeyCode);
		
		// 验证消息数据大小（Size == 0表示解析完成）
		checkf(Size == 0, TEXT("%d"), Size);
		
		// 创建按键释放事件并设置参数
		FEvent KeyUpEvent(EventType::KEY_UP);
		KeyUpEvent.SetKeyUp(KeyCode);
		NORMAL_LOG("OnKeyUp==KeyCode = %u", KeyCode);
		keyboard_update(KeyCode, VK_SPACE, true, 0);
		return true;
	}
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理按键释放事件实现 - JSON版本（Handle Key Up Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的按键释放事件。解析JSON数据，创建按键释放事件，并触发本地按键事件。
	*  
	*  JSON消息格式（JSON Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  {                                                               |
	*   |    "type": EToStreamMsg::KeyUp (数字类型)                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "keyCode": 键码 (数字类型，如65表示'A')                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "repeat": 重复标志 (数字类型，可选，0或1)                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  }                                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param data JSON对象，包含按键释放数据
	*  @return 返回true表示处理成功，false表示处理失败
	*  @note 用于处理JSON格式的按键释放事件
	*  @note 如果JSON中缺少必要字段或类型不正确，会记录警告并返回false
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::KeyUp}, {"keyCode", 65}};
	*  s_input_device.OnRtcKeyUp(msg);
	*  @endcode
	*/
	bool cinput_device::OnRtcKeyUp(const nlohmann::json & data)
	{
		// 检查JSON中是否存在"keyCode"和"repeat"字段（repeat可选）
		if (data.find("keyCode") == data.end() || data.find("repeat") == data.end())
		{
			WARNING_EX_LOG("not find 'keycode' or 'repeat' [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 验证字段类型是否为数字类型
		if (!data["keyCode"].is_number() || !data["repeat"].is_number())
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 从JSON中提取keyCode值（repeat字段暂不使用）
		FKeyCodeType KeyCode = data["keyCode"].get<FKeyCodeType>();
		FRepeatType  Repeat = data["repeat"].get<FKeyCodeType>();
		
		// 创建按键释放事件并设置参数
		FEvent KeyUpEvent(EventType::KEY_UP);
		KeyUpEvent.SetKeyUp(KeyCode);
		NORMAL_LOG("OnKeyUp==KeyCode = %u", KeyCode);
 
		return true;
	}
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理按键按下事件（字符键）实现 - 二进制版本（Handle Key Press Event Implementation - Binary Version）
	*  
	*  该方法用于处理按键按下事件（字符键）。keypress事件只在按下字符键时触发，
	*  与keydown不同，keydown在所有键按下时都会触发。
	*  
	*  事件触发顺序（Event Trigger Sequence）：
	*  - keydown: 按下键盘键时立即触发（所有键）
	*  - keypress: 紧接着keydown触发（仅字符键，如'A', '1'等）
	*  - keyup: 释放键盘键时触发（所有键）
	*  
	*  按键按下消息格式（Key Press Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::KeyPress                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  character (8 bits: 字符Unicode编码)                             |
	*   |  - 0x41: 'A'                                                     |
	*   |  - 0x30: '0'                                                     |
	*   |  - 0x20: 空格                                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param Size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note keypress只在字符键按下时触发
	*  @note 功能键（如F1-F12）不会触发keypress事件
	*  @note 需要与keydown和keyup事件配对使用
	*  
	*  使用示例：
	*  @code
	*  // 由OnMessage()方法调用，无需手动调用
	*  @endcode
	*/
	bool cinput_device::OnKeyPress(const uint8*& Data,   uint32 Size)
	{
		// TODO: KeyPress事件处理逻辑
		// 解析消息数据：提取字符编码
		GET(FCharacterType, Character);
		
		// 验证消息数据大小（Size == 0表示解析完成）
		checkf(Size == 0, TEXT("%d"), Size);
		
		// 创建按键按下事件并设置参数
		FEvent KeyUpEvent(EventType::KEY_PRESS);
		KeyUpEvent.SetKeyUp(Character);
		NORMAL_LOG("OnKeyPress==KeyCode = %u", Character);
		 
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理按键按下事件（字符键）实现 - JSON版本（Handle Key Press Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的按键按下事件（字符键）。解析JSON数据，创建按键事件。
	*  
	*  JSON消息格式：
	*  {
	*    "type": "keypress",
	*    "keyCode": 65
	*  }
	*  
	*  @param data JSON对象，包含按键按下数据
	*  @return 返回false表示未实现
	*  @note 用于处理JSON格式的按键按下事件
	*  @note 目前为占位实现，需要在将来添加完整逻辑
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::KeyPress}, {"keyCode", 65}};
	*  s_input_device.OnRtcKeyPress(msg);
	*  @endcode
	*/
	bool cinput_device::OnRtcKeyPress(const nlohmann::json & data)
	{
		// 检查JSON中是否存在"keyCode"字段
		if (data.find("keyCode") == data.end() )
		{
			WARNING_EX_LOG("not find 'keycode'  [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 验证字段类型是否为数字类型
		if (!data["keyCode"].is_number()  )
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 从JSON中提取字符编码
		FCharacterType Character = data["keyCode"].get<FCharacterType>();

		// 创建按键按下事件并设置参数
		FEvent KeyUpEvent(EventType::KEY_PRESS);
		KeyUpEvent.SetKeyUp(Character);
		NORMAL_LOG("OnKeyPress==KeyCode = %u", Character);
 
		return false;  // 目前返回false表示未完全实现
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标进入控制范围事件实现 - 二进制版本（Handle Mouse Enter Event Implementation - Binary Version）
	*  
	*  该方法用于处理鼠标进入控制范围事件。当鼠标进入可控制区域时触发。
	*  目前为占位实现，预留扩展接口。
	*  
	*  鼠标进入消息格式（Mouse Enter Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::MouseEnter                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  x (16 bits: 水平坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  y (16 bits: 垂直坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 鼠标进入控制范围时会触发此事件
	*  @note 用于通知鼠标已进入可控制区域
	*  @note 目前为占位实现，可以在将来添加鼠标状态管理逻辑
	*  
	*  使用示例：
	*  @code
	*  // 由OnMessage()方法调用，无需手动调用
	*  @endcode
	*/
	bool cinput_device::OnMouseEnter(const uint8*& Data,   uint32 size)
	{
		// TODO: OnMouseEnter事件处理逻辑
		// 可以在将来添加：将消费者添加到m_all_consumer映射表中
		/*if (!m_all_consumer.insert(std::make_pair(m_mouse_id, std::map<uint32, cmouse_info>())).second)
		{
			WARNING_EX_LOG("mouse enter insert [mouse_id = %s] failed !!!", m_mouse_id.c_str());
			return false;
		}*/
		NORMAL_EX_LOG("OnMouseEnter===>");
 
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标进入控制范围事件实现 - JSON版本（Handle Mouse Enter Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的鼠标进入事件。目前为占位实现，预留扩展接口。
	*  
	*  @param data JSON对象，包含鼠标进入数据
	*  @return 返回false表示未实现
	*  @note 用于处理JSON格式的鼠标进入事件
	*  @note 目前为占位实现，需要在将来添加完整逻辑
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::MouseEnter}, {"x", 100}, {"y", 200}};
	*  s_input_device.OnRtcMouseEnter(msg);
	*  @endcode
	*/
	bool cinput_device::OnRtcMouseEnter(const nlohmann::json & data)
	{
		// TODO: 实现JSON格式的鼠标进入事件处理逻辑
		return false;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标离开控制范围事件实现 - 二进制版本（Handle Mouse Leave Event Implementation - Binary Version）
	*  
	*  该方法用于处理鼠标离开控制范围事件。当鼠标离开可控制区域时触发。
	*  目前为占位实现，预留扩展接口。
	*  
	*  鼠标离开消息格式（Mouse Leave Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::MouseLeave                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  x (16 bits: 水平坐标)                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  y (16 bits: 垂直坐标)                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 鼠标离开控制范围时会触发此事件
	*  @note 用于通知鼠标已离开可控制区域
	*  @note 目前为占位实现，可以在将来添加鼠标状态清理逻辑
	*  
	*  使用示例：
	*  @code
	*  // 由OnMessage()方法调用，无需手动调用
	*  @endcode
	*/
	bool cinput_device::OnMouseLeave(const uint8*& Data,   uint32 size)
	{
		// TODO@chensong 2022-01-20  OnMouseLeave 
		std::map<std::string, std::map<uint32, cmouse_info>>::iterator iter =  m_all_consumer.find(m_mouse_id);
		/*if (iter == m_all_consumer.end())
		{
			WARNING_EX_LOG("mouse leave  not find  [mouse_id = %s] failed !!!", m_mouse_id.c_str());
			return false;
		}
		iter->second.clear();
		m_all_consumer.erase(iter);*/

		//std::map<uint32, cmouse_info>& cmouse_info = iter->second;
		NORMAL_EX_LOG("OnMouseLeave===>");
 
		return true; 
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标离开控制范围事件实现 - JSON版本（Handle Mouse Leave Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的鼠标离开事件。目前为占位实现，预留扩展接口。
	*  
	*  @param data JSON对象，包含鼠标离开数据
	*  @return 返回false表示未实现
	*  @note 用于处理JSON格式的鼠标离开事件
	*  @note 目前为占位实现，需要在将来添加完整逻辑
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::MouseLeave}, {"x", 100}, {"y", 200}};
	*  s_input_device.OnRtcMouseLeave(msg);
	*  @endcode
	*/
	bool cinput_device::OnRtcMouseLeave(const nlohmann::json & data)
	{
		// TODO: 实现JSON格式的鼠标离开事件处理逻辑
		return false;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标按下事件实现 - 二进制版本（Handle Mouse Down Event Implementation - Binary Version）
	*  
	*  该方法用于处理鼠标按下事件。当用户按下鼠标按键时，会将远程鼠标按下操作
	*  转换为本地鼠标事件。坐标需要反量化和反归一化处理。注意：在Web环境中，左右键可能互换。
	*  
	*  鼠标按下消息格式（Mouse Down Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::MouseDown                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  button (8 bits: 鼠标按键标志)                                  |
	*   |  - 0: 左键                                                       |
	*   |  - 1: 右键                                                       |
	*   |  - 2: 中键                                                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  x (16 bits: 水平坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  y (16 bits: 垂直坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  鼠标按下事件处理流程（Mouse Down Event Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 解析消息数据：提取button、PosX和PosY                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证消息数据大小（Size == 0表示解析完成）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 坐标反量化和反归一化：_UnquantizeAndDenormalize(PosX, PosY)  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 更新全局屏幕坐标：g_width = PosX, g_height = PosY            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 创建鼠标按下事件：FEvent(EventType::MOUSE_DOWN)              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 调用button_mouse()处理鼠标按键事件（按下，false）            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  鼠标按键映射（Mouse Button Mapping）：
	*  - Web环境可能左右键互换：remote_left -> local_right
	*  - 需要根据平台和配置进行映射
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param Size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 鼠标按下会触发本地鼠标按下事件（onclick事件）
	*  @note 在Web环境中，左右键可能互换（right <-> left）
	*  @note 坐标需要反量化和反归一化处理
	*  
	*  使用示例：
	*  @code
	*  // 由OnMessage()方法调用，无需手动调用
	*  @endcode
	*/
	bool cinput_device::OnMouseDown(const uint8*& Data,   uint32 Size)
	{
		// 解析消息数据：提取鼠标按键和坐标
		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		
		// 验证消息数据大小（Size == 0表示解析完成）
		checkf(Size == 0, TEXT("%d"), Size);
		 
		// 坐标反量化和反归一化
		_UnquantizeAndDenormalize(PosX, PosY);
		NORMAL_EX_LOG("Button = %d, PosX = %d, PoxY = %d", Button, PosX, PosY);
		
		// 更新全局屏幕坐标
		g_width = PosX;
		g_height = PosY;
		
		// 创建鼠标按下事件并处理
		FEvent MouseDownEvent(EventType::MOUSE_DOWN);
		button_mouse(MouseDownEvent, PosX, PosY, Button, false);
		 
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标按下事件实现 - JSON版本（Handle Mouse Down Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的鼠标按下事件。解析JSON数据，坐标反量化和反归一化，
	*  创建鼠标事件，并触发本地鼠标事件。
	*  
	*  JSON消息格式（JSON Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  {                                                               |
	*   |    "type": EToStreamMsg::MouseDown (数字类型)                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "button": 鼠标按键 (数字类型，0=左键，1=右键，2=中键)          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "x": 水平坐标 (数字类型，归一化或量化值)                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "y": 垂直坐标 (数字类型，归一化或量化值)                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  }                                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  JSON消息处理流程（JSON Message Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 检查JSON中是否存在"button"、"x"和"y"字段                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证字段类型是否为数字类型                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 从JSON中提取button、PosX和PosY值                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 坐标反量化和反归一化：_UnquantizeAndDenormalize(PosX, PosY)  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 创建鼠标按下事件并设置参数                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 更新全局屏幕坐标                                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param data JSON对象，包含鼠标按下数据
	*  @return 返回true表示处理成功，false表示处理失败
	*  @note 用于处理JSON格式的鼠标按下事件
	*  @note 如果JSON中缺少必要字段或类型不正确，会记录警告并返回false
	*  @note 坐标需要反量化和反归一化处理
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::MouseDown}, {"button", 0}, {"x", 100}, {"y", 200}};
	*  s_input_device.OnRtcMouseDown(msg);
	*  @endcode
	*/
	bool cinput_device::OnRtcMouseDown(const nlohmann::json & data)
	{
		// 检查JSON中是否存在"button"、"x"和"y"字段
		if (data.find("button") == data.end() || data.find("x") == data.end() || data.find("y") == data.end())
		{
			WARNING_EX_LOG("not find 'button' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 验证字段类型是否为数字类型
		if (!data["button"].is_number() || !data["y"].is_number() ||
			!data["x"].is_number())
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 从JSON中提取button、PosX和PosY值
		FButtonType Button = data["button"].get<FButtonType>();
		FPosType PosX = data["x"].get<FPosType>();
		FPosType PosY = data["y"].get<FPosType>();
		 
		// 坐标反量化和反归一化
		_UnquantizeAndDenormalize(PosX, PosY);

		// 创建鼠标按下事件并设置参数
		FEvent MouseDownEvent(EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32 active_type;

		// 获取鼠标点击信息（可选，用于调试）
		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		// TODO: ProcessEvent(MouseDownEvent);

		// 更新全局屏幕坐标
		g_width = PosX;
		g_height = PosY;
		
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height, active_type, PosX, PosY);
		 
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标释放事件实现 - 二进制版本（Handle Mouse Up Event Implementation - Binary Version）
	*  
	*  该方法用于处理鼠标释放事件。当用户释放鼠标按键时，会将远程鼠标释放操作
	*  转换为本地鼠标事件。坐标需要反量化和反归一化处理。
	*  
	*  鼠标释放消息格式（Mouse Up Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::MouseUp                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  button (8 bits: 鼠标按键标志)                                  |
	*   |  - 0: 左键                                                       |
	*   |  - 1: 右键                                                       |
	*   |  - 2: 中键                                                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  x (16 bits: 水平坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  y (16 bits: 垂直坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  鼠标释放事件处理流程（Mouse Up Event Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 解析消息数据：提取button、PosX和PosY                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证消息数据大小（Size == 0表示解析完成）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 坐标反量化和反归一化：_UnquantizeAndDenormalize(PosX, PosY)  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 更新全局屏幕坐标：g_width = PosX, g_height = PosY            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 创建鼠标释放事件：FEvent(EventType::MOUSE_UP)                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 调用button_mouse()处理鼠标按键事件（释放，true）             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param Size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 鼠标释放会触发本地鼠标释放事件
	*  @note 需要与MouseDown配对使用
	*  @note 坐标需要反量化和反归一化处理
	*  
	*  使用示例：
	*  @code
	*  // 由OnMessage()方法调用，无需手动调用
	*  @endcode
	*/
	bool cinput_device::OnMouseUp(const uint8*& Data,   uint32 Size)
	{
		// 解析消息数据：提取鼠标按键和坐标
		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		
		// 验证消息数据大小（Size == 0表示解析完成）
		checkf(Size == 0, TEXT("%d"), Size); 
		
		// 坐标反量化和反归一化
		_UnquantizeAndDenormalize(PosX, PosY);
		
		// 创建鼠标释放事件
		FEvent MouseDownEvent(EventType::MOUSE_UP); 
		NORMAL_EX_LOG("Button = %u, PosX = %d, PoxY = %d", Button,  PosX, PosY );
		
		// 更新全局屏幕坐标
		g_width = PosX;
		g_height = PosY;
		
		// 处理鼠标按键事件（释放，true）
		button_mouse(MouseDownEvent, PosX, PosY, Button, true);
 
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标释放事件实现 - JSON版本（Handle Mouse Up Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的鼠标释放事件。解析JSON数据，坐标反量化和反归一化，
	*  创建鼠标事件，并触发本地鼠标事件。
	*  
	*  JSON消息格式（JSON Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  {                                                               |
	*   |    "type": EToStreamMsg::MouseUp (数字类型)                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "button": 鼠标按键 (数字类型，0=左键，1=右键，2=中键)          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "x": 水平坐标 (数字类型，归一化或量化值)                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "y": 垂直坐标 (数字类型，归一化或量化值)                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  }                                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param data JSON对象，包含鼠标释放数据
	*  @return 返回true表示处理成功，false表示处理失败
	*  @note 用于处理JSON格式的鼠标释放事件
	*  @note 如果JSON中缺少必要字段或类型不正确，会记录警告并返回false
	*  @note 坐标需要反量化和反归一化处理
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::MouseUp}, {"button", 0}, {"x", 100}, {"y", 200}};
	*  s_input_device.OnRtcMouseUp(msg);
	*  @endcode
	*/
	bool cinput_device::OnRtcMouseUp(const nlohmann::json & data)
	{
		// 检查JSON中是否存在"button"、"x"和"y"字段
		if (data.find("button") == data.end() || data.find("x") == data.end() || data.find("y") == data.end())
		{
			WARNING_EX_LOG("not find 'button' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 验证字段类型是否为数字类型
		if (!data["button"].is_number() || !data["y"].is_number() ||
			!data["x"].is_number())
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 从JSON中提取button、PosX和PosY值
		FButtonType Button = data["button"].get<FButtonType>();
		FPosType PosX = data["x"].get<FPosType>();
		FPosType PosY = data["y"].get<FPosType>();
		
		// 坐标反量化和反归一化
		_UnquantizeAndDenormalize(PosX, PosY);
		
		// 创建鼠标释放事件并设置参数
		FEvent MouseDownEvent(EventType::MOUSE_UP);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32  active_type;
		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);

		// 更新全局屏幕坐标
		g_width = PosX;
		g_height = PosY;
		
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height, active_type, PosX, PosY);
		// TODO: ProcessEvent(MouseDownEvent);
 
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标移动事件实现 - 二进制版本（Handle Mouse Move Event Implementation - Binary Version）
	*  
	*  该方法用于处理鼠标移动事件。当鼠标移动时，会将远程鼠标移动操作转换为本地鼠标事件。
	*  坐标需要反量化和反归一化处理。支持绝对坐标和相对偏移两种模式。
	*  
	*  鼠标移动消息格式（Mouse Move Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::MouseMove                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  x (16 bits: 水平坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  y (16 bits: 垂直坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  delta_x (16 bits: 水平偏移，归一化或量化值，有符号)              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  delta_y (16 bits: 垂直偏移，归一化或量化值，有符号)              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  鼠标移动事件处理流程（Mouse Move Event Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 解析消息数据：提取PosX、PosY、DeltaX和DeltaY                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证消息数据大小（Size == 0表示解析完成）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 记录输入坐标（归一化或量化值）                                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 坐标反量化和反归一化：_UnquantizeAndDenormalize(PosX, PosY)  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 偏移反量化和反归一化：_UnquantizeAndDenormalize(DeltaX, DeltaY) |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 更新全局屏幕坐标：g_width = PosX, g_height = PosY            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 创建鼠标移动事件：FEvent(EventType::MOUSE_MOVE)              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  8. 调用abs_mouse()处理绝对坐标鼠标移动                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  坐标处理说明（Coordinate Processing Description）：
	*  - PosX/PosY: 绝对坐标（归一化或量化值），需要反量化和反归一化
	*  - DeltaX/DeltaY: 相对偏移（归一化或量化值），需要反量化和反归一化
	*  - 支持移动时按下按键的情况（拖拽操作）
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param Size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 鼠标移动会触发本地鼠标移动事件
	*  @note 坐标需要反量化和反归一化处理
	*  @note 支持移动时按下按键的情况（拖拽操作）
	*  
	*  使用示例：
	*  @code
	*  // 由OnMessage()方法调用，无需手动调用
	*  @endcode
	*/
	bool cinput_device::OnMouseMove(const uint8*& Data,   uint32 Size)
	{
		// 解析消息数据：提取坐标和偏移
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		GET(FDeltaType, DeltaX);
		GET(FDeltaType, DeltaY);
		
		// 验证消息数据大小（Size == 0表示解析完成）
		checkf(Size == 0, TEXT("%d"), Size); 
		
		// 记录输入坐标（归一化或量化值）
		NORMAL_EX_LOG("input [w: %d, h: %d] posx:%s, Posy:%s", m_int_point.X, m_int_point.Y, 
			std::to_string(PosX).c_str(), std::to_string(PosY).c_str());
		
		// 坐标反量化和反归一化
		_UnquantizeAndDenormalize(PosX, PosY);
		_UnquantizeAndDenormalize(DeltaX, DeltaY); 
		NORMAL_EX_LOG("---> PosX = %d, PoxY = %d, DeltaY = %d", PosX, PosY, DeltaY);
		
		// 创建鼠标移动事件
		FEvent MouseMoveEvent(EventType::MOUSE_MOVE);
		 
		// 更新全局屏幕坐标
		g_width = PosX;
		g_height = PosY;
		
		// 处理绝对坐标鼠标移动
		abs_mouse(MouseMoveEvent, PosX, PosY);
		 
		return true;
	}
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标移动事件实现 - JSON版本（Handle Mouse Move Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的鼠标移动事件。解析JSON数据，坐标反量化和反归一化，
	*  创建鼠标事件，并触发本地鼠标事件。支持绝对坐标和相对偏移两种模式。
	*  
	*  JSON消息格式（JSON Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  {                                                               |
	*   |    "type": EToStreamMsg::MouseMove (数字类型)                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "x": 水平坐标 (数字类型，归一化或量化值)                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "y": 垂直坐标 (数字类型，归一化或量化值)                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "delta_x": 水平偏移 (数字类型，归一化或量化值，有符号)          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "delta_y": 垂直偏移 (数字类型，归一化或量化值，有符号)          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  }                                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  JSON消息处理流程（JSON Message Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 检查JSON中是否存在"x"、"y"、"delta_x"和"delta_y"字段       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证字段类型是否为数字类型                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 从JSON中提取PosX、PosY、DeltaX和DeltaY值                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 坐标反量化和反归一化：_UnquantizeAndDenormalize(PosX, PosY)  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 偏移反量化和反归一化：_UnquantizeAndDenormalize(DeltaX, DeltaY) |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 创建鼠标移动事件并设置参数                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 更新全局屏幕坐标                                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @param data JSON对象，包含鼠标移动数据
	*  @return 返回true表示处理成功，false表示处理失败
	*  @note 用于处理JSON格式的鼠标移动事件
	*  @note 如果JSON中缺少必要字段或类型不正确，会记录警告并返回false
	*  @note 坐标需要反量化和反归一化处理
	*  @note 支持移动时按下按键的情况（拖拽操作）
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::MouseMove}, {"x", 100}, {"y", 200}, {"delta_x", 10}, {"delta_y", 20}};
	*  s_input_device.OnRtcMouseMove(msg);
	*  @endcode
	*/
	bool cinput_device::OnRtcMouseMove(const nlohmann::json & data)
	{
		if (data.find("delta_x") == data.end() || data.find("delta_y") == data.end() || data.find("x") == data.end() || data.find("y") == data.end())
		{
			WARNING_EX_LOG("not find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		 if (!data["x"].is_number() || !data["y"].is_number() || 
			 !data["delta_x"].is_number() || !data["delta_y"].is_number())
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		} 
		 
		// 从JSON中提取PosX、PosY、DeltaX和DeltaY值
		FPosType PosX = data["x"].get<FPosType>();
		FPosType PosY = data["y"].get<FPosType>();
		FDeltaType DeltaX = data["delta_x"].get<FDeltaType>();
		FDeltaType DeltaY = data["delta_y"].get<FDeltaType>();
		
		// 坐标反量化和反归一化
		_UnquantizeAndDenormalize(PosX, PosY);
		_UnquantizeAndDenormalize(DeltaX, DeltaY);
		NORMAL_EX_LOG("g_width = %d, g_height = %d, ---> PosX = %d, PoxY = %d, DeltaY = %d", g_width, g_height, PosX, PosY, DeltaY);

		// 创建鼠标移动事件并设置参数
		FEvent MouseMoveEvent(EventType::MOUSE_MOVE);
		MouseMoveEvent.SetMouseDelta(PosX, PosY, DeltaX, DeltaY);
		
		// 保存旧的全局屏幕坐标
		int32_t width = g_width;
		int32_t height = g_height;
		
		// 更新全局屏幕坐标
		g_width = PosX;
		g_height = PosY;

		// TODO: ProcessEvent(MouseMoveEvent);
		return true;
	}
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标双击事件实现 - 二进制版本（Handle Mouse Double Click Event Implementation - Binary Version）
	*  
	*  该方法用于处理鼠标双击事件。当用户双击鼠标按键时，会将远程鼠标双击操作
	*  转换为本地鼠标双击事件。坐标需要反量化和反归一化处理。
	*  双击通常由两个连续的单击事件组成。
	*  
	*  鼠标双击消息格式（Mouse Double Click Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::MouseDoubleClick                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  button (8 bits: 鼠标按键标志)                                  |
	*   |  - 0: 左键                                                       |
	*   |  - 1: 右键                                                       |
	*   |  - 2: 中键                                                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  x (16 bits: 水平坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  y (16 bits: 垂直坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  鼠标双击事件处理流程（Mouse Double Click Event Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 解析消息数据：提取button、PosX和PosY                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证消息数据大小（Size == 0表示解析完成）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 坐标反量化和反归一化：_UnquantizeAndDenormalize(PosX, PosY)  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 创建鼠标按下事件：FEvent(EventType::MOUSE_DOWN)              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 设置鼠标点击事件参数：SetMouseClick(Button, PosX, PosY)      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 更新全局屏幕坐标                                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  Windows消息映射（Windows Message Mapping）：
	*  - WM_LBUTTONDBLCLK: 左键双击
	*  - WM_RBUTTONDBLCLK: 右键双击
	*  - WM_MBUTTONDBLCLK: 中键双击
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param Size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 鼠标双击会触发本地鼠标双击事件
	*  @note 双击通常由两个连续的单击事件组成
	*  @note 坐标需要反量化和反归一化处理
	*  
	*  使用示例：
	*  @code
	*  // 由OnMessage()方法调用，无需手动调用
	*  @endcode
	*/
	bool cinput_device::OnMouseDoubleClick(const uint8*& Data,   uint32 Size)
	{
		// Windows消息映射：WM_LBUTTONDBLCLK, WM_RBUTTONDBLCLK, WM_MBUTTONDBLCLK
		// 解析消息数据：提取鼠标按键和坐标
		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		
		// 验证消息数据大小（Size == 0表示解析完成）
		checkf(Size == 0, TEXT("%d"), Size);
		
		// 坐标反量化和反归一化
		_UnquantizeAndDenormalize(PosX, PosY);

		// 创建鼠标按下事件（双击作为特殊的按下事件）
		FEvent MouseDownEvent(EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32 active_type;

		// 获取鼠标点击信息（可选，用于调试）
		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		
		// 更新全局屏幕坐标
		g_width = PosX;
		g_height = PosY;
		
		// TODO: ProcessEvent(MouseDownEvent);
		NORMAL_EX_LOG("g_width = %d, g_height = %d, ---> PosX = %d, PoxY = %d", g_width, g_height, PosX, PosY);
 
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标双击事件实现 - JSON版本（Handle Mouse Double Click Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的鼠标双击事件。目前为占位实现，预留扩展接口。
	*  
	*  @param data JSON对象，包含鼠标双击数据
	*  @return 返回false表示未实现
	*  @note 用于处理JSON格式的鼠标双击事件
	*  @note 目前为占位实现，需要在将来添加完整逻辑
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::MouseDoubleClick}, {"button", 0}, {"x", 100}, {"y", 200}};
	*  s_input_device.OnRtcMouseDoubleClick(msg);
	*  @endcode
	*/
	bool cinput_device::OnRtcMouseDoubleClick(const nlohmann::json & data)
	{
		// TODO: 实现JSON格式的鼠标双击事件处理逻辑
		return false;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标滚轮滚动事件实现 - 二进制版本（Handle Mouse Wheel Event Implementation - Binary Version）
	*  
	*  该方法用于处理鼠标滚轮滚动事件。当用户滚动鼠标滚轮时，会将远程滚轮滚动操作
	*  转换为本地滚轮事件。坐标需要反量化和反归一化处理。
	*  
	*  鼠标滚轮消息格式（Mouse Wheel Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  message_type: EToStreamMsg::MouseWheel                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  delta (16 bits: 滚轮滚动增量，有符号)                            |
	*   |  - 正值: 向上滚动（远离用户）                                     |
	*   |  - 负值: 向下滚动（靠近用户）                                     |
	*   |  - 120: 标准滚动单位（Windows）                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  x (16 bits: 水平坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  y (16 bits: 垂直坐标，归一化或量化值)                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  鼠标滚轮事件处理流程（Mouse Wheel Event Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 解析消息数据：提取Delta、PosX和PosY                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证消息数据大小（Size == 0表示解析完成）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 坐标反量化和反归一化：_UnquantizeAndDenormalize(PosX, PosY)  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 创建鼠标滚轮事件：FEvent(EventType::MOUSE_WHEEL)              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 设置滚轮事件参数：SetMouseWheel(Delta, PosX, PosY)           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 更新全局屏幕坐标                                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 调用scroll()处理滚轮滚动事件                                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  滚轮事件说明（Wheel Event Description）：
	*  - delta > 0: 向上滚动（远离用户）
	*  - delta < 0: 向下滚动（靠近用户）
	*  - delta = 120: 标准滚动单位（Windows）
	*  - Ctrl+滚轮: 通常用于缩放操作
	*  
	*  @param Data 指向消息数据的指针，会被推进到消息结束位置
	*  @param Size 消息数据的总大小，单位为字节
	*  @return 返回true表示处理成功
	*  @note 滚轮滚动会触发本地滚轮事件
	*  @note delta值表示滚动方向和距离
	*  @note 坐标需要反量化和反归一化处理
	*  
	*  使用示例：
	*  @code
	*  // 由OnMessage()方法调用，无需手动调用
	*  @endcode
	*/
	bool cinput_device::OnMouseWheel(const uint8*& Data,   uint32 Size)
	{
		// 解析消息数据：提取滚轮增量和坐标
		GET(FDeltaType, Delta);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		
		// 验证消息数据大小（Size == 0表示解析完成）
		checkf(Size == 0, TEXT("%d"), Size);
		
		// 坐标反量化和反归一化
		_UnquantizeAndDenormalize(PosX, PosY);

		// 创建鼠标滚轮事件并设置参数
		FEvent MouseWheelEvent(EventType::MOUSE_WHEEL);
		MouseWheelEvent.SetMouseWheel(Delta, PosX, PosY);
		// TODO: ProcessEvent(MouseWheelEvent);
		
		// 更新全局屏幕坐标
		g_width = PosX;
		g_height = PosY;
		
		// 处理滚轮滚动事件
		scroll(MouseWheelEvent, Delta);
		 
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 处理鼠标滚轮滚动事件实现 - JSON版本（Handle Mouse Wheel Event Implementation - JSON Version）
	*  
	*  该方法用于处理JSON格式的鼠标滚轮滚动事件。解析JSON数据，坐标反量化和反归一化，
	*  创建鼠标事件，并触发本地滚轮事件。
	*  
	*  JSON消息格式（JSON Message Format）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  {                                                               |
	*   |    "type": EToStreamMsg::MouseWheel (数字类型)                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "delta": 滚轮增量 (数字类型，有符号)                            |
	*   |    - 正值: 向上滚动（远离用户）                                   |
	*   |    - 负值: 向下滚动（靠近用户）                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "x": 水平坐标 (数字类型，归一化或量化值)                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |    "y": 垂直坐标 (数字类型，归一化或量化值)                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  }                                                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  JSON消息处理流程（JSON Message Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 检查JSON中是否存在"delta"、"x"和"y"字段                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 验证字段类型是否为数字类型                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 从JSON中提取Delta、PosX和PosY值                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 坐标反量化和反归一化：_UnquantizeAndDenormalize(PosX, PosY)  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 创建鼠标滚轮事件并设置参数                                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 更新全局屏幕坐标                                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  滚轮事件说明（Wheel Event Description）：
	*  - delta > 0: 向上滚动（远离用户）
	*  - delta < 0: 向下滚动（靠近用户）
	*  - delta = 120: 标准滚动单位（Windows）
	*  - Ctrl+滚轮: 通常用于缩放操作
	*  
	*  @param data JSON对象，包含鼠标滚轮数据
	*  @return 返回true表示处理成功，false表示处理失败
	*  @note 用于处理JSON格式的鼠标滚轮滚动事件
	*  @note 如果JSON中缺少必要字段或类型不正确，会记录警告并返回false
	*  @note delta值表示滚动方向和距离
	*  @note 坐标需要反量化和反归一化处理
	*  
	*  使用示例：
	*  @code
	*  nlohmann::json msg = {{"type", EToStreamMsg::MouseWheel}, {"delta", 120}, {"x", 100}, {"y", 200}};
	*  s_input_device.OnRtcMouseWheel(msg);
	*  @endcode
	*/
	bool cinput_device::OnRtcMouseWheel(const nlohmann::json & data)
	{
		// 检查JSON中是否存在"delta"、"x"和"y"字段
		if (data.find("delta") == data.end()  || data.find("x") == data.end() || data.find("y") == data.end())
		{
			WARNING_EX_LOG("not find 'delta'   or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 验证字段类型是否为数字类型
		if (!data["x"].is_number() || !data["y"].is_number() ||
			!data["delta"].is_number() )
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		
		// 从JSON中提取Delta、PosX和PosY值
		FDeltaType Delta = data["delta"].get<FDeltaType>();
		FPosType PosX = data["x"].get<FPosType>();
		FPosType PosY = data["y"].get<FPosType>();
		
		// 坐标反量化和反归一化
		_UnquantizeAndDenormalize(PosX, PosY);

		// 创建鼠标滚轮事件并设置参数
		FEvent MouseWheelEvent(EventType::MOUSE_WHEEL);
		MouseWheelEvent.SetMouseWheel(Delta, PosX, PosY);
		// TODO: ProcessEvent(MouseWheelEvent);
		
		// 更新全局屏幕坐标
		g_width = PosX;
		g_height = PosY;
 
		return true;
	}

	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 反量化和反归一化坐标实现 - uint16版本（Unquantize and Denormalize Coordinates Implementation - uint16 Version）
	*  
	*  该方法用于将量化并归一化的坐标值（uint16）转换为实际的像素坐标。
	*  XY坐标是视口轴上的比例值（0.0..1.0），量化到uint16（0..65536）。
	*  这允许浏览器视口和播放器视口具有不同的尺寸。
	*  
	*  坐标转换公式（Coordinate Conversion Formula）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  输入: 归一化坐标 (0.0 - 1.0)                                    |
	*   |  normalized_x = InOutX / 65536.0f (0.0 - 1.0)                  |
	*   |  normalized_y = InOutY / 65536.0f (0.0 - 1.0)                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  转换: 反归一化                                                 |
	*   |  pixel_x = normalized_x * m_int_point.X                        |
	*   |  pixel_y = normalized_y * m_int_point.Y                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  输出: 实际像素坐标                                             |
	*   |  InOutX = pixel_x (0 - m_int_point.X)                          |
	*   |  InOutY = pixel_y (0 - m_int_point.Y)                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  坐标转换示例（Coordinate Conversion Example）：
	*  - 输入: InOutX = 32768, InOutY = 32768 (归一化坐标 0.5, 0.5)
	*  - 屏幕尺寸: m_int_point.X = 1920, m_int_point.Y = 1080
	*  - 转换: InOutX = 32768 / 65536.0f * 1920 = 960
	*         InOutY = 32768 / 65536.0f * 1080 = 540
	*  - 输出: InOutX = 960, InOutY = 540 (屏幕中心点)
	*  
	*  @param InOutX 输入输出参数，输入归一化坐标（uint16），输出实际像素坐标
	*  @param InOutY 输入输出参数，输入归一化坐标（uint16），输出实际像素坐标
	*  @note 坐标转换用于将远程坐标映射到本地屏幕坐标
	*  @note 归一化坐标范围：0（左上角）到 65536（右下角）
	*  @note 需要知道本地屏幕分辨率（m_int_point）才能正确转换
	*  
	*  使用示例：
	*  @code
	*  uint16_t x = 32768, y = 32768;  // 归一化坐标 (0.5, 0.5)
	*  s_input_device._UnquantizeAndDenormalize(x, y);
	*  // x = 960, y = 540 (假设屏幕为1920x1080)
	*  @endcode
	*/
	void cinput_device::_UnquantizeAndDenormalize(uint16& InOutX, uint16& InOutY)
	{
		InOutX = InOutX / 65536.0f * m_int_point.X;
		InOutY = InOutY / 65536.0f * m_int_point.Y;
	}
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 反量化和反归一化坐标实现 - int16版本（Unquantize and Denormalize Coordinates Implementation - int16 Version）
	*  
	*  该方法用于将量化并归一化的坐标偏移值（int16）转换为实际的像素偏移。
	*  XY偏移是视口轴上的比例值（-1.0..1.0），量化到int16（-32767..32767）。
	*  这允许浏览器视口和播放器视口具有不同的尺寸。
	*  
	*  坐标偏移转换公式（Coordinate Delta Conversion Formula）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  输入: 归一化偏移 (-1.0 - 1.0)                                   |
	*   |  normalized_delta_x = InOutX / 32767.0f (-1.0 - 1.0)           |
	*   |  normalized_delta_y = InOutY / 32767.0f (-1.0 - 1.0)           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  转换: 反归一化                                                 |
	*   |  pixel_delta_x = normalized_delta_x * m_int_point.X            |
	*   |  pixel_delta_y = normalized_delta_y * m_int_point.Y            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  输出: 实际像素偏移                                             |
	*   |  InOutX = pixel_delta_x (-m_int_point.X - m_int_point.X)       |
	*   |  InOutY = pixel_delta_y (-m_int_point.Y - m_int_point.Y)       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  坐标偏移转换示例（Coordinate Delta Conversion Example）：
	*  - 输入: InOutX = 16384, InOutY = -16384 (归一化偏移 0.5, -0.5)
	*  - 屏幕尺寸: m_int_point.X = 1920, m_int_point.Y = 1080
	*  - 转换: InOutX = 16384 / 32767.0f * 1920 = 960 (向右移动)
	*         InOutY = -16384 / 32767.0f * 1080 = -540 (向上移动)
	*  - 输出: InOutX = 960, InOutY = -540 (相对移动量)
	*  
	*  @param InOutX 输入输出参数，输入归一化偏移（int16，有符号），输出实际像素偏移
	*  @param InOutY 输入输出参数，输入归一化偏移（int16，有符号），输出实际像素偏移
	*  @note 坐标偏移转换用于处理相对移动（如拖拽操作）
	*  @note 归一化偏移范围：-32767（负方向）到 32767（正方向）
	*  @note 负值表示向左/上移动，正值表示向右/下移动
	*  
	*  使用示例：
	*  @code
	*  int16_t deltaX = 16384, deltaY = -16384;  // 归一化偏移 (0.5, -0.5)
	*  s_input_device._UnquantizeAndDenormalize(deltaX, deltaY);
	*  // deltaX = 960, deltaY = -540 (假设屏幕为1920x1080)
	*  @endcode
	*/
	void cinput_device::_UnquantizeAndDenormalize(int16& InOutX, int16& InOutY)
	{ 
		InOutX = InOutX / 32767.0f * m_int_point.X;
		InOutY = InOutY / 32767.0f * m_int_point.Y;
	}
	
	/**
	*  @author chensong
	*  @date 2022-01-19
	*  @brief 工作线程函数实现（Worker Thread Function Implementation）
	*  
	*  该方法在工作线程中运行，持续从消息队列中取出消息并处理。
	*  使用异步处理避免阻塞主线程。每50毫秒循环一次，如果没有消息则休眠。
	*  
	*  工作线程处理流程（Worker Thread Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 循环直到m_stoped为true                                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 记录循环开始时间                                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 获取m_input_mutex互斥锁                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 如果消息队列不为空，将队列内容交换到temp_list               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 释放互斥锁（lock_guard自动释放）                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  6. 处理temp_list中的所有消息                                   |
	*   |     - 调用OnMessage()处理每条消息                                |
	*   |     - 从temp_list中移除已处理的消息                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  7. 如果消息队列为空，计算循环耗时                                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  8. 如果耗时小于50毫秒，休眠剩余时间                             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  9. 返回步骤2继续循环                                           |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  消息队列处理（Message Queue Processing）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  m_input_list (原始队列，受互斥锁保护)                           |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | [Buffer 1] -> [Buffer 2] -> [Buffer 3] -> ...              | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  temp_list (临时队列，用于批量处理)                              |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  | [Buffer 1] -> [Buffer 2] -> [Buffer 3] -> ...              | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  swap()操作：一次性交换整个队列，减少锁持有时间                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  性能优化说明（Performance Optimization Description）：
	*  - 使用swap()一次性交换整个队列，减少互斥锁持有时间
	*  - 在锁外处理消息，避免长时间占用锁
	*  - 每50毫秒循环一次，平衡响应性和CPU使用率
	*  - 如果队列为空则休眠，节省CPU资源
	*  
	*  @note 该方法在独立线程中运行，避免阻塞主线程
	*  @note 使用互斥锁保护消息队列，确保线程安全
	*  @note 使用swap()批量处理消息，提高性能
	*  @note 每50毫秒循环一次（TICK_TIME = 50毫秒）
	*  @note 检查m_stoped标志决定是否退出循环
	*  
	*  使用示例：
	*  @code
	*  // 由init()方法创建工作线程并调用此方法
	*  // 无需手动调用
	*  std::thread td(&cinput_device::_work_pthread, this);
	*  m_thread.swap(td);
	*  @endcode
	*/
	void cinput_device::_work_pthread()
	{
		std::list< webrtc::DataBuffer>  temp_list;
		static const uint32 TICK_TIME = 50;  // 循环周期：50毫秒
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::milliseconds ms;
		uint32_t elapse = 0;

		// 循环直到m_stoped为true
		while (!m_stoped)
		{
			pre_time = std::chrono::steady_clock::now();
			
			// 加锁交换消息队列到临时列表
			{
				std::lock_guard<std::mutex>  lock(m_input_mutex);
				if (!m_input_list.empty())
				{
					// 使用swap()一次性交换整个队列，减少锁持有时间
					temp_list.swap (m_input_list);
				}
			}

			// 处理临时列表中的所有消息（在锁外处理，避免长时间占用锁）
			while (!temp_list.empty())
			{
				OnMessage("", temp_list.front());
				temp_list.pop_front();
			}
			
			// 如果消息队列为空，计算循环耗时并休眠
			if (m_input_list.empty())
			{
				cur_time_ms = std::chrono::steady_clock::now();
				dur = cur_time_ms - pre_time;
				ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
				elapse = static_cast<uint32_t>(ms.count());
				
				// 如果耗时小于50毫秒，休眠剩余时间
				if (elapse < TICK_TIME)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - elapse));
				}
			}
		}
	}
}