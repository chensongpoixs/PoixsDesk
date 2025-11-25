/***********************************************************************************************
						created: 		2018-10-02

						author:			chensong

						purpose:		async_log
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

#include "casync_log.h"
//#include "ctime_api.h"
#include <cassert>
#include <thread>
#include <iostream>
#include <mutex>
#include "clog.h"
#include <sstream>
#include <string>
#include "ctime_api.h"
//#include "httplib.h"
#define LOG_WRITE_FILE (0)
#ifdef _MSC_VER
#include <io.h>
#include <direct.h>
#elif defined(__linux__) ||defined(__APPLE__)
#include <unistd.h>
#include <dirent.h>
#include <arpa/inet.h>
#else
// 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
#include <vector>
#include "cutil.h"
//#include <boost/filesystem.hpp>


namespace chen {

	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 日志项结构体（Log Item Structure）
	*  
	*  存储单条日志的完整信息，包括时间戳、日志级别和日志内容。
	*  
	*  日志项结构体布局（Log Item Structure Layout）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |                        clog_item Structure                     |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  timestamp: 时间戳（GMT时间，秒）                           | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  level: 日志级别（signed char，对应ELogLevelType枚举）      | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  data: 日志内容（std::string，格式化后的日志消息）         | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note 使用#pragma pack(push,1)确保结构体紧凑对齐
	*  @note timestamp使用GMT时间戳（秒）
	*  @note level对应ELogLevelType枚举值
	*  @note data存储格式化后的日志消息字符串
	*/
#pragma pack(push,1)
	struct clog_item
	{
		int				timestamp;  /**< 时间戳（Timestamp），GMT时间戳（秒），记录日志生成时间 */
		signed char		level;      /**< 日志级别（Log Level），对应ELogLevelType枚举值 */
		std::string		data;       /**< 日志内容（Log Data），格式化后的日志消息字符串 */
	};
