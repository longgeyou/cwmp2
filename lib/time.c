



#include <../lib/include/time.h>
#include <stdio.h>

#if 0
//原型参考
struct tm
{
   int tm_sec;         /* 秒，范围从 0 到 59      */
   int tm_min;         /* 分，范围从 0 到 59      */
   int tm_hour;        /* 小时，范围从 0 到 23     */
   int tm_mday;        /* 一月中的第几天，范围从 1 到 31    */
   int tm_mon;         /* 月，范围从 0 到 11(注意)  */
   int tm_year;        /* 自 1900 年起的年数      */
   int tm_wday;        /* 一周中的第几天，范围从 0 到 6 */
   int tm_yday;        /* 一年中的第几天，范围从 0 到 365   */
   int tm_isdst;       /* 夏令时               */
};
#endif


void get_time(char *out)
{
    struct tm *tp;
    time_t t;
	time(&t);
	tp = localtime(&t);
	sprintf(out, "%d/%d/%d %d:%d", 1900 + tp->tm_year, tp->tm_mon, 
	        tp->tm_mday, tp->tm_hour, tp->tm_min);
}

struct tm get_time_struct(void)
{
    struct tm *tp;
    time_t t;
	time(&t);
	tp = localtime(&t);
	return *tp;
}

