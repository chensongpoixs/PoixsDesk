/***********************************************************************************************
	created: 		2019-03-02
	
	author:			chensong
					
	purpose:		log
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


#ifndef _C_LOG_H_
#define _C_LOG_H_
#include "cnet_types.h"
#include <cstdio>
#include <cstdlib>
//#include "cnoncopyable.h"
#include "casync_log.h"
 
namespace chen {
	
	/**
	*  @author chensong
	*  @date 2019-03-02
	*  @brief 日志输出类（Log Output Class）
	*  
	*  clog类提供了流式日志输出接口，支持多种数据类型的日志记录。
	*  使用流式操作符（<<）可以方便地构建日志消息，类似于标准输出流。
	*  
	*  日志类架构（Log Class Architecture）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |                        clog Class                              |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  m_data[EBuf_Size]: 日志缓冲区（1024字节）                  | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  m_len: 当前缓冲区已使用长度                                 | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  m_level: 日志级别（ELogLevelType枚举）                      | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   |  |  operator<<: 流式输出操作符（支持多种数据类型）              | |
	*   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  日志级别分类（Log Level Categories）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogLevel_System: 系统级别日志（最高优先级）                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogLevel_Fatal: 致命错误日志（程序无法继续运行）              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogLevel_Error: 错误日志（程序可以继续运行但存在问题）        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogLevel_Warn: 警告日志（潜在问题或异常情况）                 |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogLevel_Info: 信息日志（一般性信息输出）                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogLevel_Debug: 调试日志（开发调试时使用）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogLevel_None: 无日志级别（禁用日志）                         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  日志存储方式（Log Storage Types）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogStorageScreen: 仅输出到控制台屏幕                          |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogStorageFile: 仅输出到文件                                  |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogStoragePost: 仅通过HTTP POST发送到远程服务器              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogStorageScreenFile: 同时输出到屏幕和文件                   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  ELogStorageScreenFilePost: 同时输出到屏幕、文件和远程服务器   |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  使用流程（Usage Flow）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  1. 调用clog::init()初始化日志系统                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  2. 使用LOG宏创建日志对象（自动设置日志级别和位置信息）         |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  3. 使用流式操作符（<<）构建日志消息                            |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  4. 日志对象析构时自动输出日志内容                              |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  5. 程序退出前调用clog::destroy()清理资源                      |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note 使用流式接口，支持链式调用
	*  @note 日志缓冲区大小为1024字节（EBuf_Size）
	*  @note 日志级别控制哪些日志会被实际输出
	*  @note 支持多种存储方式：屏幕、文件、远程服务器
	*  @note 使用casync_log实现异步日志输出，提高性能
	*  
	*  使用示例：
	*  @code
	*  // 初始化日志系统
	*  clog::init(ELogStorageScreenFile, "127.0.0.1", 80);
	*  
	*  // 使用流式接口输出日志
	*  LOG_INFO << "用户登录成功，ID: " << user_id << ", 时间: " << timestamp;
	*  
	*  // 使用格式化输出
	*  VAR_LOG(ELogLevel_Info, "用户 %s 登录成功，ID: %d", username, user_id);
	*  
	*  // 清理资源
	*  clog::destroy();
	*  @endcode
	*/
	class  clog //: private cnoncopyable
	{
		 
	private:
		enum { EBuf_Size = 1024 }; /**< 日志缓冲区大小（字节） */
	public:
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 默认构造函数（Default Constructor）
		*  
		*  创建一个未指定日志级别的日志对象。日志级别默认为ELogLevel_None。
		*  
		*  @note 使用此构造函数创建的日志对象不会输出任何内容
		*  @note 缓冲区长度初始化为0
		*/
		explicit clog();
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 带日志级别的构造函数（Constructor with Log Level）
		*  
		*  创建一个指定日志级别的日志对象。
		*  
		*  @param level 日志级别（ELogLevelType枚举值）
		*  
		*  @note 日志级别决定该日志是否会被实际输出
		*  @note 只有当日志级别小于等于全局日志级别时才会输出
		*/
		explicit clog(ELogLevelType level);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 带日志级别和位置信息的构造函数（Constructor with Log Level and Location）
		*  
		*  创建一个指定日志级别、函数名和行号的日志对象。
		*  自动在日志消息前添加函数名和行号信息，便于定位问题。
		*  
		*  @param level 日志级别（ELogLevelType枚举值）
		*  @param func 函数名（通常使用FUNCTION宏）
		*  @param line 行号（通常使用__LINE__宏）
		*  
		*  @note 日志格式：[函数名:行号] 日志内容
		*  @note 使用LOG_FATAL、LOG_ERROR、LOG_WARN等宏时会自动调用此构造函数
		*/
		explicit clog(ELogLevelType level, const char* func, int line);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 析构函数（Destructor）
		*  
		*  日志对象析构时，如果缓冲区有内容且日志级别符合要求，
		*  会将日志内容输出到配置的存储位置。
		*  
		*  @note 当前实现中析构函数不自动输出日志（已注释）
		*  @note 日志输出由casync_log异步处理
		*/
		~clog();
	public:
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 初始化日志系统（Initialize Log System）
		*  
		*  初始化全局日志系统，创建异步日志对象并配置存储方式。
		*  必须在其他日志操作之前调用此函数。
		*  
		*  @param storage_type 日志存储类型（默认：ELogStorageScreenFilePost）
		*                      - ELogStorageScreen: 仅输出到控制台
		*                      - ELogStorageFile: 仅输出到文件
		*                      - ELogStoragePost: 仅发送到远程服务器
		*                      - ELogStorageScreenFile: 输出到控制台和文件
		*                      - ELogStorageScreenFilePost: 输出到控制台、文件和远程服务器
		*  @param host 远程服务器地址（默认："127.0.0.1"）
		*  @param port 远程服务器端口（默认：80）
		*  
		*  @return true 初始化成功，false 初始化失败（已初始化或内存分配失败）
		*  
		*  @note 只能初始化一次，重复调用会返回false
		*  @note 如果初始化失败，后续日志操作不会执行
		*  
		*  使用示例：
		*  @code
		*  if (!clog::init(ELogStorageScreenFile, "127.0.0.1", 8080)) {
		*      // 初始化失败处理
		*  }
		*  @endcode
		*/
		static bool init(ELogStorageType storage_type = ELogStorageScreenFilePost, const std::string& host = "127.0.0.1", uint32 port = 80);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 格式化日志输出（Formatted Log Output）
		*  
		*  使用类似printf的格式化字符串输出日志。支持可变参数列表。
		*  
		*  @param level 日志级别（ELogLevelType枚举值）
		*  @param format 格式化字符串（类似printf格式）
		*  @param ... 可变参数列表
		*  
		*  @note 格式化字符串支持标准C格式说明符（%d, %s, %f等）
		*  @note 日志会输出到配置的存储位置（屏幕/文件/远程服务器）
		*  @note 如果日志系统未初始化，会输出到标准输出
		*  
		*  使用示例：
		*  @code
		*  clog::var_log(ELogLevel_Info, "用户 %s 登录成功，ID: %d", username, user_id);
		*  @endcode
		*/
		static void  var_log(ELogLevelType level, const char* format, ...);

		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 设置全局日志级别（Set Global Log Level）
		*  
		*  设置全局日志输出级别。只有小于等于此级别的日志才会被实际输出。
		*  
		*  @param level 日志级别（ELogLevelType枚举值）
		*  
		*  @note 日志级别从高到低：System > Fatal > Error > Warn > Info > Debug > None
		*  @note 如果设置为ELogLevel_Info，则Info、Warn、Error、Fatal、System级别的日志会输出
		*  @note 如果日志系统未初始化，此函数不会执行任何操作
		*  
		*  使用示例：
		*  @code
		*  clog::set_level(ELogLevel_Info);  // 只输出Info及以上级别的日志
		*  @endcode
		*/
		static void set_level(ELogLevelType level);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 销毁日志系统（Destroy Log System）
		*  
		*  清理日志系统资源，释放异步日志对象。应在程序退出前调用。
		*  
		*  @note 调用此函数后，所有日志操作将不再执行
		*  @note 如果日志系统未初始化，此函数不会执行任何操作
		*  
		*  使用示例：
		*  @code
		*  clog::destroy();  // 程序退出前清理资源
		*  @endcode
		*/
		static void destroy();
	public:
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 布尔值输出操作符（Boolean Output Operator）
		*  
		*  将布尔值输出到日志缓冲区。true输出为'1'，false输出为'0'。
		*  
		*  @param value 布尔值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(bool);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 字符输出操作符（Character Output Operator）
		*  
		*  将单个字符输出到日志缓冲区。
		*  
		*  @param value 字符值
		*  @return 返回当前日志对象的引用，支持链式调用
		*  
		*  @note 如果缓冲区已满，字符不会被添加
		*/
		clog& operator<<(char);

		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 有符号字符输出操作符（Signed Char Output Operator）
		*  
		*  有符号字符输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 有符号字符值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(signed char);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 无符号字符输出操作符（Unsigned Char Output Operator）
		*  
		*  无符号字符输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 无符号字符值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(unsigned char);

		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 有符号短整型输出操作符（Signed Short Output Operator）
		*  
		*  有符号短整型输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 有符号短整型值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(signed short);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 无符号短整型输出操作符（Unsigned Short Output Operator）
		*  
		*  无符号短整型输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 无符号短整型值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(unsigned short);

		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 有符号整型输出操作符（Signed Int Output Operator）
		*  
		*  有符号整型输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 有符号整型值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(signed int);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 无符号整型输出操作符（Unsigned Int Output Operator）
		*  
		*  无符号整型输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 无符号整型值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(unsigned int);

		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 有符号长整型输出操作符（Signed Long Output Operator）
		*  
		*  有符号长整型输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 有符号长整型值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(signed long);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 无符号长整型输出操作符（Unsigned Long Output Operator）
		*  
		*  无符号长整型输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 无符号长整型值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(unsigned long);

		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 有符号长长整型输出操作符（Signed Long Long Output Operator）
		*  
		*  有符号长长整型输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 有符号长长整型值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(signed long long);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 无符号长长整型输出操作符（Unsigned Long Long Output Operator）
		*  
		*  无符号长长整型输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 无符号长长整型值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(unsigned long long);

		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief C风格字符串输出操作符（C-Style String Output Operator）
		*  
		*  C风格字符串输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value C风格字符串指针
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(const char *);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 标准字符串输出操作符（Standard String Output Operator）
		*  
		*  标准字符串输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 标准字符串引用
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(const std::string&);

		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 单精度浮点数输出操作符（Float Output Operator）
		*  
		*  单精度浮点数输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 单精度浮点数值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(float);
		
		/**
		*  @author chensong
		*  @date 2019-03-02
		*  @brief 双精度浮点数输出操作符（Double Output Operator）
		*  
		*  双精度浮点数输出操作符（当前实现为空，不执行任何操作）。
		*  
		*  @param value 双精度浮点数值
		*  @return 返回当前日志对象的引用，支持链式调用
		*/
		clog& operator<<(double);
	private:
		char m_data[EBuf_Size]; /**< 日志缓冲区（Log Buffer），用于存储日志内容，大小为1024字节 */
		int  m_len;             /**< 当前缓冲区已使用长度（Current Buffer Length），表示已写入的字节数 */
		ELogLevelType m_level;  /**< 日志级别（Log Level），决定该日志是否会被实际输出 */
	
	};


