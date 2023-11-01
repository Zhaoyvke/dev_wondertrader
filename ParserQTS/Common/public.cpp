#include "public.h"

#ifdef Windows_Version
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <shlwapi.h>
#else
#include <sys/timeb.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#endif

bool PUTIL_GetLocalTime(UTIL_TimePtr pTime)
{
	if (NULL == pTime)
	{
		return false;
	}

#ifdef Windows_Version
	SYSTEMTIME stSysTime;
	GetLocalTime(&stSysTime);
	pTime->iYear = stSysTime.wYear;
	pTime->iMon = stSysTime.wMonth;
	pTime->iDay = stSysTime.wDay;
	pTime->iHour = stSysTime.wHour;
	pTime->iMin = stSysTime.wMinute;
	pTime->iSec = stSysTime.wSecond;
	pTime->iWday = stSysTime.wDayOfWeek;
	pTime->iMillisec = stSysTime.wMilliseconds;
#else
	struct timeb stTime = {0};
	if (0 != ftime(&stTime))
	{
		return false;
	}
	struct tm stNow = {0};
	localtime_r(&stTime.time, &stNow); 

	pTime->iYear = stNow.tm_year + 1900;
	pTime->iMon = stNow.tm_mon + 1;
	pTime->iDay = stNow.tm_mday;
	pTime->iHour = stNow.tm_hour;
	pTime->iMin = stNow.tm_min;
	pTime->iSec = stNow.tm_sec;
	pTime->iWday = stNow.tm_wday;
	pTime->iMillisec = stTime.millitm;
#endif
	return true;
}

/// 将SYSTEMTIME转换为日期时间格式YYYYMMDDHHMMSSMMM
long long PUTIL_SystemTimeToDateTime(UTIL_TimePtr stTime)
{
	return (((((stTime->iYear * 100ll + stTime->iMon) * 100 + stTime->iDay) * 100 + stTime->iHour) * 100 + stTime->iMin) * 100 + stTime->iSec) * 1000 + stTime->iMillisec;
}

/// 获取当前程序的路径，不带最后的目录\或/
/// @param  sPath      路径，由调用者申请和释放空间
/// @param  nPathLen   路径长度
/// @return bool
bool GetModulePath(char* sPath, int nPathLen)
{
#ifdef Windows_Version
    DWORD nwLen = GetModuleFileNameA(NULL, sPath, nPathLen);
    if (0 == nwLen)
    {
        return false;
    }
    PathRemoveFileSpecA(sPath);
    return true;
#else
    if (NULL == getcwd(sPath, nPathLen))
    {
        return false;
    }
    int nLen = strlen(sPath);
    if (nLen >= nPathLen)
    {
        return false;
    }
    return true;
#endif
}