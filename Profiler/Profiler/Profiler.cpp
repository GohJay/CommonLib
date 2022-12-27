#include "Profiler.h"
#include <Windows.h>
#include <stdio.h>
#include <strsafe.h>
#include <processthreadsapi.h>

#define MAX_NAME      64
#define MAX_PROFILE   50
#define MAX_THREAD    20

enum PROFILE_FLAG
{
    INIT = 0,
    RESET,
    USING
};
struct PROFILE
{
    LONG lFlag;                 // 프로파일의 사용 여부.
    DWORD64 iTotalTime;         // 전체 사용시간 카운터 Time. (출력시 호출회수로 나누어 평균 구함)
    DWORD64 iMin;               // 최소 사용시간 카운터 Time. (초단위로 계산하여 저장)
    DWORD64 iMax;               // 최대 사용시간 카운터 Time. (초단위로 계산하여 저장)
    DWORD64 iCall;              // 누적 호출 횟수.
    LARGE_INTEGER lStartTime;   // 프로파일 실행 시간.
    WCHAR szName[MAX_NAME];     // 프로파일 이름.
};
class ProfileManager
{
public:
    ProfileManager();
    ~ProfileManager();
private:
    inline PROFILE* GetCurrentProfileArray();
    inline bool FindIndex(PROFILE* profileArray, const wchar_t* tag, int& ref);
    inline bool NextIndex(PROFILE* profileArray, int& ref);
    friend void Jay::ProfileBegin(const wchar_t* tag);
    friend void Jay::ProfileEnd(const wchar_t* tag);
    friend void Jay::ProfileDataOutText(const wchar_t* filename);
    friend void Jay::ProfileReset(void);
private:
    PROFILE _profileTable[MAX_THREAD][MAX_PROFILE];
    DWORD _threadIdTable[MAX_THREAD];
    DWORD _threadCount;
    DWORD _tlsProfile;
    LARGE_INTEGER _freq;
};

ProfileManager::ProfileManager()
{
    ZeroMemory(_profileTable, sizeof(_profileTable));
    ZeroMemory(_threadIdTable, sizeof(_threadIdTable));
    _threadCount = 0;
    _tlsProfile = TlsAlloc();
    QueryPerformanceFrequency(&_freq);
}
ProfileManager::~ProfileManager()
{
    TlsFree(_tlsProfile);
}
PROFILE* ProfileManager::GetCurrentProfileArray()
{
    PROFILE* profileArray = (PROFILE*)TlsGetValue(_tlsProfile);
    if (profileArray == NULL)
    {
        int count = InterlockedIncrement(&_threadCount) - 1;
        if (count >= MAX_THREAD)
            return NULL;

        _threadIdTable[count] = GetCurrentThreadId();
        TlsSetValue(_tlsProfile, _profileTable[count]);
        profileArray = _profileTable[count];
    }
    return profileArray;
}
bool ProfileManager::FindIndex(PROFILE* profileArray, const wchar_t* tag, int& ref)
{
    for (int index = 0; index < MAX_PROFILE; index++)
    {
        if (wcscmp(profileArray[index].szName, tag) == 0)
        {
            ref = index;
            return true;
        }
    }
    return false;
}
bool ProfileManager::NextIndex(PROFILE* profileArray, int& ref)
{
    for (int index = 0; index < MAX_PROFILE; index++)
    {
        if (profileArray[index].lFlag == PROFILE_FLAG::INIT)
        {
            ref = index;
            return true;
        }
    }
    return false;
}

ProfileManager g_ProfileManager;
void Jay::ProfileBegin(const wchar_t* tag)
{
    PROFILE* profileArray = g_ProfileManager.GetCurrentProfileArray();
    if (profileArray == NULL)
        return;

    int index;
    if (!g_ProfileManager.FindIndex(profileArray, tag, index))
    {
        if (!g_ProfileManager.NextIndex(profileArray, index))
            return;
    }

    switch (profileArray[index].lFlag)
    {
    case PROFILE_FLAG::INIT:
        StringCchCopy(profileArray[index].szName, MAX_NAME, tag);
    case PROFILE_FLAG::RESET:
        profileArray[index].iTotalTime = 0;
        profileArray[index].iMax = 0;
        profileArray[index].iMin = 0;
        profileArray[index].iCall = 0;
        profileArray[index].lFlag = PROFILE_FLAG::USING;
    default:
        break;
    }

    QueryPerformanceCounter(&profileArray[index].lStartTime);
}
void Jay::ProfileEnd(const wchar_t* tag)
{
    PROFILE* profileArray = g_ProfileManager.GetCurrentProfileArray();
    if (profileArray == NULL)
        return;

    int index;
    if (!g_ProfileManager.FindIndex(profileArray, tag, index))
        return;

    LARGE_INTEGER lEndTime;
    QueryPerformanceCounter(&lEndTime);

    DWORD64 between = lEndTime.QuadPart - profileArray[index].lStartTime.QuadPart;
    if (profileArray[index].iMin > between || profileArray[index].iMin == 0)
        profileArray[index].iMin = between;
    if (profileArray[index].iMax < between)
        profileArray[index].iMax = between;
    profileArray[index].iTotalTime += between;
    profileArray[index].iCall++;
}
void Jay::ProfileDataOutText(const wchar_t* filename)
{
    FILE* pFile;
    if (_wfopen_s(&pFile, filename, L"wt") != 0)
        return;

    LONGLONG microsecond = g_ProfileManager._freq.QuadPart / (1000 * 1000);
    for (int count = 0; count < g_ProfileManager._threadCount && count < MAX_THREAD; count++)
    {
        fwprintf_s(pFile, L"TID,Name,Average,Max,Min,Call\n");

        PROFILE* profileArray = g_ProfileManager._profileTable[count];
        DWORD threadId = g_ProfileManager._threadIdTable[count];
        for (int index = 0; index < MAX_PROFILE; index++)
        {
            if (profileArray[index].lFlag != PROFILE_FLAG::USING || profileArray[index].iCall <= 2)
                continue;

            const wchar_t* name = profileArray[index].szName;
            double max = (double)profileArray[index].iMax / microsecond;
            double min = (double)profileArray[index].iMin / microsecond;
            unsigned long long call = profileArray[index].iCall - 2;
            double totaltime = profileArray[index].iTotalTime - profileArray[index].iMax - profileArray[index].iMin;
            double average = (totaltime / call) / microsecond;

            fwprintf_s(pFile, L"0x%04X,%ls,%.4lfus,%.4lfus,%.4lfus,%I64d\n", threadId, name, average, max, min, call);
        }

        fwprintf_s(pFile, L"\n\n");
    }
    
    fclose(pFile);
}
void Jay::ProfileReset(void)
{
    for (int count = 0; count < g_ProfileManager._threadCount && count < MAX_THREAD; count++)
    {
        PROFILE* profileArray = g_ProfileManager._profileTable[count];
        for (int index = 0; index < MAX_PROFILE; index++)
        {
            if (profileArray[index].lFlag == PROFILE_FLAG::USING)
                profileArray[index].lFlag = PROFILE_FLAG::RESET;
        }
    }
}
