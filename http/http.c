

#include <../http/include/http.h>
#include <../libexter/include/digcacl.h>
#include <../libexter/include/md5.h>


#include "sys/types.h"//数据类型定义
#include "sys/socket.h" //提供socket函数及数据结构
#include "netinet/in.h" //定义数据结构sockaddr_in
#include "arpa/inet.h" //供IP地址转换函数
#include "unistd.h"




const char http_method_info[][16]={
    "GET",
    "PUT",
    "POST",
    "OPTIONS",
    "HEAD",
    "DELECT",
    "TRACE"
};



const char * RESPONSE_200 = "HTTP/1.1 200 OK\r\n"
                            "Connection: close\r\n"
                            "Content-Length: 2\r\n"
                            "Content-Type: text/xml; charset=\"utf-8\"\r\n"
                            "\r\nOK";
const char * RESPONSE_400 = "HTTP/1.1 400 Bad request\r\n"
                            "Server: CWMP-Agent\r\nConnection: close\r\n"
                            "Content-Length: 5\r\n"
                            "\r\nError";
const char * RESPONSE_401 = "HTTP/1.1 401 Unauthorized\r\n"
                            "WWW-Authenticate: Digest qop=\"%s\" nonce=\"%s\" opaque=\"%s\" realm=\"%s\"\r\n"
                            "Server: TR069Agent\r\n"
                            "Content-Length: 0\r\n"
                            "\r\n";






//=======================================================
//用户链表
//创建链表头
http_user_t *user_list_create(void)
{
    http_user_t *head = (http_user_t *)pool_malloc(sizeof(http_user_t));
    if(head == NULL)
        return NULL;
    head->next = NULL;
    return head;
}
//末尾添加节点
http_user_t *user_list_add_node(http_user_t *head)
{
    if(head == NULL)
        return NULL;
    http_user_t *node = user_obj_create();
    if(node == NULL)
        return NULL;
    node->next = NULL;
    http_user_t *prob = head;
    while(prob->next != NULL)
        prob = prob->next;
    prob->next = node;
    return node;
}
//删除节点
void user_list_del_node(http_user_t *head, http_user_t *node)
{
    if(head == NULL || node == NULL)
        return;
    http_user_t *prob = head->next;
    http_user_t *last = head;
    while(prob != NULL)
    {
        if(prob == node)
        {
            last->next = prob->next;
            user_obj_free(prob);
            return;
        }
        last = prob;
        prob = prob->next;
    }
}
//统计个数
int user_list_get_num(http_user_t *head)
{
    if(head == NULL)
        return 0;
    int cnt = 0;
    http_user_t *prob = head->next;
    while(prob != NULL)
    {
        cnt++;
        prob = prob->next;
    }
    return cnt;
}
//清空链表
void user_list_clear(http_user_t *head)
{   
    if(head == NULL)
        return ;
    http_user_t *prob, *last;
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
        user_obj_free(prob);
    }
}

//显示
void user_list_show(http_user_t *head)
{
    LOG_SHOW("================user list show===================\n");
    if(head == NULL)
    {
        LOG_SHOW("(head is NULL)\n");
    }
    else
    {
        http_user_t *prob = head->next;
        char buf[512];
        int num = user_list_get_num(head); 
        LOG_SHOW("totle num is %d\n", num);
        while(prob != NULL)
        {
            snprintf(buf, 512, "ip=%s  port=%d  fd=%d\n", prob->ip, prob->port, prob->fd);
            LOG_SHOW("%s", buf);
            prob = prob->next;
        }        
    }
    LOG_SHOW("\n");
}
//测试
void user_list_test(void)
{
    http_user_t *head = user_list_create();
    http_user_t *node1 = user_list_add_node(head);
    strcpy(node1->ip, "192.168.1.1");
    node1->port = 8081;
    node1->fd = 1;
    http_user_t *node2 = user_list_add_node(head);
    strcpy(node2->ip, "192.168.1.2");
    node2->port = 8082;
    node2->fd = 2;
    http_user_t *node3 = user_list_add_node(head);
    strcpy(node3->ip, "192.168.1.3");
    node3->port = 8083;
    node3->fd = 3;

    user_list_show(head);

    user_list_del_node(head, node2);
    user_list_show(head);

    user_list_clear(head);
    user_list_show(head);
}
//应用
int user_list_del_timeout(http_user_t *head, int second)
{
    if(head == NULL || second <= 0)
        return -1;
    int num = 0;
    time_t now;
    http_user_t *prob = head->next;
    http_user_t *last = head;
    while(prob != NULL)
    {
        now = time(NULL);   //可能时间无法获取，这里没考虑  
        if(now - prob->freshTime > second)
        {     
            //LOG_DEBUG("========>second=%d\n", second);
            num++;
            last->next = prob->next;
            LOG_INFO("user %s:%d fd=%d is timeout", prob->ip, prob->port, prob->fd);
            user_obj_free(prob);
            prob = last->next; 
            return 1;//continue;
        }        
        last = prob;
        prob = prob->next;
    }   
    return num;
}

