




#include <include/cpe.h>
#include <../http/include/http.h>
#include <../lib/include/common.h>
#include <../cwmp/include/ini.h>



#include "sys/types.h"//数据类型定义
#include "sys/socket.h" //提供socket函数及数据结构




#define CFG_HOME_DIR "/mnt/hgfs/share/PRO/cwmp2/cpe"
#define CFG_LOG_DIR "log"
#define CFG_LOG_FILE_NAME "log.txt"
#define CFG_TEST_DIR "test"
#define CFG_TEST_FILE_NAME "test.txt"
#define CFG_CFG_DIR "cfg"
#define CFG_CFG_FILE_NAME "cfg.txt"


cfg_t *cpe_cfg_init(void)
{
    cfg_t *cfg = get_locl_cfg_manager();
    //目录配置
    dir_cfg_t *dir = &(cfg->dirCfg);
    strncpy(dir->homeDir, CFG_HOME_DIR, CFG_DIR_NAME_MAX_BYTE);
    snprintf(dir->logFileFullName, CFG_DIR_NAME_MAX_BYTE, "%s/%s/%s", 
            CFG_HOME_DIR, CFG_LOG_DIR, CFG_LOG_FILE_NAME);
    snprintf(dir->testFileFullName, CFG_DIR_NAME_MAX_BYTE, "%s/%s/%s",
            CFG_HOME_DIR, CFG_TEST_DIR, CFG_TEST_FILE_NAME);
    snprintf(dir->cfgFileFullName, CFG_DIR_NAME_MAX_BYTE, "%s/%s/%s",
            CFG_HOME_DIR, CFG_CFG_DIR, CFG_CFG_FILE_NAME);   
    
    
    //ini配置信息
    ini_cfg_t *ini = &(cfg->iniCfg);
    buf_truple_t *doc = buf_improt_file(dir->cfgFileFullName, BUF_IMPORT_FILE_DEFAULT_SIZE);
    ini_list_t *iniList = ini_doc2list(doc);
    ini_key_value_t *key;
    key = ini_get_keyvalue_by_name(iniList, "cwmp:username");
    if(key != NULL)
        strncpy(ini->username, key->value, 128);
    key = ini_get_keyvalue_by_name(iniList, "cwmp:password");
    if(key != NULL)
        strncpy(ini->password, key->value, 128);      
    key = ini_get_keyvalue_by_name(iniList, "cwmp:uri");
    if(key != NULL)
        strncpy(ini->uri, key->value, 128);
    key = ini_get_keyvalue_by_name(iniList, "cwmp:host");
    if(key != NULL)
        strncpy(ini->host, key->value, 128);  
    key = ini_get_keyvalue_by_name(iniList, "cwmp:port");
    if(key != NULL)
        strncpy(ini->port, key->value, 32);  
    key = ini_get_keyvalue_by_name(iniList, "cwmp:userAgent");
    if(key != NULL)
        strncpy(ini->userAgent, key->value, 32);
    key = ini_get_keyvalue_by_name(iniList, "cwmp:auth-realm");
    if(key != NULL)
        strncpy(ini->auth.realm, key->value, 32);  
    key = ini_get_keyvalue_by_name(iniList, "cwmp:auth-nonce");
    if(key != NULL)
        strncpy(ini->auth.nonce, key->value, 32);  
    key = ini_get_keyvalue_by_name(iniList, "cwmp:auth-qop");
    if(key != NULL)
        strncpy(ini->auth.qop, key->value, 32);  
    key = ini_get_keyvalue_by_name(iniList, "cwmp:auth-uri");
    if(key != NULL)
        strncpy(ini->auth.uri, key->value, 128);  
    buf_list_free(doc);   
    //ini 配置中的用户及其密码
    int i;
    ini_key_value_t *key1, *key2;
    cfg->userCfg.userNum = 0;
    for(i = 0;;i++)
    {
        key1 = ini_get_keyvalue_by_name_and_idx(iniList, "cwmp:username", i);
        key2 = ini_get_keyvalue_by_name_and_idx(iniList, "cwmp:password", i);
        if(key1 != NULL && key2 !=  NULL)
        {
            strncpy(cfg->userCfg.userArray[cfg->userCfg.userNum].key, key1->value, KEY_NAME_MAX_BYTE); 
            strncpy(cfg->userCfg.userArray[cfg->userCfg.userNum].value, key2->value, VALUE_STR_MAX_BYTE); 
            cfg->userCfg.userNum++;
        }
        else
            break;
    }
    ini_model_list_clear(iniList);

            
    return cfg;           
}


