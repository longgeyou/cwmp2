


#include <../lib/include/common.h>

//把字符串分割为词组，分割的标志是空格、换行符等（'\r'  '\n'  ）
//注：words是一组指针，指向 str 某处作为一个词组的开始，在该词组末尾设置 '\0' 
int strpro_str2words(char *str, char **words, int maxNum)
{
    int i, mark, cnt;
    i = 0; mark = 0; cnt = 0;
    while(str[i] != '\0')
    {
        if(str[i] == ' ' || str[i] == '\r' || str[i] == '\n')
        {
            if(mark == 1)
                str[i] = '\0';
            mark = 0;
        }
        else
        {
            if(mark == 0)
            {
                words[cnt++] = str + i; 
                if(cnt >= maxNum)
                    return maxNum;
            }
                     
            mark = 1; 
           
        }
        i++;
        if(i >= STRPRO_MAX_LEN)
            //return -1;
            break;
    }
       
    return cnt;
}
//以 " = " 为标志，把字符串分割为两个部分  0：代表失败  1：代表成功
int strpro_divis_equation(const char *str, char *key, char *val)
{
	int i, cnt1, cnt2, mark;
	i = cnt1 = cnt2 = mark = 0;
	while(str[i] != '\0')
	{
		if(str[i] == '=')
		{
			mark = 1;
			i++;
			continue;
		}
					
		if(mark == 0)
			key[cnt1++] = str[i];
		else
			val[cnt2++] = str[i];
	
		i++;
        if(i >= STRPRO_MAX_LEN)
            return -1;
	}
	key[cnt1] = '\0';
	val[cnt2] = '\0';

	if(cnt1 <= 0 || cnt2 <= 0)
	    return 0;
	return 1;

}

//以某个字符为标志，把字符串分割为两个部分（例如 key:value ）
int strpro_divis_by_ch(const char *str, char ch, char *key, char *val)
{
	int i, cnt1, cnt2, mark;
	i = cnt1 = cnt2 = mark = 0;
	while(str[i] != '\0')
	{
		if(str[i] == ch)
		{
			mark = 1;
			i++;
			continue;
		}
					
		if(mark == 0)
			key[cnt1++] = str[i];
		else
			val[cnt2++] = str[i];
	
		i++;
        if(i >= STRPRO_MAX_LEN)
            return -1;
	}
	key[cnt1] = '\0';
	val[cnt2] = '\0';

	if(cnt1 <= 0 || cnt2 <= 0)
	    return 0;
	return 1;

}


//提取有效字符（除去两边的 " "或者"\r"或者\n"）
int strpro_get_viald_str(char *in, char *out)
{
	int pos1, pos2, len, mark1, mark2;
	len = strlen(in);
	int i;
	pos1 = 0; pos2 = len - 1;
	mark1 = mark2 = 0;
	for(i = 0; i < len; i++)
	{
		if(mark1 == 0 && in[i] != ' ' && in[i] != '\r' && in[i] != '\n')
			mark1 = 1;
		if(mark2 == 0 && in[len - i - 1] != ' ' && in[len - i - 1] != '\r' && in[len - i - 1] != '\n')
			mark2 = 1;
		if(mark1 == 0)
			pos1++;
		if(mark2 == 0)
			pos2--;
		
	}
	//LOG_SHOW("==>%s<==pos1=%d pos2=%d\n", in, pos1, pos2);
	if(pos2 - pos1 >= 0)
	{
		for(i = 0; i < (pos2 - pos1 + 1); i++)
			out[i] = in[pos1 + i];
		out[pos2 - pos1 + 1] = '\0';
		return 1;
	}
	return 0;
}


//提取字符串中第一个括号里面的内容，指定为 "[ ]" 或者 "( )" 或者 "[ ]"
int strpro_get_branket_content(char *in, char *out, int outMaxLen, char ch1, char ch2)
{
    if(in == NULL || out ==NULL || outMaxLen <= 0)
        return -1;

    int i, len, cnt, mark;
    len = strlen(in);
    cnt = 0; mark = 0;
    for(i= 0; i < len; i++)
    {
        if(in[i] == ch1 && mark == 0)
        {   
            mark = 1;
            continue;
        }
        else if(in[i] == ch2 && mark == 1)
        {
            out[cnt] = '\0';
            return 1;

        }
            
        if(mark == 1)
        {
            if(cnt >= outMaxLen)
            {
                out[outMaxLen] = '\0';
                return 2;
            }     
            out[cnt++] = in[i];           
        }        
    }
    return 0;
}