//user 对象创建
http_user_t *user_obj_create()
{
    http_user_t *user = (http_user_t *)pool_malloc(sizeof(http_user_t));
    if(user == NULL)
        return NULL;
    user_obj_init(user);
    user->recv.head = buf_list_create();
    user->recv.payload = buf_create(BUF_CREATE_DEFAULT_SIZE);
    
    return user;
}

//user 对象的释放
void user_obj_free(http_user_t *user)
{
    if(user == NULL)
        return;
    //关闭socket
    close(user->fd);
        
    //线程退出
    pthread_cancel(user->thRecv);
    
    //释放内存
    if(user->recv.head != NULL)
        buf_list_free(user->recv.head);
    if(user->request.payload != NULL && user->request.payloadCreateMark == 1)
        buf_free(user->request.payload);
    pool_free(user);

    
    LOG_INFO("user fd=%d has been free", user->fd);
}

//user 对象的初始化
void user_obj_init(http_user_t *user)
{
    if(user == NULL)
        return;
    cfg_t *cfg = get_locl_cfg_manager();
    
    //1、请求的基础配置
    http_request_t *req = &(user->request);
    //1.1 用户名和密码
    strncpy(req->userName, cfg->iniCfg.username, 64);
    strncpy(req->password, cfg->iniCfg.password, 64);
    //1.2 请求头部信息
    strncpy(req->head.host, cfg->iniCfg.host, 64);
    req->head.port = atoi(cfg->iniCfg.port);
    strncpy(req->head.uri, cfg->iniCfg.uri, 128);
    strncpy(req->head.userAgent, cfg->iniCfg.userAgent, 64);
    //1.3 认证信息
    strncpy(req->auth.realm, cfg->iniCfg.auth.realm, 32);
    strncpy(req->auth.nonce, cfg->iniCfg.auth.nonce, 32);
    strncpy(req->auth.cnonce, cfg->iniCfg.auth.cnonce, 32);
    strncpy(req->auth.qop, cfg->iniCfg.auth.qop, 32);
    strncpy(req->auth.nc, cfg->iniCfg.auth.nc, 32);
    strncpy(req->auth.uri, cfg->iniCfg.auth.uri, 128);

}
//=============================================================



//http 头部解析
//"%s %s HTTP/1.1\r\n"
//        "Host: %s:%d\r\n"
//        "User-Agent: %s\r\n"
//        "Accept: */*\r\n"
//        "Content-Type: text/xml; charset=utf-8\r\n"
//        "Content-Length: %d\r\n"
//"Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", cnonce=\"%s\", qop=\"%s\", qnonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n"
//"\r\n"
#define HTTP_HEAD_KEY_VAL_MAX_NUM 32
#define HTTP_HEAD_KEY_VAL_MAX_BYTE 64
#define HTTP_HEAD_FIRST_LINE_VALID_MARK_1 "HTTP/1.1"
#define HTTP_HEAD_FIRST_LINE_VALID_MARK_2 "http/1.1"
/*
char realm[32];
	char nonce[32];
	char cnonce[32];
	char response[32];
	char qop[32];
	char nc[32];
    char uri[128];

*/

