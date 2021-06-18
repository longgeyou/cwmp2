



#include <../lib/include/time.h>
#include <stdio.h>

#if 0
//ԭ�Ͳο�
struct tm
{
   int tm_sec;         /* �룬��Χ�� 0 �� 59      */
   int tm_min;         /* �֣���Χ�� 0 �� 59      */
   int tm_hour;        /* Сʱ����Χ�� 0 �� 23     */
   int tm_mday;        /* һ���еĵڼ��죬��Χ�� 1 �� 31    */
   int tm_mon;         /* �£���Χ�� 0 �� 11(ע��)  */
   int tm_year;        /* �� 1900 ���������      */
   int tm_wday;        /* һ���еĵڼ��죬��Χ�� 0 �� 6 */
   int tm_yday;        /* һ���еĵڼ��죬��Χ�� 0 �� 365   */
   int tm_isdst;       /* ����ʱ               */
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

