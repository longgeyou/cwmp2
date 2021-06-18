




#ifndef _HTTP_H_
#define _HTTP_H_


#include <../lib/include/common.h>


#include <sys/time.h>
#include <pthread.h>
#include <poll.h>


#define HTTP_BYTE_SIZE_LARGE 1024*4

//http方法
typedef enum http_method_e{
    HTTP_GET,
    HTTP_PUT,
    HTTP_POST,
    HTTP_OPTIONS,
    HTTP_HEAD,
    HTTP_DELETE,
    HTTP_TRACE,
    HTTP_UNKNOWN
}http_method_e;




//套接字结构体
//原型  int socket(int domain, int type, int protocol);
typedef struct http_socket_t{
    int val; //socket 数值
    int domain;
    int type;
    int protocol;
    int addrFamily;
    char addrIp[32];
    int addrPort;
    int maxUserNum;
}http_socket_t;


//摘要认证头部
typedef struct http_digest_auth_t{
	char realm[32];
	char nonce[32];
	char cnonce[32];
	char response[32 + 8];  //注意：要大于32字节，不然会有越界行为
	char qop[32];
	char nc[32];
    char uri[128];
}http_digest_auth_t;


//头部解析结构体
typedef struct http_head_parse_t{
	int method;
	int methodOk;
	
	char uri[128];
	int uriOk;
	
	char host[64];
	int hostOk;
	
	int port;
	int portOk;
	
	char userAgent[64];
	int userAgentOk;
	
	int len;
	int lenOk;

    char userName[54];
	http_digest_auth_t auth;
	int authMark;       //0 没有auth信息  1：有auth信息
}http_head_parse_t;

//用户接受数据结构体
typedef struct http_user_recv_t{
    //状态记录和流程控制
    int en;     //0-拒绝接收；1-开始接收
    int step;       //0-开始，准备接收http头部；1-成功接收头部，准备接收负载；2-成功接收负载
    int httpHeadRecvOk; //0：http头部接收不完整      1：http头部接收成功
    int httpHeadParseOk;    //0：http头部解析失败        1：http头部解析成功
    int authOk;         //0：摘要认证失败  1：摘要认证成功
        
    //数据缓存
    buf_truple_t *head;
    http_head_parse_t headParse;  //http头部解析
    buf_truple_t *payload;      //负载
}http_user_recv_t;


//请求数据头部
typedef struct http_req_header_t{
	http_method_e method;
	char uri[128];
	char host[64];
	int port;
	char userAgent[64];
	int len;

}http_req_header_t;

//请求数据结构体
typedef struct http_request_t{
	http_req_header_t head;
	http_digest_auth_t auth;
	int authUsed;
	buf_truple_t *payload;
	int payloadCreateMark;
	int payloadUsed;
	int payloadByte;

	char userName[64];
	char password[64];
	int ready;
}http_request_t;


//用户
typedef struct http_user_t{
    struct http_user_t *next;

    int enable;     //一般在连接对方成功时使能；使能该用户才能发送http信息
    
    char ip[32];
    int port;
    int fd; 
    time_t buildTime;      //建立的时间
    time_t freshTime;       //刷新的时间

    pthread_t thRecv;       //数据接收线程句柄
    int threadRecvStatus;   //0 代表未开启  1 代表已经开启

    http_user_recv_t recv;  //接收信息

    http_request_t request; //请求

    //接收到数据处理的函数句柄
    int (*recv_data_pro)(http_user_t *);
    int sessionMark[8];
}http_user_t;


//服务器
typedef struct http_server_t{
    http_socket_t socket;
    http_user_t *userList;

    //过程状态
    int socketMark;     //socket 赋值
    int bindMark;       //socket绑定
    int listenMark;     //监听
    
    //线程状态和句柄
    pthread_t thRecv;
    pthread_t thTimeout;
    int thRecvMark;
    int thTimeoutMark;

    int userTimeout;    //用户超时时间，单位是秒
    
    //接收到数据处理的函数句柄（传递给 http_user_t）
    int (*recv_data_pro)(http_user_t *);
}http_server_t;

//客户端
typedef struct http_client_t{
    http_socket_t socket;
    http_user_t user;

    //目标ip和端口
    char destIp[32];
    int destPort;

    //过程状态
    int socketMark;
    int bindMark;
    int connectMark;

    //接收到数据处理的函数句柄（传递给 http_user_t）
    int (*recv_data_pro)(http_user_t *);
}http_client_t;




http_user_t *user_obj_create();
void user_obj_free(http_user_t *user);
void user_obj_init(http_user_t *user);


void user_list_test(void);

//http 读取（非阻塞读取）
int http_recv(http_user_t *user, unsigned char *data, int size);
//http 发送
int http_send(http_user_t *user, unsigned char *data, int size);
http_request_t http_request_create(
    http_req_header_t head,
	http_digest_auth_t auth,
	int authUse,
	char *userName,
	char *password,
	void *data,
	int len);



void http_socket_set(
    http_socket_t *so,
    int domain,
    int type,
    int protocol,
    int addrFamily,
    char *addrIp,
    int addrPort,
    int maxUserNum);

http_server_t *http_server_create(void);
int http_server_start(http_server_t *server);


http_client_t *http_client_create(void);
int http_client_start(http_client_t *client);




//摘要认证计算
int http_calc_digest_response(char * user, char * pwd,
                            char * realm,
                            char * nonce,
                            char * uri,
                            char * cnonce,
                            char * nc,
                            char * qop,
                            char * response);


//http 认证结果
int http_auth_result(http_digest_auth_t auth, char *username, char *response);



#endif