int http_head_parse(http_user_t *user)
{
    if(user == NULL)
    {
        LOG_ALARM("user is NULL");
        return RET_CODE_PARAM;
    }
    buf_truple_t *doc = user->recv.head;
    if(doc == NULL)
    {
        LOG_ALARM("doc is NULL");
        return RET_CODE_PARAM;
    }
    http_head_parse_t *parse = &(user->recv.headParse);    
    parse->methodOk = 0;
    parse->uriOk = 0;
    parse->hostOk = 0;   
    parse->portOk = 0;
    parse->userAgentOk = 0;
    parse->lenOk = 0;
    parse->authMark = 0;
    
    //查看第一行是否符合条件
    buf_truple_t *firstLine;
    firstLine = buf_list_get_node(doc, 0);
    if(!((strpro_str_contain(firstLine->data, HTTP_HEAD_FIRST_LINE_VALID_MARK_1) == 1 ||
            strpro_str_contain(firstLine->data, HTTP_HEAD_FIRST_LINE_VALID_MARK_2) == 1)))
    {
        LOG_ALARM("http head first line is not ok");
        return RET_CODE_PROCESS;
    }
    
    //第一行解析
    char *words[3] = {0};
    char strTmp[128];
    strncpy(strTmp, firstLine->data, 128);
    int number = strpro_str2words(strTmp, words, 3);
    if(strcasecmp(words[0], "post") == 0)
        parse->method = HTTP_POST;
    else if(strcasecmp(words[0], "get") == 0)
        parse->method = HTTP_GET;
    else if(strcasecmp(words[0], "put") == 0)
        parse->method = HTTP_PUT;
    else
        parse->method = HTTP_UNKNOWN;
    parse->methodOk = 1;
    strncpy(parse->uri, words[1], 128);
    parse->uriOk = 1;
    //观察http头部结构特点发现，每一行字符串可以根据 "," 或者 ";" 分隔成子串，每个子串可以通过
    // ":" 或者 "=" 分隔为 键值对，最终得到键值对的数组
    //注： http头部第一行内容不符合这里的规律，所以要单独解析
    int i, num, keyValCnt, ret;
    key_value_t keyValArray[HTTP_HEAD_KEY_VAL_MAX_NUM] = {0};
    char out[HTTP_HEAD_KEY_VAL_MAX_NUM][HTTP_HEAD_KEY_VAL_MAX_BYTE];
    char *outp[HTTP_HEAD_KEY_VAL_MAX_NUM];
    char chArray[] = ",;";
    for(i = 0; i < HTTP_HEAD_KEY_VAL_MAX_NUM; i++)
        outp[i] = out[i];
    buf_truple_t *prob = doc->next;
    key_value_t kvTmp, kvTmp2;
    keyValCnt = 0;
    while(prob != NULL)
    {
        num = strpro_divis_by_multiple_ch(chArray, strlen(chArray), prob->data, outp, HTTP_HEAD_KEY_VAL_MAX_NUM, HTTP_HEAD_KEY_VAL_MAX_BYTE);
        for(i = 0; i < num; i++)
        {
            if(keyValCnt < HTTP_HEAD_KEY_VAL_MAX_NUM - 1)
            {
                if(key_value_get_from_str(":=", 2, outp[i],&kvTmp))
                {
                    //去掉key 和 value 两边的无效字符" "、"\r"、"\n"
                    ret = 0;
                    ret += strpro_get_viald_str(kvTmp.key, kvTmp2.key);
                    ret += strpro_get_viald_str(kvTmp.value, kvTmp2.value);
                    if(ret == 2)
                        keyValArray[keyValCnt++] = kvTmp2;
                }  
            }
                
        }
        prob = prob->next;
    }

    //key_value_array_show(keyValArray, keyValCnt);
    //1、得到 “Host"、"User-Agent"、"Content-Length" 对应的值
    char valueTmp[128];
    if(key_value_case_inquire(keyValArray, keyValCnt, "Host", valueTmp, 128))
    {
        strncpy(parse->host, valueTmp, 64);
        parse->hostOk = 1;        
    }
    if(key_value_case_inquire(keyValArray, keyValCnt, "User-Agent", valueTmp, 128))
    {
        strncpy(parse->userAgent, valueTmp, 64);
        parse->userAgentOk = 1;        
    }
    if(key_value_case_inquire(keyValArray, keyValCnt, "Content-Length", valueTmp, 128))
    {
        parse->len = atoi(valueTmp);
        if(!(parse->len == 0 && valueTmp[0] != '0'))
            parse->lenOk = 1;        
    }
    //2、最后一行为"\r\n"，前一行为摘要认证内容，参考rfc2617
    buf_truple_t *authLine;
    prob = doc;
    authLine = prob;
    while(prob->next != NULL)
    {
        authLine = prob;
        prob = prob->next;
    }
    if(authLine != NULL && authLine != doc)
    {
        if(strpro_str_contain(authLine->data, "Authorization"))
            parse->authMark = 1;
    }
    
    
    //3、解析认证内容 username、 realm、 nonce、 cnonce,  qop,  nc,  uri、 response
    if(key_value_case_inquire(keyValArray, keyValCnt, "username", valueTmp, 128))
    { 
        strpro_remove_colon(valueTmp);
        strncpy(parse->userName, valueTmp, 64);
    }
    if(key_value_case_inquire(keyValArray, keyValCnt, "realm", valueTmp, 128))
    {
        strpro_remove_colon(valueTmp);
        strncpy(parse->auth.realm, valueTmp, 32);
    }
    if(key_value_case_inquire(keyValArray, keyValCnt, "nonce", valueTmp, 128))
    {
        strpro_remove_colon(valueTmp);
        strncpy(parse->auth.nonce, valueTmp, 32);
    }
    if(key_value_case_inquire(keyValArray, keyValCnt, "cnonce", valueTmp, 128))
    {
        strpro_remove_colon(valueTmp);
        strncpy(parse->auth.cnonce, valueTmp, 32);
    }
    if(key_value_case_inquire(keyValArray, keyValCnt, "qop", valueTmp, 128))
    {
        strpro_remove_colon(valueTmp);
        strncpy(parse->auth.qop, valueTmp, 32);   
    }
    if(key_value_case_inquire(keyValArray, keyValCnt, "nc", valueTmp, 128))
    {
        strpro_remove_colon(valueTmp);
        strncpy(parse->auth.nc, valueTmp, 32); 
    }
    if(key_value_case_inquire(keyValArray, keyValCnt, "uri", valueTmp, 128))
    {
        strpro_remove_colon(valueTmp);
        strncpy(parse->auth.uri, valueTmp, 128);
    }
    if(key_value_case_inquire(keyValArray, keyValCnt, "response", valueTmp, 128))
    {
        strpro_remove_colon(valueTmp);
        strncpy(parse->auth.response, valueTmp, 32);
    }

#if 0
    LOG_INFO(
    "============parse result=============\n"
    "\nmethod:%d\n"
    "uri:%s\n"
    "Host:%s\n"
    "User-Agent:%s\n"
    "Content-Length:%d\n"
    "username:%s\n"
    "reaml:%s\n"
    "nonce:%s\n"
    "cnonce:%s\n"
    "qop:%s\n"
    "nc:%s\n"
    "uri:%s\n"
    "response:%s\n"
    "authmark=%d\n",
    parse->method, parse->uri, parse->host, parse->userAgent, parse->len,
    parse->userName, parse->auth.realm, parse->auth.nonce, parse->auth.cnonce, 
    parse->auth.qop, parse->auth.nc, parse->auth.uri, parse->auth.response, parse->authMark);
#endif

    return RET_CODE_OK;
}



