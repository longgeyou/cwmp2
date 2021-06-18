

#ifndef _INI_H_
#define _INI_H_

#include <../lib/include/common.h>


//ע��ֵ�Ĵ�С������
#define INI_KEY_MAX_LEN 256
#define INI_VALUE_MAX_LEN 256
#define INI_MODEL_MAX_LEN 64


//��ֵ����
typedef enum ini_value_type_e{
    INT_VALUE_TYPE_OTHER = 0,
    INT_VALUE_TYPE_INT,
    INT_VALUE_TYPE_STRING
}ini_value_type_e;

typedef struct ini_key_value_t{
    char name[INI_KEY_MAX_LEN];
    char value[INI_VALUE_MAX_LEN];
    int valueType;
    struct ini_key_value_t *next;
}ini_key_value_t;


typedef struct ini_model_t{
    char name[128];
    struct ini_model_t *next;
    ini_key_value_t *keyList;
}ini_model_t;

typedef ini_model_t ini_list_t;



//===================================================================
//ģ������
//��������ͷ
ini_model_t *ini_model_list_create(void);
//��ӽڵ�
ini_model_t *ini_model_list_add_node(ini_model_t *head);
//ɾ���ڵ�
void ini_model_list_del_node(ini_model_t *head, ini_model_t *node);
//�õ��ڵ����
int ini_model_list_get_num(ini_model_t *head);
//�������
void ini_model_list_clear(ini_model_t *head);
//������ʾ
void ini_model_list_show(ini_model_t *head);
//����
void ini_model_list_test(void);

//Ӧ�ã��ҵ���һ��ƥ���ֵ�Ե� keyvalue �ڵ㣨���� cwmp:enable , ����cwmpΪģ������ enableΪ��ֵ����
ini_key_value_t *ini_get_keyvalue_by_name(ini_model_t *head, char *name);
//Ӧ�ã��ҵ�ƥ��ĵ�n�� keyvalue �ڵ�, idx ��0 ��ʼ
ini_key_value_t *ini_get_keyvalue_by_name_and_idx(ini_model_t *head, char *name, int idx);

//===============================================================


//===================================================================
//��ֵ�� ����
//��������
ini_key_value_t *ini_key_list_create(void);
//��ӽڵ�
ini_key_value_t *ini_key_list_add_node(ini_key_value_t *head);
//ɾ���ڵ�
void ini_key_list_del_node(ini_key_value_t *head, ini_key_value_t *node);
//��ȡ����
int ini_key_list_get_num(ini_key_value_t *head);
//�������
void ini_key_list_clear(ini_key_value_t *head);
//������ʾ
void ini_key_list_show(ini_key_value_t *head);
//����
void ini_key_list_test();
//===================================================================


//===================================================================
//ini�ļ�����
//int �ĵ�����Ϊ����
ini_list_t *ini_doc2list(buf_truple_t *doc);
//����ת��Ϊ�ı����ռ�ݵ��ֽ�����һ��������Ϊ�ѣ�
int ini_key_list_need_byte(ini_key_value_t *head);
int ini_model_list_need_byte(ini_list_t *head);
//����ת��Ϊ�ı����� 
buf_truple_t *ini_list2doc(ini_model_t *head);
//===================================================================


void ini_test1(char *testPath);
void ini_test2(void);
void ini_test3(char *file);




#endif



