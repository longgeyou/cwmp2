


#include <../cwmp/include/ini.h>


//解析 ini 文件 配置文件.....
/*=====================================================================例子：
[cwmp]
enable=1;
soap_env=SOAP-ENV
soap_enc=SOAP-ENC
acs_auth=0
cpe_auth=0
event_filename=/etc/cwmpevent.bin

acs_url=http://192.168.0.70:8000/otomc/acs/webservice.action
#acs_url=http://192.168.28.246:9999/
ca_file=/etc/ssl/private/ca.pem
ca_password=123456

cpe_manufacture=ChinaNMS
cpe_oui=A00001
cpe_sn=30000000000000000
cpe_name=000000	
cpe_pc=OT2800
cpe_specver=V1.0
cpe_hwver=V1.0
cpe_version=V1.2.7.29
cpe_username=cwmp
cpe_password=cwmp
acs_username=cwmp
acs_password=cwmp

[cwmpd]
httpd_port=5400
http_timeout=-1
======================================================================*/
/*=====================================================================规则
（1）配置的内容包括 键值名-数值，每一行作为一个键值对，类似于“字典”
     数据；键值名和数值通过 "=" 连接；
（2）"#" 符号时作为注释符号，代表这一行略过；
（3）形如 [label] 内容，代表不同的模块，其后面的键值对“属于”这个模块 
======================================================================*/
/*=====================================================================方式
（1）链表A存储 模块
     链表B存储 模块下的键值对
======================================================================*/



//类型名
const char iniValueTypeName[][64]={
    "other",
    "integer",
    "string"    
};



//===================================================================
//模块链表
//创建链表头
ini_model_t *ini_model_list_create(void)
{
   ini_model_t *head = (ini_model_t *)malloc(sizeof(ini_model_t));
   if(head == NULL)
        return NULL;
   head->next = NULL;
   return head;
}
//添加节点
ini_model_t *ini_model_list_add_node(ini_model_t *head)
{
    if(head == NULL)
        return NULL;
    ini_model_t *node = (ini_model_t *)malloc(sizeof(ini_model_t));
    if(node == NULL)
        return NULL;
    ini_model_t *prob = head;
    while(prob->next != NULL)    
        prob = prob->next;
    prob->next = node;
    node->next = NULL;
    node->keyList = ini_key_list_create();
    return node;
}
//删除节点
void ini_model_list_del_node(ini_model_t *head, ini_model_t *node)
{
    if(head == NULL)
        return ;
    ini_model_t *prob = head->next;
    ini_model_t *last = head;
    while(prob != NULL)
    {
        if(prob == node)
        {
            last->next = prob->next;
            free(prob);
            return;
        }
        last = prob;
        prob = prob->next;
    }
}
//得到节点个数
int ini_model_list_get_num(ini_model_t *head)
{
    if(head == NULL)
        return -1;
    ini_model_t *prob = head->next;
    int cnt = 0;
    while(prob != NULL)
    {
        cnt++;
        prob = prob->next;
    }
    return cnt;
}

//链表清除
void ini_model_list_clear(ini_model_t *head)
{
    if(head == NULL)
        return;
    ini_model_t *prob;
    ini_model_t *last;
    while(1)
    {
        prob = head->next;
        last = head;
        if(prob == NULL)
            break;
        while(prob->next != NULL)
        {
            last = prob;
            prob = prob->next;
        }
        free(prob);
        last->next = NULL;
        
    }
}

//链表显示
void ini_model_list_show(ini_model_t *head)
{
    LOG_SHOW("====================ini model list show=====================\n");
    if(head == NULL)
    {
        LOG_SHOW("(head is null)\n");
        return;
    }
    if(ini_model_list_get_num(head) == 0)
     {
        LOG_SHOW("(node num is 0)\n");
        return;
    }
    ini_model_t *prob = head->next;
    int cnt = 0;
    while(prob != NULL)
    {
         LOG_SHOW("[model %d]%s\n", cnt++, prob->name);     
         if(prob->keyList != NULL)ini_key_list_show(prob->keyList);
         prob = prob->next;
    }       
}