//================================================================================
//http接收和发送数据
//http 读取（非阻塞读取）
int http_recv(http_user_t *user, unsigned char *data, int size)
{
    int len;
	len = recv(user->fd, data, size, MSG_DONTWAIT);
	return len;
}

//http 读取一行（注意：要保证size大小在数组data的范围之内，不然会有数组溢出错误）
int http_recv_line(http_user_t *user, unsigned char *data, int size)
{
    unsigned char ch;
    int ret, i;
    for(i = 0; i < size; i++)
    {
        ret = recv(user->fd, &ch, 1, MSG_DONTWAIT);
        if(ret == 0)
            return 0;
        else if(ret > 0)
        {
            data[i] = ch;
            if(ch == '\n' && i >= 1 && data[i - 1] == '\r')
                return i + 1;
        }
        else if(ret == -1)
        {
            if(i == 0)
                return -1;
            return i;
        }
    }
    return -2;  //表示没有接收到换行符
}

//http接收头部
#define RECV_HEAD_LINE_MAX_SIZE 1024
#define HTTP_HEAD_MAX_LINE_NUM 16
int http_recv_head(http_user_t *user)
{
    if(user == NULL)
        return -2;
     if(user->recv.head == NULL)
        return -2;
    int ret, recvLen, cnt;
    unsigned char tmp[RECV_HEAD_LINE_MAX_SIZE] = {0};
    ret = http_recv_line(user, tmp, RECV_HEAD_LINE_MAX_SIZE);
    recvLen = 0; cnt = 0; user->recv.httpHeadRecvOk = 0;
    if(ret == -2)
        return -3;  //没有接收到http头的标志
    if(ret == -1)
        return -1;  //无数据
    if(ret == 0)
        return 0;   //对方断开连接
    
    if(strpro_str_contain(tmp, HTTP_HEAD_FIRST_LINE_VALID_MARK_1) == 1 || 
         strpro_str_contain(tmp, HTTP_HEAD_FIRST_LINE_VALID_MARK_2) == 1)
    {
        recvLen += ret;
        buf_list_in(user->recv.head, tmp, ret);
        while(1)
        {
            ret = http_recv_line(user, tmp, 1024);
            if(ret == -1)
                return -1;
            if(ret == -2)
                return recvLen;          //http头部的某一行字节数超过限定长度
            else if(ret == 0)
                return 0;
            else if(ret > 0)
            {
                 recvLen += ret;
                buf_list_in(user->recv.head, tmp, ret);
                if(tmp[0] == '\r' && tmp[1] == '\n')
                {
                    user->recv.httpHeadRecvOk = 1;
                    return recvLen;
                }                              
            }
                           
            cnt++;
            if(cnt >= HTTP_HEAD_MAX_LINE_NUM)
                return recvLen;            
        }
    }
    else
    {
        buf_list_in(user->recv.head, tmp, ret);
        return ret;
    }
  
    return -5;  
}

