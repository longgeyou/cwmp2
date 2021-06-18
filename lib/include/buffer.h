


#ifndef _BUFFER_H_
#define _BUFFER_H_


#define BUF_IMPORT_FILE_DEFAULT_SIZE 1024*10
#define BUF_CREATE_DEFAULT_SIZE 16



typedef struct buf_truple_t{
    unsigned char *data;
    int size;
    int used;
    struct buf_truple_t *next;
}buf_truple_t;

typedef buf_truple_t buf_list_head_t;



void buf_stat_show(void);

buf_truple_t *buf_create(int size);
//����buf�Ĵ�С
int buf_new_size(buf_truple_t *t, int size);
int buf_set_value(buf_truple_t *t, unsigned char *data, int size);
void buf_free(buf_truple_t *t);
buf_truple_t *buf_improt_file(const char *fileName, int maxSize);
void buf_show(buf_truple_t *t);
void buf_append(buf_truple_t *t, char *data);
//׷�����ݣ����Զ�����
int buf_append_data(buf_truple_t *t, unsigned char *data, int size);
//�������
void buf_clear_data(buf_truple_t *t);




//����洢
buf_truple_t *buf_list_create(void);
void buf_list_in(buf_truple_t *, void *, int );
void buf_list_in_list(buf_truple_t *head, buf_truple_t *headNew);

void buf_list_clear(buf_truple_t *);
void buf_list_free(buf_truple_t *);
int buf_list_get_size(buf_truple_t *);
int buf_list_get_num(buf_truple_t *head);

buf_truple_t *buf_list_get_node(buf_truple_t *head, int idx);
void buf_list_show(buf_truple_t *);
buf_truple_t *buf_list2truple(buf_truple_t *list);
buf_truple_t *buf_list2truple_for_line(buf_truple_t *list);
buf_list_head_t *buf_truple2list(buf_truple_t *buf);
buf_list_head_t *buf_list_read_file(char *fileName, int maxSize);


//����
void buf_list_test(void);
void buf_test(void);






#endif 




