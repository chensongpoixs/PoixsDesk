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

#ifndef _C_ASYNC_LOG_H
#define _C_ASYNC_LOG_H
#include "clog_define.h"
#include <string>
#include <iostream>
//#include <cstring>
#include <iostream>
#include <fstream>
#include <iostream>
#include <fstream>
#include <memory>
#include <ctime>
#include <cstdio>
#include <cstdlib>
//#include "cnoncopyable.h"
//#include "clog_color.h"
#include <thread>
#include <list>
#include "cnet_types.h"
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <ostream>

namespace chen
{
	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 日志项结构体前向声明（Log Item Structure Forward Declaration）
	*  
	*  日志项结构体，用于存储单条日志的完整信息。
	*  详细定义在casync_log.cpp中。
	*/
	struct clog_item;

	/**
	*  @author chensong
	*  @date 2018-10-02
	*  @brief 异步日志类（Asynchronous Log Class）
	*  
	*  casync_log类提供了高性能的异步日志输出功能。使用独立的工作线程处理日志写入，
	*  避免日志操作阻塞主线程，提高程序性能。
	*  
	*  异步日志类架构（Asynchronous Log Class Architecture）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |                    casync_log Class                           |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  主线程（Main Thread）                                     | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  append_var(): 添加日志到队列                           | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  m_log_item: 日志队列（线程安全）                       | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  工作线程（Worker Thread）                                  | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  _work_pthread(): 从队列取出日志并处理                  | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  |  |  _handler_log_item(): 输出日志到屏幕/文件/远程服务器    | | |
	*   |  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  日志处理流程（Log Processing Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 主线程调用append_var()添加日志到队列                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 工作线程通过条件变量被唤醒                                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 工作线程从队列批量取出日志项                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 工作线程根据存储类型输出日志                               |
	*   |  |  - ELogStorageScreen: 输出到控制台                         |
	*   |  |  - ELogStorageFile: 写入文件                               |
	*   |  |  - ELogStoragePost: 发送到远程服务器                       |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  日志存储类型（Log Storage Types）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogStorageScreen: 仅输出到控制台屏幕                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogStorageFile: 仅输出到文件（按日期自动分割）              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogStoragePost: 仅通过HTTP POST发送到远程服务器             |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogStorageScreenFile: 同时输出到屏幕和文件                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogStorageScreenFilePost: 同时输出到屏幕、文件和远程服务器 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note 使用独立工作线程处理日志，避免阻塞主线程
	*  @note 使用条件变量实现高效的线程间通信
	*  @note 支持多种存储方式：屏幕、文件、远程服务器
	*  @note 文件日志按日期自动分割，支持过期日志自动清理
	*  @note 线程安全，支持多线程并发写入日志
	*  
	*  使用示例：
	*  @code
	*  casync_log* log = new casync_log();
	*  log->init(ELogStorageScreenFile, "127.0.0.1", 8080, 7);
	*  log->set_level(ELogLevel_Info);
	*  log->append_var(ELogLevel_Info, "用户 %s 登录成功", username);
	*  // ... 使用日志 ...
	*  log->destroy();
	*  delete log;
	*  @endcode
	*/
	class casync_log //: private cnoncopyable
	{
		 
