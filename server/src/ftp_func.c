#include "../include/factory.h"

void cleanup(void* p){
    pthread_mutex_t *mutex=(pthread_mutex_t*)p;
    pthread_mutex_unlock(mutex);
}

void* thread_func(void *p){
    pfactory_t pf=(pfactory_t)p;
    pque_t pq=&pf->que;
    pNode_t pdelete;
    while(1){
        pthread_mutex_lock(&pq->mutex);
        pthread_cleanup_push(cleanup,&pq->mutex);
        if(pf->start_flag&&0==pq->que_size)
        {
            pthread_cond_wait(&pf->cond,&pq->mutex);
        }
        if(!pf->start_flag)
        {
            pthread_exit(NULL);
        }
        que_get(pq,&pdelete);//从队列里获取任务
        pthread_mutex_unlock(&pq->mutex);
        pthread_cleanup_pop(0);
        server_func(pdelete->new_fd);
        free(pdelete);
        pdelete=NULL;
    }
}

void sigfunc_exit(int signum)
{
    printf("signal is send\n");
    write(exit_fds[1],&signum,1);
    int status;
    wait(&status);
    if(WIFEXITED(status))
    {
        printf("exit value=%d\n",WEXITSTATUS(status));
    }
    exit(0);
}
