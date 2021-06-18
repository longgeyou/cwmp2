



#include <include/acs.h>
#include <../lib/include/common.h>
#include <../http/include/http.h>
#include <../cwmp/include/ini.h>



#include "sys/types.h"//数据类型定义
#include "sys/socket.h" //提供socket函数及数据结构




#define CFG_HOME_DIR "/mnt/hgfs/share/PRO/cwmp2/acs"
#define CFG_LOG_DIR "log"
#define CFG_LOG_FILE_NAME "log.txt"
#define CFG_TEST_DIR "test"
#define CFG_TEST_FILE_NAME "test.txt"
#define CFG_CFG_DIR "cfg"
#define CFG_CFG_FILE_NAME "cfg.txt"

cfg_t *acs_cfg_init(void)
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
    //key_value_array_show(cfg->userCfg.userArray, cfg->userCfg.userNum);

    
    //cfg_show(cfg);
    return cfg;           
}


typedef struct acs_manager_t{
    http_server_t *server;

}acs_manager_t;

acs_manager_t acsLocalManager;

void acs_init(void)
{

    ;

}


//测试
#define HTTP_SERVER_MAX_USER_NUM 15
void acs_test(void)
{
    http_server_t *server = http_server_create();
    //socket配置
    http_socket_set(
    &(server->socket),
    AF_INET,
    SOCK_STREAM,
    0,
    AF_INET,
    "192.168.137.20",
    8080,  
    HTTP_SERVER_MAX_USER_NUM);
    server->socketMark = 1;
    //超时时间设置
    server->userTimeout = 10;
    
    http_server_start(server);
}


typedef struct acs_session_t{
    http_server_t *server;
    int step;
}acs_session_t;



int acs_recv_data_pro(http_user_t *user)
{


;
}




//acs 会话
//0 得到来自cpe的inform信息
//1 等待接收cpe的http post和soap信封内容，并做处理
//2 回复cep http 的post，并发送包含cpe rpc方法的soap信封
//3 重复1、2步骤，直到得到会话终止信号（包括对方断开连接、自己主动放弃连接、长
//  时间未收到http post信息），结束会话
int acs_session(acs_session_t *session)
{
    session->step = 0;
    if(session == NULL)
    {
        LOG_ALARM("session is NULL");
        return RET_CODE_PARAM;
    }

    while(1)
    {
        switch(session->step)
        {
            case 0:
            {
                
            }
            break;
            case 1:
            {

            }
            break;
            case 2:
            {

            }
            break;
            case 3:
            {

            }
            break;
            default:
            {


            }
            break;
        }
        sleep(2);
    }
    
}




void acs_test2()
{

    http_server_t *server = http_server_create();
    //socket配置
    http_socket_set(
    &(server->socket),
    AF_INET,
    SOCK_STREAM,
    0,
    AF_INET,
    "192.168.137.20",
    8080,  
    HTTP_SERVER_MAX_USER_NUM);
    server->socketMark = 1;
    //超时时间设置
    server->userTimeout = 10;
    server->recv_data_pro = acs_recv_data_pro;
    http_server_start(server);

    //开启会话
    acs_session_t session;
    session.server = server;
    if(server != NULL)
        acs_session(session);
    else
    {
        LOG_ALARM("server is NULL");
    }


}




