#ifndef __STRING_UTIL__H_
#define __STRING_UTIL__H_
#include <stringapiset.h>

namespace Jay
{
	inline int M2W(const char* multiByteStr, wchar_t* wideCharStr, int wideCharLen)
	{
		return MultiByteToWideChar(CP_ACP, 0, multiByteStr, -1, wideCharStr, wideCharLen);
	}
	inline int W2M(const wchar_t* wideCharStr, char* multiByteStr, int multiByteLen)
	{
		return WideCharToMultiByte(CP_UTF8, 0, wideCharStr, -1, multiByteStr, multiByteLen, NULL, NULL);
	}
}

#endif
