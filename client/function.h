#include <curses.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/msg.h>
#include <pthread.h>
#include <setjmp.h>
#include <sys/time.h>
#include <signal.h>
#include <strings.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <syslog.h>
#include <sys/select.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#define FILENAME "file"
#define ARGC_CHECK(argc,val) {if(argc!=val) \
	{printf("error args\n"); return -1;}}
#define ERROR_CHECK(ret,retval,funcName) {if(ret==retval)\
	{perror(funcName); return -1;}}
#define THREAD_ERROR_CHECK(ret,funcName) {if(ret!=0) \
	{printf("%s:%s\n",funcName,strerror(ret)); return -1;}}

//父进程管理子进程所使用的数据结构
typedef struct{
	pid_t pid;
	int fd;
	short busy;
}process_data;
//小火车变长结构体发送文件
typedef struct{
	int data_len;
	char buf[1000];
}train_t;
int recvMsg(int,int*);
int trainFile(int);
int sendMsg(int,int);
int make_child(process_data*,int);
int tcp_init(int*,char*,char*);
int childHandle(int);
int recvCycle(int,void*,int);
int sendCycle(int,void*,int);
