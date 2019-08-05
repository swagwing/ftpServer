#include "factory.h"

void factory_init(pfactory_t pf,int thread_num,int capacity)
{
	memset(pf,0,sizeof(factory_t));
	pf->ptr_arr=(pthread_t*)calloc(thread_num,sizeof(pthread_t));
	pf->thread_num=thread_num;
	pthread_cond_init(&pf->cond,NULL);
	que_init(&pf->que,capacity);
}

void factory_start(pfactory_t pf)
{
	int i;
	if(0==pf->start_flag)
	{
		pf->start_flag=1;
		for(i=0;i<pf->thread_num;i++)
		{
			pthread_create(pf->ptr_arr+i,NULL,thread_func,pf);
		}
	}
}


void system_init(const char* config_file_name,char* ip,char* port,char* thread_num,char* capacity){
    FILE* fp;
    fp=fopen(config_file_name,"r");
    if(NULL==fp){
        perror("fopen");
        return;
    }
    while(!feof(fp)){
        fscanf(fp,"%s%s%s%s",ip,port,thread_num,capacity);
    }
}

