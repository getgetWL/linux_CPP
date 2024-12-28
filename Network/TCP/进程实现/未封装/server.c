#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>

typedef struct node
{
	int value;
	struct node *next;
} NODE;

typedef struct pack
{
	unsigned char ver;
	unsigned char type;
	unsigned int len;
	char data[0];
} PACK;

void get_signal(int signal)
{
	//异步回收子进程资源
	printf("有子进程退出!\n");
	wait(NULL);
}

void show(NODE *head)
{
	if (head == NULL)
	{
		printf("空链表！\n");
	}
	for (; head != NULL; head = head->next)
	{
		printf("[%d]-->", head->value);
	}
}

NODE *free_list(NODE *head)
{
	NODE *temp = NULL;
	while (head != NULL)
	{
		temp = head;
		head = head->next;
		free(temp);
	}
	return NULL;
}

int main()
{
	signal(SIGCHLD, get_signal);
	int sockfd, ret;
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // 创立套接字
	if (sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	// 设置端口复用
	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in seraddr;		// 为字节套指定本地端口和地址
	seraddr.sin_family = AF_INET;	// 协议族主机字节序（AF_INET指定IPv4)
	seraddr.sin_port = htons(8000); // 自定义端口，网路字节序
	printf("网路字节序端口:%d\n", seraddr.sin_port);
	inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
	printf("网路字节序IP:%d\n", seraddr.sin_addr.s_addr);

	ret = bind(sockfd, (struct sockaddr *)&seraddr, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		perror("bind");
		return -1;
	}
	ret = listen(sockfd, 20); // 设置服务端挂起连队列的大小
	if (ret < 0)
	{
		perror("listen");
		return -1;
	}

	pid_t pid;
	struct sockaddr_in cliaddr;
	int addrlen = sizeof(struct sockaddr_in);
	int port;
	char ip[64];

	while (1)
	{
		printf("accept***\n");
		int cfd;													 // 成功返回新的文件描述符，专门用来和建立连接的客户端
		cfd = accept(sockfd, (struct sockaddr *)&cliaddr, &addrlen); // 等待客户连接,不接收客户端IP和端口号
		printf("accept over***\n");
		if (cfd < 0)
		{
			perror("accept");
			break;
		}
		inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, 64);
		port = ntohs(cliaddr.sin_port);
		printf("客户端[%s||%d]成功连接！\n", ip, port);

		pid = fork();
		if (pid > 0)
		{
			close(cfd);
			continue;
		}
		else if (pid == 0)
		{
			close(sockfd);
			PACK *p = NULL;
			p = (PACK *)malloc(sizeof(PACK));
			assert(p != NULL);
			NODE *head = NULL;
			while (1)
			{
				printf("read  PACK***\n");
				ret = read(cfd, p, sizeof(PACK));
				printf("read PACK over-->ret=%d\n", ret);
				if (ret == 0)
				{
					printf("客户端[%s||%d]断开连接！\n", ip, port);
					break;
				}
				if (ret < 0)
				{
					perror("read");
					break;
				}
				int n = p->len;
				printf("客户端[%s||%d]传入%d个节点\n", ip, port, n);

				while (n != 0)
				{
					NODE *pnew = NULL;
					pnew = (NODE *)malloc(sizeof(NODE));
					assert(pnew != NULL);
					ret = read(cfd, pnew, sizeof(NODE));
					if (ret == 0)
					{
						break;
					}
					if (ret < 0)
					{
						perror("read");
						break;
					}
					pnew->next = NULL;

					pnew->next = head;
					head = pnew;

					n--;
				}
				printf("遍历链表\n");
				show(head);
				head = free_list(head);
			}

			free(p);
			close(cfd);
			return 0;
		}
	}
	close(sockfd);
}
