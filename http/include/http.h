




#ifndef _HTTP_H_
#define _HTTP_H_


#include <../lib/include/common.h>


#include <sys/time.h>
#include <pthread.h>
#include <poll.h>


#define HTTP_BYTE_SIZE_LARGE 1024*4

//http����
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




//�׽��ֽṹ��
//ԭ��  int socket(int domain, int type, int protocol);
typedef struct http_socket_t{
    int val; //socket ��ֵ
    int domain;
    int type;
    int protocol;
    int addrFamily;
    char addrIp[32];
    int addrPort;
    int maxUserNum;
}http_socket_t;


//ժҪ��֤ͷ��
typedef struct http_digest_auth_t{
	char realm[32];
	char nonce[32];
	char cnonce[32];
	char response[32 + 8];  //ע�⣺Ҫ����32�ֽڣ���Ȼ����Խ����Ϊ
	char qop[32];
	char nc[32];
    char uri[128];
}http_digest_auth_t;


//ͷ�������ṹ��
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
	int authMark;       //0 û��auth��Ϣ  1����auth��Ϣ
}http_head_parse_t;

//�û��������ݽṹ��
typedef struct http_user_recv_t{
    //״̬��¼�����̿���
    int en;     //0-�ܾ����գ�1-��ʼ����
    int step;       //0-��ʼ��׼������httpͷ����1-�ɹ�����ͷ����׼�����ո��أ�2-�ɹ����ո���
    int httpHeadRecvOk; //0��httpͷ�����ղ�����      1��httpͷ�����ճɹ�
    int httpHeadParseOk;    //0��httpͷ������ʧ��        1��httpͷ�������ɹ�
    int authOk;         //0��ժҪ��֤ʧ��  1��ժҪ��֤�ɹ�
        
    //���ݻ���
    buf_truple_t *head;
    http_head_parse_t headParse;  //httpͷ������
    buf_truple_t *payload;      //����
}http_user_recv_t;


//��������ͷ��
typedef struct http_req_header_t{
	http_method_e method;
	char uri[128];
	char host[64];
	int port;
	char userAgent[64];
	int len;

}http_req_header_t;

//�������ݽṹ��
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


//�û�
typedef struct http_user_t{
    struct http_user_t *next;

    int enable;     //һ�������ӶԷ��ɹ�ʱʹ�ܣ�ʹ�ܸ��û����ܷ���http��Ϣ
    
    char ip[32];
    int port;
    int fd; 
    time_t buildTime;      //������ʱ��
    time_t freshTime;       //ˢ�µ�ʱ��

    pthread_t thRecv;       //���ݽ����߳̾��
    int threadRecvStatus;   //0 ����δ����  1 �����Ѿ�����

    http_user_recv_t recv;  //������Ϣ

    http_request_t request; //����

    //���յ����ݴ���ĺ������
    int (*recv_data_pro)(http_user_t *);
    int sessionMark[8];
}http_user_t;


//������
typedef struct http_server_t{
    http_socket_t socket;
    http_user_t *userList;

    //����״̬
    int socketMark;     //socket ��ֵ
    int bindMark;       //socket��
    int listenMark;     //����
    
    //�߳�״̬�;��
    pthread_t thRecv;
    pthread_t thTimeout;
    int thRecvMark;
    int thTimeoutMark;

    int userTimeout;    //�û���ʱʱ�䣬��λ����
    
    //���յ����ݴ���ĺ�����������ݸ� http_user_t��
    int (*recv_data_pro)(http_user_t *);
}http_server_t;

//�ͻ���
typedef struct http_client_t{
    http_socket_t socket;
    http_user_t user;

    //Ŀ��ip�Ͷ˿�
    char destIp[32];
    int destPort;

    //����״̬
    int socketMark;
    int bindMark;
    int connectMark;

    //���յ����ݴ���ĺ�����������ݸ� http_user_t��
    int (*recv_data_pro)(http_user_t *);
}http_client_t;




http_user_t *user_obj_create();
void user_obj_free(http_user_t *user);
void user_obj_init(http_user_t *user);


void user_list_test(void);

//http ��ȡ����������ȡ��
int http_recv(http_user_t *user, unsigned char *data, int size);
//http ����
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




//ժҪ��֤����
int http_calc_digest_response(char * user, char * pwd,
                            char * realm,
                            char * nonce,
                            char * uri,
                            char * cnonce,
                            char * nc,
                            char * qop,
                            char * response);


//http ��֤���
int http_auth_result(http_digest_auth_t auth, char *username, char *response);



#endif







