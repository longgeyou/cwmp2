

#ifndef _CFG_H_
#define _CFG_H_
#include <../lib/include/keyval.h>


//文件目录配置
#define CFG_DIR_NAME_MAX_BYTE 256
typedef struct dir_cfg_t{
    char homeDir[CFG_DIR_NAME_MAX_BYTE];
    char logFileFullName[CFG_DIR_NAME_MAX_BYTE];
    char testFileFullName[CFG_DIR_NAME_MAX_BYTE];
    char cfgFileFullName[CFG_DIR_NAME_MAX_BYTE];
}dir_cfg_t;

//认证配置文件
typedef struct auth_cfg_t{
    char realm[32];
	char nonce[32];
	char cnonce[32];
	char qop[32];
	char nc[32];
    char uri[128];      
}auth_cfg_t;

//ini 配置文件
typedef struct ini_cfg_t{
    auth_cfg_t auth;
    char username[128];
    char password[128];
	char uri[128];
	char host[128];
	char  port[32];
	char userAgent[128];
}ini_cfg_t;

//用户密码信息
#define CFG_USER_MAX_NUM 10
typedef struct user_cfg_t{
   key_value_t userArray[CFG_USER_MAX_NUM];
   int userNum;
}user_cfg_t;


typedef struct cfg_t{
    int initMark;   //初始化标志 0代表未被初始化  1代表已经初始化

    dir_cfg_t dirCfg;
    ini_cfg_t iniCfg;
    user_cfg_t userCfg;
}cfg_t;




cfg_t *get_locl_cfg_manager();
void cfg_show(cfg_t *cfg);



#endif





