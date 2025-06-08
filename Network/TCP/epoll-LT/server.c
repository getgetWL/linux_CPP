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

//默认水平模式LT
//水平触发（LT，Level-Triggered）：

// 在水平触发模式下，当文件描述符上有新的数据可读或可写时，epoll_wait会立即返回并通知应用程序。即使应用程序没有处理完所有的数据，下一次epoll_wait调用仍然会返回该文件描述符上的事件。
// 对于读操作，如果文件描述符上的接收缓冲区中有任何数据可读（不为空），epoll_wait会返回该文件描述符可读的事件。即使应用程序没有读取所有数据，下一次epoll_wait调用仍然会返回相同的可读事件。
// 对于写操作，如果文件描述符上的发送缓冲区有足够的空间可以写入数据，epoll_wait会返回该文件描述符可写的事件。
// 水平触发模式适用于典型的轮询方式，应用程序可以重复调用epoll_wait来处理文件描述符上的I/O事件，直到所有事件都被处理完毕。


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
				//将每次读取的
				ret = read(temp, buff, 10);
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