/**
*  @author chensong
*  @date 2019-03-02
*  @brief 日志类简化宏（Log Class Simplified Macro）
*  
*  简化日志类名称，使用LOG代替chen::clog。
*  
*  @note 使用LOG宏可以简化代码，避免重复输入命名空间
*/
#define LOG chen::clog

	/**
	*  @author chensong
	*  @date 2019-03-02
	*  @brief 标准日志宏定义（Standard Log Macro Definitions）
	*  
	*  提供不同日志级别的便捷宏，自动设置日志级别和位置信息。
	*  
	*  日志级别宏（Log Level Macros）：
	*  
	*    0                   1                   2                   3
	*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  LOG_SYSTEM: 系统级别日志（最高优先级）                        |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  LOG_FATAL: 致命错误日志（自动包含函数名和行号）                |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  LOG_ERROR: 错误日志（自动包含函数名和行号）                    |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  LOG_WARN: 警告日志（自动包含函数名和行号）                     |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  LOG_INFO: 信息日志（无位置信息）                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*   |  LOG_DEBUG: 调试日志（无位置信息）                               |
	*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*  
	*  @note LOG_FATAL、LOG_ERROR、LOG_WARN自动包含函数名和行号
	*  @note LOG_INFO、LOG_DEBUG、LOG_SYSTEM不包含位置信息
	*  @note 使用流式接口：LOG_INFO << "消息内容";
	*  
	*  使用示例：
	*  @code
	*  LOG_INFO << "用户登录成功，ID: " << user_id;
	*  LOG_ERROR << "连接失败，错误码: " << error_code;
	*  @endcode
	*/