//测试
void ini_model_list_test(void)
{
    ini_model_t *head = ini_model_list_create();
    ini_model_t *node1 = ini_model_list_add_node(head);
    strcpy(node1->name, "name1");
    ini_model_t *node2 = ini_model_list_add_node(head);
    strcpy(node2->name, "name2");
    ini_model_t *node3 = ini_model_list_add_node(head);
    strcpy(node3->name, "name3");
    ini_model_list_show(head);

    ini_model_list_del_node(head, node2);
    ini_model_list_show(head);

    ini_model_list_clear(head);
    ini_model_list_show(head);
}

//应用：找到第一个匹配键值对的 keyvalue 节点（例如 cwmp:enable , 其中cwmp为模块名， enable为键值名）
ini_key_value_t *ini_get_keyvalue_by_name(ini_model_t *head, char *name)
{
     if(head == NULL)
        return NULL;
    char mode[INI_MODEL_MAX_LEN];
    char key[INI_KEY_MAX_LEN];
    strpro_divis_by_ch(name, ':', mode, key);
    //LOG_SHOW("mode:%s    -->key:%s<--\n", mode, key);
    ini_model_t *prob = head->next;
    ini_key_value_t *subProb;
    while(prob != NULL)
    {
        if(strcmp(prob->name, mode) == 0)
        {            
            if(prob->keyList == NULL)
                continue;
            subProb = prob->keyList->next;      
            while(subProb != NULL)
            {
                if(strcmp(subProb->name, key) == 0)
                {
                    return subProb;
                }
                subProb = subProb->next;
            }
        }      
        prob = prob->next;
    }
    return NULL;   
}
//应用：找到匹配的第n个 keyvalue 节点, idx 从0 开始
ini_key_value_t *ini_get_keyvalue_by_name_and_idx(ini_model_t *head, char *name, int idx)
{
    if(head == NULL || name == NULL || idx < 0)
        return NULL;
    char mode[INI_MODEL_MAX_LEN];
    char key[INI_KEY_MAX_LEN];
    int cnt;
    strpro_divis_by_ch(name, ':', mode, key);
    //LOG_SHOW("mode:%s    -->key:%s<--\n", mode, key);
    ini_model_t *prob = head->next;
    ini_key_value_t *subProb;
    cnt = 0;
    while(prob != NULL)
    {
        if(strcmp(prob->name, mode) == 0)
        {            
            if(prob->keyList == NULL)
                continue;
            subProb = prob->keyList->next;      
            while(subProb != NULL)
            {
                if(strcmp(subProb->name, key) == 0)
                {
                    if(cnt == idx)
                        return subProb;
                     cnt++;
                }
                subProb = subProb->next;
            }
        }      
        prob = prob->next;
    }
    return NULL;       

}

//===================================================================



//===================================================================
//键值对 链表
//创建链表
ini_key_value_t *ini_key_list_create(void)
{
    ini_key_value_t *head = (ini_key_value_t *)malloc(sizeof(ini_key_value_t));
    if(head == NULL)
        return NULL;
    head->next = NULL;
    return head;
}
//添加节点
ini_key_value_t *ini_key_list_add_node(ini_key_value_t *head)
{
    if(head == NULL)
        return NULL;
    ini_key_value_t *node = (ini_key_value_t *)malloc(sizeof(ini_key_value_t));
    if(node == NULL)
        return NULL;
    ini_key_value_t *prob = head;
    while(prob->next != NULL)
        prob = prob->next;
        
    node->next = NULL;
    prob->next = node;
    return node;
}
//删除节点
void ini_key_list_del_node(ini_key_value_t *head, ini_key_value_t *node)
{
    if(head == NULL)
        return ;
    ini_key_value_t *prob = head->next;
    ini_key_value_t *last = head;
    while(prob != NULL)
    {
        if(prob == node)
        {
            last->next = prob->next;
            free(prob);
            return;
        }
        last = prob;
        prob = prob->next;
    }
}
//获取个数
int ini_key_list_get_num(ini_key_value_t *head)
{
    if(head == NULL)
        return -1;
    int cnt = 0;
    ini_key_value_t *prob = head->next;
    while(prob != NULL)
    {
        cnt++;
        prob = prob->next;
    }
    return cnt;
}
//链表清除
void ini_key_list_clear(ini_key_value_t *head)
{
    if(head == NULL)
        return ;
    ini_key_value_t *prob;
    ini_key_value_t *last;
    while(1)
    {
        prob = head->next;
        last = head;
        if(prob == NULL)
            break;
        while(prob->next != NULL)
        {
            last = prob;
            prob = prob->next;
        }
        free(prob);
        last->next = NULL;
    }

}

