



#ifndef _POOL_H_
#define _POOL_H_




void *pool_malloc(int size);
void pool_free(void *header);
void pool_stat_show(void);




#endif



