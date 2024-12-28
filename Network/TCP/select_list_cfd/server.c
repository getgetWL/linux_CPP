#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

#define SERADDR "127.0.0.1"
#define SERPORT 8100

typedef struct node
{
	int cfd;
	struct node *next;
} NODE;

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

NODE *del_node(NODE *head, NODE *current)
{
	if (current == head)
	{
		head = head->next;
	}
	else
	{
		NODE *tail = head;
		while (tail->next != current)
		{
			tail = tail->next;
		}
		tail->next = current->next;
	}
	free(current);
	return head;
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
	int count = 0;
	int cfd, sockfd, ret;
	struct sockaddr_in cliaddr;
	int addrlen = sizeof(struct sockaddr_in);

	sockfd = sockfd_init();
	if (sockfd < 0)
	{
		return -1;
	}
	// 定义文件描述符集合
	fd_set set, rset;
	// 定义最大描述符
	int maxfd;
	// 设置为0
	FD_ZERO(&set);
	// 将sockfd加入集合中
	FD_SET(sockfd, &set);
	maxfd = sockfd;

	char buff[1024];
	NODE *head = NULL;
	NODE *pnew = NULL;

	while (1)
	{
		rset = set;
		printf("select...\n");
		ret = select(maxfd + 1, &rset, NULL, NULL, NULL);
		printf("select over...\n");
		// 出错
		if (ret < 0)
		{
			perror("select");
			break;
		}
		//判断sockfd是否在rset集合中
		if (FD_ISSET(sockfd, &rset))
		{ // 若为真，则表示有客户端连接
			printf("accept...\n");
			cfd = accept(sockfd, (struct sockaddr *)&cliaddr, &addrlen);
			printf("accept over...\n");
			if (cfd < 0)
			{
				perror("accept");
				continue; // 再次进入循环
			}
			// 除以上情况
			FD_SET(cfd, &set); // 将cfd的值加入set中，只添加值，不储存变量
			if (cfd > maxfd)
			{
				maxfd = cfd;
			}
			// 将cfd值村入链表
			pnew = (NODE *)malloc(sizeof(NODE));
			pnew->next = NULL;
			pnew->cfd = cfd;

			pnew->next = head;
			head = pnew;
		}

		printf("count=%d\n", ++count);
		NODE *temp = head;
		while (temp != NULL)
		{
			if (!FD_ISSET(temp->cfd, &rset))
			{
				temp = temp->next;
				continue;
			}
			printf("read...\n");
			ret = read(temp->cfd, buff, 1024); // 将不是的删除，保留找到的
			printf("read over...\n");
			if (ret < 0)
			{
				perror("read");
				close(temp->cfd);
				FD_CLR(temp->cfd, &set);
				NODE *p = temp->next;			  // 临时保存temp->next
				head = del_node(head, temp->cfd); // 从链表中删除cfd
				temp = p;
				continue;
			}
			else if (ret == 0)
			{
				printf("断开！\n");
				close(temp->cfd);
				FD_CLR(temp->cfd, &set);
				NODE *p = temp->next;
				head = del_node(head, temp->cfd);
				temp = p;
				continue;
			}
			buff[ret] = '\0';
			printf("buff:%s\n", buff);
			temp = temp->next;
		}
	}
	head = free_list(head);
	return 0;
}
