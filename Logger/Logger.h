#ifndef __LOG__H_
#define __LOG__H_
#include <Windows.h>

#define LOG_LEVEL_DEBUG		0
#define LOG_LEVEL_SYSTEM	1
#define LOG_LEVEL_ERROR		2

namespace Jay
{
	class Logger
	{
		/**
		* @file		Logger.h
		* @brief	File Logger Class
		* @details	파일 로그 출력용 클래스
		* @author   고재현
		* @date		2022-11-22
		* @version  1.0.1
		**/
	private:
		Logger();
		~Logger();
	public:
		static void SetLogLevel(int logLevel);
		static void WriteLog(const wchar_t * tag, int logLevel, const wchar_t * fmt, ...);
	private:
		static int _logLevel;
		static Logger _instance;
	};
}

#endif //__LOG__H_
