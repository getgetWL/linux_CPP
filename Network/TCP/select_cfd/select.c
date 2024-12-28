#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#if 0
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

参数说明：
nfds:  		监听最大文件描述符+1
readfds:  	监听读集合事件的起始地址
writefds: 	监听写集合事件的起始地址, NULL
exceprfds: 	监听异常集合事件的起始地址, NULL
timeout:  	设置超时时间

struct timeval 
{
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* microseconds */
};

返回值：
>0: 有响应文件描述符的个数
=0：超时时间到
-1: 出错

void FD_CLR(int fd, fd_set *set);  	//将fd从set集合中删除
int  FD_ISSET(int fd, fd_set *set);	//判断fd是否在set集合中，在返回真，否则返回假
void FD_SET(int fd, fd_set *set); 	//将fd加入set集合中
void FD_ZERO(fd_set *set);    		//清空set集合

#endif

int main()
{
	int maxfd, ret;

	fd_set set, rset;

	FD_SET(0, &set);

	maxfd = 0;

	char buff[1024];

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	while (1)
	{
		rset = set;
		printf("select...\n");
		printf("buff-->%s\n", buff);
		ret = select(maxfd + 1, &rset, NULL, NULL, NULL); // 每过去1s，则tv.tv_sec减1
		printf("ret=%d\n", ret);
		printf("select  over...\n");
		if (ret < 0)
		{
			perror("select");
			break;
		}

		if (0 == ret)
		{
			printf("time  out...\n");
			tv.tv_sec = 5; // 重新设置
			tv.tv_usec = 0;
			continue;
		}
		for (int i = 0; i <= maxfd; i++)
		{
			if (FD_ISSET(i, &rset))
			{
				printf("read...\n");
				ret = read(i, buff, 1024);
				printf("read  over...\n");
				buff[ret] = '\0';
				printf("buff: %s\n", buff);
			}
		}
	}
	return 0;
}
