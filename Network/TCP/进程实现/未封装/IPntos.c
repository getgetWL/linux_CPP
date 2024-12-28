#include <stdio.h>
#include <string.h>

int IPINTToIPString(unsigned int ip, char *ipstring)
{
    if(NULL == ipstring)
    {
        return -1;
    }
    sprintf(ipstring,"%d.%d.%d.%d",*((unsigned char *)&ip),
                                   *((unsigned char *)&ip + 1),
                                   *((unsigned char *)&ip + 2),
                                   *((unsigned char *)&ip + 3));
    //   0000 0001 /0000 0000 /0000 0000 /0111 1111
    printf("%p\n",((unsigned char *)&ip));
    printf("%p\n",((unsigned char *)&ip+1));
    printf("%d\n",*((unsigned char *)&ip));
    printf("%d\n",*((unsigned char *)&ip+1)); 
    printf("%d\n",*((unsigned char *)&ip+2)); 
    printf("%d\n",*((unsigned char *)&ip+3));
    return 0;
}

unsigned int IPStringToIPINT(char *ipstring)
{
	unsigned int ret = 0;

	sscanf(ipstring,"%d.%d.%d.%d",&ret,\
	                              (unsigned char *)&ret + 1,\
	                              (unsigned char *)&ret + 2,\
				      (unsigned char *)&ret + 3);

	return ret;
}

int main(){
    char ipstring[32];
    IPINTToIPString(16777343,ipstring);
    printf("转化主机IP值：\n");
    printf("%s\n",ipstring);

    int IP_net;
    IP_net=IPStringToIPINT(ipstring);
    printf("转化网络主机IP值：\n");
    printf("%d\n",IP_net);

    return 0;
    }
