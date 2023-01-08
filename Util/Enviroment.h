#ifndef __ENVIROMENT__H_
#define __ENVIROMENT__H_
#include <Windows.h>

namespace Jay
{
	inline
		size_t GetUserAddressRange()
	{
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		return (size_t)systemInfo.lpMaximumApplicationAddress;
	}
	inline
		size_t GetKernelAddressRange()
	{
		return ~GetUserAddressRange();
	}
}

#endif