void cpe_init(void)
{

    ;
}




void cpe_test(void)
{
    http_client_t *client = http_client_create();
    //配置socket
    http_socket_set(
    &(client->socket),
    AF_INET,
    SOCK_STREAM,
    0,
    AF_INET,
    "127.0.0.1",
    8080,  
    0);
    client->socketMark = 1;
    //配置目标ip和端口
    strcpy(client->destIp, "192.168.137.20");
    client->destPort = 8080;
    
    http_client_start(client);

#if 0
    char fmt[] = "POST / HTTP/1.1\r\n"
    "Host: 127.0.01:8080\r\n"
    "User-Agent: cpe-arent\r\n"
    "Accept: */*\r\n"
    "Content-Type: text/xml; charset=utf-8\r\n"
    "Content-Length: %d\r\n"
    "Authorization: Digest ;username=\"long\", realm=\"abcdefg\", nonce=\"hijklmn\", cnonce=\"opqrest\", "
    "qop=\"auth\", nc=\"aa\", uri=\"/mnt/long\", response=\"hhishodfhaiohfdisf1\"\r\n\r\n";
    
    char sendHead[1024] = {0};
    char sendData[] = "message\r\n";
    sprintf(sendHead, fmt, strlen(sendData));
    
    http_send(&(client->user), sendHead, strlen(sendHead));
    http_send(&(client->user), sendData, strlen(sendData));
#else
    http_req_header_t head;
	http_digest_auth_t auth;
	int authUse;
	char userName[] = "long";
	char password[] = "123456";
    strcpy(head.host, "192.168.137.20");
    head.port = 8080;
    strcpy(head.uri, "/");
    strcpy(head.userAgent, "cpe agent");
    strcpy(auth.realm, "find");
    strcpy(auth.nonce, "my");
    strcpy(auth.cnonce, "best");
    strcpy(auth.qop, "auth");
    strcpy(auth.nc, "status");
    strcpy(auth.uri, "/");
	
    client->user.request = http_request_create(head, auth, 1, userName, password, NULL, 0);
    http_post(&(client->user), "message", strlen("message"));     
#endif
    sleep(5);

    close(client->user.fd);
}




typedef struct cpe_session_t{
    http_client_t *client;
    //状态表示和流程控制
    int step;   
}cpe_session_t;



int cpe_recv_data_pro(http_user_t *user)
{
    


}

//cpe会话
//0 向服务器发起连接，发送inform信息
//1 发送http post，可以附带acs方法
//2 接收htpp 的回应，响应soap信封内容
//3 重复1、2步骤，直到得到终止会话的信息（包括超时、多次未接收到有效http回应），结束会话
int cpe_session(cpe_session_t *session)
{
    session->step = 0;
    http_user_t *user = session->client->user;
    if(user == NULL)
    {
        LOG_ERROR("user isn NULL");
        return RET_CODE_PARAM;
    }
    int ret;
    
    while(1)
    {
        switch(session->step)
        {
            case 0:
            {
                http_post(user, "inform message", strlen("inform message"));  
                session->step = 1;
            }
            break;
            case 1:
            {
                http_post(user, "cpe send ...", strlen("cpe send ..."));  
                session->step = 1;
            }
            break;
            case 2:
            {
                
            }
            break;
            case 3:
            break;
            default:
            break;
        }

        sleep(2);
    }

}


void cpe_test2(void)
{

    
    http_client_t *client = http_client_create();
    //配置socket
    http_socket_set(
    &(client->socket),
    AF_INET,
    SOCK_STREAM,
    0,
    AF_INET,
    "127.0.0.1",
    8080,  
    0);
    client->socketMark = 1;
    //配置目标ip和端口
    strcpy(client->destIp, "192.168.137.20");
    client->destPort = 8080;
    //数据接处理收函数
    client->recv_data_pro = cpe_recv_data_pro;
    http_client_start(client);

    //创建会话
    cpe_session_t session;
    session->client = client;
    if(client != NULL)
        cpe_session(&session);
    else
    {
        LOG_ERROR("client is NULL");
    }

    
}