	private:
		typedef std::condition_variable					ccond;        /**< 条件变量类型别名（Condition Variable Type Alias），用于线程间同步 */
		typedef std::atomic_bool						catomic_bool;  /**< 原子布尔类型别名（Atomic Bool Type Alias），用于线程安全的布尔标志 */
	public:
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 默认构造函数（Default Constructor）
		*  
		*  初始化异步日志对象，设置默认参数。
		*  
		*  @note 默认主机地址：127.0.0.1
		*  @note 默认端口：80
		*  @note 默认日志级别：ELogLevel_Num（所有级别）
		*  @note 默认存储类型：ELogStorageScreenFilePost（屏幕+文件+远程）
		*  @note 默认日志路径：./log
		*  @note 默认过期天数：3天
		*/
		explicit casync_log()
			: m_host("127.0.0.1")
			, m_port(80)
			, m_level_log(ELogLevel_Num)
			, m_storage_type(ELogStorageScreenFilePost)
			, m_stoped(false)
			, m_date_time(0)
			, m_path("./log")
			, m_log_item()
			, m_expired_log_day(3)
		{
		}
		
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 析构函数（Destructor）
		*  
		*  清理异步日志对象资源。自动调用destroy()停止工作线程并关闭文件。
		*  
		*  @note 确保在析构前调用destroy()以正确清理资源
		*/
		~casync_log();
		
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 初始化异步日志系统（Initialize Asynchronous Log System）
		*  
		*  初始化异步日志系统，创建日志目录、打开日志文件、启动工作线程。
		*  
		*  @param storage_type 日志存储类型（位标志组合）
		*                      - ELogStorageScreen: 输出到控制台
		*                      - ELogStorageFile: 输出到文件
		*                      - ELogStoragePost: 发送到远程服务器
		*                      - 可以组合使用（如：ELogStorageScreenFile）
		*  @param host 远程服务器地址（如果启用ELogStoragePost）
		*  @param port 远程服务器端口（如果启用ELogStoragePost）
		*  @param day 日志文件保留天数（默认3天，最小1天）
		*  
		*  @return true 初始化成功，false 初始化失败（目录创建失败或文件打开失败）
		*  
		*  @note 如果日志目录不存在，会自动创建
		*  @note 文件日志按日期自动分割（ELogName_AutoDate）
		*  @note 启动独立工作线程处理日志写入
		*  @note 只能初始化一次，重复调用可能导致资源泄漏
		*  
		*  使用示例：
		*  @code
		*  if (!log->init(ELogStorageScreenFile, "127.0.0.1", 8080, 7)) {
		*      // 初始化失败处理
		*  }
		*  @endcode
		*/
		bool init(ELogStorageType storage_type , const   std::string  & host, uint32 port, uint32 day = 3);
		
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 销毁异步日志系统（Destroy Asynchronous Log System）
		*  
		*  停止工作线程，关闭日志文件，清理所有资源。
		*  应在程序退出前调用此函数。
		*  
		*  @note 设置停止标志并唤醒工作线程
		*  @note 等待工作线程结束
		*  @note 关闭并刷新日志文件
		*  @note 清空日志队列
		*  
		*  使用示例：
		*  @code
		*  log->destroy();  // 程序退出前清理资源
		*  @endcode
		*/
		void destroy();
	public:
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 添加格式化日志到队列（Append Formatted Log to Queue）
		*  
		*  使用类似printf的格式化字符串添加日志到队列。日志会在工作线程中异步处理。
		*  
		*  @param level 日志级别（ELogLevelType枚举值）
		*  @param format 格式化字符串（类似printf格式）
		*  @param ap 可变参数列表（va_list类型）
		*  
		*  @note 如果日志级别不符合要求或系统已停止，函数直接返回
		*  @note 日志缓冲区最大大小为50KB（LOG_BUF_MAX_SIZE）
		*  @note 使用条件变量通知工作线程有新日志
		*  @note 线程安全，支持多线程并发调用
		*  
		*  使用示例：
		*  @code
		*  va_list ap;
		*  va_start(ap, format);
		*  log->append_var(ELogLevel_Info, "用户 %s 登录成功", ap);
		*  va_end(ap);
		*  @endcode
		*/
		void append_var(ELogLevelType level, const char* format, va_list ap);

		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 设置日志输出级别（Set Log Output Level）
		*  
		*  设置日志输出级别。只有小于此级别的日志才会被实际输出。
		*  
		*  @param level 日志级别（ELogLevelType枚举值）
		*  
		*  @note 日志级别从高到低：System > Fatal > Error > Warn > Info > Debug > None
		*  @note 如果设置为ELogLevel_Info，则Info、Warn、Error、Fatal、System级别的日志会输出
		*  @note 线程安全，可以在运行时动态修改
		*  
		*  使用示例：
		*  @code
		*  log->set_level(ELogLevel_Info);  // 只输出Info及以上级别的日志
		*  @endcode
		*/
		void set_level(ELogLevelType level);
	private:
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 初始化日志（Initialize Log）
		*  
		*  内部初始化函数（当前实现为空，返回true）。
		*  
		*  @return true 初始化成功
		*/
		bool			_init_log();
	private:
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 工作线程函数（Worker Thread Function）
		*  
		*  独立工作线程的主函数。从日志队列中取出日志项并处理。
		*  
		*  @note 使用条件变量等待新日志或停止信号
		*  @note 批量处理日志项，提高性能
		*  @note 根据存储类型输出日志到不同位置
		*  @note 在停止时会处理队列中剩余的日志
		*/
		void			_work_pthread();
		
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 获取新的日志缓冲区（Get New Log Buffer）
		*  
		*  创建新的日志项对象（clog_item）。
		*  
		*  @return 日志项智能指针，如果创建失败返回nullptr
		*  
		*  @note 使用std::make_shared创建共享指针
		*/
		std::shared_ptr<clog_item>		_get_new_buf();
		
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 处理日志项（Handle Log Item）
		*  
		*  处理单个日志项，根据存储类型输出到屏幕、文件或远程服务器。
		*  
		*  @param log_item_ptr 日志项智能指针
		*  
		*  @note 格式化日志输出："[时间] [级别] 日志内容"
		*  @note 根据存储类型输出到不同位置
		*  @note 文件输出会自动刷新
		*/
		void			_handler_log_item(const std::shared_ptr<clog_item> log_item_ptr);
		
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 检查并处理日志文件（Check and Handle Log File）
		*  
		*  检查当前日期，如果日期变化则创建新的日志文件。
		*  
		*  @note 按日期自动分割日志文件
		*  @note 如果日期变化，关闭旧文件并打开新文件
		*  @note 触发过期日志文件清理
		*/
		void			_handler_check_log_file();
		
