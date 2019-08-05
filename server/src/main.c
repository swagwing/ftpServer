#include "factory.h"

int main(int argc,char* argv[])
{
    ARGC_CHECK(argc,2);
    pipe(exit_fds);
    while(fork())
    {
        signal(SIGUSR1,sigfunc_exit); //设定退出机制
        wait(NULL);
    }
    close(exit_fds[1]);
    factory_t f;
    char ip[128]={0};
    char port[128]={0};
    char th[10]={0};
    char ca[10]={0};
    system_init(argv[1],ip,port,th,ca);
    int thread_num=atoi(th);
    int capacity=atoi(ca);
    factory_init(&f, thread_num,capacity);
    factory_start(&f);
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in ser;
    memset(&ser,0,sizeof(ser));
    ser.sin_family=AF_INET;
    ser.sin_port=htons(atoi(port));
    ser.sin_addr.s_addr=inet_addr(ip);
    int ret;
    int reuse=1;
    setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
    ret=bind(sfd,(struct sockaddr*)&ser,sizeof(ser));
    if(-1==ret)
    {
        perror("bind");
        return -1;
    }
    //设置边缘触发
    int epfd=epoll_create(1);
    struct epoll_event event,evs[2];
    event.events=EPOLLIN;
    event.data.fd=sfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
    event.data.fd=exit_fds[0];
    epoll_ctl(epfd,EPOLL_CTL_ADD,exit_fds[0],&event);
    listen(sfd,2*thread_num);
    int new_fd;
    pque_t pq=&f.que;
    int fd_ready_num;
    int i;
    while(1)
    {
        fd_ready_num=epoll_wait(epfd,evs,2,-1);
        for(i=0;i<fd_ready_num;++i)
        {
            if(evs[i].data.fd==sfd)
            {
                new_fd=accept(sfd,NULL,NULL);
                ERROR_CHECK(new_fd,-1,"accept");
                pNode_t pnew=(pNode_t)calloc(1,sizeof(Node_t));
                pnew->new_fd=new_fd;
                pthread_mutex_lock(&pq->mutex);
                que_insert(pq,pnew); //任务放入队列
                pthread_mutex_unlock(&pq->mutex);
                pthread_cond_signal(&f.cond); //唤醒一个子线程
            } //if结束

            if(evs[i].data.fd==exit_fds[0])
            {
                printf("start exit\n");
                f.start_flag=0;
                for(i=0;i<f.thread_num;++i)
                {
                    pthread_cond_signal(&f.cond);
                }
                for(i=0;i<f.thread_num;++i)
                {
                    pthread_join(f.ptr_arr[i],NULL);
                }
                exit(0);
            }
        }
    }
    return 0;
}

