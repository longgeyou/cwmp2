


/*============================
             acs
==============================*/


#include <stdio.h>
#include <../lib/include/common.h>
#include <include/acs.h>
#include <../http/include/http.h>
#include <../cwmp/include/ini.h>




void main_test(void);


int main(void)
{
    main_test();
    return 0;
}


void main_test(void)

{
    //³õÊ¼»¯
    cfg_t *cfg = acs_cfg_init();
    pool_init();
    log_init(cfg->dirCfg.homeDir, cfg->dirCfg.logFileFullName);
    buf_init();
    

    //log_test();
    //user_list_test();
    acs_test();
    //keyval_test();
    //strpro_test();
    //ini_test1(cfg->dirCfg.testFileFullName);
    //ini_test3(cfg->dirCfg.testFileFullName);
    //buf_test();
    //LOG_SHOW("fadsf");
    
    while(1)
        sleep(5);
}