		/**
		*  @author chensong
		*  @date 2018-10-02
		*  @brief 检查并删除过期日志文件（Check and Delete Expired Log Files）
		*  
		*  检查日志目录中的过期日志文件并删除。
		*  
		*  @note 根据m_expired_log_day设置删除超过保留天数的日志文件
		*  @note 当前实现为空（已注释），需要根据实际需求实现
		*/
		void			_check_expired_log_file();
	private: 
		std::string				m_host;              /**< 远程服务器地址（Remote Server Host），用于HTTP POST发送日志 */
		uint32					m_port;              /**< 远程服务器端口（Remote Server Port），用于HTTP POST发送日志 */
		int32					m_level_log;         /**< 当前日志输出级别（Current Log Output Level），小于此级别的日志会被过滤 */
		ELogStorageType			m_storage_type;      /**< 日志存储类型（Log Storage Type），位标志组合，支持多种存储方式 */
		catomic_bool			m_stoped;            /**< 停止标志（Stop Flag），原子布尔类型，用于通知工作线程停止 */
		std::thread				m_thread;            /**< 工作线程（Worker Thread），独立线程处理日志写入 */
		std::mutex				m_lock;              /**< 互斥锁（Mutex Lock），保护日志队列的线程安全访问 */
		ccond					m_condition;         /**< 条件变量（Condition Variable），用于工作线程等待新日志或停止信号 */
		std::ofstream			m_fd;                /**< 日志文件流（Log File Stream），用于写入日志文件 */
		int32_t					m_date_time;         /**< 当前日志文件日期戳（Current Log File Date Stamp），用于判断是否需要创建新文件 */
		std::string				m_path;              /**< 日志文件路径（Log File Path），日志文件存储目录 */
		std::list<std::shared_ptr<clog_item>>	m_log_item;  /**< 日志队列（Log Queue），存储待处理的日志项，线程安全 */
		uint32					m_expired_log_day;   /**< 日志文件保留天数（Expired Log File Retention Days），超过此天数的日志文件会被删除 */
	};
}


#endif // !#define _C_ASYNC_LOG_H
