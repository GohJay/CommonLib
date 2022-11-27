#ifndef __PROFILER__H_
#define __PROFILER__H_

#define PROFILE_MODE	1

#if PROFILE_MODE
#define PRO_BEGIN(tag)	ProfileBegin(tag);
#define PRO_END(tag)	ProfileEnd(tag);
#else
#define PRO_BEGIN(tag)
#define PRO_END(tag)
#endif

namespace Jay
{
	/**
	* @file		Profile.h
	* @brief	Profile Global Function
	* @details	Profile ������ ���� ���� �Լ�
	* @author   ������
	* @date		2022-5-28
	* @version  1.0.0
	**/
	void ProfileBegin(const wchar_t* tag);
	void ProfileEnd(const wchar_t* tag);
	void ProfileDataOutText(const wchar_t* filename);
	void ProfileReset(void);

	/**
	* @file		Profile.h
	* @brief	Profile Guard Class
	* @details	Profile ������ ���۰� ���Ḧ �����ϱ� ���� Guard Class
	* @author   ������
	* @date		2022-5-28
	* @version  1.0.0
	**/
	class GUARD_PROFILE
	{
	public:
		GUARD_PROFILE(const wchar_t* tag) : _tag(tag)
		{
			PRO_BEGIN(_tag)
		}
		~GUARD_PROFILE()
		{
			PRO_END(_tag)
		}
	private:
		const wchar_t* _tag;
	};
}

#endif !__PROFILE__H_