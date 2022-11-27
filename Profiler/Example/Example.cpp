#include "../Profiler/Profiler.h"
#include <Windows.h>
#include <stdio.h>
#pragma comment(lib, "Profiler.lib")

using namespace Jay;
void TestC()
{
	for (int i = 0; i < 1000; i++)
	{
		GUARD_PROFILE profile(L"C");
		OutputDebugStringA("test\n");
	}
}
void TestB()
{
	for (int i = 0; i < 100; i++)
	{
		GUARD_PROFILE profile(L"B");
		TestC();
	}
}
void TestA()
{
	for (int i = 0; i < 10; i++)
	{
		GUARD_PROFILE profile(L"A");
		TestB();
	}
}
int main()
{
	TestA();
	/*profilereset();
	testa();*/
	ProfileDataOutText(L"Profile.csv");
    return 0;
}
