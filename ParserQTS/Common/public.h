#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#ifdef _WIN32  //Windows
#define Windows_Version
#endif


typedef struct{
	int iSec;     /* seconds after the minute - [0,59] */
	int iMin;     /* minutes after the hour - [0,59] */
	int iHour;    /* hours since midnight - [0,23] */

	int iDay;    /* day of the month - [1,31] */
	int iMon;     /* months since January - [1,12] */
	int iYear;    /* year*/

	int iWday;    /* days since Sunday - [0,6] */

	int iMillisec;  /* Milliseconds after the second - [0,999] */
}UTIL_Time, *UTIL_TimePtr;

#ifndef __cplusplus // C ���Ի���û��bool
#define bool int
#define false 0
#define true 1
#endif


bool PUTIL_GetLocalTime(UTIL_TimePtr pTime);

/// ��SYSTEMTIMEת��Ϊ����ʱ���ʽYYYYMMDDHHMMSSMMM
long long PUTIL_SystemTimeToDateTime(UTIL_TimePtr stTime);


bool GetModulePath(char* sPath, int nPathLen);

#endif