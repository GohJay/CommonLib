#ifndef __ASYNC_LOG__H_
#define __ASYNC_LOG__H_
#include <Windows.h>

#define LOG_LEVEL_DEBUG		0
#define LOG_LEVEL_SYSTEM	1
#define LOG_LEVEL_ERROR		2

namespace Jay
{
	class AsyncLogger
	{
		/**
		* @file		AsyncLogger.h
		* @brief	Async File Logger Class
		* @details	파일 로그 비동기 출력용 클래스
		* @author   고재현
		* @date		2022-12-24
		* @version  1.0.0
		**/
	private:
		struct LOG
		{
			wchar_t type[32];
			int logLevel;
			wchar_t buffer[256];
			bool truncated;
		};
	private:
		AsyncLogger();
		~AsyncLogger();
	public:
		static void SetLogLevel(int logLevel);
		static void SetLogPath(const wchar_t* logPath);
		static void WriteLog(const wchar_t* type, int logLevel, const wchar_t* fmt, ...);
		static void WriteHex(const wchar_t* type, int logLevel, const wchar_t* log, BYTE* byte, int byteLen);
	private:
		static void CALLBACK WriteProc(ULONG_PTR dwData);
		static unsigned int WINAPI WorkerThread(LPVOID lpParam);
	private:
		static DWORD _logIndex;
		static int _logLevel;
		static wchar_t _logPath[MAX_PATH];
		static HANDLE _hWorkerThread;
		static HANDLE _hExitThreadEvent;
		static AsyncLogger _instance;
	};
}

#endif //__ASYNC_LOG__H_
