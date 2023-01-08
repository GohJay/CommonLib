#ifndef __SOCKET__H_
#define __SOCKET__H_
#include <Windows.h>

namespace Jay
{
	bool Startup();
	bool Cleanup();
	int DomainToIP(const wchar_t* domain, IN_ADDR* address, int addressCount);
	bool IPToString(IN_ADDR* address, wchar_t* stringBuf, size_t stringBufSize);
}

#endif
