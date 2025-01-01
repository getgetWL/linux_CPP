#! /bin/bash
# 预定义变量
#   $0  表示本脚本的文件名
#   $1  表示传递给程序的第一个参数，并以此类推 $2...$9等等
#   $#  包含命令行参数的个数
#   $@  包含所有命令行参数："$1 ... $9"
#   $?  包含前一个命令的退出状态，正常退出返回0，反之为非0
#   $*  包含所有命令行参数："$1 ... $9"
#   $$  包含正在执行进程的ID号


echo "program name is $0"
# 输入参数个数
echo "There are totally $# parameters passed to this program"
# 展示全部输入参数
echo "the parameters are $*"
echo "the parameters are $@"
#上一命令执行结果
echo "the last result is $?"


#整数比较
#   a -eq b 测试a、b是否相等
#   a -ne b 测试a、b是否不相等
#   a -gt b 测试a是否大于b
#   a -ge b 测试a是否大于等于b
#   a -lt b 测试a是否小于b
#   a -le b 测试a是否小于等于b
if [ $# -eq 1 ]; then
    touch $1
    echo "touch $1 already"
else
    echo "there is no parameters!"
fi

#文件测试
#   -d name 测试name是否为一个目录
#   -f name 测试name是否为普通文件
#   -L name 测试name是否为符号链接
#   -r name 测试name是否为存在且为可读
#   -w name 测试name是否为存在且为可写
#   -x name 测试name是否为存在且为可执行
#   -s name 测试name是否为存在且其长度不为0
#   f1 -nt f2 测试文件f1是否比文件f2更新
#   f1 -ot f2 测试文件f1是否比文件f2更旧

if [ -d "a.txt" ]; then
    echo "there is catalogue"
else
    echo "a.txt is not a catalogue"
fi

if [ -d "lll" ]; then
    echo "there is catalogue"
else
    echo "lll is not a catalogue"
fi


#从键盘读入变量值 关键词 read
# echo "Input date with formal yyyy mm dd"
# read year month day
# echo "the day is $year/$month/$day"


#算术运算符 关键词  expr
#注意定义变量和运算符时，=不能有空格
val1=1
val2=2
add=`expr $val1 + $val2`
echo "val1 + val2 ="$add