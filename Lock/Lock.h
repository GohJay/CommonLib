#ifndef  __LOCK__H_
#define  __LOCK__H_
#include <Windows.h>

namespace Jay
{
	class CSLock
	{
		/**
		* @file		Lock.h
		* @brief	CriticalSection Wrapping Class
		* @details	CriticalSection Lock Class
		* @author	고재현
		* @date		2022-11-03
		* @version	1.0.0
		**/
	public:
		CSLock();
		~CSLock();
	public:
		void Lock();
		void UnLock();
	private:
		CRITICAL_SECTION _lock;
	};

	class AddressLock
	{
		/**
		* @file		Lock.h
		* @brief	AddressLock Class
		* @details	WaitOnAddress Lock Class
		* @author   고재현
		* @date		2022-10-25
		* @version  1.0.0
		**/
	public:
		AddressLock();
		~AddressLock();
	public:
		void Lock();
		void UnLock();
	private:
		volatile long _lock;
	};

	class SpinLock
	{
		/**
		* @file		Lock.h
		* @brief	SpinLock Class
		* @details	Busy-wait Lock Class (non context switch)
		* @author   고재현
		* @date		2022-10-13
		* @version  1.0.0
		**/
	public:
		SpinLock();
		~SpinLock();
	public:
		void Lock();
		void UnLock();
	private:
		volatile long _lock;
	};

	template<typename T>
	class GUARDLOCK
	{
		/**
		* @file		Lock.h
		* @brief	Lock 객체에 대한 Guard Class
		* @details	Lock 객체 사용으로 발생할 수 있는 Dead-lock을 방지하기 위한 Guard Class
		* @author   고재현
		* @date		2022-10-13
		* @version  1.0.0
		**/
	public:
		GUARDLOCK(T* lock) : _lock(lock)
		{
			this->_lock->Lock();
		}
		~GUARDLOCK()
		{
			this->_lock->UnLock();
		}
	private:
		T* _lock;
	};
}

#endif
