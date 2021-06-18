



#ifndef _STRPRO_H_
#define _STRPRO_H_



//得到单词最长长度
#define WORDS_MAX_LEN 128
//处理的字符串最长长度
#define STRPRO_MAX_LEN 1024


int strpro_str2words(char *str, char **words, int maxNum);
int strpro_divis_equation(const char *str, char *key, char *val);
int strpro_get_viald_str(char *in, char *out);
int strpro_get_branket_content(char *in, char *out, int outMaxLen, char ch1, char ch2);
//以某个字符为标志，把字符串分割为两个部分（例如 key:value ）
int strpro_divis_by_ch(const char *str, char ch, char *key, char *val);
//字符串1 是否包含了字符串2
int strpro_str_contain(char *str1, char *str2);
int strpro_divis_by_multiple_ch(char *chArray, int num, char *str, char **out, int outMaxNum, int outMaxlen);

//去除字符串两边的冒号, 例如字符串 ""OK"" 变为 "OK",
void strpro_remove_colon(char *in);




void strpro_test(void);


#endif






