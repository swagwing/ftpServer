#include "factory.h"

void sigfunc(int signum)
{
    printf("%d is coming\n",signum);
}

//获取当前时间并将其转为字符串
char *my_time()
{
    char *p;
    time_t t;
    t=time(NULL);
    p=ctime(&t);
    return p;
}

int server_func(int new_fd)
{
    train t;
    signal(SIGPIPE,sigfunc);
    int ret;
    int dataLen;
    char buf[1000]={0};
    char path[512]={0};
    char fileName[128]={0};
    char command[10]={0};
    char usr_name[10]={0};
    char *salt;
    char *crypt_code;
    char crypt_code_client[30]={0};
    char query_value[20]={0};
    char *time_operate;
    char operate[100]={0};
    char directory[50]={0};
    int code=0;
    int precode=0;
    char fileType[5]={0};
    //验证账户、密码
    while(1){
        bzero(usr_name,sizeof(usr_name));
        ret=recv_n(new_fd,(char*)&dataLen,4); //对端断开，recv得到0
        ERROR_CHECK(ret,-1,"recv_n");
        recv_n(new_fd,usr_name,dataLen);
        strcpy(query_value,"salt");
        salt=query_login(query_value,usr_name);
        t.data_len=strlen(salt);
        strcpy(t.buf,salt);
        ret=send_n(new_fd,(char*)&t,4+t.data_len);
        ERROR_CHECK(ret,-1,"send_n");
        if(strcmp(salt,"null")==0)
        {
            continue;
        }
        bzero(crypt_code_client,sizeof(crypt_code_client));
        recv_n(new_fd,(char*)&dataLen,4);
        recv_n(new_fd,crypt_code_client,dataLen);
        bzero(query_value,sizeof(query_value));
        strcpy(query_value,"crypt_code");
        crypt_code=query_login(query_value,usr_name);
        //发送登录是否成功标志
        if(strcmp(crypt_code_client,crypt_code)==0)
        {
            t.data_len=strlen("success");
            strcpy(t.buf,"success");
        }else{
            t.data_len=strlen("failure");
            strcpy(t.buf,"failure");
        }
        ret=send_n(new_fd,(char*)&t,4+t.data_len);
        if(strcmp(t.buf,"success")==0)
        {
            break;
        }
    }
    printf("usr_name:%s\n",usr_name);
    //获取客户端输入命令并响应
    while(1){
        bzero(command,sizeof(command));
        ret=recv_n(new_fd,(char*)&dataLen,4); //对端断开，recv得到0
        ERROR_CHECK(ret,-1,"recv_n");
        recv_n(new_fd,command,dataLen);
        time_operate=my_time();
        printf("time_operate:%s\n",time_operate); //操作时间测试信息打印
        printf("%s\n",command);
        if(strncmp(command,"pwd",3)==0){
            int code_tmp=code;
            int precode_tmp=precode;
            strcpy(operate,command);
            insert_log(usr_name,operate,time_operate);
            bzero(buf,sizeof(buf));
            bzero(path,sizeof(path));
            printf("code:%d precode:%d\n",code,precode);
            while(code!=0){
                bzero(fileName,sizeof(fileName));
                bzero(buf,sizeof(buf));
                strcpy(fileName,query_files_filename(code));
                printf("filename:%s\n",fileName);
                printf("code:%d precode:%d\n",code,precode);
                strcpy(buf,path);
                sprintf(path,"%s%s%s","/",fileName,buf);
                printf("path:%s\n",path);
                code=precode;
                if(code!=0){
                precode=query_files_precode(code);
                }
            }
            if(strlen(path)==0){
            printf("path1:%s\n",path);
            bzero(buf,sizeof(buf));
            strcpy(buf,path);
            sprintf(path,"%s%s","/",buf);
            }
            printf("path2:%s\n",path);
            t.data_len=strlen(path);
            strcpy(t.buf,path);
            printf("t.buf:%s\n",t.buf);
            ret=send_n(new_fd,(char*)&t,4+t.data_len);
            code=code_tmp;
            precode=precode_tmp;
        }//pwd结束
        if(strcmp(command,"cd")==0){
            bzero(directory,sizeof(directory));
            recv_n(new_fd,(char*)&dataLen,4);
            recv_n(new_fd,directory,dataLen);
            printf("directory_total:%s\n",directory);
            sprintf(operate,"%s%s%s",command," ",directory);
            insert_log(usr_name,operate,time_operate);
            if(strcmp(directory,"..")==0){
                if(precode!=0){
                    code=precode;
                    precode=query_files_precode(code);
                }else{
                    code=precode=0;
                }
                printf("cd ..:code:%d precode:%d\n",code,precode);
            } //cd ..
            else if(strncmp(directory,"/",1)==0){
                int length_path=strlen(directory);
                bzero(path,sizeof(path));
                strcpy(path,directory);
                int i;
                int j;
                code=0;
                precode=0;
                bzero(directory,sizeof(directory));
                for(i=1,j=0;i<length_path;++i,++j){
                    if(strncmp(path+i,"/",1)!=0){
                        directory[j]=path[i];
                    }else{
                        precode=code;
                        strcpy(fileType,"d");
                        code=query_files(precode,directory,fileType,usr_name);
                        bzero(directory,sizeof(directory));
                        j=-1;
                    }
                }
                precode=code;
                strcpy(fileType,"d");
                printf("%d %s %s %s\n",precode,directory,fileType,usr_name);
                code=query_files(precode,directory,fileType,usr_name);
                printf("code:%d\n",code);
                printf("precode:%d\n",precode);
            }//cd +绝对路径
            else{
                strcpy(fileType,"d");
                precode=code;
                printf("precode:%d directory:%s fileType:%s usr_name:%s\n",precode,directory,fileType,usr_name);
                code=query_files(precode,directory,fileType,usr_name);
                printf("precode:%d code:%d\n",precode,code);
            }//cd+目录
        }//cd完成
        if(strcmp(command,"ls")==0){
            printf("precode:%d code:%d\n",precode,code);
            bzero(buf,sizeof(buf));
            printf("before,buf:%s",buf);
            strcpy(buf,query_files_ls(code,usr_name));
            t.data_len=sizeof(buf);
            strcpy(t.buf,buf);
            send_n(new_fd,(char*)&t,4+t.data_len);
        }//ls完成
        if(strncmp(command,"gets",4)==0){
            bzero(fileName,sizeof(fileName));
            recv_n(new_fd,(char*)&dataLen,4);
            recv_n(new_fd,fileName,dataLen);
            sprintf(operate,"%s%s%s",command," ",fileName);
            insert_log(usr_name,operate,time_operate);
            printf("fileName:%s\n",fileName);
            t.data_len=strlen(fileName);
            strcpy(t.buf,fileName);
            ret=send_n(new_fd,(char*)&t,4+t.data_len);
            ERROR_CHECK(ret,-1,"send_n");
            //发送文件长度
            t.data_len=sizeof(off_t);
            struct stat sbuf;
            stat(fileName,&sbuf);
            memcpy(t.buf,&sbuf.st_size,sizeof(off_t));
            ret=send_n(new_fd,(char*)&t,4+t.data_len);
            if(-1==ret)
            {
                goto end;
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
                ret=send_n(new_fd,(char*)&t,4+t.data_len);
                if(-1==ret)
                {
                    goto end;
                }
            }
            //发送结束
            send_n(new_fd,(char*)&t,4+t.data_len);
        }//gets结束
        //如果客户端输入puts，上传文件
        if(strcmp(command,"puts")==0)
        {
            bzero(fileName,sizeof(fileName));
            //接收文件名
            recv_n(new_fd,(char*)&dataLen,4);
            recv_n(new_fd,fileName,dataLen);
            sprintf(operate,"%s%s%s",command," ",fileName);
            insert_log(usr_name,operate,time_operate);
            printf("filename:%s\n",fileName);
            int fd;
            fd=open(fileName,O_CREAT|O_WRONLY,0666);
            ERROR_CHECK(fd,-1,"open");
            //接文件大小
            off_t fileSize=0,downLoadSize=0,oldSize=0,sliceSize=0;
            recv_n(new_fd,(char*)&dataLen,4);
            recv_n(new_fd,(char*)&fileSize,dataLen);
            sliceSize=fileSize/100;
            while(1)
            {
                ret=recv_n(new_fd,(char*)&dataLen,4);
                //如果服务器断开，客户端跳出循环
                if(-1==ret){
                    printf("server is not online\n");
                    break;
                }
                if(dataLen>0)
                {
                    ret=recv_n(new_fd,buf,dataLen);
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
                    printf("\r100%%     \n");
                    break;
                }
            }
            close(fd);
        }//puts结束
    }

end:
    close(new_fd);
    return 0;
}
