


#ifndef _KEYVAL_H_
#define _KEYVAL_H_



//键值对
#define KEY_NAME_MAX_BYTE 256 
#define VALUE_STR_MAX_BYTE 256
typedef struct key_value_t{
    char key[KEY_NAME_MAX_BYTE];
    char value[VALUE_STR_MAX_BYTE];
}key_value_t;


int key_value_inquire(key_value_t *array, int arrayNum, char *key, char *value, int valueMaxByte);

//根据键位查询，忽略大小写
int key_value_case_inquire(key_value_t *array, int arrayNum, char *key, char *value, int valueMaxByte);


int key_value_get_from_str(char *chArray, int num, char *str, key_value_t *kv);

void key_value_array_show(key_value_t *array, int num);

void keyval_test();

#endif





