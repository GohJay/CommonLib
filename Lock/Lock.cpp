#include "Lock.h"
#include <stdio.h>
#pragma comment(lib, "Synchronization.lib")

using namespace Jay;

SRWLock::SRWLock()
{
	InitializeSRWLock(&_lock);
}
SRWLock::~SRWLock()
{
}
void SRWLock::Lock(LOCK_TYPE type)
{
	switch (type)
	{
	case EXCLUSIVE:
		AcquireSRWLockExclusive(&_lock);
		break;
	case SHARED:
		AcquireSRWLockShared(&_lock);
		break;
	default:
		break;
	}
}
void SRWLock::UnLock(LOCK_TYPE type)
{
	switch (type)
	{
	case EXCLUSIVE:
		ReleaseSRWLockExclusive(&_lock);
		break;
	case SHARED:
		ReleaseSRWLockShared(&_lock);
		break;
	default:
		break;
	}
}

CSLock::CSLock()
{
	InitializeCriticalSection(&_lock);
}
CSLock::~CSLock()
{
	DeleteCriticalSection(&_lock);
}
void CSLock::Lock(LOCK_TYPE type)
{
	EnterCriticalSection(&_lock);
}
void CSLock::UnLock(LOCK_TYPE type)
{
	LeaveCriticalSection(&_lock);
}

AddressLock::AddressLock() : _lock(FALSE)
{
}
AddressLock::~AddressLock()
{
}
void AddressLock::Lock(LOCK_TYPE type)
{
	long compare = TRUE;
	while (InterlockedExchange(&_lock, TRUE) != FALSE)
	{
		WaitOnAddress(&_lock, &compare, sizeof(long), INFINITE);
	}
}
void AddressLock::UnLock(LOCK_TYPE type)
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
void SpinLock::Lock(LOCK_TYPE type)
{
	while (InterlockedExchange(&_lock, TRUE) != FALSE)
	{
		YieldProcessor();
	}
}
void SpinLock::UnLock(LOCK_TYPE type)
{
	_lock = FALSE;
}
