



#ifndef _STRPRO_H_
#define _STRPRO_H_



//�õ����������
#define WORDS_MAX_LEN 128
//������ַ��������
#define STRPRO_MAX_LEN 1024


int strpro_str2words(char *str, char **words, int maxNum);
int strpro_divis_equation(const char *str, char *key, char *val);
int strpro_get_viald_str(char *in, char *out);
int strpro_get_branket_content(char *in, char *out, int outMaxLen, char ch1, char ch2);
//��ĳ���ַ�Ϊ��־�����ַ����ָ�Ϊ�������֣����� key:value ��
int strpro_divis_by_ch(const char *str, char ch, char *key, char *val);
//�ַ���1 �Ƿ�������ַ���2
int strpro_str_contain(char *str1, char *str2);
int strpro_divis_by_multiple_ch(char *chArray, int num, char *str, char **out, int outMaxNum, int outMaxlen);

//ȥ���ַ������ߵ�ð��, �����ַ��� ""OK"" ��Ϊ "OK",
void strpro_remove_colon(char *in);




void strpro_test(void);


#endif






