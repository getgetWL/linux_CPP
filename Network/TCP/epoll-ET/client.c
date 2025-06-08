#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void hangle(int sig)
{
	printf("recv  sig: %d\n", sig);
}

int main()
{

	signal(SIGPIPE, hangle);
	int sockfd, ret;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in seraddr;
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(8100);
	inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
	ret = connect(sockfd, (struct sockaddr *)&seraddr, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		perror("connect");
		return -1;
	}

	printf("sockfd-->%d\n", sockfd);
	char buff[1024];
	char answer[1024];
	while (1)
	{
		printf("请输入数据:");
		scanf("%s", buff);
		ret = write(sockfd, buff, strlen(buff));
		if (ret < 0)
		{
			perror("write");
			return -1;
		}

		ret = read(sockfd, answer, 1024);
		if (ret < 0)
		{
			perror("read");
			return -1;
		}
		if (ret == 0)
		{
			printf("断开连接！\n");
			return -1;
		}
		printf("%s\n", answer);
		memset(answer,0,sizeof(answer));
	}

	close(sockfd);

	return 0;
}
