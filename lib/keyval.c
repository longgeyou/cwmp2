


//键值对辅助工具
//在 pool.c 中也有所提及
#include <../lib/include/common.h>





//根据键位查询
int key_value_inquire(key_value_t *array, int arrayNum, char *key, char *value, int valueMaxByte)
{
    if(array == NULL || arrayNum <= 0 || key == NULL || value == NULL)
        return 0;
    int i;
    for(i = 0; i < arrayNum; i++)
    {
        if(strcmp(array[i].key, key) == 0)    
        {
            strncpy(value, array[i].value, valueMaxByte);
            return 1;
        }
    }

    return 0;
}
//根据键位查询，忽略大小写
int key_value_case_inquire(key_value_t *array, int arrayNum, char *key, char *value, int valueMaxByte)
{
    if(array == NULL || arrayNum <= 0 || key == NULL || value == NULL)
        return 0;
    int i;
    for(i = 0; i < arrayNum; i++)
    {
        if(strcasecmp(array[i].key, key) == 0)    
        {
            strncpy(value, array[i].value, valueMaxByte);
            return 1;
        }
    }

    return 0;
}



//由字符串得到键值对，分割的标志可以有多种字符，例如：
// "name: ok" "score= 100"，分隔符可以是":"，也可以是 "="
int key_value_get_from_str(char *chArray, int num, char *str, key_value_t *kv)
{
    if(chArray == NULL || str == NULL || kv == NULL)
        return 0;
    int len = strlen(str);
    if(len <= 0)
        return 0;
    if(num <= 0)
        return 0;
    int i, mark, j, continueMark, keyCnt, valueCnt;
    char  key[KEY_NAME_MAX_BYTE];
    char value[VALUE_STR_MAX_BYTE];
    mark = 0; keyCnt = 0; valueCnt = 0;
    for(i = 0; i < len; i++)
    {
        continueMark = 0;
        if(mark == 0)
        {
            
            for(j = 0; j < num; j++)
            {
                if(str[i] == chArray[j])
                {
                    mark = 1;
                    continueMark = 1;
                }
            }
        }
        if(continueMark == 1)
            continue;
        if(mark == 0)
        {
            if(keyCnt < KEY_NAME_MAX_BYTE - 1)
                key[keyCnt++] = str[i];
        }
        else
        {
            if(valueCnt < VALUE_STR_MAX_BYTE - 1)
              value[valueCnt++] = str[i];
        }
          
    }
    if(mark != 1 || keyCnt<= 0 || valueCnt <= 0)
        return 0;
    key[keyCnt] = '\0';
    value[valueCnt] = '\0';
    strcpy(kv->key, key);
    strcpy(kv->value, value);
    return 1;
}

//显示
void key_value_array_show(key_value_t *array, int num)
{
    if(array == NULL)
        return;
    int i;    
    printf("====================key value show====================\n");
    if(num <= 0)
    {
        printf("(nil)");
        return;
    }
    for(i = 0; i < num; i++)
    {
        printf("[%d][%s][%s]\n", i, array[i].key, array[i].value);
    }
    printf("=======================================================\n");
}



//keyval 测试
void keyval_test()
{
//===============================================================
//    key_value_t array[]={
//        {"AA", "10"},
//        {"BB", "20"},
//        {"CC", "30"}
//
//    };
//    char val[VALUE_STR_MAX_BYTE];
//    if(key_value_inquire(array, 3, "BB", val))
//         printf("BB is %s\n", val);
//    else 
//        printf("not match\n");
//    key_value_array_show(array, 3);
//===============================================================
    char str[] = "name: dasf";
    char str2[] = "score = 30";
    key_value_t array[] = {
        {" ", " "},
        {" ", " "}
    };
    key_value_get_from_str(":=", 2, str, &(array[0])); 
    key_value_get_from_str(":=", 2, str2, &(array[1])); 
    key_value_array_show(array, 2);
    
    

}