//链表显示
void ini_key_list_show(ini_key_value_t *head)
{   
    LOG_SHOW("================ini key-value list show=================\n");
    if(head == NULL)
    {
        LOG_SHOW("(null)\n");
        return;
    }
    int num = ini_key_list_get_num(head);
    if(num == 0)
    {
        LOG_SHOW("(node num is 0)\n");
        return;
    }
    ini_key_value_t *prob = head->next;
    int cnt = 0;
    while(prob != NULL)
    {
        LOG_SHOW("[kv %d][type %s]%s=%s\n", cnt++, iniValueTypeName[prob->valueType], prob->name, prob->value);
        prob = prob->next;
    }
        
}

//测试

void ini_key_list_test()
{
    ini_key_value_t *head = ini_key_list_create();
    ini_key_value_t *node1 = ini_key_list_add_node(head);
    strcpy(node1->name, "key1");
    strcpy(node1->value,"value1");
    node1->valueType = INT_VALUE_TYPE_INT;

    ini_key_value_t *node2 = ini_key_list_add_node(head);
    strcpy(node2->name, "key2");
    strcpy(node2->value,"value2");
    node2->valueType = INT_VALUE_TYPE_INT;

    ini_key_value_t *node3 = ini_key_list_add_node(head);
    strcpy(node3->name, "key3");
    strcpy(node3->value,"value3");
    node1->valueType = INT_VALUE_TYPE_INT;

    ini_key_list_show(head);

    ini_key_list_del_node(head, node2);
    ini_key_list_show(head);

    ini_key_list_clear(head);
    ini_key_list_show(head);
}

//===================================================================


//===================================================================
//ini文件操作
//int 文档解析为链表
ini_list_t *ini_doc2list(buf_truple_t *doc)
{
    ini_list_t *list =  ini_model_list_create();
    buf_list_head_t *buf = buf_truple2list(doc);  
    buf_truple_t *prob = buf->next;  
    char out[INI_KEY_MAX_LEN + INI_VALUE_MAX_LEN + 4] = {0};
    char key[INI_KEY_MAX_LEN] = {0};
    char value[INI_VALUE_MAX_LEN] = {0};
    char model[INI_MODEL_MAX_LEN] = {0};
    
    int mark = 0;   //代表还未出现 模块名, 即 "[ ]"
    ini_model_t *currentNode = ini_model_list_add_node(list); 
    strcpy(currentNode->name, "null");
    while(prob != NULL)
    {
        if(strpro_get_viald_str(prob->data, out))
        {
            if(out[0] == '#');  //跳过
            else if(out[0] == '[')
            {               
                if(strpro_get_branket_content(out, model, INI_MODEL_MAX_LEN, '[', ']'))
                {
                    if(mark == 0)
                        mark = 1;                                            
                    else                    
                        currentNode = ini_model_list_add_node(list);
                    strcpy(currentNode->name, model);
               }            
            }
            else if(strpro_divis_equation(out, key, value))
            {
                ini_key_value_t *node = ini_key_list_add_node(currentNode->keyList);
                strcpy(node->name, key);
                strcpy(node->value, value);
            }
        }
        prob = prob->next;

    }

    //释放
    buf_list_free(buf);
    return list;
}