//字符串1 是否包含了字符串2
int strpro_str_contain(char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return 0;
    int len1, len2;
    len1 = strlen(str1);
    len2 = strlen(str2);
    if(len1 <= 0 || len2 <= 0 || len2 > len1)
        return 0;
    int i, pos;
    //printf("mark-----len1=%d  len2=%d\n", len1, len2);
    for(i = 0; i < len1 - len2 + 1; i++)
    {
        if(str1[i] == str2[0])
        {
            //printf("====>%s   %s    %d\n", str1+i, str2, len2);
            if(strncmp(str1 + i, str2, len2) == 0)
                return 1;
        }
    }
    return 0;
}

//根据几个字符来分割字符串，例如
//  字符串 "fasd; name:aa  score=90, name:bb score=80", 根据 "," 和";"分割为
// "fasd"  "name:aa score=90" "name:bb score=80"
#define STRPRO_OUT_MAX_LEN 512 
int strpro_divis_by_multiple_ch(char *chArray, int num, char *str, char **out, int outMaxNum, int outMaxlen)
{
    if(chArray == NULL || str == NULL || out == NULL)
        return 0;
    int len = strlen(str);
    if(len <= 0)
        return 0;
    int i, j;
    char tmp[STRPRO_MAX_LEN] = {0};
    int tmpCnt, outCnt, continueMark;
    tmpCnt = 0; outCnt = 0;
    for(i = 0; i < len; i++)
    {   
        continueMark = 0;
        for(j = 0; j < num; j++)
        {
            if(str[i] == chArray[j] && tmpCnt > 0)
            {
                tmp[tmpCnt] = '\0';
                strcpy(out[outCnt++], tmp);      
                tmpCnt = 0;
                continueMark = 1;
                break;
            }
        }
        if(continueMark == 1)
            continue;
        if(i == len - 1)    //结束
        {
            if(tmpCnt < outMaxlen - 1)
                tmp[tmpCnt++] = str[i];
            tmp[tmpCnt] = '\0';
            if(outCnt < outMaxNum)
                strcpy(out[outCnt++], tmp);      
            tmpCnt = 0;
        }
        if(tmpCnt < outMaxlen - 1)
            tmp[tmpCnt++] = str[i];
    }
    if(outCnt > 0)
        return outCnt;
    return 0;
}


//去除字符串两边的冒号, 例如字符串 ""OK"" 变为 "OK",
void strpro_remove_colon(char *in)
{
    if(in == NULL)
        return;
    int len, i, start, end, mark, cnt;
    len = strlen(in);
    char *buf = (char *)malloc(len + 8);
    if(buf == NULL)
        return;
    memcpy(buf, in, len);
    
    mark = 0; start = 0; end = len - 1;
    for(i = 0; i < len; i++)
    {
        if(buf[i] == '"')
        {
            if(mark == 0)
            {
                mark = 1;
                start = i;
            }
            else
            {
                end = i;
            }

        }
    }
    cnt = 0;
    for(i = 0; i < len; i++)
    {
        if(i > start && i < end)
            in[cnt++] = buf[i];
    }
    in[cnt] = '\0';
}




void strpro_test(void)
{
//========================================================================
//       LOG_SHOW("=====>%d\n", strpro_str_contain("1236 --", "1236"));
//       LOG_SHOW("=====>%d\n", strpro_str_contain("12", "120"));
//       LOG_SHOW("=====>%d\n", strpro_str_contain("1236  ", "1236 1"));
//       LOG_SHOW("=====>%d\n", strpro_str_contain("1236 ?.<", " ?"));
//========================================================================
//    char chArray[] = ";,";
//    char str[] = "fasd; name:aa  score=90, name:bb score=80";
//    char out[5][32];
//    char *outp[5];
//    int i;
//    int outNum;
//    for(i = 0; i < 5; i++)
//        outp[i] = out[i];
//    outNum = strpro_divis_by_multiple_ch(chArray, 2, str, outp, 5, 32);
//    for(i = 0; i < outNum; i++)
//    {
//        printf("[%d]%s\n", i, out[i]);
//    }
//    if(outNum == 0)
//        printf("return is 0\n");
//========================================================================
    char str[] = "fdasf\"fds\"fsd\"f";
    strpro_remove_colon(str);
    printf("===>%s\n", str);
    
       
}







