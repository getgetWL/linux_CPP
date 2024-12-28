// 广播发送者代码实现
#include <stdio.h>  //printf
#include <stdlib.h> //exit
#include <sys/types.h>
#include <sys/socket.h> //socket
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h>  //htons inet_addr
#include <unistd.h>     //close
#include <string.h>

// 广播发送进程： IP地址 xxx.xxx.xxx.255  MAC地址：FFFFFFFF
// 即在当前xxx.xxx.xxx网段下所有主机都可以接受到此消息

// 发送者：
// 第一步：创建套接字 socket()
// 第二步：设置为允许发送广播权限 setsockopt()
// 第三步：向广播地址发送数据 sendto()

/*
int setsockopt (int __fd, int __level, int __optname,const void *__optval, socklen_t __optlen)
__fd:       文件描述符socket
__level:    协议层次--> SOL_SOCKET 套接字层次  IPPROTO_TCP tcp层次  IPPROTO_IP IP层次
__optname:  选项名称--> SO_BROADCAST 允许发送广播数据（SOL_SOCKET层次的）
__optval:   选项开关值，int类型 0、1
__optlen:   选项开关值大小，
*/

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int sockfd;                      // 文件描述符
    struct sockaddr_in broadcataddr; // 服务器网络信息结构体
    socklen_t addrlen = sizeof(broadcataddr);

    // 第一步：创建套接字
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("fail to socket");
        exit(1);
    }

    // 第二步：设置为允许发送广播权限
    int on = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
    {
        perror("fail to setsockopt");
        exit(1);
    }

    // 第三步：填充广播信息结构体
    broadcataddr.sin_family = AF_INET;
    broadcataddr.sin_addr.s_addr = inet_addr(argv[1]); // 192.168.169.255 
    broadcataddr.sin_port = htons(atoi(argv[2]));

    // 第四步：进行通信
    char buf[128] = "";
    while (1)
    {
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = '\0';

        if (sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&broadcataddr, addrlen) < 0)
        {
            perror("fail to sendto");
            exit(1);
        }
    }

    return 0;
}