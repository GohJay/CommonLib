#include "AsyncLogger.h"
#include "FileUtil.h"
#include <locale.h>
#include <wchar.h>
#include <strsafe.h>
#include <time.h>
#include <process.h>

#define LOG_LEVEL_DEBUGW			L"DEBUG"
#define LOG_LEVEL_SYSTEMW			L"SYSTEM"
#define LOG_LEVEL_ERRORW			L"ERROR"
#define LOG_ERROR_BUFFER_TRUNCATED	L"!!! Buffer Truncated !!!"

using namespace Jay;

long AsyncLogger::_logIndex;
int AsyncLogger::_logLevel;
wchar_t AsyncLogger::_logPath[MAX_PATH];
ObjectPool<AsyncLogger::LOG> AsyncLogger::_logPool(0, false);
HANDLE AsyncLogger::_hWorkerThread;
HANDLE AsyncLogger::_hExitThreadEvent;
AsyncLogger AsyncLogger::_instance;

AsyncLogger::AsyncLogger()
{
	//--------------------------------------------------------------------
	// Initial
	//--------------------------------------------------------------------	
	_logIndex = 0;
	_logLevel = LOG_LEVEL_SYSTEM;
	_logPath[0] = L'\0';
	setlocale(LC_ALL, "");

	//--------------------------------------------------------------------
	// Thread Begin
	//--------------------------------------------------------------------
	_hExitThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	_hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, NULL);
}
AsyncLogger::~AsyncLogger()
{
	//--------------------------------------------------------------------
	// Exit Thread Event
	//--------------------------------------------------------------------
	QueueUserAPC(WriteProc, _hWorkerThread, NULL);

	//--------------------------------------------------------------------
	// Thread Exit Wait
	//--------------------------------------------------------------------
	WaitForSingleObject(_hWorkerThread, INFINITE);

	//--------------------------------------------------------------------
	// Release
	//--------------------------------------------------------------------
	CloseHandle(_hExitThreadEvent);
	CloseHandle(_hWorkerThread);
}
void AsyncLogger::SetLogLevel(int logLevel)
{
	_logLevel = logLevel;
}
void AsyncLogger::SetLogPath(const wchar_t* logPath)
{
	wcscpy_s(_logPath, logPath);

	int len = wcslen(_logPath);
	if (_logPath[len - 1] == L'\\')
		_logPath[len - 1] = L'\0';
}
void AsyncLogger::WriteLog(const wchar_t * type, int logLevel, const wchar_t * fmt, ...)
{
	if (_logLevel > logLevel)
		return;

	const wchar_t* pLogLevel;
	switch (logLevel)
	{
	case LOG_LEVEL_DEBUG:
		pLogLevel = LOG_LEVEL_DEBUGW;
		break;
	case LOG_LEVEL_SYSTEM:
		pLogLevel = LOG_LEVEL_SYSTEMW;
		break;
	case LOG_LEVEL_ERROR:
		pLogLevel = LOG_LEVEL_ERRORW;
		break;
	default:
		return;
	}

	//--------------------------------------------------------------------
	// Alloc from ObjectPool
	//--------------------------------------------------------------------
	LOG* stLog = _logPool.Alloc();

	StringCchCopy(stLog->type, sizeof(stLog->type) / 2, type);
	stLog->logLevel = pLogLevel;

	va_list args;
	va_start(args, fmt);
	HRESULT ret = StringCchVPrintf(stLog->buffer, sizeof(stLog->buffer) / 2, fmt, args);
	va_end(args);

	stLog->truncated = SUCCEEDED(ret) ? false : true;

	//--------------------------------------------------------------------
	// Write Post
	//--------------------------------------------------------------------
	QueueUserAPC(WriteProc, _hWorkerThread, (ULONG_PTR)stLog);
}
void AsyncLogger::WriteHex(const wchar_t* type, int logLevel, const wchar_t* log, BYTE* byte, int byteLen)
{
	if (_logLevel > logLevel)
		return;

	const wchar_t* pLogLevel;
	switch (logLevel)
	{
	case LOG_LEVEL_DEBUG:
		pLogLevel = LOG_LEVEL_DEBUGW;
		break;
	case LOG_LEVEL_SYSTEM:
		pLogLevel = LOG_LEVEL_SYSTEMW;
		break;
	case LOG_LEVEL_ERROR:
		pLogLevel = LOG_LEVEL_ERRORW;
		break;
	default:
		return;
	}

	//--------------------------------------------------------------------
	// Alloc from ObjectPool
	//--------------------------------------------------------------------
	LOG* stLog = _logPool.Alloc();

	StringCchCopy(stLog->type, sizeof(stLog->type) / 2, type);
	stLog->logLevel = pLogLevel;

	wchar_t hex[4];
	HRESULT ret = StringCchPrintf(stLog->buffer, sizeof(stLog->buffer) / 2, L"%s - ", log);
	for (int i = 0; i < byteLen; i++)
	{
		StringCchPrintf(hex, sizeof(hex) / 2, L"%02X", byte[i]);
		ret = StringCchCat(stLog->buffer, sizeof(stLog->buffer) / 2, hex);
	}

	stLog->truncated = SUCCEEDED(ret) ? false : true;

	//--------------------------------------------------------------------
	// Write Post
	//--------------------------------------------------------------------
	QueueUserAPC(WriteProc, _hWorkerThread, (ULONG_PTR)stLog);
}
void AsyncLogger::WriteProc(ULONG_PTR dwData)
{
	LOG* stLog = (LOG*)dwData;
	if (stLog == NULL)
	{
		SetEvent(_hExitThreadEvent);
		return;
	}

	if (!ExistFile(_logPath))
	{
		if (!MakeDirectory(_logPath))
			SetLogPath(L".");
	}

	tm stTime;
	time_t timer = time(NULL);
	localtime_s(&stTime, &timer);

	wchar_t logFile[MAX_PATH];
	StringCchPrintf(logFile
		, MAX_PATH
		, L"%s\\%d%02d_%s.txt"
		, _logPath
		, stTime.tm_year + 1900
		, stTime.tm_mon + 1
		, stLog->type);

	//--------------------------------------------------------------------
	// Write File
	//--------------------------------------------------------------------
	FILE* pFile;
	if (_wfopen_s(&pFile, logFile, L"at") == 0)
	{
		fwprintf_s(pFile
			, L"[%s] [%d-%02d-%02d %02d:%02d:%02d / %s / %09d] %s\n"
			, stLog->type
			, stTime.tm_year + 1900
			, stTime.tm_mon + 1
			, stTime.tm_mday
			, stTime.tm_hour
			, stTime.tm_min
			, stTime.tm_sec
			, stLog->logLevel
			, ++_logIndex
			, stLog->buffer);

		if (stLog->truncated)
		{
			fwprintf_s(pFile
				, L"[%s] [%d-%02d-%02d %02d:%02d:%02d / %s / %09d] %s\n"
				, stLog->type
				, stTime.tm_year + 1900
				, stTime.tm_mon + 1
				, stTime.tm_mday
				, stTime.tm_hour
				, stTime.tm_min
				, stTime.tm_sec
				, LOG_LEVEL_ERRORW
				, ++_logIndex
				, LOG_ERROR_BUFFER_TRUNCATED);
		}

		fclose(pFile);
	}

	//--------------------------------------------------------------------
	// Free to ObjectPool
	//--------------------------------------------------------------------
	_logPool.Free(stLog);
}
unsigned int __stdcall AsyncLogger::WorkerThread(LPVOID lpParam)
{
	//--------------------------------------------------------------------
	// Alertable Wait 상태 진입
	//--------------------------------------------------------------------
	WaitForSingleObjectEx(_hExitThreadEvent, INFINITE, TRUE);
	return 0;
}
