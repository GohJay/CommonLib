#ifndef __PROFILER__H_
#define __PROFILER__H_

#define PROFILE_MODE	1

#if PROFILE_MODE
#define PRO_BEGIN(tag)	Jay::ProfileBegin(tag);
#define PRO_END(tag)	Jay::ProfileEnd(tag);
#else
#define PRO_BEGIN(tag)
#define PRO_END(tag)
#endif

namespace Jay
{
	/**
	* @file		Profile.h
	* @brief	Profile Global Function
	* @details	Profile 측정을 위한 전역 함수 (thread-safe)
	* @author	고재현
	* @date		2022-12-27
	* @version	1.0.1
	**/
	void ProfileBegin(const wchar_t* tag);
	void ProfileEnd(const wchar_t* tag);
	void ProfileDataOutText(const wchar_t* filename);
	void ProfileReset(void);

	/**
	* @file		Profile.h
	* @brief	Profile Guard Class
	* @details	Profile 측정의 시작과 종료를 보장하기 위한 Guard Class
	* @author	고재현
	* @date		2022-5-28
	* @version	1.0.0
	**/
	class ProfileGuard
	{
	public:
		ProfileGuard(const wchar_t* tag) : _tag(tag)
		{
			PRO_BEGIN(_tag)
		}
		~ProfileGuard()
		{
			PRO_END(_tag)
		}
	private:
		const wchar_t* _tag;
	};
}

#endif !__PROFILE__H_
