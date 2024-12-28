#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <string.h>

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
	int epfd, cfd, sockfd, ret;
	struct sockaddr_in cliaddr;
	int addrlen = sizeof(struct sockaddr_in);

	sockfd = sockfd_init();
	int maxfd;
	if (sockfd < 0)
	{
		return -1;
	}
	// 自定义集合空间，大小为10，epfd为关联标识符
	epfd = epoll_create(10);
	// 失败-1
	if (epfd < 0)
	{
		perror("epoll_create");
		return -1;
	}
	// 与自定义数量epoll_create参数相同即可
	// ev作为epol_ctl的传入参数载体
	struct epoll_event ev, evs[10];
	ev.events = EPOLLIN; // 告诉内核监听sockfd描述符对应的读事件
	ev.data.fd = sockfd;
	// 加入epfd的描述符集合
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

	char buff[1024];
	char answer[1024] = "服务器已接收！";
	while (1)
	{
		printf("wait...\n");
		//-1起阻塞作用，监听epfd关联的集合，将准备好的文件描述符放入evs[]中]
		count = epoll_wait(epfd, evs, 10, -1); // 返回监听好的文件符个数
		printf("wait over...\n");
		if (count < 0)
		{
			perror("epoll_wait");
			break;
		}

		for (int i = 0; i < count; i++)
		{
			int temp = evs[i].data.fd;
			if (temp == sockfd)
			{
				printf("accept...\n");
				// 接收客户端
				cfd = accept(temp, (struct sockaddr *)&cliaddr, &addrlen);
				printf("accept over...\n");
				if (cfd < 0)
				{
					perror("accept");
					continue;
				}

				ev.data.fd = cfd;
				// 加入epfd关联的文件描述符
				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
			}
			else
			{ // 客户端发来数据
				printf("read...\n");
				ret = read(temp, buff, 1024);
				printf("read over...\n");
				if (ret < 0)
				{
					perror("read");								// 打印错误信息
					close(temp);								// 关闭文件描述符
					epoll_ctl(epfd, EPOLL_CTL_DEL, temp, NULL); // 删除该文件描述符
					continue;
				}
				else if (ret == 0)
				{
					printf("断开！\n");
					close(temp);
					epoll_ctl(epfd, EPOLL_CTL_DEL, temp, NULL);
					continue;
				}
				buff[ret] = '\0';
				printf("buff:%s\n", buff);
				// 服务器回复
				ret = write(temp, answer, strlen(answer));
				if (ret < 0)
				{
					perror("write");
					close(temp);
					epoll_ctl(epfd, EPOLL_CTL_DEL, temp, NULL); // 删除该文件描述符
					continue;
				}
			}
		}
	}
	return 0;
}
