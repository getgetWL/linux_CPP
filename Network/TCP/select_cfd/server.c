#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#define SERADDR "127.0.0.1"
#define SERPORT 8100

// 成功返回监听套接字， 失败返回-1
int sockfd_init()
{
	int sockfd, ret;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return -1;
	}

	// 设置端口复用
	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in seraddr;
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(SERPORT);
	inet_pton(AF_INET, SERADDR, &seraddr.sin_addr.s_addr);
	ret = bind(sockfd, (struct sockaddr *)&seraddr, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		perror("bind");
		return -1;
	}

	ret = listen(sockfd, 20);
	if (ret < 0)
	{
		perror("listen");
		return -1;
	}

	return sockfd;
}

int main()
{
	int count = 0;
	int cfd, sockfd, ret;
	struct sockaddr_in cliaddr;
	int addrlen = sizeof(struct sockaddr_in);

	sockfd = sockfd_init();
	int maxfd;
	if (sockfd < 0)
	{
		return -1;
	}
	fd_set set, rset;
	FD_ZERO(&set);
	FD_SET(sockfd, &set);
	maxfd = sockfd;

	char buff[1024];
	while (1)
	{
		rset = set;
		printf("select...\n");
		ret = select(maxfd + 1, &rset, NULL, NULL, NULL);
		printf("select over...\n");
		if (ret < 0)
		{
			perror("select");
			break;
		}
		if (FD_ISSET(sockfd, &rset))
		{ // 若为真，则表示有客户端连接
			printf("accept...\n");
			cfd = accept(sockfd, (struct sockaddr *)&cliaddr, &addrlen);
			printf("accept over...\n");
			if (cfd < 0)
			{
				perror("accept");
				continue;
			}

			FD_SET(cfd, &set); // 将cfd的值加入set中，只添加值，不储存变量
			if (cfd > maxfd)
			{
				maxfd = cfd;
			}
		}
		printf("count=%d\n", ++count);
		for (int i = 0; i <= maxfd; i++)
		{
			if (i == sockfd)
			{
				continue;
			}
			// 第一次添加时，会在此处退出，直至退出for循环，再次进入while循环后，便可查找到刚加入的cfd
			if (!FD_ISSET(i, &rset))
			{
				continue;
			}
			printf("read...\n");
			ret = read(i, buff, 1024); // 将不是的删除，保留找到的
			printf("read over...\n");
			if (ret < 0)
			{
				perror("read");
				close(i);
				FD_CLR(i, &set);
				continue;
			}
			else if (ret == 0)
			{
				printf("断开！\n");
				close(i);
				FD_CLR(i, &set);
				continue;
			}
			buff[ret] = '\0';
			printf("buff:%s\n", buff);
		}
	}
	return 0;
}