//线程：用户的数据接收
void * http_user_recv_data_thread(void *arg)
{
    if(arg == NULL)
    {
        LOG_ERROR("arg is NULL");
        return NULL;
    }
    http_user_t *user = (http_user_t *)arg;
    http_user_recv_t *recv = &(user->recv);

    //参数检测
    if(user->fd < 0)
    {
        LOG_INFO("fd is not ok");
        return NULL;
    }
   struct pollfd fds[1] = {0};
   fds[0].fd = user->fd;
   fds[0].events = POLLIN;
   int recvRet, ret2;
   recv->en = 1;
   recv->step = 0;
    while(1)
    {
        poll(fds, 1, 10000);//超时等待
        if((fds[0].revents & POLLIN) && recv->en)
        {
            char buf[1024] = {0};
            //更新刷新时间
            user->freshTime = time(NULL);;
            //LOG_DEBUG("poll is comming, fd is %d\n", user->fd);
            switch(recv->step)
            {
                case 0:     //准备接收头部
                {
                    buf_list_clear(recv->head);
                    recvRet = http_recv_head(user);
                    if(recv->httpHeadRecvOk == 1)
                    {                      
                        LOG_DEBUG("recv http head OK");
                        http_head_parse(user);
                        //摘要认证
                        if(recv->headParse.authMark == 0)//没有摘要认证，回复401
                        {   
                            ;
                        }
                        else    
                        {
                            ret2 = http_auth_result(recv->headParse.auth, recv->headParse.userName, 
                                                    recv->headParse.auth.response);  
                            LOG_DEBUG("auth result %d", ret2);
                            recv->step = 1;
                        }        
                    }                      
                }
                break;
                case 1:     //准备接收负载
                {
                    //LOG_DEBUG("lenOk=%d  len=%d\n", recv->headParse.lenOk, recv->headParse.len);
                    if(recv->headParse.lenOk == 1 && recv->headParse.len > 0)   
                    {   
                        buf_new_size(recv->payload, recv->headParse.len);
                        recvRet = http_recv(user, recv->payload->data, recv->headParse.len);
                        LOG_DEBUG("recv payload, ret=%d", recvRet);
                        buf_show(recv->payload);
                        recv->step = 2;
                    }                     
                }
                break;
                case 2:     //等待处理
                {
                    user->recv_data_pro(user);
                }
                break;
                default:
                {
                    recv->step = 0;
                }
                break;
            }

            
            if(recvRet > 0)
            {
                if(recv->step == 0)
                {
                    if(recv->httpHeadRecvOk == 1)
                    {
                        LOG_INFO("recv http head, size=%d\n", buf_list_get_size(recv->head));               
                        //buf_list_show(user->recv.head);
                    }
                    else if(recv->head->next != NULL)
                    {
                        LOG_SHOW("recv info, size=%d\n", strlen(recv->head->next->data));
                        LOG_SHOW("%s[end mark]\n",  recv->head->next->data);
                    }
                }   
            }
            else if(recvRet == 0)
            {
                LOG_INFO("fd=%d loses connect", user->fd);
                recv->en = 0;
            }
            else if(recvRet == -1)
            {
                LOG_ALARM("recv return -1");
            }
        }
        else
        {
            sleep(1);
        }
    }

}


//http 发送
int http_send(http_user_t *user, unsigned char *data, int size)
{
    int len;
    len = send(user->fd, data, size, 0);
    return len;
}


http_request_t http_request_create(
    http_req_header_t head,
	http_digest_auth_t auth,
	int authUse,
	char *userName,
	char *password,
	void *data,
	int len)
{
    http_request_t req;
    req.head = head;
    req.auth = auth;
    req.authUsed = authUse;
    if(userName != NULL)
        strncpy(req.userName, userName, 64);
    if(password != NULL)
        strncpy(req.password, password, 64);

    req.payload = buf_create(BUF_CREATE_DEFAULT_SIZE);
    req.payloadCreateMark = 1;
        
    req.payloadUsed = 0;
    if(data != NULL && req.payload != NULL) 
    {
        req.payloadUsed = 1;
        buf_set_value(req.payload, (unsigned char *)data, len);
        req.payloadByte = len;
    }
    req.ready = 1;
    return req;
}

//http发送请求
int http_send_request(http_user_t *user)
{
    //LOG_SHOW("======================mark 0\n");
    if(user == NULL)
    {
        LOG_ALARM("user is NULL");
        return RET_CODE_PARAM;
    }
    http_request_t *req = &(user->request); 
    if(req->ready != 1)
    {
        LOG_ALARM("request is not ready");
        return RET_CODE_PROCESS;
    }
    if(user->enable != 1)
    {
        LOG_ALARM("user is not enable");
        return RET_CODE_PROCESS;
    }
#if 0    
    if(req->payload == NULL || req->payloadCreateMark == 0)
    {
        req->payload = buf_create(BUF_CREATE_DEFAULT_SIZE);
        req->payloadCreateMark = 1;
        req->payloadUsed = 0;
    }
#endif
    int len, lenTmp;
    char bufTmp[1024] = {0};

    len = 0;
    buf_truple_t *buf = buf_create(BUF_CREATE_DEFAULT_SIZE);  
    if(buf == NULL)
    {
        LOG_ERROR("buf is NULL");
        return RET_CODE_PROCESS;
    }
        
    //LOG_SHOW("======================mark 1\n");
    char fmtHead[] = "%s %s HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "User-Agent: %s\r\n"
        "Accept: */*\r\n"
        "Content-Type: text/xml; charset=utf-8\r\n"
        "Content-Length: %d\r\n";
    if(req->head.method > HTTP_UNKNOWN || req->head.method < 0)
        req->head.method = HTTP_UNKNOWN;
    lenTmp = snprintf(bufTmp, 1024, fmtHead,
                        http_method_info[req->head.method],
                        req->head.uri,
                        req->head.host,
                        req->head.port,
                        req->head.userAgent,
                        req->payloadByte);       
    //LOG_SHOW("======================mark 2\n");
    buf_append_data(buf, bufTmp, lenTmp);
    len += lenTmp;
    //LOG_DEBUG("[1]========>lenTmp=%d", lenTmp);
    if(req->authUsed)
    {   
        http_calc_digest_response(req->userName, req->password,
                    req->auth.realm,
                    req->auth.nonce,
                    req->auth.uri,
                    req->auth.cnonce,
                    req->auth.nc,
                    req->auth.qop,
                    req->auth.response);
        char fmtAuth[] = "Authorization: Digest ,"
                         "username=\"%s\", "
                         "realm=\"%s\", "
                         "nonce=\"%s\", "
                         "uri=\"%s\", "
                         "response=\"%s\", "
                         "cnonce=\"%s\", "
                         "qop=\"%s\", "
                         "nc=\"%s\"\r\n";                
        lenTmp = snprintf(bufTmp, 1024, fmtAuth,
                        req->userName,
                        req->auth.realm,
                        req->auth.nonce,
                        req->auth.uri,
                        req->auth.response,
                        req->auth.cnonce,
                        req->auth.qop,
                        req->auth.nc);
        buf_append_data(buf, bufTmp, lenTmp);
        len += lenTmp;
        //LOG_DEBUG("[2]========>lenTmp=%d", lenTmp);
    }    
    //LOG_SHOW("======================mark 3 \n"); 
    buf_append_data(buf, "\r\n", 2);
    len += 2;
    if(req->payloadUsed)
    {
         buf_append_data(buf, req->payload->data, req->payloadByte); 
         len += req->payloadByte;
    }
    int sendLen = http_send(user, buf->data, len);
    //LOG_DEBUG("[3]========>payloadByte=%d", req->payloadByte);
    LOG_DEBUG("send size=%d fd=%d", sendLen, user->fd);
    buf_show(buf);
    buf_free(buf);
    return RET_CODE_OK;

}