//链表转化为文本大概占据的字节数（一般有冗余为佳）
int ini_key_list_need_byte(ini_key_value_t *head)
{   
    if(head == NULL)
        return 0;
    ini_key_value_t *prob = head->next;
    int byte = 0;
    while(prob != NULL)
    {
        byte += strlen(prob->name) + 3;
        byte += strlen(prob->value) + 3;
        prob = prob->next;
    }
    return byte;
}
int ini_model_list_need_byte(ini_list_t *head)
{
    if(head == NULL)
        return -1;
    ini_model_t *prob = head->next;
    int byte = 0;
    while(prob != NULL)
    {
        byte += strlen(prob->name) + 3;
        byte += ini_key_list_need_byte(prob->keyList);
        prob = prob->next;
    }
    return byte;
}

//链表转化为文本内容 
buf_truple_t *ini_list2doc(ini_model_t *head)
{
    int len = ini_model_list_need_byte(head);
    buf_truple_t *buf = buf_create(len);
    ini_model_t *prob = head->next;
    int pos = 0;
     while(prob != NULL)
    {
        sprintf(buf->data + pos, "[%s]\r\n", prob->name);
        pos += strlen(prob->name) + 4;
        ini_key_value_t *prob2 = prob->keyList->next;
        while(prob2 != NULL)
        {
             sprintf(buf->data + pos, "%s=%s\r\n", prob2->name, prob2->value);
             pos += strlen(prob2->name) + strlen(prob2->value) + 3;
             prob2 = prob2->next;
        }
        prob = prob->next;
    }    
   
    return buf;
}


//测试
void ini_test1(char *fileName)
{
    if(fileName == NULL)
        return;
   
    buf_truple_t *doc = buf_improt_file(fileName, BUF_IMPORT_FILE_DEFAULT_SIZE);
    ini_list_t *list = ini_doc2list(doc);
    ini_model_list_show(list);
    
    //释放内存
    buf_free(doc);
    ini_model_list_clear(list);   
}

void ini_test2(void)
{
    ini_list_t *list = ini_model_list_create();    
    ini_model_t *model1 = ini_model_list_add_node(list);
    strcpy(model1->name, "model1");
    ini_key_value_t *node1 = ini_key_list_add_node(model1->keyList);
    strcpy(node1->name, "key1");
    strcpy(node1->value, "value1");
    ini_key_value_t *node2 = ini_key_list_add_node(model1->keyList);
    strcpy(node2->name, "key2");
    strcpy(node2->value, "value2");

    ini_model_t *model2 = ini_model_list_add_node(list);
    strcpy(model2->name, "model2");
    ini_key_value_t *node3 = ini_key_list_add_node(model2->keyList);
    strcpy(node3->name, "key3");
    strcpy(node3->value, "value3");
    ini_key_value_t *node4 = ini_key_list_add_node(model2->keyList);
    strcpy(node4->name, "key4");
    strcpy(node4->value, "value4");

    buf_truple_t *doc = ini_list2doc(list);
    ini_model_list_show(list);
    buf_show(doc);

    //释放内存
    buf_free(doc);
    ini_model_list_clear(list);

}
void ini_test3(char *file)
{
    if(file == NULL)
        return;
    buf_truple_t *doc = buf_improt_file(file, BUF_IMPORT_FILE_DEFAULT_SIZE);
    //LOG_SHOW("%s\n", file);
    //buf_show(doc);
    ini_list_t *list = ini_doc2list(doc);
    //ini_model_list_show(list);

    ini_key_value_t *key = ini_get_keyvalue_by_name(list, "cwmp:soap_enc");        
    if(key != NULL)LOG_SHOW("key=%s    value=%s\n", key->name, key->value);
    //释放内存
    buf_free(doc);
    ini_model_list_clear(list);   
}

//===================================================================











