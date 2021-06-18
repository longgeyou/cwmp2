

#ifndef _INI_H_
#define _INI_H_

#include <../lib/include/common.h>


//注：值的大小有限制
#define INI_KEY_MAX_LEN 256
#define INI_VALUE_MAX_LEN 256
#define INI_MODEL_MAX_LEN 64


//数值类型
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
//模块链表
//创建链表头
ini_model_t *ini_model_list_create(void);
//添加节点
ini_model_t *ini_model_list_add_node(ini_model_t *head);
//删除节点
void ini_model_list_del_node(ini_model_t *head, ini_model_t *node);
//得到节点个数
int ini_model_list_get_num(ini_model_t *head);
//链表清除
void ini_model_list_clear(ini_model_t *head);
//链表显示
void ini_model_list_show(ini_model_t *head);
//测试
void ini_model_list_test(void);

//应用：找到第一个匹配键值对的 keyvalue 节点（例如 cwmp:enable , 其中cwmp为模块名， enable为键值名）
ini_key_value_t *ini_get_keyvalue_by_name(ini_model_t *head, char *name);
//应用：找到匹配的第n个 keyvalue 节点, idx 从0 开始
ini_key_value_t *ini_get_keyvalue_by_name_and_idx(ini_model_t *head, char *name, int idx);

//===============================================================


//===================================================================
//键值对 链表
//创建链表
ini_key_value_t *ini_key_list_create(void);
//添加节点
ini_key_value_t *ini_key_list_add_node(ini_key_value_t *head);
//删除节点
void ini_key_list_del_node(ini_key_value_t *head, ini_key_value_t *node);
//获取个数
int ini_key_list_get_num(ini_key_value_t *head);
//链表清除
void ini_key_list_clear(ini_key_value_t *head);
//链表显示
void ini_key_list_show(ini_key_value_t *head);
//测试
void ini_key_list_test();
//===================================================================


//===================================================================
//ini文件操作
//int 文档解析为链表
ini_list_t *ini_doc2list(buf_truple_t *doc);
//链表转化为文本大概占据的字节数（一般有冗余为佳）
int ini_key_list_need_byte(ini_key_value_t *head);
int ini_model_list_need_byte(ini_list_t *head);
//链表转化为文本内容 
buf_truple_t *ini_list2doc(ini_model_t *head);
//===================================================================


void ini_test1(char *testPath);
void ini_test2(void);
void ini_test3(char *file);




#endif



