#ifndef __SOCKET__H_
#define __SOCKET__H_
#include <Windows.h>

#define MAX_ADDR	8

namespace Jay
{
	bool Startup();
	bool Cleanup();
	int DomainToIP(const wchar_t* domain, IN_ADDR* address);
	bool IPToString(IN_ADDR* address, wchar_t* stringBuf, size_t stringBufSize);
}

#endif
