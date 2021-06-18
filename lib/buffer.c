


#include <../lib/include/common.h>



//alloc函数：从栈上分配内存
//malloc函数：从堆上分配内存
//colloc函数：从堆上分配一组内存
//注：频繁的动态内存分配会产生大量内存碎片，从而导致程序效率变低


typedef struct buf_manager_t{
    //buf 数据统计信息
    int mallocCnt;
    int mallocFaildCnt;
    int mallocTotleSize;  
    int freeCnt;
    int freeTotleSize;
    int spaceSize;     
    
}buf_manager_t;

buf_manager_t bufLocalManager;

void buf_init(void)
{
    bufLocalManager.mallocCnt = 0;
    bufLocalManager.mallocFaildCnt = 0;
    bufLocalManager.mallocTotleSize = 0;
    bufLocalManager.freeCnt = 0;
    bufLocalManager.freeTotleSize = 0;
    bufLocalManager.spaceSize = 0;    
}

void buf_stat_show(void)
{
    printf("==========buf stat show============\n");
    printf("mallocCnt=%d\n"
            "mallocFaildCnt=%d\n"
            "mallocTotleSize=%d\n"
            "freeCnt=%d\n"
            "freeTotleSize=%d\n"
            "spaceSize=%d\n",
            bufLocalManager.mallocCnt,
            bufLocalManager.mallocFaildCnt,
            bufLocalManager.mallocTotleSize,
            bufLocalManager.freeCnt,
            bufLocalManager.freeTotleSize,
            bufLocalManager.spaceSize);
    printf("============== end ===============\n");
}



void *buf_data_malloc(int size)
{
    void *bufData = pool_malloc(size);
    
    bufLocalManager.mallocCnt++;
    if(bufData == NULL)
        bufLocalManager.mallocFaildCnt++;
    bufLocalManager.mallocTotleSize += size;  
    bufLocalManager.spaceSize += size;
    return bufData;
}


#define BUF_DATA_SIZE_ADD 8
void buf_data_free(buf_truple_t *header)
{
    if(header == NULL)
        return;

    bufLocalManager.freeCnt++;
    bufLocalManager.freeTotleSize += header->size + BUF_DATA_SIZE_ADD;
    bufLocalManager.spaceSize -= header->size + BUF_DATA_SIZE_ADD;
    pool_free(header->data);
}

//在某些场景下替代数组，如 buf 大小不固定，要求按需分配；buf 大小根据变量来确定
#define BUF_DATA_SIZE_ALARM_SIZE 1024*1024*100
buf_truple_t *buf_create(int size)
{ 
    buf_truple_t *t = (buf_truple_t *)pool_malloc(sizeof(buf_truple_t));
    if(t == NULL) 
        return NULL;
    
    //判断参数
    if(size < 0) 
        return NULL;  
    if(size > BUF_DATA_SIZE_ALARM_SIZE)
        printf("buf arlarm: size is bigger than %d", BUF_DATA_SIZE_ALARM_SIZE);
    t->data = (unsigned char *)buf_data_malloc(size + BUF_DATA_SIZE_ADD);  
    if(t->data == NULL)
        return NULL;
    memset(t->data , 0, size + BUF_DATA_SIZE_ADD);
    t->size = size;
    t->used = 0;
    return t;
}
//更改buf的大小(如果给定的size 小于或者等于原来的size，则不动作...)
int buf_new_size(buf_truple_t *t, int size)
{
    if(t == NULL)
        return 0;
    if(size < 0)
        return 0;
    if(size <= t->size) 
        return 0;
    
    unsigned char *newData = (unsigned char *)buf_data_malloc(size + BUF_DATA_SIZE_ADD);
    if(newData == NULL)
        return -1; 
    memcpy(newData, t->data, t->size);   
    buf_data_free(t);
    t->data = newData;
    t->size = size;
    return 1;
}
//赋值
int buf_set_value(buf_truple_t *t, unsigned char *data, int size)
{
    if(t == NULL)
        return 0;
    buf_new_size(t, size);

    if(t->data != NULL)
    {
        memcpy(t->data, data, size);
        t->used = size;
        if(t->used > t->size)
            t->used = t->size;
        t->data[t->used] = '\0';
        return 1;
    }
    
    return 0;
}

//追加数据，会自动扩容
int buf_append_data(buf_truple_t *t, unsigned char *data, int size)
{
    if(t == NULL || data == NULL)
        return 0;
    int pos = t->used;
    //printf("====>pos=%d   size=%d   t->used=%d  t->size=%d\n", pos, size, t->used, t->size);
    buf_new_size(t, t->used + size);
    
    memcpy(t->data + pos, data, size);
    t->used += size;
    if(t->used > t->size)
        t->used = t->size;
    t->data[t->used] = '\0';
    return 1;
}

