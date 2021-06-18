




/*============================
             cpe
==============================*/





#include <../lib/include/common.h>
#include <../http/include/http.h>
#include <include/cpe.h>





int main(void)
{
    cfg_t *cfg = cpe_cfg_init();
    log_init(cfg->dirCfg.homeDir, cfg->dirCfg.logFileFullName);
    pool_init();
    buf_init();
    
    cpe_test();
    //buf_test();
    
    //while(1)
        sleep(5);
   
        
    return 0;
}