#define LOG_SYSTEM LOG(chen::ELogLevel_System)  /**< 系统级别日志宏（System Level Log Macro） */
#define LOG_FATAL  LOG(chen::ELogLevel_Fatal, FUNCTION, __LINE__)  /**< 致命错误日志宏（Fatal Error Log Macro），自动包含函数名和行号 */
#define LOG_ERROR  LOG(chen::ELogLevel_Error, FUNCTION, __LINE__)  /**< 错误日志宏（Error Log Macro），自动包含函数名和行号 */
#define LOG_WARN   LOG(chen::ELogLevel_Warn, FUNCTION, __LINE__)    /**< 警告日志宏（Warning Log Macro），自动包含函数名和行号 */
#define LOG_INFO   LOG(chen::ELogLevel_Info)                        /**< 信息日志宏（Info Log Macro） */
#define LOG_DEBUG  LOG(chen::ELogLevel_Debug)                       /**< 调试日志宏（Debug Log Macro） */

/**
*  @author chensong
*  @date 2019-03-02
*  @brief 格式化日志输出宏（Formatted Log Output Macro）
*  
*  简化格式化日志输出函数调用。
*  
*  @note 等同于clog::var_log()函数调用
*/
#define VAR_LOG LOG::var_log

/**
*  @author chensong
*  @date 2019-03-02
*  @brief 格式化日志输出便捷宏（Formatted Log Output Convenience Macros）
*  
*  提供不同日志级别的格式化输出便捷宏，使用类似printf的格式化字符串。
*  
*  格式化日志宏（Formatted Log Macros）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  NORMAL_LOG: 信息级别格式化日志（等同于INFO级别）              |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  ERROR_LOG: 错误级别格式化日志                                  |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  WARNING_LOG: 警告级别格式化日志                                |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  SYSTEM_LOG: 系统级别格式化日志（最高优先级）                  |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  DEBUG_LOG: 调试级别格式化日志                                  |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  @note 格式化字符串支持标准C格式说明符（%d, %s, %f等）
*  @note 支持可变参数列表
*  
*  使用示例：
*  @code
*  NORMAL_LOG("用户 %s 登录成功，ID: %d", username, user_id);
*  ERROR_LOG("连接失败，错误码: %d", error_code);
*  @endcode
*/
#define NORMAL_LOG(format, ...)		VAR_LOG(chen::ELogLevel_Info, format, ##__VA_ARGS__)      /**< 信息级别格式化日志宏（Info Level Formatted Log Macro） */
#define ERROR_LOG(format, ...)		VAR_LOG(chen::ELogLevel_Error, format, ##__VA_ARGS__)     /**< 错误级别格式化日志宏（Error Level Formatted Log Macro） */
#define WARNING_LOG(format, ...)	VAR_LOG(chen::ELogLevel_Warn, format, ##__VA_ARGS__)        /**< 警告级别格式化日志宏（Warning Level Formatted Log Macro） */
#define SYSTEM_LOG(format, ...)		VAR_LOG(chen::ELogLevel_System, format, ##__VA_ARGS__)   /**< 系统级别格式化日志宏（System Level Formatted Log Macro） */
#define DEBUG_LOG(format, ...)		VAR_LOG(chen::ELogLevel_Debug, format, ##__VA_ARGS__)      /**< 调试级别格式化日志宏（Debug Level Formatted Log Macro） */

