/*
universal references(通用引用)

template<typename T>
void f( T&& param)
{
    g(forward<T>(param));
}

函数模板的&&是一个未定义的引用类型，称为universal references，它必须被初始化，它是左值引用还是右值引用却决于它的初始化，

注意:只有当发生自动类型推断时（如函数模板的类型自动推导，或auto关键字），&&才是一个universal references

在C++源码中std::forward<T>(t)重载两个版本:
template<typename T>
接收左值:
T&& forward(typename std::remove_reference<T>::type& t) noexcept
{
    return static_cast<T&&>(t);
}
接收右值:
template<typename T>
T&& forward(typename std::remove_reference<T>::type&& t) noexcept
{
    return static_cast<T&&>(t);
}

引用折叠: 当模板实例化实际参数类型是引用时,它与模板代码中出现的引用结合在一起,会出现引用的引用
则需要将其转化为单一的引用
T& &   ==> T&
T& &&  ==> T&
T&& &  ==> T&
T&& && ==> T&&

1 如果传入的是string的左值,则T会被替换为 string&
    void f( string& && param) 折叠为string&
    {
        g(forward<string&>(param)); ==> 将string&在forward中处理: return static_cast<string& &&>(t); 
                                        再引用折叠规则, return static_cast<string&>(t);将t静态转化为左值引用
    }

2 如果传入的是string的右值时,模板传入g()的依旧时左值形参string,使用forward左值引用的重载(但返回右值),则T会被替换为 string
    void f( string&& param)
    {
        g(forward<string>(param));  ==> 在forward中处理: return static_cast<string&&>(t); 将t静态转化为右值引用
    }

*/

#include <iostream>
#include <cstring>
#include <vector>
using namespace std;

#if 0

#include <iostream>
#include <type_traits>
#include <string>
using namespace std;

template<typename T>
void f(T&& param){
    if (std::is_same<string, T>::value)
        std::cout << "string" << std::endl;
    else if (std::is_same<string&, T>::value)
        std::cout << "string&" << std::endl;
    else if (std::is_same<string&&, T>::value)
        std::cout << "string&&" << std::endl;
    else if (std::is_same<int, T>::value)
        std::cout << "int" << std::endl;
    else if (std::is_same<int&, T>::value)
        std::cout << "int&" << std::endl;
    else if (std::is_same<int&&, T>::value)
        std::cout << "int&&" << std::endl;
    else
        std::cout << "unkown" << std::endl;
}

int main()
{
    int x = 1;
    f(1); // 参数是右值 T推导成了int, 所以是int&& param, 右值引用
    f(x); // 参数是左值 T推导成了int&, 所以是int&&& param, 折叠成 int&,左值引用
    int && a = 2;
    f(a); //虽然a是右值引用，但它还是一个左值， T推导成了int&
    string str = "hello";
    f(str); //参数是左值 T推导成了string&
    f(string("hello")); //参数是右值， T推导成了string
    f(std::move(str));//参数是右值， T推导成了string
}

#endif

void RunCode(int &&m)
{
    cout << "rvalue ref" << endl;
}
void RunCode(int &m)
{
    cout << "lvalue ref" << endl;
}
void RunCode(const int &&m)
{
    cout << "const rvalue ref" << endl;
}
void RunCode(const int &m)
{
    cout << "const lvalue ref" << endl;
}

// 这里利用了universal references，如果写T&,就不支持传入右值，而写T&&，既能支持左值，又能支持右值
// 在universal references和std::forward的合作下，才能够完美的转发,缺一不可
template <typename T>
void perfectForward(T &&t)
{
    RunCode(std::forward<T>(t));
}

template <typename T>
void notPerfectForward(T &&t)
{
    //此时t是有值的变量,是左值,所以传入RunCode(t)都是左值
    RunCode(t);
}

int main()
{
    int a = 0;
    int b = 0;
    const int c = 0;
    const int d = 0;

    notPerfectForward(a);       // lvalue ref
    notPerfectForward(move(b)); // lvalue ref
    notPerfectForward(c);       // const lvalue ref
    notPerfectForward(move(d)); // const lvalue ref

    cout << endl;
    perfectForward(a);       // lvalue ref
    perfectForward(move(b)); // rvalue ref
    perfectForward(c);       // const lvalue ref
    perfectForward(move(d)); // const rvalue ref
}
