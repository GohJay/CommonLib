#ifndef __EXCEPTION__H_
#define __EXCEPTION__H_
#include <exception>

namespace Jay
{
	class Exception : public std::exception
	{
	public:
		Exception(int errCode, const char* errMessage) 
			: _errCode(errCode), std::exception(errMessage)
		{
		}
		~Exception()
		{
		}
	public:
		int GetLastError()
		{
			return _errCode;
		}
	private:
		int _errCode;
	};
}

#endif
