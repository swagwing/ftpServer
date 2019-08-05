#include "function.h"

int recvCycle(int newFd,void* p,int len)
{
    int total=0;
    int ret;
    char *pStart=(char*)p;
    while(total<len)
    {
        ret=recv(newFd,pStart+total,len-total,0);
        //如果对端断开，返回-1
        if(0==ret)
        {
            return -1;
        }
        total=total+ret;
    }
    return 0;
}

int sendCycle(int newFd,void *p,int len)
{
	int total=0;
	int ret;
    char *buf=(char*)p;
	while(total<len)
	{
		ret=send(newFd,buf+total,len-total,0);
		if(-1==ret)
		{
			printf("errno=%d\n",errno);
			return -1;
		}
		total=total+ret;
	}
	return 0;
}