int http_send_request_by_method(http_user_t *user, int method)
{
    if(user == NULL)
        return RET_CODE_PARAM;
    user->request.head.method = method;
    return http_send_request(user);
}

int http_post(http_user_t *user, unsigned char *data, int len)
{
    //LOG_DEBUG("=========================>mark 1\n");
    if(user == NULL || data == NULL)
        return RET_CODE_PARAM;
    //LOG_DEBUG("=========================>mark 3\n");
    if(user->request.payload == NULL)
        return RET_CODE_PROCESS;
    buf_set_value(user->request.payload, data, len);
    user->request.payloadByte = len;
    user->request.payloadUsed = 1;
    user->request.ready = 1;
    //LOG_DEBUG("=========================>mark 2\n");
    return http_send_request_by_method(user, HTTP_POST);
}
//================================================================================




//socket 赋值
void http_socket_set(
    http_socket_t *so,
    int domain,
    int type,
    int protocol,
    int addrFamily,
    char *addrIp,
    int addrPort,
    int maxUserNum)
{
    so->domain = domain;
    so->type = type;
    so->protocol = protocol;
    so->addrFamily = addrFamily;
    strcpy(so->addrIp, addrIp);
    so->addrPort = addrPort;
    so->maxUserNum = maxUserNum;
}


//socket绑定ip和端口
int http_socket_bind(http_socket_t *so)
{
    int ret;
    //LOG_INFO("http_bind is comming.......");
    //参数检测
    if(so == NULL)
        return RET_CODE_PARAM;
    if(so->addrPort < 0 || so->addrPort > 65536)
    {
        LOG_ERROR("socket port exceed limmit");
        return RET_CODE_PARAM;
    }
        
    //创建套接字
	so->val = socket(so->domain, so->type, so->protocol);
	if(so->val < 0)
	{
		LOG_ERROR("create socket failed");
	    return RET_CODE_PROCESS;
	}
	struct sockaddr_in  addr;
	addr.sin_family = so->addrFamily;
	addr.sin_addr.s_addr = inet_addr(so->addrIp);
    addr.sin_port = htons(so->addrPort);  
    	
	//绑定ip和端口号
	ret = bind(so->val, (struct sockaddr *)(&addr), sizeof(struct sockaddr_in));
	if(ret != 0)
	{
		LOG_ERROR("scoket bind failed");
		goto faild;
	}
	LOG_INFO("bind %s[%d]  successfully, fd=%d", so->addrIp, so->addrPort, so->val);
    return RET_CODE_OK;
faild:
    close(so->val);
    return RET_CODE_PROCESS;
}


//====================================================================
//服务器操作
//创建服务器
http_server_t *http_server_create(void)
{
    http_server_t *server = (http_server_t *)pool_malloc(sizeof(http_server_t));
    server->socketMark = 0;
    server->bindMark = 0;
    server->listenMark = 0;
    server->userTimeout = 1;
    server->userList = user_list_create();
    return server;
}