#pragma pack(pop)
//	static const unsigned int LOG_ITEM_SIZE = sizeof(clog_item);
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 日志缓冲区最大大小（Log Buffer Maximum Size）
	*  
	*  单条日志消息的最大缓冲区大小，单位为字节。
	*  
	*  @note 默认值：50KB（1024 * 50）
	*  @note 原设计为1MB，但Windows栈空间较小，改为50KB
	*  @note 超过此大小的日志消息会被截断
	*/
	static const unsigned int LOG_BUF_MAX_SIZE = 1024 * 50; // default 1024 * 1024 statck win too small

	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 日志颜色类型枚举（Log Color Type Enumeration）
	*  
	*  定义日志在控制台输出时的颜色类型。
	*  
	*  @note 用于控制台彩色输出（当前实现中未使用）
	*  @note 不同日志级别可以对应不同颜色，便于快速识别
	*/
	enum ELogColorType
	{
		ELCT_White = 0,  /**< 白色（White） */
		ELCT_Red,        /**< 红色（Red） */
		ELCT_Pink,       /**< 粉色（Pink） */
		ELCT_Yellow,     /**< 黄色（Yellow） */
		ELCT_Blue,       /**< 蓝色（Blue） */
		ELCT_Green,      /**< 绿色（Green） */
		ELCT_Max         /**< 最大值（Maximum） */
	};
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 日志名称颜色映射结构体（Log Name Color Mapping Structure）
	*  
	*  将日志级别名称映射到对应的颜色类型。
	*  
	*  @note 用于控制台彩色输出（当前实现中未使用）
	*/
	struct clog_name_color
	{
		const char* name;        /**< 日志级别名称（Log Level Name） */
		ELogColorType color;    /**< 对应的颜色类型（Corresponding Color Type） */
	};
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 日志级别名称颜色映射表（Log Level Name Color Mapping Table）
	*  
	*  全局静态数组，将日志级别映射到名称和颜色。
	*  
	*  映射关系（Mapping Relationship）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  [0] "": White（空，对应ELogLevel_None）                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  [1] "system": Blue（系统级别）                                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  [2] "fatal": Red（致命错误）                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  [3] "error": Pink（错误）                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  [4] "warn": Yellow（警告）                                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  [5] "info": White（信息）                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  [6] "debug": Green（调试）                                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note 数组索引对应ELogLevelType枚举值
	*  @note 用于格式化日志输出时的级别名称显示
	*/
	static const clog_name_color g_log_name_colors[] =
	{
		{ "",		ELCT_White },   /**< 空级别（None Level） */
		{ "system", ELCT_Blue },   /**< 系统级别（System Level） */
		{ "fatal",  ELCT_Red },    /**< 致命错误级别（Fatal Level） */
		{ "error",  ELCT_Pink },   /**< 错误级别（Error Level） */
		{ "warn",	ELCT_Yellow },  /**< 警告级别（Warning Level） */
		{ "info",	ELCT_White },   /**< 信息级别（Info Level） */
		{ "debug",	ELCT_Green },   /**< 调试级别（Debug Level） */
	};

	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 生成日志文件名（Generate Log File Name）
	*  
	*  根据路径、名称、扩展名和命名类型生成完整的日志文件路径。
	*  
	*  @param p 输出缓冲区指针（Output Buffer Pointer），用于存储生成的文件路径
	*  @param path 日志文件目录路径（Log File Directory Path）
	*  @param name 日志文件名称（Log File Name），不包含扩展名
	*  @param ext 日志文件扩展名（Log File Extension），如".log"
	*  @param name_type 日志文件命名类型（Log File Naming Type）
	*                  - ELogName_Date: 使用日期命名（YYYY-MM-DD）
	*                  - ELogName_DateTime: 使用日期时间命名（YYYY-MM-DD_HH-MM-SS）
	*                  - ELogName_AutoDate: 自动按日期命名，支持日期戳递增
	*  @param day_stamp 日期戳引用（Day Stamp Reference），用于ELogName_AutoDate类型
	*  
	*  @note 文件命名格式：路径/日期时间_名称.扩展名
	*  @note ELogName_AutoDate类型会自动更新day_stamp，用于按日期自动分割日志文件
	*  @note 使用sprintf格式化字符串，确保缓冲区足够大
	*  
	*  使用示例：
	*  @code
	*  char filename[1024];
	*  int day_stamp = 0;
	*  gen_log_file_name(filename, "./log", "rtc_native", ".log", ELogName_AutoDate, day_stamp);
	*  // 结果：./log/2024-01-20_12-30-45_rtc_native.log
	*  @endcode
	*/
	static void gen_log_file_name(char* p, const std::string& path, const std::string& name
		, const std::string& ext, ELogNameType name_type, int& day_stamp)
	{
		p += sprintf(p, "%s", path.c_str());
		char dateTime[ASCII_DATETIME_LEN] = { 0 };
		if (name_type == ELogName_Date)
		{
			ctime_base_api::datetime_format(dateTime, 0, -1, -1);
			 
		}
		else if (name_type == ELogName_DateTime)
		{
			ctime_base_api::datetime_format(dateTime, 0, 0, 0);
			 
		}
		else if (name_type == ELogName_AutoDate)
		{
			if (day_stamp == 0)
			{
				day_stamp = ctime_base_api::get_today_stamp_time64(0);
			}
			ctime_base_api::datetime_format(dateTime, 0, 0, 0);
			 
			day_stamp += ETC_Day;
		}
		sprintf(p, "%s_%s%s", dateTime, name.c_str(), ext.c_str());
	}
	 
	/*casync_log::casync_log()
		: m_host("127.0.0.1")
		, m_port(80)  
		, m_level_log(ELogLevel_Num)
		, m_storage_type(ELogStorageScreenFilePost)
		, m_stoped(false)
		, m_date_time(0)
		, m_path("./log")
		, m_expired_log_day(3)
	{ 
	}*/

	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 析构函数实现（Destructor Implementation）
	*  
	*  清理异步日志对象资源。当前实现为空，实际清理由destroy()函数完成。
	*  
	*  @note 建议在析构前显式调用destroy()以确保正确清理
	*/
	casync_log::~casync_log()
	{
		
	}
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 初始化异步日志系统实现（Initialize Asynchronous Log System Implementation）
	*  
	*  初始化异步日志系统，创建日志目录、打开日志文件、启动工作线程。
	*  
	*  @param storagetype 日志存储类型（位标志组合）
	*  @param host 远程服务器地址
	*  @param port 远程服务器端口
	*  @param day 日志文件保留天数（最小1天）
	*  
	*  @return true 初始化成功，false 初始化失败
	*  
	*  @note 如果日志目录不存在，会自动创建（Windows使用_mkdir，Linux/Mac使用mkdir）
	*  @note 如果启用文件存储，会创建第一个日志文件
	*  @note 启动独立工作线程处理日志写入
	*  @note 过期天数最小为1天
	*/
	bool casync_log::init(ELogStorageType storagetype, const   std::string  & host, uint32 port, uint32 day)
	{ 
		//NORMAL_EX_LOG("host = %s, port = %d, show_screen = %d", host.c_str(), port, show_screen);


#ifdef _MSC_VER
		if (::_access(m_path.c_str(), 0) == -1)
		{

			::_mkdir(m_path.c_str());
		}
#elif defined(__GNUC__) ||defined(__APPLE__)
		if (::access(m_path.c_str(), 0) == -1)
		{
			::mkdir(m_path.c_str(), 0777);
		}
#else
		// 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
		return false;
#endif
		if (storagetype & ELogStorageFile)
		{
			char log_name[1024] = {0};
			gen_log_file_name(log_name, m_path + "/", "rtc_native", ".log", ELogName_AutoDate, m_date_time);
			m_fd.open(log_name, std::ios::out | std::ios::trunc);
			if (!m_fd.is_open())
			{
				std::cout << "not open log file dest url = " << log_name << std::endl;
				return false;
			}
		}
		m_host = host;
		m_port = port;
		m_storage_type = storagetype;
		m_expired_log_day = day > 0 ? day : 1;
		std::thread td(&casync_log::_work_pthread, this);
		m_thread.swap(td);
		//std::move(m_thread, td);
		return true;
	}
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 添加格式化日志到队列实现（Append Formatted Log to Queue Implementation）
	*  
	*  使用类似printf的格式化字符串添加日志到队列。日志会在工作线程中异步处理。
	*  
	*  @param level 日志级别（ELogLevelType枚举值）
	*  @param format 格式化字符串（类似printf格式）
	*  @param ap 可变参数列表（va_list类型）
	*  
	*  @note 如果日志级别不符合要求或系统已停止，函数直接返回
	*  @note 使用vsnprintf格式化字符串，最大缓冲区为LOG_BUF_MAX_SIZE（50KB）
	*  @note 创建日志项并添加到队列，使用互斥锁保护
	*  @note 使用条件变量通知工作线程有新日志
	*  @note 线程安全，支持多线程并发调用
	*/
	void casync_log::append_var(ELogLevelType level, const char * format, va_list ap)
	{
		
		if (level < ELogLevel_None || level > ELogLevel_Num  || level >= m_level_log)
		{
			return;
		}

		if (m_stoped)
		{
			return;
		}
		 
		int32 cnt = -1;
		char buffer[LOG_BUF_MAX_SIZE] = {0};
		cnt = vsnprintf(buffer, LOG_BUF_MAX_SIZE, format, ap);
		if (cnt <= 0)
		{ 
			return;
		}
		{
			
			std::shared_ptr<clog_item>  log_item_ptr = _get_new_buf();
			if (!log_item_ptr)
			{
				return;
			}
			{
				std::lock_guard<std::mutex> lock{ m_lock };

				//得到一个新的log

				log_item_ptr->level = level;
				log_item_ptr->timestamp = static_cast<int>(ctime_base_api::get_gmt());
				log_item_ptr->data = buffer;
				m_log_item.push_back(log_item_ptr);
			}
		}
		
		{
			m_condition.notify_one();
		}
	}
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 设置日志输出级别实现（Set Log Output Level Implementation）
	*  
	*  设置日志输出级别。只有小于此级别的日志才会被实际输出。
	*  
	*  @param level 日志级别（ELogLevelType枚举值）
	*  
	*  @note 日志级别从高到低：System > Fatal > Error > Warn > Info > Debug > None
	*  @note 线程安全，可以在运行时动态修改
	*/
	void casync_log::set_level(ELogLevelType level)
	{
		m_level_log = level;
	}
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 初始化日志实现（Initialize Log Implementation）
	*  
	*  内部初始化函数（当前实现为空，返回true）。
	*  
	*  @return true 初始化成功
	*/
	bool casync_log::_init_log()
	{
		return true;
	}
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 工作线程函数实现（Worker Thread Function Implementation）
	*  
	*  独立工作线程的主函数。从日志队列中取出日志项并处理。
	*  
	*  @note 使用条件变量等待新日志或停止信号
	*  @note 批量处理日志项，提高性能（一次性取出所有待处理日志）
	*  @note 根据存储类型输出日志到不同位置
	*  @note 在停止时会处理队列中剩余的日志，确保不丢失
	*  @note 如果启用文件存储，会检查并处理日志文件（按日期分割）
	*/
	void casync_log::_work_pthread()
	{
		std::shared_ptr<clog_item>  log_item_ptr;
		std::list<std::shared_ptr<clog_item>> log_items;
		while (!m_stoped  )
		{
			//当又有数据就立即写入日志文件中
			if (m_log_item.size() <= 0)
			{
				std::unique_lock<std::mutex> lock(m_lock);
				//有数据或者停止才可以写入日志中
				m_condition.wait(lock, [this]() {return (m_log_item.size() > 0 || m_stoped); });
			}
			if (m_log_item.empty())
            {
                continue;
            }
            {
                std::lock_guard<std::mutex> lock{m_lock};
                log_items = m_log_item;
                m_log_item.clear();
            }
			if (m_storage_type & ELogStorageFile)
			{
				_handler_check_log_file();
			}
			
			while (!log_items.empty())
			{
				{
//					std::lock_guard<std::mutex> lock{ m_lock };
					log_item_ptr = log_items.front();
                    log_items.pop_front();
				}

				if (!log_item_ptr)
				{
					continue;
				}
                
				_handler_log_item(log_item_ptr);
				log_item_ptr = nullptr;
			}


			
			 
		}


		if (m_log_item.size() > 0)
		{
			while (!m_log_item.empty())
			{
				{
					//					std::lock_guard<std::mutex> lock{ m_lock };
					log_item_ptr = m_log_item.front();
					m_log_item.pop_front();
				}

				if (!log_item_ptr)
				{
					continue;
				}

				_handler_log_item(log_item_ptr);
				log_item_ptr = nullptr;
			}
		}
	}
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 获取新的日志缓冲区实现（Get New Log Buffer Implementation）
	*  
	*  创建新的日志项对象（clog_item）。
	*  
	*  @return 日志项智能指针，如果创建失败返回nullptr
	*  
	*  @note 使用std::make_shared创建共享指针，自动管理内存
	*  @note 如果内存分配失败，返回nullptr
	*/
	std::shared_ptr<clog_item>   casync_log::_get_new_buf()
	{
		std::shared_ptr<clog_item>  log_item_ptr = std::make_shared<clog_item>( clog_item());
		if (!log_item_ptr)
		{
			return NULL;
		}
		return log_item_ptr;
	}

	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 处理日志项实现（Handle Log Item Implementation）
	*  
	*  处理单个日志项，根据存储类型输出到屏幕、文件或远程服务器。
	*  
	*  @param log_item_ptr 日志项智能指针
	*  
	*  @note 格式化日志输出："[时间] [级别] 日志内容"
	*  @note 时间格式：YYYY-MM-DD HH:MM:SS
	*  @note 根据存储类型输出到不同位置：
	*        - ELogStorageScreen: 输出到std::cout
	*        - ELogStorageFile: 写入文件并刷新
	*        - ELogStoragePost: 发送到远程服务器（当前实现已注释）
	*  @note 文件输出会自动添加换行符并刷新
	*  @note 控制台输出也会自动添加换行符并刷新
	*/
	void casync_log::_handler_log_item(const std::shared_ptr<clog_item>   log_item_ptr)
	{
		std::ostringstream m_stream;
		 
		
		char dateTime[ASCII_DATETIME_LEN] = { 0 };
		ctime_base_api::time64_datetime_format(log_item_ptr->timestamp, dateTime, '-', ' ', ':');
 	
		m_stream << '[' << dateTime << "] [" << g_log_name_colors[log_item_ptr->level].name << "] ";
		m_stream << log_item_ptr->data;
		if ( (m_storage_type & ELogStoragePost) && log_item_ptr->data.length() > 0)
		{
			//m_stream  << "\n";
			////std::string host =webrtc::g_cfg.get_string(webrtc::ECI_MediaSoup_Host) ;
			//httplib::Client cli(m_host, m_port);
			//std::string url = "http://"+ m_host + ":" +std::to_string(m_port) +"/decoder/decoder.php";
			//auto res = cli.Post(url.c_str(), m_stream.str().c_str(), "application/json");
			//if (res && res->status == 200)
			//{
			//	// send ok	
			//}
			//else
			//{
			//	printf("[http][error]send host = %s , [port = %d]\n", m_host.c_str(), m_port);
			//	fflush(stdout);
			//	//fflush();
			//}
			
		}

		if ((m_storage_type & ELogStorageFile) && m_fd.is_open() && log_item_ptr->data.length() > 0)
		{
			m_fd.write(m_stream.str().c_str(), m_stream.str().length());
			 
			m_fd.write("\n", 1);
			m_fd.flush();

		}
		// ELogStorageScreenFilePost
		if ( (m_storage_type & ELogStorageScreen) && log_item_ptr->data.length() > 0)
		{
			std::cout.write(m_stream.str().c_str(), m_stream.str().length()); 
			std::cout.write("\n", 1);
			std::cout.flush();
		}
	}
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 检查并处理日志文件实现（Check and Handle Log File Implementation）
	*  
	*  检查当前日期，如果日期变化则创建新的日志文件。
	*  
	*  @note 按日期自动分割日志文件
	*  @note 如果日期变化，关闭旧文件并打开新文件
	*  @note 触发过期日志文件清理
	*  @note 使用ELogName_AutoDate类型自动生成文件名
	*/
	void			casync_log::_handler_check_log_file()
	{
		int32 day_time = ctime_base_api::get_today_stamp_time64(0);
		if (day_time > m_date_time)
		{
		
			if (m_fd.is_open())
			{
				m_fd.flush();
				m_fd.close();
			}

			char log_name[1024] = {0};
			gen_log_file_name(log_name, m_path + "/", "rtc_native", ".log", ELogName_AutoDate, m_date_time);
			m_fd.open(log_name, std::ios::out | std::ios::trunc);
			if (!m_fd.is_open())
			{
				std::cout << "not open log file dest url = " << log_name << std::endl;
				//return false;
			}
		
			_check_expired_log_file();
		}
	}
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 检查并删除过期日志文件实现（Check and Delete Expired Log Files Implementation）
	*  
	*  检查日志目录中的过期日志文件并删除。
	*  
	*  @note 根据m_expired_log_day设置删除超过保留天数的日志文件
	*  @note 当前实现为空（已注释），需要根据实际需求实现
	*  @note 原实现使用boost::filesystem库，需要依赖外部库
	*  @note 可以替换为C++17的std::filesystem或自定义实现
	*/
	void casync_log::_check_expired_log_file()
	{
		std::vector<std::string>   filenames;
		//if (path_util::get_path_all_filenames(m_path, filenames) > 0)
		//{

		//	std::time_t expired_date_time = m_date_time - (ETC_Day * (m_expired_log_day > 0 ? m_expired_log_day : 1));
		//	for (const std::string& fname : filenames)
		//	{
		//		if (boost::filesystem::is_regular_file(fname))
		//		{
		//			std::time_t file_time = boost::filesystem::last_write_time(fname);
		//			if (file_time < expired_date_time)
		//			{
		//				// delete file !!!
		//				if (!boost::filesystem::remove(boost::filesystem::path(fname)))
		//				{
		//					WARNING_EX_LOG("delete file = %s failed !!!",  fname.c_str());
		//					std::cerr << "delete file = " << fname << " !!! " << std::endl;
		//				}
		//			}
		//		}
		//	}
		//}
	
	
	}
	
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 销毁异步日志系统实现（Destroy Asynchronous Log System Implementation）
	*  
	*  停止工作线程，关闭日志文件，清理所有资源。
	*  
	*  @note 设置停止标志（原子操作）并唤醒所有等待的线程
	*  @note 等待工作线程结束（join）
	*  @note 关闭并刷新日志文件
	*  @note 清空日志队列
	*  @note 确保所有日志都被处理完毕
	*/
	void casync_log::destroy()
	{
		
		m_stoped.store(true);

		m_condition.notify_all();
		
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		if (m_fd.is_open())
		{
			m_fd.flush();
			m_fd.close();
		}
		
		m_log_item.clear();
		 
	}
}//namespace chen