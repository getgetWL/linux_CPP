#include <iostream>
using namespace std;

/*
1、左值引用
    左值是指表达式结束后依然存在的持久化对象，所有的具名变量或者对象都是左值，而右值不具名。
2、右值应用
    右值是指表达式结束时就不再存在的临时对象. 右值分为将亡值和纯右值。
    纯右值:c++98标准中右值的概念，如非引用返回的函数返回的临时变量值；一些运算表达式，如1+2产生的临时变量；不跟对象关联的字面量值，如2，'c'，true这些值都不能够被取地址。
    将亡值:c++11新增的和右值引用相关的表达式，这样的表达式通常时将要移动的对象、T&&函数返回值、std::move()函数的返回值等.

    int&& a = 1; //实质上就是将不具名(匿名)变量取了个别名
    int b = 1;
    int && c = b; //编译错误！ 不能将一个左值复制给一个右值引用




*/

class Copyable
{
public:
    Copyable() {}
    Copyable(const Copyable &o)
    {
        cout << "Copied" << endl;
    }

    ~Copyable()
    {
        cout << "析构函数" << endl;
    }
};
Copyable ReturnRvalue()
{
    return Copyable(); // 返回一个临时对象,是右值
}
void AcceptVal(Copyable a)
{
    cout << "AcceptVal" << endl;
}
//常量左值引用重载版本可以绑定非常量左值、常量左值、右值，而且在绑定右值的时候，常量左值引用还可以像右值引用一样将右值的生命期延长，缺点是，只能读不能改
void AcceptRef(const Copyable &a)
{
    cout << "AcceptRef  &" << endl;
}

void AcceptRef(const Copyable &&a)
{
    cout << "AcceptRef  &&" << endl;
}

int main()
{
    //ReturnRvalue()返回的右值本来在表达式语句结束后，其生命也就该终结了（因为是临时变量），而通过右值引用，该右值又重获新生，
    //其生命期将与右值引用类型变量a的生命期一样，只要a还活着，该右值临时变量将会一直存活下去。实际上就是给那个临时变量取了个名字。
    //注意：这里a的类型是右值引用类型(int &&)，但是如果从左值和右值的角度区分它，它实际上是个左值。因为可以对它取地址，而且它还有名字，是一个已经命名的右值。
    {
        Copyable && a = ReturnRvalue();
        const Copyable & c = ReturnRvalue(); 
        // Copyable & b = ReturnRvalue();   //报错,因为ReturnRvalue()返回是右值,右值不可改变
    }
    Copyable temp;
    cout << "pass by value 值传参: " << endl;
    AcceptVal(ReturnRvalue());  // 应该调用两次拷贝构造函数

    cout << "pass by reference 引用传参: " << endl;
    AcceptRef(ReturnRvalue());  // ReturnRvalue()为右值调用右值引用,应该只调用一次拷贝构造函数
    AcceptRef(temp);            // temp为左值,调用左值引用
    return 0;
}

// 运行发现未调用拷贝构造函数

// 这是由于编译器默认开启了返回值优化(RVO/NRVO, RVO, Return Value Optimization 返回值优化，或者NRVO， Named Return Value Optimization)。
// 编译器很聪明，发现在ReturnRvalue内部生成了一个对象，返回之后还需要生成一个临时对象调用拷贝构造函数很麻烦。所以直接优化成了1个对象对象，避免拷贝;
// 而这个临时变量又被赋值给了AcceptVal函数的形参a，还是没必要。所以最后这三个变量都用一个变量替代了，不需要调用拷贝构造函数。

// 关闭返回值优化编译命令  g++ main.cpp -o main -fno-elide-constructors  --std=c++11