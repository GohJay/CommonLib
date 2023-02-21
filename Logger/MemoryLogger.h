#ifndef __MEMORY_LOGGER__H_
#define __MEMORY_LOGGER__H_
#include <Windows.h>

namespace Jay
{
	class MemoryLogger
	{
		/**
		* @file		MemoryLogger.h
		* @brief	Memory Logger Class
		* @details	�޸� �α� ��¿� Ŭ����
		* @author	������
		* @date		2022-12-13
		* @version	1.0.0
		**/
	private:
		MemoryLogger();
		~MemoryLogger();
	public:
		static int WriteLog(const char* buffer, int size);
		static bool SaveFile(const wchar_t* filename);
	private:
		static LONG _index;
		static char* _buffer;
		static MemoryLogger _instance;
	};
}

#endif
