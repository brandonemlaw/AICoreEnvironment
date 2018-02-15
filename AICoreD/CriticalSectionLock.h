#pragma once
#include <windows.h>


class CriticalSectionLock
{

public:
	CriticalSectionLock(CRITICAL_SECTION& cs)
		: criticalSection(cs)
	{
		EnterCriticalSection(&criticalSection);
	}
	~CriticalSectionLock()
	{
		LeaveCriticalSection(&criticalSection);
	}
private:
	CRITICAL_SECTION&  criticalSection;
};