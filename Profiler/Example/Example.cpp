#include "../Profiler/Profiler.h"
#include <Windows.h>
#include <stdio.h>
#include <process.h>
#pragma comment(lib, "Profiler.lib")
#pragma comment(lib, "Winmm.lib")

#define MAX_THREAD 7

using namespace Jay;
void TestB()
{
	for (int i = 0; i < 1000; i++)
	{
		GUARD_PROFILE profile(L"B");
	}
}
void TestA()
{
	for (int i = 0; i < 1000; i++)
	{
		GUARD_PROFILE profile(L"A");
		TestB();
	}
}
unsigned int WINAPI TestThread(LPVOID lpParam)
{
	for (int i = 0; i < 10; i++)
	{
		TestA();
	}
	return 0;
}
int main()
{
	timeBeginPeriod(1);

	HANDLE hTestThread[MAX_THREAD];
	for (int i = 0; i < MAX_THREAD; i++)
		hTestThread[i] = (HANDLE)_beginthreadex(NULL, 0, TestThread, NULL, CREATE_SUSPENDED, NULL);

	for (int i = 0; i < MAX_THREAD; i++)
		ResumeThread(hTestThread[i]);

	WaitForMultipleObjects(MAX_THREAD, hTestThread, TRUE, INFINITE);
	for (int i = 0; i < MAX_THREAD; i++)
		CloseHandle(hTestThread[i]);

	ProfileDataOutText(L"Profile.csv");

	timeEndPeriod(1);
    return 0;
}
