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
	int32_t  g_width = 0;
	int32_t  g_height = 0;
	using FKeyCodeType = uint8;
	using FCharacterType = TCHAR;
	using FRepeatType = uint8;
	using FButtonType = uint8;
	using FPosType = uint16;
	using FDeltaType = int16;
	//using FTouchesType = TArray<FTouch>;
	using FControllerIndex = uint8;
	using FControllerButtonIndex = uint8;
	using FControllerAnalog = double;
	using FControllerAxis = uint8;

	 

#define REGISTER_INPUT_DEVICE(type, handler_ptr) if (false == m_input_device.insert(std::make_pair(type, handler_ptr)).second){	 ERROR_EX_LOG("[type = %s][handler_ptr = %s]", #type, #handler_ptr);	return false;}

#define REGISTER_RTC_INPUT_DEVICE(type, handler_ptr) if (false == m_rtc_input_device.insert(std::make_pair(type, handler_ptr)).second){	 ERROR_EX_LOG("[type = %s][handler_ptr = %s]", #type, #handler_ptr);	return false;}



//struct c_init_input_device
//{
//	c_init_input_device()
//	{
//		s_input_device.init();
//	}
//};

    //static   c_init_input_device input_device_init;
	static HWND g_main_mouse_down_up = NULL;
	static HWND g_child_mouse_down_up = NULL;