//服务器开始监听
int http_server_listen(http_server_t *server)
{
    if(server == NULL)
        return RET_CODE_PARAM;
    int ret;
    ret = listen(server->socket.val, server->socket.maxUserNum);
	if(ret < 0)
	{
		LOG_ERROR("listen is error\r\n");
		return RET_CODE_PROCESS;
	}  
	server->listenMark = 1;
    return RET_CODE_OK;
}
//接受用户的连接
//注：注意指针参数作为输出量，需要传入指针的指针，才能保证得到修改后的值；两次遇到这种错误了......
int http_accept_user_connect(http_server_t *server, http_user_t **newUser)
{   
    unsigned char *ipv4;
    struct sockaddr_in addr;
	int len = sizeof(struct sockaddr_in);
	int newid = -1;
	*newUser = NULL;
    if(server == NULL)
    {
        LOG_ERROR("server is NULL");
        return RET_CODE_PARAM;
    }

    if(server->socketMark == 0 || server->bindMark == 0 || server->listenMark == 0)
    {
        LOG_SHOW("server obj is not ready");
        return RET_CODE_PARAM;
    }

     //查看用户个数是否超过限制
    if(user_list_get_num(server->userList) >= server->socket.maxUserNum)
    {
        LOG_ERROR("user num is full");
        return RET_CODE_OVERFLOW;
    }

	//接受用户的连接
    newid = accept(server->socket.val, (struct sockaddr *)&addr, &len);
    if(newid == -1)
    {
        LOG_ERROR("accept error");
        return RET_CODE_PROCESS;
    } 
    ipv4 = (unsigned char *)(&(addr.sin_addr.s_addr));  //默认只支持ipv4，有待扩展
    LOG_INFO("newid=%d; accept user from ip:%d.%d.%d.%d:%d", newid, ipv4[0], ipv4[1], ipv4[2], ipv4[3], (ntohs(addr.sin_port)));
    
    //用户信息存入链表
    http_user_t *node = user_list_add_node(server->userList);
    if(node == NULL)
    {
        LOG_ERROR("node is NULL");
        return RET_CODE_PROCESS;
    }
    sprintf(node->ip, "%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
    node->port = ntohs(addr.sin_port);
    node->fd = newid;
    node->buildTime = time(NULL);
    node->freshTime = node->buildTime;
    node->enable = 1;
    node->recv_data_pro = server->recv_data_pro;
    
    *newUser = node;
    
    //LOG_DEBUG("========>newUser=%p node=%p\n", *newUser, node);  
    return RET_CODE_OK;
}
//线程：处理用户的连接
void * httpd_server_accept_thread(void *arg)
{
    if(arg == NULL)
    {   
        LOG_ERROR("arg is NULL");
        return NULL;
    } 
    
    http_server_t *server = (http_server_t *)arg;
    //参数检测
    if(server->socket.val < 0)
        return NULL;
   struct pollfd fds[1] = {0};
   fds[0].fd = server->socket.val;
   fds[0].events = POLLIN;
   int ret;
   http_user_t *newUser;
    while(1)
    {
        poll(fds, 1, 10000);//超时等待
        if(fds[0].revents & POLLIN)
        {
            //LOG_INFO("poll is comming");
            ret = http_accept_user_connect(server, &newUser);
            if(newUser == NULL)
            {
                LOG_ALARM("newUser is NULL");
                continue;
            }
            if(ret == RET_CODE_OK)
            {
                //开启数据接收线程
                //LOG_INFO("thread create is comming......\n");
                pthread_t th;
                ret = pthread_create(&th, NULL, http_user_recv_data_thread, (void *)newUser);
                if(ret == -1)
                {
                    LOG_ALARM("pthread create faild");
                     newUser->threadRecvStatus = 0;
                }
                else
                {
                    //LOG_INFO("pthread create OK");
                    newUser->thRecv = th;
                    newUser->threadRecvStatus = 1;                     
                }              
            }
        }
        else
        {
            sleep(1);
        }
    }
}
//线程：用户连接超时检测
void *http_server_timeout_thread(void *arg)
{
    if(arg == NULL)
    {   
        LOG_ERROR("arg is NULL");
        return NULL;
    } 
    
    http_server_t *server = (http_server_t *)arg;
    while(1)
    {
        user_list_del_timeout(server->userList, server->userTimeout);
        sleep(2);
    }

}
//开启服务器
int http_server_start(http_server_t *server)
{
    int ret;
    if(server == NULL)
    {
        LOG_ERROR("server is NULL");
        return RET_CODE_PARAM;
    }

    if(server->socketMark == 0)
    {
        LOG_SHOW("server socket is not ready");
        return RET_CODE_PARAM;
    }    
    //socket绑定
    ret = http_socket_bind(&(server->socket));
    if(ret != RET_CODE_OK)
    {
        LOG_INFO("socket bind faild");
        close(server->socket.val);
        return RET_CODE_PROCESS;
    }
    server->bindMark = 1;
    //监听
    ret = http_server_listen(server);
    if(ret != RET_CODE_OK)
    {
        LOG_INFO("server listen is faild");
        close(server->socket.val);
        return RET_CODE_PROCESS;
    }
    server->listenMark = 1;

    //开启线程：接收新的用户
    pthread_t thRecv;
    ret = pthread_create(&thRecv, NULL, httpd_server_accept_thread, (void *)server);
    if(ret == -1)
    {
        LOG_INFO("recv thread create faild");
        server->thRecvMark = 0;
        return RET_CODE_PROCESS;
    }
    else
    {
        server->thRecv = thRecv;
        server->thRecvMark = 1;
    }

    //开启线程：超时清除用户
    pthread_t thTimeout;
    ret = pthread_create(&thTimeout, NULL, http_server_timeout_thread, (void *)server);
     if(ret == -1)
    {
        LOG_INFO("timeout thread create faild");
        server->thTimeoutMark = 0;
        return RET_CODE_PROCESS;
    }
    else
    {
        server->thTimeout = thTimeout;
        server->thTimeoutMark = 1;
    }
    return RET_CODE_OK;
}
//======================================================================================



//======================================================================================
//客户端操作
//创建客户端
http_client_t *http_client_create(void)
{
   http_client_t *client = (http_client_t *)pool_malloc(sizeof(http_client_t));
   client->socketMark = 0;
   client->bindMark = 0;
   client->connectMark = 0;
   return client;
}

//连接到服务器
int http_client_connect_to_server(http_client_t *client)
{
    if(client == NULL)
    {
        LOG_ERROR("client is NULL");
        return RET_CODE_PARAM;
    }

    int ret;
    socklen_t len;
    struct sockaddr_in  addr;
	addr.sin_family = client->socket.addrFamily;
	addr.sin_addr.s_addr = inet_addr(client->destIp);
    addr.sin_port = htons(client->destPort);
    len = sizeof(struct sockaddr_in);
    
    //连接到服务器
	//LOG_INFO("connect......fd=%d", client->socket.val);
    ret = connect(client->socket.val, (const struct sockaddr *)&addr, len);
    if(ret < 0)
    {
        LOG_ERROR("connect error, destIp=%s detsPort=%d", client->destIp, client->destPort);
        return RET_CODE_PROCESS;
    }
    
    LOG_INFO("connect to %s:%d successed", client->destIp, client->destPort);

    return RET_CODE_OK;

}

//开启客户端
int http_client_start(http_client_t *client)
{
    int ret;
    if(client == NULL)
    {
        LOG_ERROR("client is NULL");
        return RET_CODE_PARAM;
    }
    //查看socket是否已配置
    if(client->socketMark == 0)
    {
        LOG_ALARM("socket is not ready");
        return RET_CODE_PROCESS;
    }
    //socket绑定
    client->bindMark = 0;
    ret = http_socket_bind(&(client->socket));
    if(ret != RET_CODE_OK)
    {
        LOG_ERROR("socket bind faild");
        return RET_CODE_PROCESS;
    }
    client->user.fd = client->socket.val;
    client->bindMark = 1;
    //连接服务器
    client->connectMark = 0;
    ret = http_client_connect_to_server(client);
    if(ret != RET_CODE_OK)
    {
        LOG_ERROR("socket connect faild");
        close(client->socket.val);
        return RET_CODE_PROCESS;
    }
    client->connectMark = 1;
    client->user->enable = 1;
    client->user.recv_data_pro = client->recv_data_pro;
    //开启数据接收线程
    pthread_t th;
    ret = pthread_create(&th, NULL, http_user_recv_data_thread, (void *)(&(client->user)));
    if(ret == -1)
    {
        LOG_ALARM("pthread create faild");
        client->user.threadRecvStatus = 0;
    }
    else
    {
        //LOG_INFO("pthread create OK");
        client->user.thRecv = th;
        client->user.threadRecvStatus = 1;                     
    }    
    
    return RET_CODE_OK;
}
//======================================================================================

//摘要认证计算
int http_calc_digest_response(char * user, char * pwd,
                            char * realm,
                            char * nonce,
                            char * uri,
                            char * cnonce,
                            char * nc,
                            char * qop,
                            char * response)
{
    char HA1[HASHHEXLEN+1];
    char HA2[HASHHEXLEN+1];
    char respTmp[HASHHEXLEN+1];
    char pszAlg[] = "MD5";
    char pszMethod[] = "POST";    
    char HEntity[HASHHEXLEN+1] = {0};       
    DigestCalcHA1(pszAlg, user, realm, pwd, nonce, cnonce, HA1);
    DigestCalcResponse( HA1, nonce, nc, cnonce, qop, pszMethod,  uri, HEntity, respTmp);
    strncpy(response, respTmp, HASHHEXLEN);
    response[HASHHEXLEN] = '\0';
    return RET_CODE_OK;
}


//http 认证结果
int http_auth_result(http_digest_auth_t auth, char *username, char *response)
{
    if(username == NULL || response == NULL)
        return -1;
    char responseTmp[32 + 8];
    char password[256] = {0};
    int ret;
    
    cfg_t *cfg = get_locl_cfg_manager();
    if(cfg == NULL)
        return 0;
    ret = key_value_inquire(cfg->userCfg.userArray, cfg->userCfg.userNum, username, password, 256);
    if(ret == 0)    //找到用户密码
        return 0;
    
    http_calc_digest_response(username, password, auth.realm, auth.nonce,
                                auth.uri, auth.cnonce, auth.nc, auth.qop, responseTmp);

    LOG_INFO("\nresponse:\t%s\nresponseTmp:\t%s\n", response, responseTmp);
    if(strncmp(response, responseTmp, 32) == 0)
        return 1;
    return 0;
}


void http_test(void)
{
   ;
}







