#include <stdio.h>  //printf
#include <stdlib.h> //exit
#include <sys/types.h>
#include <sys/socket.h> //socket
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h>  //htons inet_addr
#include <unistd.h>     //close
#include <string.h>

/*
相对于广播，多播具有可控性，只有加入多播组的接收者才可以接收数据，否则接收不到

IPv4的D类地址是多播地址
十进制：224.0.0.1~239.255.255.254 范围任意一个
十六进制：E0.00.00.01~EF.FF.FF.FE
其数据包中mac地址是由 硬件mac地址前3字节与ip地址后23bit拼接而成，第24bit固定为0

功能：设置一个套接字的选项（属性）
int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
参数：
socket：文件描述符
level：协议层次 设置为：IPPROTO_IP IP层次
option_name：选项的名称 设置为：IP_ADD_MEMBERSHIP 加入多播组
option_value：设置的选项的值为结构体 ip_mreq
    struct ip_mreq
    {
        struct in_addr imr_multiaddr; //组播ip地址
        struct in_addr imr_interface; //主机地址  一般设置为：INADDR_ANY 任意主机地址（自动获取你的主机地址）
    };
option_len：option_value的长度
返回值：成功：0  失败：‐1
*/

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int sockfd; // 文件描述符
    struct sockaddr_in groupcastaddr;
    socklen_t addrlen = sizeof(groupcastaddr);

    // 第一步：创建套接字
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("fail to socket");
        exit(1);
    }

    // 第二步：设置为加入多播组
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(argv[1]);
    mreq.imr_interface.s_addr = INADDR_ANY;
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("fail to setsockopt");
        exit(1);
    }

    // 第三步：填充组播信息结构体
    groupcastaddr.sin_family = AF_INET;
    groupcastaddr.sin_addr.s_addr = inet_addr(argv[1]); // 224.x.x.x - 239.x.x.x
    groupcastaddr.sin_port = htons(atoi(argv[2]));

    // 第四步：将套接字与广播信息结构体绑定
    if (bind(sockfd, (struct sockaddr *)&groupcastaddr, addrlen) < 0)
    {
        perror("fail to bind");
        exit(1);
    }

    // 第五步：进行通信
    char text[32] = "";
    struct sockaddr_in sendaddr;

    while (1)
    {
        if (recvfrom(sockfd, text, sizeof(text), 0, (struct sockaddr *)&sendaddr, &addrlen) < 0)
        {
            perror("fail to recvfrom");
            exit(1);
        }

        printf("[%s - %d]: %s\n", inet_ntoa(sendaddr.sin_addr), ntohs(sendaddr.sin_port), text);
    }

    return 0;
}