/**
*  @author chensong
*  @date 2019-03-02
*  @brief 带位置信息的格式化日志输出便捷宏（Formatted Log Output Macros with Location Info）
*  
*  提供自动包含函数名和行号的格式化日志输出便捷宏。
*  日志格式：[函数名][行号] 格式化消息内容
*  
*  带位置信息的格式化日志宏（Formatted Log Macros with Location）：
*  
*    0                   1                   2                   3
*    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  NORMAL_EX_LOG: 信息级别格式化日志（自动包含函数名和行号）      |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  WARNING_EX_LOG: 警告级别格式化日志（自动包含函数名和行号）      |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   |  ERROR_EX_LOG: 错误级别格式化日志（自动包含函数名和行号）        |
*   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*  
*  @note 自动在日志消息前添加函数名和行号信息，便于定位问题
*  @note 格式化字符串支持标准C格式说明符（%d, %s, %f等）
*  @note 使用FUNCTION宏获取函数名，__LINE__宏获取行号
*  
*  使用示例：
*  @code
*  NORMAL_EX_LOG("用户 %s 登录成功，ID: %d", username, user_id);
*  // 输出：[OnUserLogin][123] 用户 admin 登录成功，ID: 1001
*  
*  ERROR_EX_LOG("连接失败，错误码: %d", error_code);
*  // 输出：[ConnectServer][456] 连接失败，错误码: 1001
*  @endcode
*/
#define NORMAL_EX_LOG(format, ...)	NORMAL_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)  /**< 信息级别格式化日志宏（带位置信息）（Info Level Formatted Log Macro with Location） */
#define WARNING_EX_LOG(format, ...)	WARNING_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__) /**< 警告级别格式化日志宏（带位置信息）（Warning Level Formatted Log Macro with Location） */
#define ERROR_EX_LOG(format, ...)	ERROR_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)    /**< 错误级别格式化日志宏（带位置信息）（Error Level Formatted Log Macro with Location） */
} // namespace chen

#endif //!#define _C_LOG_H_