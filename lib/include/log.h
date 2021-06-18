
#ifndef _LOG_H_
#define _LOG_H_



#define LOG_DIR_NAME_MAX_LEN 256
#define LOG_FILE_NAME_MAX_LEN 256



//ö�ٺͽṹ��
typedef enum log_level_e{
    LOG_LEVEL_START = 0,
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ALARM,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_SHOW_SIMPLE,
    LOG_LEVEL_END
}log_level;


//�����꣨ע��ɱ��������÷���
#define LOG_RECORD(l, fmt, ...) do{va_list ap;\
    char __buf[256] = {0};\
    sprintf(__buf, "file:%s  fun:%s  line:%d \n\t", __FILE__, __FUNCTION__, __LINE__);\
    log_record(l, __buf, fmt, ##__VA_ARGS__);\
}while(0);

#define LOG_DEBUG(fmt, ...) LOG_RECORD(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...) LOG_RECORD(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__);
#define LOG_INFO(fmt, ...) LOG_RECORD(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__);
#define LOG_ALARM(fmt, ...) LOG_RECORD(LOG_LEVEL_ALARM, fmt, ##__VA_ARGS__);
#define LOG_SHOW(fmt, ...) LOG_RECORD(LOG_LEVEL_SHOW_SIMPLE, fmt, ##__VA_ARGS__);




//log�������ṹ��
typedef struct log_level_obj_t{
    char name[32];      //����
    int cnt;       //����
}log_level_obj;

typedef struct log_manager_t{
    char homeDir[LOG_DIR_NAME_MAX_LEN];
    char logFileFullName[LOG_FILE_NAME_MAX_LEN];
    int maxSize;        //��־�ļ�����ֽ���
    log_level_obj llo[LOG_LEVEL_END];
}log_manager_t;


//����ȫ�ֱ���
static const log_level_obj gLLO[]={      //�������Ӧ���� log_level ö��ֵ��Ӧ
    {""},
    {"debug", 0},
    {"info", 0},
    {"arlam", 0},
    {"error", 0},
    {"show", 0},
    {""}
};



//��־��¼
void log_record(int level, const char *info, const char *fmt, ...);
void log_test();



#endif