///	cinput_device   g_input_device_mgr;
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
	cinput_device::~cinput_device() {}

 
	 
	bool cinput_device::init()
	{
		if (m_init)
		{
			return true;

		}
		m_init = true;
		//g_hrawinput.resize(RAW_INPUT_SIZE);
		 // win
		 

		REGISTER_INPUT_DEVICE(RequestQualityControl, &cinput_device::OnKeyChar);
		REGISTER_INPUT_DEVICE(KeyDown, &cinput_device::OnKeyDown);
		REGISTER_INPUT_DEVICE(KeyUp, &cinput_device::OnKeyUp);
		REGISTER_INPUT_DEVICE(KeyPress, &cinput_device::OnKeyPress);
		////////////////////////////////////////////////////////////
		REGISTER_INPUT_DEVICE(MouseEnter, &cinput_device::OnMouseEnter);
		REGISTER_INPUT_DEVICE(MouseLeave, &cinput_device::OnMouseLeave);
		REGISTER_INPUT_DEVICE(MouseDown, &cinput_device::OnMouseDown);
		REGISTER_INPUT_DEVICE(MouseUp, &cinput_device::OnMouseUp);
		REGISTER_INPUT_DEVICE(MouseMove, &cinput_device::OnMouseMove);
		REGISTER_INPUT_DEVICE(MouseWheel, &cinput_device::OnMouseWheel);
		REGISTER_INPUT_DEVICE(MouseDoubleClick, &cinput_device::OnMouseDoubleClick);



		///////////////////
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


		std::thread td(&cinput_device::_work_pthread, this);
		m_thread.swap(td);
		return true;
	}
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
			m_input_device.clear();
		}
	}
	void cinput_device::startup()
	{
		
	}
	bool cinput_device::set_point(uint32 x, uint32 y)
	{
		m_int_point.X = x;
		m_int_point.Y = y;
		return true;
	}
	bool cinput_device::OnMessage(const std::string & consumer_id, const webrtc::DataBuffer& Buffer)
	{
		//NORMAL_LOG("consumer_id = %s", consumer_id.c_str());
		m_mouse_id = consumer_id;
		const uint8* Data = Buffer.data.data();
		uint32 Size = static_cast<uint32>(Buffer.data.size());

		GET(EToStreamMsg, MsgType);
		
		M_INPUT_DEVICE_MAP::iterator iter =  m_input_device.find(MsgType);
		if (iter == m_input_device.end())
		{
			//RTC_LOG(LS_ERROR) << "input_device not find id = " << MsgType;
			//log --->  not find type 
			ERROR_EX_LOG("input_device msg_type = %d not find failed !!!", MsgType);
			return false;
		}
		
		/*if (MsgType != MouseEnter)
		{
			std::map<std::string, std::map<uint32, cmouse_info>>::const_iterator consumer_iter = m_all_consumer.find(consumer_id);
			if (consumer_iter == m_all_consumer.end())
			{
				WARNING_EX_LOG("consumer_id = %s not Enter !!!", consumer_id.c_str());
				return false;
			}

		} */
		//NORMAL_EX_LOG("move type =%d", MsgType);
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::microseconds microseconds;
		uint32_t elapse = 0;
		//备往：尽管GetFocus返回NULL，但可能另一线程的队列与拥有输入焦点的窗口相关。
		//便用GetForeyroundWindow函数来获得用户目前工作的窗口。
		//可以使用AttachThreadlnPut函数把线程的消息队列与另一线程的窗口关联起来。
		

		//HWND wnd = GetFocus();





		
		   //说明：SetForegroundWindow在debug模式一直成功，非debug模式会有失败的情况，解决方法是利用AttachThreadInput
        //AttachThreadInput这个函数可以使两个线程的输入队列共享。 
        //如果我们把当前的焦点的输入队列跟我们要显示的窗口的输入队列共享，
        //我们就可以让我们的窗口SetForegroundWindow 成功，然后成功 获得焦点，显示在前台
		/*
		HWND hForgroundWnd = GetForegroundWindow(); \
						DWORD dwForeID = ::GetWindowThreadProcessId(hForgroundWnd, NULL); \
	DWORD dwCurID = ::GetCurrentThreadId();		\
	::AttachThreadInput(dwCurID, dwForeID, TRUE);	\
	::ShowWindow(g_wnd, SW_SHOWNORMAL);		\
	::SetForegroundWindow(g_wnd);	\
	::AttachThreadInput(dwCurID, dwForeID, FALSE);	\
		*/
       /*
	   SetWindowPos(handle, -1, 0, 0, 0, 0, 0x001 | 0x002 | 0x040);    //将这个窗口永远置于最上面
    //---------------------------在需要激活窗体的地方添加如下代码-----------------------//
    PostMessage(handle, WM_LBUTTONDOWN, 0, 0);
    PostMessage(handle, WM_LBUTTONUP, 0, 0);
	   */
		//BringWindowToTop();

		 (this->*(iter->second))(Data, Size);
		 cur_time_ms = std::chrono::steady_clock::now();
		 dur = cur_time_ms - pre_time;
		 microseconds = std::chrono::duration_cast<std::chrono::microseconds>(dur);
		 elapse = static_cast<uint32_t>(microseconds.count());
		 if (elapse > 900)
		 {
			 WARNING_EX_LOG("input_device  microseconds = %lu", microseconds);
		 }
		 return true;
		//return true;
	}

	bool cinput_device::OnMessage(const nlohmann::json & datachannel)
	{
		 
		/*const uint8 * Data = (const uint8*)(datachannel.c_str());
		uint32 Size = datachannel.size();*/

		//std::string hex = hexmem((const void *)Data, (size_t)Size);
		//NORMAL_EX_LOG("hex = %s", hex.c_str);
		//GET(EToStreamMsg, MsgType);

		 
		if (datachannel.find("type") == datachannel.end())
		{
			WARNING_EX_LOG("not find msg_type [data = %s]", datachannel.dump().c_str());
			return false;
		}
		if (!datachannel["type"].is_number() )
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", datachannel.dump().c_str());
			return false;
		}
		EToStreamMsg msg_type = datachannel["type"].get<EToStreamMsg>();
		M_RTC_INPUT_DEVICE_MAP::iterator iter = m_rtc_input_device.find(msg_type);
		if (iter == m_rtc_input_device.end())
		{
			//RTC_LOG(LS_ERROR) << "input_device not find id = " << MsgType;
			//log --->  not find type 
			ERROR_EX_LOG("rtc input_device msg_type = %d not find failed !!!", msg_type);
			return false;
		}

		 
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::microseconds microseconds;
		uint32_t elapse = 0;
		 

		(this->*(iter->second))(datachannel);
		cur_time_ms = std::chrono::steady_clock::now();
		dur = cur_time_ms - pre_time;
		microseconds = std::chrono::duration_cast<std::chrono::microseconds>(dur);
		elapse = static_cast<uint32_t>(microseconds.count());
		if (elapse > 900)
		{
			WARNING_EX_LOG("input_device  microseconds = %lu", microseconds);
		}
		return true;
		return true;
	}

	void cinput_device::insert_message(const webrtc::DataBuffer & Buffer)
	{
		{
			std::lock_guard<std::mutex> lock(m_input_mutex);
			m_input_list.push_back(Buffer);
		}
	}


	bool cinput_device::OnRequestQualityControl(const uint8*& Data, uint32 size)
	{
		// 这边需要启动引擎推送视频流
		return true;
	}
	/**
	* 输入字符
	*/
	bool cinput_device::OnKeyChar(const uint8*& Data,   uint32 size)
	{
		NORMAL_LOG("KeyChar");
		//WINDOW_MAIN();
		//SET_POINT(vec);
		//WINDOW_CHILD();
		//if (childwin)
		//{
		//	::PostMessageW(childwin, WM_CHAR, Character, 1);
		//}
		//else if (mwin)
		//{
		//	::PostMessageW(mwin, WM_CHAR, Character, 1);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}

		return true;
	}
	bool cinput_device::OnRtcKeyChar(const nlohmann::json & data)
	{
		return false;
	}
	/**
	* 按下键
	*/
	bool cinput_device::OnKeyDown(const uint8*& Data,  uint32 Size)
	{
		 
		GET(FKeyCodeType, KeyCode);
		GET(FRepeatType, Repeat);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("key down: %d, repeat: %d"), KeyCode, Repeat);
		// log key down -> repeat keyCode Repeat 
		FEvent KeyDownEvent(EventType::KEY_DOWN);
		KeyDownEvent.SetKeyDown(KeyCode, Repeat != 0);
		NORMAL_LOG("OnKeyDown==KeyCode = %u, Repeat = %u", KeyCode, Repeat);
		 
		//KeyDownEvent.GetKeyDown();
		//ProcessEvent(KeyDownEvent);
		//WINDOW_MAIN();
		//SET_POINT(vec);
		//WINDOW_CHILD();
		//if (childwin)
		//{
		//	::PostMessageW(childwin, WM_KEYDOWN, KeyCode, 1);
		//}
		//else if (mwin)
		//{
		//	::PostMessageW(mwin, WM_KEYDOWN, KeyCode, 1);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}
		return true;
	}

	bool cinput_device::OnRtcKeyDown(const nlohmann::json & data)
	{
		//GET(FKeyCodeType, KeyCode);
		//GET(FRepeatType, Repeat);
		 
		if (data.find("keyCode") == data.end() || data.find("repeat") == data.end())
		{
			WARNING_EX_LOG("not find 'keycode' or 'repeat' [data = %s]", data.dump().c_str());
			return false;
		}
		if (!data["keyCode"].is_number() || !data["repeat"].is_number())
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		FKeyCodeType KeyCode = data["keyCode"].get<FKeyCodeType>();
		FRepeatType  Repeat = data["repeat"].get<FKeyCodeType>();
		//checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("key down: %d, repeat: %d"), KeyCode, Repeat);
		// log key down -> repeat keyCode Repeat 
		FEvent KeyDownEvent(EventType::KEY_DOWN);
		KeyDownEvent.SetKeyDown(KeyCode, Repeat != 0);
		NORMAL_LOG("OnKeyDown==KeyCode = %u, Repeat = %u", KeyCode, Repeat);
 
		return true;
	}

	/**
	* 松开键
	*/
	bool cinput_device::OnKeyUp(const uint8*& Data,   uint32 Size)
	{
		GET(FKeyCodeType, KeyCode);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("key up: %d"), KeyCode);
		// log key up -> KeyCode
		FEvent KeyUpEvent(EventType::KEY_UP);
		KeyUpEvent.SetKeyUp(KeyCode);
		NORMAL_LOG("OnKeyUp==KeyCode = %u", KeyCode);
		 
		return true;
	}
	bool cinput_device::OnRtcKeyUp(const nlohmann::json & data)
	{

		if (data.find("keyCode") == data.end() || data.find("repeat") == data.end())
		{
			WARNING_EX_LOG("not find 'keycode' or 'repeat' [data = %s]", data.dump().c_str());
			return false;
		}
		if (!data["keyCode"].is_number() || !data["repeat"].is_number())
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		FKeyCodeType KeyCode = data["keyCode"].get<FKeyCodeType>();
		FRepeatType  Repeat = data["repeat"].get<FKeyCodeType>();
		FEvent KeyUpEvent(EventType::KEY_UP);
		KeyUpEvent.SetKeyUp(KeyCode);
		NORMAL_LOG("OnKeyUp==KeyCode = %u", KeyCode);
 
		return true;
	}
	/**
	*keydown：按下键盘键
	*	keypress：紧接着keydown事件触发（只有按下字符键时触发）
	*	keyup：释放键盘键
	*/
	bool cinput_device::OnKeyPress(const uint8*& Data,   uint32 Size)
	{
		// TODO@chensong 2022-01-20  KeyPress -->>>>> net

		GET(FCharacterType, Character);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("key up: %d"), KeyCode);
		// log key up -> KeyCode
		FEvent KeyUpEvent(EventType::KEY_PRESS);
		KeyUpEvent.SetKeyUp(Character);
		NORMAL_LOG("OnKeyPress==KeyCode = %u", Character);
		 
		return true;
	}

	bool cinput_device::OnRtcKeyPress(const nlohmann::json & data)
	{
		if (data.find("keyCode") == data.end() )
		{
			WARNING_EX_LOG("not find 'keycode'  [data = %s]", data.dump().c_str());
			return false;
		}
		if (!data["keyCode"].is_number()  )
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		FCharacterType Character = data["keyCode"].get<FCharacterType>();

		FEvent KeyUpEvent(EventType::KEY_PRESS);
		KeyUpEvent.SetKeyUp(Character);
		NORMAL_LOG("OnKeyPress==KeyCode = %u", Character);
 
		return false;
	}

	bool cinput_device::OnMouseEnter(const uint8*& Data,   uint32 size)
	{
		// TODO@chensong 2022-01-20  OnMouseEnter -->>>>> net 
		/*if (!m_all_consumer.insert(std::make_pair(m_mouse_id, std::map<uint32, cmouse_info>())).second)
		{
			WARNING_EX_LOG("mouse enter insert [mouse_id = %s] failed !!!", m_mouse_id.c_str());
			return false;
		}*/
		NORMAL_EX_LOG("OnMouseEnter===>");
 

		return true;
		 
	}

	bool cinput_device::OnRtcMouseEnter(const nlohmann::json & data)
	{
		return false;
	}

	/** 
	*鼠标离开控制范围？
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

	bool cinput_device::OnRtcMouseLeave(const nlohmann::json & data)
	{
		return false;
	}
 
	


	/**
	* 鼠标按下 
	* right <-> left 
	*/
	bool cinput_device::OnMouseDown(const uint8*& Data,   uint32 Size)
	{

		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseDown at (%d, %d), button %d"), PosX, PosY, Button);
		// log mousedown -> log posX , poxY -> Button 
		//NORMAL_EX_LOG("Button = %d, PosX = %d, PoxY = %d", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseDownEvent(EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32 active_type;

		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		 
		g_width = PosX;
		g_height = PosY;
		button_mouse(MouseDownEvent, Button, false);
		/*
		PosX = g_width;
		PosY = g_height;*/
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height, active_type, PosX, PosY );
		//g_move_init = true;
		 
		return true;
	
	}

	bool cinput_device::OnRtcMouseDown(const nlohmann::json & data)
	{
		if (data.find("button") == data.end() || data.find("x") == data.end() || data.find("y") == data.end())
		{
			WARNING_EX_LOG("not find 'button' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		if (!data["button"].is_number() || !data["y"].is_number() ||
			!data["x"].is_number())
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		FButtonType Button = data["button"].get<FButtonType>();
		FPosType PosX = data["x"].get<FPosType>();
		FPosType PosY = data["y"].get<FPosType>();
		 
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseDown at (%d, %d), button %d"), PosX, PosY, Button);
		// log mousedown -> log posX , poxY -> Button 
		//NORMAL_EX_LOG("Button = %d, PosX = %d, PoxY = %d", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseDownEvent(EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32 active_type;

		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		//ProcessEvent(MouseDownEvent);

		g_width = PosX;
		g_height = PosY;

		/*
		PosX = g_width;
		PosY = g_height;*/
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height, active_type, PosX, PosY);
		 
		return true;
	}

	/** 
	*松开鼠标
	* 
	*/
	bool cinput_device::OnMouseUp(const uint8*& Data,   uint32 Size)
	{
		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseUp at (%d, %d), button %d"), PosX, PosY, Button);
		// log mouseup posx, posy, button 
		//NORMAL_EX_LOG("Button = %u, PosX = %d, PoxY = %d ", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);
		//NORMAL_EX_LOG("PosX = %d, PoxY = %d", PosX, PosY );
		FEvent MouseDownEvent(EventType::MOUSE_UP);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32  active_type;
		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		 
		g_width = PosX;
		g_height = PosY;
		button_mouse(MouseDownEvent, Button, true);
		/*
		PosX = g_width;
		PosY = g_height;*/
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height,  active_type, PosX, PosY );
		//ProcessEvent(MouseDownEvent);
		//g_move_init = false;
	 
		return true;
	}

	bool cinput_device::OnRtcMouseUp(const nlohmann::json & data)
	{
		if (data.find("button") == data.end() || data.find("x") == data.end() || data.find("y") == data.end())
		{
			WARNING_EX_LOG("not find 'button' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		if (!data["button"].is_number() || !data["y"].is_number() ||
			!data["x"].is_number())
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		FButtonType Button = data["button"].get<FButtonType>();
		FPosType PosX = data["x"].get<FPosType>();
		FPosType PosY = data["y"].get<FPosType>();
		/*	GET(FButtonType, Button);
			GET(FPosType, PosX);
			GET(FPosType, PosY);
			checkf(Size == 0, TEXT("%d"), Size);*/
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseUp at (%d, %d), button %d"), PosX, PosY, Button);
		// log mouseup posx, posy, button 
		//NORMAL_EX_LOG("Button = %u, PosX = %d, PoxY = %d ", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);
		//NORMAL_EX_LOG("PosX = %d, PoxY = %d", PosX, PosY );
		FEvent MouseDownEvent(EventType::MOUSE_UP);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32  active_type;
		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);

		g_width = PosX;
		g_height = PosY;
		/*
		PosX = g_width;
		PosY = g_height;*/
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height, active_type, PosX, PosY);
		//ProcessEvent(MouseDownEvent);
		//g_move_init = false;
 
		return true;
		return false;
	}



	/** 
	*鼠标移动
	*/
	bool cinput_device::OnMouseMove(const uint8*& Data,   uint32 Size)
	{
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		GET(FDeltaType, DeltaX);
		GET(FDeltaType, DeltaY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseMove to (%d, %d), delta (%d, %d)"), PosX, PosY, DeltaX, DeltaY);
		// log mousemove to posx, posy, [DeltaX, DeltaY]
		//NORMAL_EX_LOG("PosX = %d, PoxY = %d, DeltaY = %d", PosX, PosY, DeltaY);
		
		//RTC_LOG(LS_INFO) << "mousemove -->  PosX = " << PosX << ", PoxY = " << PosY << ", DeltaY = " << DeltaY;
		_UnquantizeAndDenormalize(PosX, PosY);
		_UnquantizeAndDenormalize(DeltaX, DeltaY);
		//RTC_LOG(LS_INFO) << "mousemove <==>  PosX = " << PosX << ", PoxY = " << PosY << ", DeltaY = " << DeltaY;
		NORMAL_EX_LOG("g_width = %d, g_height = %d, ---> PosX = %d, PoxY = %d, DeltaY = %d", g_width, g_height, PosX, PosY, DeltaY);
		
		FEvent MouseMoveEvent(EventType::MOUSE_MOVE);
		MouseMoveEvent.SetMouseDelta(PosX, PosY, DeltaX, DeltaY);
		int32_t width = g_width;
		int32_t height = g_height;
		g_width = PosX;
		g_height = PosY;
		abs_mouse(MouseMoveEvent, PosX, PosY);
		/*
		PosX = g_width;
		PosY = g_height;*/
		
		 
		//ProcessEvent(MouseMoveEvent);
		return true;
	}
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
		 
		FPosType PosX = data["x"].get<FPosType>();
		FPosType PosY = data["y"].get<FPosType>();
		FDeltaType DeltaX = data["delta_x"].get<FDeltaType>();
		FDeltaType DeltaY = data["delta_y"].get<FDeltaType>();
			/*GET(FPosType, PosX);
			GET(FPosType, PosY);
			GET(FDeltaType, DeltaX);
			GET(FDeltaType, DeltaY);
			checkf(Size == 0, TEXT("%d"), Size);*/
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseMove to (%d, %d), delta (%d, %d)"), PosX, PosY, DeltaX, DeltaY);
		// log mousemove to posx, posy, [DeltaX, DeltaY]
		//NORMAL_EX_LOG("PosX = %d, PoxY = %d, DeltaY = %d", PosX, PosY, DeltaY);

		//RTC_LOG(LS_INFO) << "mousemove -->  PosX = " << PosX << ", PoxY = " << PosY << ", DeltaY = " << DeltaY;
		_UnquantizeAndDenormalize(PosX, PosY);
		_UnquantizeAndDenormalize(DeltaX, DeltaY);
		//RTC_LOG(LS_INFO) << "mousemove <==>  PosX = " << PosX << ", PoxY = " << PosY << ", DeltaY = " << DeltaY;
		NORMAL_EX_LOG("g_width = %d, g_height = %d, ---> PosX = %d, PoxY = %d, DeltaY = %d", g_width, g_height, PosX, PosY, DeltaY);

		FEvent MouseMoveEvent(EventType::MOUSE_MOVE);
		MouseMoveEvent.SetMouseDelta(PosX, PosY, DeltaX, DeltaY);
		int32_t width = g_width;
		int32_t height = g_height;
		g_width = PosX;
		g_height = PosY;

		/*
		PosX = g_width;
		PosY = g_height;*/

 
		//ProcessEvent(MouseMoveEvent);
		return true;
		return false;
	}
	/** 
	* 鼠标双击
	*/
	bool cinput_device::OnMouseDoubleClick(const uint8*& Data,   uint32 Size)
	{
		//WM_LBUTTONDBLCLK
		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseDown at (%d, %d), button %d"), PosX, PosY, Button);
		// log mousedown -> log posX , poxY -> Button 
		//NORMAL_EX_LOG("Button = %d, PosX = %d, PoxY = %d", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseDownEvent(EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32 active_type;

		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		g_width = PosX;
		g_height = PosY;
		/*
		PosX = g_width;
		PosY = g_height;*/
		//ProcessEvent(MouseDownEvent);
		NORMAL_EX_LOG("g_width = %d, g_height = %d, ---> PosX = %d, PoxY = %d", g_width, g_height, PosX, PosY);

		//NORMAL_EX_LOG("active_type = %d, PosX = %d, PoxY = %d", active_type, PosX, PosY );
 
		return true;
	}

	bool cinput_device::OnRtcMouseDoubleClick(const nlohmann::json & data)
	{
		return false;
	}

	/** 
	* 鼠标的滚轮滚动 
	*/
	bool cinput_device::OnMouseWheel(const uint8*& Data,   uint32 Size)
	{
		GET(FDeltaType, Delta);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseWheel, delta %d"), Delta);
		// mouseWheel delta -> 
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseWheelEvent(EventType::MOUSE_WHEEL);
		MouseWheelEvent.SetMouseWheel(Delta, PosX, PosY);
		//ProcessEvent(MouseWheelEvent);
		g_width = PosX;
		g_height = PosY;
		scroll(MouseWheelEvent, Delta);
		/*PosX = g_width;
		PosY = g_height;*/
		 
		return true;
	}

	bool cinput_device::OnRtcMouseWheel(const nlohmann::json & data)
	{
		if (data.find("delta") == data.end()  || data.find("x") == data.end() || data.find("y") == data.end())
		{
			WARNING_EX_LOG("not find 'delta'   or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		if (!data["x"].is_number() || !data["y"].is_number() ||
			!data["delta"].is_number() )
		{
			WARNING_EX_LOG("  find 'delta_x' or 'delta_y' or 'x' or'y'  [data = %s]", data.dump().c_str());
			return false;
		}
		FDeltaType Delta = data["delta"].get<FDeltaType>();
		FPosType PosX = data["x"].get<FPosType>();
		FPosType PosY = data["y"].get<FPosType>();
		/*GET(FDeltaType, Delta);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);*/
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseWheel, delta %d"), Delta);
		// mouseWheel delta -> 
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseWheelEvent(EventType::MOUSE_WHEEL);
		MouseWheelEvent.SetMouseWheel(Delta, PosX, PosY);
		//ProcessEvent(MouseWheelEvent);
		g_width = PosX;
		g_height = PosY;
		/*PosX = g_width;
		PosY = g_height;*/
 
		return true;
	}

	// XY positions are the ratio (0.0..1.0) along a viewport axis, quantized
	// into an uint16 (0..65536). This allows the browser viewport and player
	// viewport to have a different size.
	void cinput_device::_UnquantizeAndDenormalize(uint16& InOutX, uint16& InOutY)
	{
		InOutX = InOutX / 65536.0f * m_int_point.X;
		InOutY = InOutY / 65536.0f * m_int_point.Y;
	}
	
	// XY deltas are the ratio (-1.0..1.0) along a viewport axis, quantized
	// into an int16 (-32767..32767). This allows the browser viewport and
	// player viewport to have a different size.
	void cinput_device::_UnquantizeAndDenormalize(int16& InOutX, int16& InOutY)
	{ 
		InOutX = InOutX / 32767.0f * m_int_point.X;
		InOutY = InOutY / 32767.0f * m_int_point.Y;
	}
	void cinput_device::_work_pthread()
	{
		std::list< webrtc::DataBuffer>  temp_list;
		static const uint32 TICK_TIME = 50;
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::milliseconds ms;
		uint32_t elapse = 0;

		while (!m_stoped)
		{
			pre_time = std::chrono::steady_clock::now();
			{
				std::lock_guard<std::mutex>  lock(m_input_mutex);
				if (!m_input_list.empty())
				{
					temp_list.swap (m_input_list);
					//m_input_list.clear();
				}
			}

			while (!temp_list.empty())
			{
				OnMessage("", temp_list.front());
				temp_list.pop_front();
				 
			}
			if (m_input_list.empty())
			{
				 
				cur_time_ms = std::chrono::steady_clock::now();
				dur = cur_time_ms - pre_time;
				ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
				elapse = static_cast<uint32_t>(ms.count());
				if (elapse < TICK_TIME)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - elapse));
				}
			}
		}
	}
}