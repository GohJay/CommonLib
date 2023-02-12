#ifndef __STRING_UTIL__H_
#define __STRING_UTIL__H_
#include <string>

namespace Jay
{
	inline void MultiByteToUnicode() = delete;
	inline void UnicodeToMultiByte(const wchar_t* unicode, char* multibyte)
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_UTF8, 0, unicode, -1, multibyte, len, NULL, NULL);
	}
	inline void MultiByteToWString(const char* multibyte, std::wstring& wstr)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, multibyte, -1, NULL, 0);
		wstr.reserve(len);
		MultiByteToWideChar(CP_ACP, 0, multibyte, -1, &wstr[0], len);
	}
	inline void UnicodeToString(const wchar_t* unicode, std::string& str)
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
		str.reserve(len);
		WideCharToMultiByte(CP_UTF8, 0, unicode, -1, &str[0], len, NULL, NULL);
	}
}

#endif
