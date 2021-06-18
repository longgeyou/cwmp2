


#include <../lib/include/common.h>


cfg_t localCfgMg;


cfg_t *get_locl_cfg_manager()
{
    return &localCfgMg;
}



void cfg_show(cfg_t *cfg)
{
    printf("======================cfg show===================\n");
    printf(
    "[dirCfg]\n"
    "logFileFullName=%s\n"
    "testFileFullName=%s\n"
    "cfgFileFullName=%s\n"
    "[ini]\n"
    "username=%s\n"
    "uri=%s\n"
    "host=%s\n"
    "port=%s\n"
    "userAgent=%s\n"
    "[ini/auth]\n"
    "auth-nonce=%s\n"
    "auth-cnonce=%s\n"
    "auth-qop=%s\n"
    "auth-nc=%s\n"
    "auth-uri=%s\n",
    cfg->dirCfg.logFileFullName, cfg->dirCfg.testFileFullName, cfg->dirCfg.cfgFileFullName,
    cfg->iniCfg.username, cfg->iniCfg.password, cfg->iniCfg.uri, cfg->iniCfg.host, cfg->iniCfg.port,
    cfg->iniCfg.auth.nonce, cfg->iniCfg.auth.cnonce, cfg->iniCfg.auth.qop, cfg->iniCfg.auth.nc, cfg->iniCfg.auth.uri     
    );
    printf("=======================end========================\n");


}



