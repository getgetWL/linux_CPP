/*-------------单向无头节点链表----------*/
#ifndef STUDENT_LIST
#define STUDENT_LIST

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct node
{
    int value;
    struct node *next;
} NODE;

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

// 前插创建链表
NODE *list_node()
{
    NODE *pnew = NULL;
    NODE *head = NULL;
    char ch = 'Y';
    while (ch == 'Y' || ch == 'y')
    {
        pnew = (NODE *)malloc(sizeof(NODE));
        assert(pnew != NULL);
        pnew->next = NULL;
        printf("请输入节点value:");
        scanf("%d", &(pnew->value));
        while (getchar() != '\n')
            ;

        pnew->next = head;
        head = pnew;

        printf("是否添加节点：");
        scanf("%c", &ch);
        while (getchar() != '\n')
            ;
    }
    return head;
}

// 长度
int len_list(NODE *head)
{
    int len = 0;
    while (head != NULL)
    {
        head = head->next;
        len++;
    }
    return len;
}
// 释放链表
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

NODE *find_by_value(NODE *head, int f_value)
{
    while (head != NULL)
    {
        if (head->value == f_value)
        {
            break;
        }
        head = head->next;
    }
    return head;
}

#endif