//清空数据
void buf_clear_data(buf_truple_t *t)
{
    if(t == NULL)
        return;
    memset(t->data, 0, t->size);
}


//字符串追加内容（不会自动扩容）
void buf_append_str(buf_truple_t *t, char *data)
{
    if(t == NULL || data == NULL)
        return;
    int len = strlen(t->data);
    int lenAppend = strlen(data);
    if(t->size < (len + lenAppend))
    {
        lenAppend = t->size - len;
    }
    //LOG_SHOW("=====>len=%d  lenAppend=%d\n", len, lenAppend);
    snprintf(t->data + len, lenAppend + 1,  "%s", data);    
}

//释放buf
void buf_free(buf_truple_t *t)
{   
    if(t == NULL)
        return;
    buf_data_free(t);
    pool_free(t);
}

//
void buf_show(buf_truple_t *t)
{
	if(t == NULL)
	{
		LOG_SHOW("===========buf show==========\n[null] \n");
	}
	else
	{	    
        LOG_SHOW("===========buf show[space:%d used:%d]==========\n%s[end mark]\n", t->size, t->used, t->data);
	    LOG_SHOW("===================== end ========================\n");	    		
	}
}

//文件到buf
buf_truple_t *buf_improt_file(const char *fileName, int maxSize)
{
	FILE *fp = fopen(fileName, "r");
	if(fp == NULL)
		return NULL;
	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	if(maxSize < len)
		len = maxSize;
	buf_truple_t *t = buf_create(len); 
	if(t == NULL)
		return NULL;
	fseek(fp, 0, SEEK_SET);
	int blockSize = 256;
	long pos = fread((void *)t->data, blockSize, len / blockSize, fp);
	int rem = fread((void *)(t->data + (pos * blockSize)), 1, len - pos, fp);
	t->size = pos * blockSize + rem;
	//LOG_SHOW("====>len=%d\n", len);
	fclose(fp);
	return t;
}


//=======================================================================
//链表存储buf
buf_truple_t *buf_list_create(void)
{
    buf_truple_t *hand = (buf_truple_t *)pool_malloc(sizeof(buf_truple_t));
    if(hand == NULL)
        return NULL;
    hand->next = NULL;
    return hand;
}
void buf_list_in(buf_truple_t *head, void *data, int size)
{
    if(head == NULL)
        return;
    buf_truple_t *node = (buf_truple_t *)pool_malloc(sizeof(buf_truple_t));
    node->data = (unsigned char *)buf_data_malloc(size + 3);
    memcpy(node->data, data, size);     //会出现数组越界？？
    node->data[size] = '\0';
    node->size = size;
    node->next = NULL;
    buf_truple_t *prob = head;
    while(prob->next != NULL)
        prob = prob->next;
    prob->next = node;
}

//添加一个链表 (注意： head 如果等于 headNew，则会出错， 所以做判断，直接不动作)
void buf_list_in_list(buf_truple_t *head, buf_truple_t *headNew)
{
    if(head == NULL || headNew == NULL || head == headNew)
        return ;
    buf_truple_t *prob = headNew->next;
    while(prob != NULL)
    {
        buf_list_in(head, prob->data, prob->size);
        prob = prob->next;
    }
}



void buf_list_clear(buf_truple_t *head)
{
    if(head == NULL)
        return;
    buf_truple_t *prob;
    buf_truple_t *last;
    while(1)
    {
        prob = head;
        last = head;
        while(prob->next != NULL)
        {
            last = prob;
            prob = prob->next;
        }
        if(prob == head)
            break;
        last->next = NULL;
        buf_free(prob);
    } 

}
void buf_list_free(buf_truple_t *head)
{
    if(head == NULL)
        return;
    buf_list_clear(head);   
    pool_free(head);
}

int buf_list_get_size(buf_truple_t *head)
{
    if(head == NULL)
        return -1;
    int len;
    buf_truple_t *prob = head->next;

    len = 0;
    while(prob != NULL)
    {
        len += prob->size; 
        prob = prob->next;
    }
    return len;
}
int buf_list_get_num(buf_truple_t *head)
{
    if(head == NULL)
        return -1;
    int cnt = 0;
    buf_truple_t *prob = head->next;

    while(prob != NULL)
    {
        cnt++;
        prob = prob->next;
    }
    return cnt;
}


