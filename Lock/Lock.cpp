#include "Lock.h"
#include <stdio.h>
#pragma comment(lib, "Synchronization.lib")

using namespace Jay;
CSLock::CSLock()
{
	InitializeCriticalSection(&_lock);
}
CSLock::~CSLock()
{
	DeleteCriticalSection(&_lock);
}
void CSLock::Lock()
{
	EnterCriticalSection(&_lock);
}
void CSLock::UnLock()
{
	LeaveCriticalSection(&_lock);
}

AddressLock::AddressLock() : _lock(FALSE)
{
}
AddressLock::~AddressLock()
{
}
void AddressLock::Lock()
{
	long compare = TRUE;
	while (InterlockedExchange(&_lock, TRUE) != FALSE)
	{
		WaitOnAddress(&_lock, &compare, sizeof(long), INFINITE);
	}
}
void AddressLock::UnLock()
{
	_lock = FALSE;
	WakeByAddressSingle((void*)&_lock);
}

SpinLock::SpinLock() : _lock(FALSE)
{
}
SpinLock::~SpinLock()
{
}
void SpinLock::Lock()
{
	while (InterlockedExchange(&_lock, TRUE) != FALSE)
	{
		YieldProcessor();
	}
}
void SpinLock::UnLock()
{
	_lock = FALSE;
}
