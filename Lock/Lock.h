#ifndef  __LOCK__H_
#define  __LOCK__H_
#include <Windows.h>

namespace Jay
{
	enum LOCK_TYPE
	{
		EXCLUSIVE = 0,
		SHARED
	};

	class SRWLock
	{
		/**
		* @file		Lock.h
		* @brief	SRWLOCK Wrapping Class
		* @details  Slim Reader-Writer Lock Class
		* @author	고재현
		* @date		2023-01-21
		* @version	1.0.1
		**/
	public:
		SRWLock();
		~SRWLock();
	public:
		void Lock(LOCK_TYPE type = EXCLUSIVE);
		void UnLock(LOCK_TYPE type = EXCLUSIVE);
	private:
		SRWLOCK _lock;
	};

	class CSLock
	{
		/**
		* @file		Lock.h
		* @brief	CriticalSection Wrapping Class
		* @details	CriticalSection Lock Class
		* @author	고재현
		* @date		2023-01-21
		* @version	1.0.1
		**/
	public:
		CSLock();
		~CSLock();
	public:
		void Lock(LOCK_TYPE type = EXCLUSIVE);
		void UnLock(LOCK_TYPE type = EXCLUSIVE);
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
		* @date		2023-01-21
		* @version	1.0.1
		**/
	public:
		AddressLock();
		~AddressLock();
	public:
		void Lock(LOCK_TYPE type = EXCLUSIVE);
		void UnLock(LOCK_TYPE type = EXCLUSIVE);
	private:
		volatile long _lock;
	};

	class SpinLock
	{
		/**
		* @file		Lock.h
		* @brief	SpinLock Class
		* @details	Busy-wait Lock Class
		* @author   고재현
		* @date		2023-01-21
		* @version	1.0.1
		**/
	public:
		SpinLock();
		~SpinLock();
	public:
		void Lock(LOCK_TYPE type = EXCLUSIVE);
		void UnLock(LOCK_TYPE type = EXCLUSIVE);
	private:
		volatile long _lock;
	};

	template<typename T>
	class GUARD_LOCK
	{
		/**
		* @file		Lock.h
		* @brief	Lock 객체에 대한 Guard Class
		* @details	Lock 객체 사용으로 발생할 수 있는 Dead-lock을 방지하기 위한 Guard Class
		* @author   고재현
		* @date		2023-01-21
		* @version	1.0.1
		**/
	public:
		GUARD_LOCK(T* lock, LOCK_TYPE type = EXCLUSIVE) 
			: _lock(lock), _type(type)
		{
			_lock->Lock(_type);
		}
		~GUARD_LOCK()
		{
			_lock->UnLock(_type);
		}
	private:
		T* _lock;
		LOCK_TYPE _type;
	};
}

#endif
