#define _XOPEN_SOURCE
#include "function.h"
#include <shadow.h>

void changeNonblock(int fd)
{
    int status=fcntl(fd,F_GETFL);
    status=status|O_NONBLOCK;
    fcntl(fd,F_SETFL,status);
}

int main(int argc,char* argv[])
{
	ARGC_CHECK(argc,3);
	int socketFd;
	socketFd=socket(AF_INET,SOCK_STREAM,0);
	ERROR_CHECK(socketFd,-1,"socket");
	struct sockaddr_in ser;
	bzero(&ser,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr(argv[1]);
	int ret;
	ret=connect(socketFd,(struct sockaddr*)&ser,sizeof(ser));
	ERROR_CHECK(ret,-1,"connect");
	printf("connect success\n");
    char buf[1000]={0};
    char fileName[128]={0};
    char command[10]={0};
    train_t t;
    int dataLen;
    char usr_name[10]={0};
    char usr_pwd[10];
    char* crypt_code=NULL;
    char salt[10]={0};
    char result[20]={0};
    //验证账号、密码
    while(1){
        printf("name:");
        scanf("%s",usr_name);
        printf("password:");
        initscr(); //启动curses模式，在此是关闭了终端回显，实现密码效果
        scanf("%s",usr_pwd);
        printf("\n");
        endwin();
        //发送用户名
        t.data_len=strlen(usr_name);
        strcpy(t.buf,usr_name);
        ret=sendCycle(socketFd,&t,4+t.data_len);
        ERROR_CHECK(ret,-1,"sendCycle");
        //接收salt值
        bzero(salt,sizeof(salt));
        ret=recvCycle(socketFd,&dataLen,4);
        ERROR_CHECK(ret,-1,"recvCycle");
        recvCycle(socketFd,salt,dataLen);
        ERROR_CHECK(ret,-1,"recvCycle");
        if(strcmp(salt,"null")==0)
        {
            printf("wrong usr_name\n");
            continue;
        }
        crypt_code=crypt(usr_pwd,salt);
        t.data_len=strlen(crypt_code);
        strcpy(t.buf,crypt_code);
        ret=sendCycle(socketFd,&t,4+t.data_len);
        ERROR_CHECK(ret,-1,"sendCycle");
        //接收密码验证结果
        ret=recvCycle(socketFd,&dataLen,4);
        ERROR_CHECK(ret,-1,"recvCycle");
        ret=recvCycle(socketFd,result,dataLen);
        ERROR_CHECK(ret,-1,"recvCycle");
        if(strcmp(result,"success")==0){
            printf("login successfully\n");
            break;
        }else{
            printf("wrong password\n");
        }
    }//账号、密码验证结束
    //客户端输入请求，并向服务器端发送请求
    while(1){
        bzero(command,sizeof(command));
        scanf("%s",command);
        printf("%s %ld\n",command,strlen(command));
        if(strncmp(command,"gets",4)==0){
            printf("gets input\n");
            t.data_len=strlen(command);
            strcpy(t.buf,command);
            ret=sendCycle(socketFd,&t,4+t.data_len);
            bzero(buf,sizeof(buf));
            scanf("%s",buf);
            t.data_len=strlen(buf);
            strcpy(t.buf,buf);
            sendCycle(socketFd,&t,4+t.data_len);
            //接收文件名
            bzero(fileName,sizeof(fileName));
            recvCycle(socketFd,&dataLen,4);
            recvCycle(socketFd,fileName,dataLen);
            printf("recieve fileName:%s\n",fileName);
            int fd;
            fd=open(fileName,O_CREAT|O_WRONLY,0666);
            ERROR_CHECK(fd,-1,"open");
            //接文件大小
            off_t fileSize=0,downLoadSize=0,oldSize=0,sliceSize=0;
            recvCycle(socketFd,&dataLen,4);
            recvCycle(socketFd,&fileSize,dataLen);
            sliceSize=fileSize/100;
            while(1){
                ret=recvCycle(socketFd,&dataLen,4);
                //如果服务器端断开，客户端跳出循环
                if(-1==ret){
                    printf("server is not online\n");
                    close(fd);
                    goto end;
                }
                if(dataLen>0){
                    ret=recvCycle(socketFd,buf,dataLen);
                    if(-1==ret){
                        break;
                    }
                    write(fd,buf,dataLen);
                    downLoadSize+=dataLen;
                    if(downLoadSize-oldSize>sliceSize)
                    {
                        printf("\r%5.2f%%",(float)downLoadSize/fileSize*100);
                        fflush(stdout);
                        oldSize=downLoadSize;
                    }
                }else{
                    printf("\r100%%    \n");
                    break;
                }
            }
            close(fd);
        }//gets结束
        if(strcmp(command,"puts")==0){
            printf("puts input\n");
            t.data_len=strlen(command);
            strcpy(t.buf,command);
            ret=sendCycle(socketFd,&t,4+t.data_len);
            bzero(fileName,sizeof(fileName));
            scanf("%s",fileName);
            //发送文件名
            t.data_len=strlen(fileName);
            strcpy(t.buf,fileName);
            ret=sendCycle(socketFd,&t,4+t.data_len);
            if(-1==ret)
            {
                break;
            }
            //发送文件大小
            t.data_len=sizeof(off_t);
            struct stat sbuf;
            stat(fileName,&sbuf);
            memcpy(t.buf,&sbuf.st_size,sizeof(off_t));
            ret=sendCycle(socketFd,(char*)&t,4+t.data_len);
            if(-1==ret)
            {
                break;
            }
            //发送文件内容
            int fd=open(fileName,O_RDONLY);
            if(-1==fd)
            {
                perror("open");
                return -1;
            }
            while(t.data_len=read(fd,t.buf,sizeof(t.buf)))
            {
                ret=sendCycle(socketFd,(char*)&t,4+t.data_len);
                if(-1==ret){
                    break;
                }
            }
            //发送结束符
            sendCycle(socketFd,(char*)&t,4);
        }//puts结束
        //客户端输入命令pwd
        if(strcmp(command,"pwd")==0){
            t.data_len=strlen(command);
            strcpy(t.buf,command);
            ret=sendCycle(socketFd,&t,4+t.data_len);
            ERROR_CHECK(ret,-1,"sendCycle");
            bzero(buf,sizeof(buf));
            ret=recvCycle(socketFd,&dataLen,4);
            if(-1==ret){
                goto end;
            }
            ret=recvCycle(socketFd,buf,dataLen);
            if(-1==ret){
                goto end;
            }
            printf("buf:%s\n",buf);
            ERROR_CHECK(ret,-1,"recvCle");
        }//pwd结束
        //客户端输入命令cd
        if(strcmp(command,"cd")==0){
            t.data_len=strlen(command);
            strcpy(t.buf,command);
            ret=sendCycle(socketFd,&t,4+t.data_len);
            ERROR_CHECK(ret,-1,"sendCycle");
            bzero(buf,sizeof(buf));
            scanf("%s",buf);
            t.data_len=strlen(buf);
            strcpy(t.buf,buf);
            ret=sendCycle(socketFd,&t,4+t.data_len);
            ERROR_CHECK(ret,-1,"sendCycle");
        }//cd结束
        //客户端输入命令ls
        if(strcmp(command,"ls")==0){
            t.data_len=strlen(command);
            strcpy(t.buf,command);
            ret=sendCycle(socketFd,&t,4+t.data_len);
            ERROR_CHECK(ret,-1,"sendCycle");
            bzero(buf,sizeof(buf));
            ret=recvCycle(socketFd,&dataLen,4);
            if(-1==ret){
                goto end;
            }
            ret=recvCycle(socketFd,buf,dataLen);
            if(-1==ret){
                goto end;
            }
            printf("%s\n",buf);
        }//ls完成
    }
end:
    close(socketFd);
	return 0;
}
