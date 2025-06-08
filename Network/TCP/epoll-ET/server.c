#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#define SERADDR "127.0.0.1"
#define SERPORT 8100

// 默认水平模式LT
// 边缘触发（ET，Edge-Triggered）

// 在边缘触发模式下，epoll_wait只在文件描述符状态发生变化时才返回，并且只通知应用程序一次。也就是说，只有当文件描述符从无事件变为有事件时，epoll_wait才会返回。
// 对于读操作，仅当文件描述符上的接收缓冲区由空变为非空时，epoll_wait才会返回该文件描述符可读的事件。
// 对于写操作，仅当文件描述符上的发送缓冲区由满变为非满时，epoll_wait才会返回该文件描述符可写的事件。
// 边缘触发模式要求应用程序在接收到事件后立即处理所有可用的数据，因为下一次epoll_wait调用不会返回相同的事件。

//	注意点：
// 	边缘触发必须使用非阻塞的socket
//

// 设置文件描述符为非阻塞模式
void set_nonblocking(int fd)
{
	// 获取与文件描述符关联的文件状态标志,返回一个整数值，该值包含了文件描述符 fd 的当前状态标志。
	//  这些标志可以是以下几种之一或其组合：
	//  O_RDONLY: 只读模式打开。
	//  O_WRONLY: 只写模式打开。
	//  O_RDWR: 读写模式打开。
	//  O_APPEND: 每次写操作都追加到文件末尾。
	//  O_NONBLOCK: 非阻塞模式。
	//  O_SYNC: 同步 I/O 操作。

	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	printf("fd状态为:%o\n", flags);
	//	设置描述符的状态
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
	flags = fcntl(fd, F_GETFL, 0);
	printf("修改后fd状态为:%o\n", flags);
}

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
	// 用于将文件描述符（fd）设置为非阻塞模式的系统调用
	set_nonblocking(sockfd);

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
	ev.events = EPOLLIN | EPOLLET; // 告诉内核监听sockfd描述符对应的读事件，并采用ET模式
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
				while (1)
				{
					printf("accept...\n");
					// 接收客户端
					cfd = accept(temp, (struct sockaddr *)&cliaddr, &addrlen);
					printf("accept over...\n");
					if (cfd < 0)
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
						{
							// 没有更多连接，退出循环
							break;
						}
						else
						{
							perror("accept");
							break;
						}
					}
					set_nonblocking(cfd);
					// 设置为边缘触发
					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = cfd;
					// 加入epfd关联的文件描述符
					epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
				}
			}
			else
			{ // 客户端发来数据
				while (1)
				{
					printf("read...\n");
					// 将每次读取的
					ret = read(temp, buff, 10);
					printf("read over...\n");
					printf("ret=%d\n", ret);
					if (ret > 0)
					{
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
					else if (ret == 0)
					{
						printf("断开！\n");
						close(temp);
						epoll_ctl(epfd, EPOLL_CTL_DEL, temp, NULL);
						break;
					}
					else
					{
						//读取完成时会返回 EAGAIN/EWOULDBLOCK
						if (errno == EAGAIN || errno == EWOULDBLOCK)
						{
							// 数据已读完，退出循环
							printf("已读完\n");
							break;
						}
						else
						{
							perror("recv");
							close(temp);
							epoll_ctl(epfd, EPOLL_CTL_DEL, temp, NULL);
							break;
						}
					}
				}
			}
		}
	}
	return 0;
}
