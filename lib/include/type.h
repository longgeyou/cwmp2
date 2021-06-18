


//类型定义头文件
#ifndef _TYPE_H_
#define _TYPE_H_


typedef enum ret_code_e{
    RET_CODE_OK = 0,        //成功
    RET_CODE_OTHER = -1,     //其他错误
    RET_CODE_PARAM = -2,   //参数错误
    RET_CODE_PROCESS = -3,  //过程错误
    RET_CODE_OVERFLOW = -4  //溢出错误（或者说超过限制）
     
}ret_code;






#endif



