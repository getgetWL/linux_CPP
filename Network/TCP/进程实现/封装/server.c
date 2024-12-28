#include <sys/wait.h>
#include "student_list.c"
#include "my_sock.c"

typedef struct pack
{
	unsigned char ver;
	unsigned char type;
	unsigned int len;
	char data[0];
} PACK;

void get_signal(int signal)
{
	printf("有子进程退出!\n");
	wait(NULL);
}

int main()
{
	signal(SIGCHLD, get_signal);
	int sockfd, ret;
	sockfd = sockfd_init();
	if (sockfd < 0)
	{
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
