#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


typedef struct node{
    int value;
    struct node *next;
}NODE;

typedef struct pack{
    unsigned char ver;
    unsigned char type;
    unsigned int  len;
    char data[0];
}PACK;

void show(NODE *head){
    if(head==NULL){
	printf("空链表！\n");
    }
    for(;head!=NULL;head=head->next){
	printf("[%d]-->",head->value);
    }
}

NODE* list_node(){
    NODE *pnew=NULL;
    NODE *head=NULL;
    char ch='Y';
    while(ch=='Y'||ch=='y'){
	pnew=(NODE*)malloc(sizeof(NODE));
	assert(pnew!=NULL);
	pnew->next=NULL;
	printf("请输入节点value:");
	scanf("%d",&(pnew->value));
	while(getchar()!='\n');

	pnew->next=head;
	head=pnew;

	printf("是否添加节点：");
	scanf("%c",&ch);
	while(getchar()!='\n');
    }
    return head;
}
int len_list(NODE *head){
    int len=0;
    while(head!=NULL){
	head=head->next;
	len++;
    }
    return len;
}
NODE *free_list(NODE *head){
    NODE *temp=NULL;
    while(head!=NULL){
	temp=head;
	head=head->next;
	free(temp);
    }
    return NULL;
}

void get_signal(int sign){
    printf("receive signal:%d\n",sign);
}

int main(){
    signal(SIGPIPE,get_signal);
    int sockfd,ret;
    sockfd=socket(AF_INET,SOCK_STREAM,0);//创立套接字
    if(sockfd<0){
	perror("socket");
	return -1;
    }

    struct sockaddr_in seraddr;
    seraddr.sin_family=AF_INET;
    seraddr.sin_port=htons(8000);
    inet_pton(AF_INET,"127.0.0.1",&seraddr.sin_addr.s_addr);

    ret=connect(sockfd,(struct sockaddr*)&seraddr,sizeof(struct sockaddr_in));
    if(ret<0){
	perror("connect");
	return -1;
    }
    NODE *head=NULL;
    PACK *pk=NULL;
    pk=(PACK*)malloc(sizeof(PACK));
    assert(pk!=NULL);
    while(1){
	head=list_node();
	show(head);
	printf("\n");
	int len=len_list(head);
	pk->ver=0;
	pk->type=0;
	pk->len=len;
	printf("传输包头\n");
	ret=write(sockfd,pk,sizeof(PACK));
	printf("传输包头结束\n");
	if(ret<0){
	    printf("write\n");
	    break;
	}
	NODE *temp=NULL;
	temp=head;
	while(temp!=NULL){
	    ret=write(sockfd,temp,sizeof(NODE));
	    if(ret<0){
		perror("write");
		break;
	    }
	    temp=temp->next;
	}
	printf("发送成功\n");
	head=free_list(head);
    }
    free(pk);
    close(sockfd);
    return 0;
    }
