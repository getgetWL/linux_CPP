#include <stdio.h>

// 判断当前系统的字节序

union un
{
    int a;
    char b;
};

int main(int argc, char const *argv[])
{
    union un myun;
    myun.a = 0x12345678;

    printf("a = %#x\n", myun.a);
    printf("b = %#x\n", myun.b);

    if (myun.b == 0x78)
    {
        printf("小端存储\n");
    }
    else
    {
        printf("大端存储\n");
    }

    return 0;
}

#if 0//小端字节序
int main()
{
	int a = 0x11223344;

	char *p = (char*)&a;

	printf("*p=%#x\n", *p);
	return 0;
}

#endif
