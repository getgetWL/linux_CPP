#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

typedef struct data
{
    int cfd;
    int port;
    char ip[64];
} DATA;

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
    printf("有子进程退出!\n");
    wait(NULL);
}

void show(NODE *head)
{
    if (head == NULL)
    {
        printf("空链表!");
    }
    for (; head != NULL; head = head->next)
    {
        printf("[%d]-->", head->value);
    }
    printf("\n");
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
void *pthread_task(void *argv)
{
    int ret;
    DATA temp = *((DATA *)argv);
    free(argv); // 释放申请的动态空间
    int cfd = temp.cfd;

    PACK *p = NULL;
    p = (PACK *)malloc(sizeof(PACK));
    assert(p != NULL);
    NODE *head = NULL;
    while (1)
    {
        printf("read  PACK...\n");
        ret = read(cfd, p, sizeof(PACK));
        printf("read PACK over, ret=%d\n", ret);
        if (ret == 0)
        {
            printf("客户端[%s||%d] 断开连接...\n", temp.ip, temp.port);
            break;
        }
        if (ret < 0)
        {
            perror("read");
            break;
        }
        int n = p->len;
        printf("客户端[%s||%d]传入%d个节点...\n", temp.ip, temp.port, n);

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
        show(head);
        head = free_list(head);
    }
    close(cfd);
    free(p);
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

    struct sockaddr_in seraddr;                                                  // 为套接字指定本地端口和地址
    seraddr.sin_family = AF_INET;                                                // 协议族主机字节序（AF_INET指定IPv4)
    seraddr.sin_port = htons(8000);                                              // 自定义端口
    inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);                   // 将点分十进制的IP转化为网络字节序形式的IP地址
    ret = bind(sockfd, (struct sockaddr *)&seraddr, sizeof(struct sockaddr_in)); // 套接字指定本地端口及IP
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
    // 接受客户端IP及端口信息
    struct sockaddr_in cliaddr; // IPv4地址族结构体变量
    int addrlen = sizeof(struct sockaddr_in);
    char ip[64];
    int port;

    // 并发线程
    pthread_t pth;
    while (1)
    {
        printf("accept...\n");
        int cfd;                                                     // 成功返回新的文件描述符，专门用来和建立连接的客户端
        cfd = accept(sockfd, (struct sockaddr *)&cliaddr, &addrlen); // 等待客户连接,接收客户端IP和端口号
        printf("accept over\n");
        if (cfd < 0)
        {
            perror("accept");
            return -1;
        }

        port = ntohs(cliaddr.sin_port);                       // 将网络端口字节序转化为主机端口字节序
        inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, 64); // 将网络字节序IP转化为主机字节序IP
        printf("客户端[%s||%d]连接成功！\n", ip, port);

        // 单独申请内存空间存储cfd及相关IP和端口信息，防止后面的线程的cfd的值覆盖前面的cfd
        DATA *temp = (DATA *)malloc(sizeof(DATA));
        assert(temp != NULL);
        temp->cfd = cfd;
        temp->port = port;
        strcpy(temp->ip, ip);

        if (pthread_create(&pth, NULL, pthread_task, temp) != 0)
        { // 传递temp数据
            perror("pthread_create");
            break;
        }
        // 设置分离属性，线程结束资源由系统回收
        pthread_detach(pth);
    }
    close(sockfd);
    return 0;
}
