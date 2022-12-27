#include "MemoryLogger.h"
#include <stdio.h>

#define BUF_SIZE	32

using namespace Jay;
LONG MemoryLogger::_index = -1;
char* MemoryLogger::_buffer;
MemoryLogger MemoryLogger::_instance;
MemoryLogger::MemoryLogger()
{
	_buffer = new char[INT_MAX];
}
MemoryLogger::~MemoryLogger()
{
	delete[] _buffer;
}
int MemoryLogger::WriteLog(const char* buffer, int size)
{
	if (size > BUF_SIZE)
		size = BUF_SIZE;

	LONG idx = InterlockedIncrement(&_index);
	if (idx <= INT_MAX / BUF_SIZE)
	{
		char* offset = _buffer + (idx * BUF_SIZE);
		memmove(offset, buffer, size);
	}
	return idx;
}
bool MemoryLogger::SaveFile(const wchar_t* filename)
{
	FILE* pFile;
	if (_wfopen_s(&pFile, filename, L"wb") != 0)
		return false;

	LONG idx = InterlockedIncrement(&_index);
	int bufferSize = idx * BUF_SIZE;
	fwrite(_buffer, bufferSize, 1, pFile);
	fclose(pFile);
	return true;
}