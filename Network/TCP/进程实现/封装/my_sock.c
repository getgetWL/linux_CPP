#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#define SERPORT 8000
#define SERADDR "127.0.0.1"

int sockfd_init()
{
    int sockfd, ret;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return -1;
    }
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in hoaddr;
    hoaddr.sin_family = AF_INET;
    hoaddr.sin_port = htons(SERPORT);
    inet_pton(AF_INET, SERADDR, &hoaddr.sin_addr.s_addr);
    ret = bind(sockfd, (struct sockaddr *)&hoaddr, sizeof(struct sockaddr_in));
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
