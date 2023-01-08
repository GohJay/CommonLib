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
#define LOG_LEVEL_UNKNOWNW			L"UNKNOWN"
#define LOG_ERROR_BUFFER_TRUNCATED	L"!!! Buffer Truncated !!!"

using namespace Jay;

DWORD AsyncLogger::_logIndex;
int AsyncLogger::_logLevel;
wchar_t AsyncLogger::_logPath[MAX_PATH];
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

	LOG* pLog = (LOG*)malloc(sizeof(LOG));

	StringCchCopy(pLog->type, sizeof(pLog->type) / 2, type);
	
	pLog->logLevel = logLevel;

	va_list args;
	va_start(args, fmt);
	HRESULT ret = StringCchVPrintf(pLog->buffer, sizeof(pLog->buffer) / 2, fmt, args);
	va_end(args);

	pLog->truncated = FAILED(ret);

	//--------------------------------------------------------------------
	// Write Post
	//--------------------------------------------------------------------
	QueueUserAPC(WriteProc, _hWorkerThread, (ULONG_PTR)pLog);
}
void AsyncLogger::WriteHex(const wchar_t* type, int logLevel, const wchar_t* log, BYTE* byte, int byteLen)
{
	if (_logLevel > logLevel)
		return;

	LOG* pLog = (LOG*)malloc(sizeof(LOG));

	StringCchCopy(pLog->type, sizeof(pLog->type) / 2, type);	
	
	pLog->logLevel = logLevel;
	
	wchar_t hex[4];
	HRESULT ret = StringCchPrintf(pLog->buffer, sizeof(pLog->buffer) / 2, L"%s - ", log);
	for (int i = 0; i < byteLen && SUCCEEDED(ret); i++)
	{
		StringCchPrintf(hex, sizeof(hex) / 2, L"%02X", byte[i]);
		ret = StringCchCat(pLog->buffer, sizeof(pLog->buffer) / 2, hex);
	}
	
	pLog->truncated = FAILED(ret);

	//--------------------------------------------------------------------
	// Write Post
	//--------------------------------------------------------------------
	QueueUserAPC(WriteProc, _hWorkerThread, (ULONG_PTR)pLog);
}
void AsyncLogger::WriteProc(ULONG_PTR dwData)
{
	LOG* pLog = (LOG*)dwData;
	if (pLog == NULL)
	{
		SetEvent(_hExitThreadEvent);
		return;
	}

	const wchar_t* pLogLevel;
	switch (pLog->logLevel)
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
		pLogLevel = LOG_LEVEL_UNKNOWNW;
		break;
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
		, pLog->type);

	//--------------------------------------------------------------------
	// Write File
	//--------------------------------------------------------------------
	FILE* pFile;
	if (_wfopen_s(&pFile, logFile, L"at") == 0)
	{
		fwprintf_s(pFile
			, L"[%s] [%d-%02d-%02d %02d:%02d:%02d / %s / %09d] %s\n"
			, pLog->type
			, stTime.tm_year + 1900
			, stTime.tm_mon + 1
			, stTime.tm_mday
			, stTime.tm_hour
			, stTime.tm_min
			, stTime.tm_sec
			, pLogLevel
			, ++_logIndex
			, pLog->buffer);

		if (pLog->truncated)
		{
			fwprintf_s(pFile
				, L"[%s] [%d-%02d-%02d %02d:%02d:%02d / %s / %09d] %s\n"
				, pLog->type
				, stTime.tm_year + 1900
				, stTime.tm_mon + 1
				, stTime.tm_mday
				, stTime.tm_hour
				, stTime.tm_min
				, stTime.tm_sec
				, LOG_LEVEL_ERRORW
				, _logIndex
				, LOG_ERROR_BUFFER_TRUNCATED);
		}

		fclose(pFile);
	}

	free(pLog);
}
unsigned int __stdcall AsyncLogger::WorkerThread(LPVOID lpParam)
{
	DWORD ret;
	do
	{
		//--------------------------------------------------------------------
		// Alertable Wait 상태 진입
		//--------------------------------------------------------------------
		ret = WaitForSingleObjectEx(_hExitThreadEvent, INFINITE, TRUE);
	} while (ret == WAIT_IO_COMPLETION);
	return 0;
}
