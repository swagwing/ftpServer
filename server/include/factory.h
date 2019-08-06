#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"
#define FILENAME "file"

typedef struct{
    int data_len; //控制数据，火车头，记录火车装载内容长度
    char buf[1000]; //火车车厢
}train;

typedef struct{
    pthread_t *ptr_arr;
    int thread_num;
    pthread_cond_t cond;
    que_t que;
    short start_flag;
}factory_t,*pfactory_t;
void* thread_func(void*);
void factory_init(pfactory_t,int,int);
void factory_start(pfactory_t);
int server_func(int);
int send_n(int,char*,int);
int recv_n(int,char*,int);
void cleanup(void*);
void* thread_func(void*);
void sigfunc_exit(int);
char *my_time();
int exit_fds[2];
#define ERROR_CHECK(ret,retval,funcName) {if(ret==retval)\
	{perror(funcName); return -1;}}
void system_init(const char*,char*,char*,char*,char*);
char* query_login(char*,char*);
void insert_log(char*,char*,char*);
int query_files(int,char*,char*,char*);
int query_files_precode(int);
char *query_files_filename(int);
char *query_files_ls(int,char*);
#endif