//索引以0 为开始
buf_truple_t *buf_list_get_node(buf_truple_t *head, int idx)
{
    if(idx < 0 || head == NULL)
        return NULL;
    buf_truple_t *prob = head->next;
    int cnt;
    cnt = 0;
    while(prob != NULL)
    {
        if(cnt == idx)
            return prob;
        cnt++;
        prob = prob->next;
    }
    return NULL; 
}

void buf_list_show(buf_truple_t *head)
{
    int cnt;
    buf_truple_t *prob = head->next;
    LOG_SHOW("=============buf list show===========\n");
    if(head->next == NULL)
    {
        LOG_DEBUG("null:");
        return;
    }
        
    cnt = 0;
    while(prob != NULL)
    {
        prob->data[prob->size] = '\0';
        LOG_SHOW("[line:%d size:%d]%s\n", cnt++, prob->size, (const char *)prob->data); 
        prob = prob->next;
    } 
    LOG_SHOW("[end mark]================== end =================\n");
}

//buf list 到 buf
buf_truple_t *buf_list2truple(buf_truple_t *list)
{
	int size = buf_list_get_size(list);
	buf_truple_t *t = buf_create(size);
	if(t == NULL || list == NULL)
		return NULL;
	buf_truple_t *prob = list->next;
	int pos = 0;
	while(prob != NULL)
	{
		memcpy((t->data) + pos, prob->data, prob->size);
		pos += prob->size;
		prob = prob->next;
	}
	return t;
}

//buf list 到 buf, 每一条增加换行符，只适用于文本操作
buf_truple_t *buf_list2truple_for_line(buf_truple_t *list)
{
	int size = buf_list_get_size(list);
	buf_truple_t *t = buf_create(size + buf_list_get_num(list));
	if(t == NULL || list == NULL)
		return NULL;
	buf_truple_t *prob = list->next;
	int pos = 0;
	while(prob != NULL)
	{
	    if(prob->data != NULL)
	    {
            sprintf(t->data + pos, "%s\n", prob->data);
		    pos += strlen(prob->data) + 1;
	    }
		
		prob = prob->next;
	}

	return t;
}


//buf 根据 '\n'来转换为 buf list
buf_list_head_t *buf_truple2list(buf_truple_t *buf)
{
    if(buf ==NULL)
        return NULL;
    int i;
    buf_list_head_t *list = buf_list_create();
    if(list == NULL)
        return NULL;
    long pos;
    pos = 0; 
    //buf_show(buf);
    for(i = 0; i < buf->size; i++)
    {
        if(buf->data[i] == '\n')
        {
            buf_list_in(list, buf->data + pos, i - pos + 1);
            pos = i + 1;
        }
    }
    if(i - pos + 1 > 0)buf_list_in(list, buf->data + pos, i - pos + 1);
    return list;

}

//读取文件文本内容到 buf list，每一行用一个节点存储
buf_list_head_t *buf_list_read_file(char *fileName, int maxSize)
{       
    if(fileName == NULL || maxSize <= 0)
         return NULL;
    buf_truple_t *buf = buf_improt_file(fileName, maxSize);
    buf_list_head_t *list = buf_truple2list(buf);
    return list;
}

//==================================================================================

//测试用
void buf_test(void)
{
    buf_truple_t *buf = buf_create(8);
    //buf_append(buf, "123\n");
    //buf_append(buf, "456\n");
    //buf_show(buf);

    //buf_free(buf);

    //buf_stat_show();
    //pool_stat_show();

    buf_append_data(buf, "sdfa", 4);    
    buf_append_data(buf, "12345", 5);


    buf_show(buf);
    buf_clear_data(buf);
    buf_show(buf);

    //LOG_SHOW("OK\n");
    
}

void buf_list_test(void)
{
    buf_truple_t *head = buf_list_create();
    buf_list_in(head, "look", strlen("look"));
    buf_list_in(head, "job", strlen("job"));
    buf_list_in(head, "good manner", strlen("good manner"));

     buf_truple_t *head2 = buf_list_create();
    buf_list_in(head2, "look", strlen("look"));
    buf_list_in(head2, "job", strlen("job"));
    buf_list_in(head2, "good manner", strlen("good manner"));
    buf_list_in_list(head, head2);
    buf_list_show(head);

	buf_truple_t *t = buf_list2truple(head);
	LOG_SHOW("buf=%s  size=%d\n", t->data, t->size);
	buf_list_free(head);
	buf_free(t);
	
   /* buf_truple_t *buf = buf_create(20);
    strcpy(buf->data, "dfafas");
    buf_new_size(buf, 5);
    LOG_SHOW("buf=%s  size=%d\n", buf->data, buf->size);*/
    
}







