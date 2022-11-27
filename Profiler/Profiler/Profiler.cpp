#include "Profiler.h"
#include <Windows.h>
#include <stdio.h>

class ProfileManager
{
private:
	enum PROFILE_FLAG
	{
		INIT = 0,
		RESET = 1,
		USING = 2
	};
	struct Profile
	{
		LONG lFlag;					// ���������� ��� ����. (�迭�ÿ���)
		__int64 iTotalTime;			// ��ü ���ð� ī���� Time. (��½� ȣ��ȸ���� ������ ��� ����)
		__int64 iMin;				// �ּ� ���ð� ī���� Time. (�ʴ����� ����Ͽ� ����)
		__int64 iMax;				// �ִ� ���ð� ī���� Time. (�ʴ����� ����Ͽ� ����)
		__int64 iCall;				// ���� ȣ�� Ƚ��.
		LARGE_INTEGER lStartTime;	// �������� ���� �ð�.
		WCHAR szName[64];			// �������� �̸�.
	};
public:
	ProfileManager();
	~ProfileManager();
private:
	inline int FindProfileIndex(const wchar_t* tag);
	inline int NextProfileIndex(void);
	friend void Jay::ProfileBegin(const wchar_t* tag);
	friend void Jay::ProfileEnd(const wchar_t* tag);
	friend void Jay::ProfileDataOutText(const wchar_t* filename);
	friend void Jay::ProfileReset(void);
private:
	Profile _profileArray[64];
	INT _profileCnt;
	LONGLONG _microsecond;
};
ProfileManager::ProfileManager()
{
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	_microsecond = Freq.QuadPart / (1000 * 1000);
}
ProfileManager::~ProfileManager()
{
}
inline
	int ProfileManager::NextProfileIndex()
{
	int index;
	if (_profileCnt < sizeof(_profileArray))
		index = _profileCnt++;
	else
		index = -1;
	return index;
}
inline
	int ProfileManager::FindProfileIndex(const wchar_t * tag)
{
	int index = 0;
	for (; index < _profileCnt; index++)
	{
		if (wcscmp(_profileArray[index].szName, tag) == 0)
			return index;
	}
	return -1;
}

ProfileManager g_ProfileManager;
void Jay::ProfileBegin(const wchar_t* tag)
{
	int index = g_ProfileManager.FindProfileIndex(tag);
	if (index == -1)
	{
		index = g_ProfileManager.NextProfileIndex();
		if (index == -1)
			return;
	}

	switch (g_ProfileManager._profileArray[index].lFlag)
	{
	case ProfileManager::INIT:
		wcscpy_s(g_ProfileManager._profileArray[index].szName, tag);
	case ProfileManager::RESET:
		g_ProfileManager._profileArray[index].iTotalTime = 0;
		g_ProfileManager._profileArray[index].iMax = 0;
		g_ProfileManager._profileArray[index].iMin = 0;
		g_ProfileManager._profileArray[index].iCall = 0;
		g_ProfileManager._profileArray[index].lFlag = ProfileManager::USING;
		break;
	default:
		break;
	}
	QueryPerformanceCounter(&g_ProfileManager._profileArray[index].lStartTime);
}
void Jay::ProfileEnd(const wchar_t* tag)
{
	int index = g_ProfileManager.FindProfileIndex(tag);
	if (index == -1)
		return;

	LARGE_INTEGER lEndTime;
	QueryPerformanceCounter(&lEndTime);

	__int64 between = lEndTime.QuadPart - g_ProfileManager._profileArray[index].lStartTime.QuadPart;
	if (g_ProfileManager._profileArray[index].iMin > between || g_ProfileManager._profileArray[index].iMin == 0)
		g_ProfileManager._profileArray[index].iMin = between;
	if (g_ProfileManager._profileArray[index].iMax < between)
		g_ProfileManager._profileArray[index].iMax = between;
	g_ProfileManager._profileArray[index].iTotalTime += between;
	g_ProfileManager._profileArray[index].iCall++;
}
void Jay::ProfileDataOutText(const wchar_t* filename)
{
	FILE* pFile;
	if (_wfopen_s(&pFile, filename, L"at") != 0) return;
	fwprintf_s(pFile, L"Name,Average,Max,Min,Call\n");
	for (int index = 0; index < g_ProfileManager._profileCnt; index++)
	{
		if (g_ProfileManager._profileArray[index].lFlag != ProfileManager::USING || g_ProfileManager._profileArray[index].iCall <= 2)
			continue;
		
		const wchar_t* name = g_ProfileManager._profileArray[index].szName;
		double max = (double)g_ProfileManager._profileArray[index].iMax / g_ProfileManager._microsecond;
		double min = (double)g_ProfileManager._profileArray[index].iMin / g_ProfileManager._microsecond;
		long long call = g_ProfileManager._profileArray[index].iCall - 2;
		double totaltime = g_ProfileManager._profileArray[index].iTotalTime - g_ProfileManager._profileArray[index].iMax - g_ProfileManager._profileArray[index].iMin;
		double average = (totaltime / call) / g_ProfileManager._microsecond;
		fwprintf_s(pFile, L"%ls,%.4lfus,%.4lfus,%.4lfus,%I64d\n", name, average, max, min, call);
	}
	fwprintf_s(pFile, L"\n\n");
	fclose(pFile);
}
void Jay::ProfileReset(void)
{
	int index = 0;
	for (; index < g_ProfileManager._profileCnt; index++)
	{
		g_ProfileManager._profileArray[index].lFlag = ProfileManager::RESET;
	}
}
