/*
虚继承：解决菱形继承的冗余问题，为最远派生类提供唯一基类成员
        class Base1:virtual public Base
        class Base2:virtual public Base
虚基类：Base
1、虚基类的构造函数需要在每层派生类的构造函数初始化列表中显式调用。
   如果虚基类有带参数的构造函数，需要在所有派生类的构造函数初始化列表中明确调用虚基类的构造函数，并传递相应的参数。
*/

#include <iostream>

class Base
{
public:
    Base(int arg0):date(arg0)
    {
        std::cout<<"using Base::Base() "<<std::endl;
    };
    ~Base()
    {
        std::cout<<"using Base::~Base() "<<std::endl;
    };
    int date;
    void fun()
    {
        std::cout<<"using Base::fun() "<<std::endl;
    }
};

class Base1:virtual public Base
{
public:
    Base1(int arg1,int arg0):Base(arg0),date1(arg1)
    {
        std::cout<<"using Base1::Base1() "<<std::endl;
    };
    ~Base1()
    {
        std::cout<<"using Base1::~Base1() "<<std::endl;
    };
    int date1;
};

class Base2:virtual public Base
{
public:
    Base2(int arg2,int arg0):Base(arg0),date2(arg2)
    {
        std::cout<<"using Base2::Base2() "<<std::endl;
    };
    ~Base2()
    {
        std::cout<<"using Base2::~Base2() "<<std::endl;
    };
    int date2;
};

class Derived:public Base1,public Base2
{
public:
    //虚基类的构造函数只在最远派生类Base(arg0)传参生效
    //Base1(arg1,arg0),Base2(arg2,arg0)会被忽略
    //遵循先给基类传参，后给本类成员传参原则，析构相反
    Derived(int arg0,int arg1,int arg2,int arg3):Base(arg0),Base1(arg1,arg0),Base2(arg2,arg0),date3(arg3)
    {
        std::cout<<"using Derived::Derived() "<<std::endl;
    };
    ~Derived()
    {
        std::cout<<"using Derived::~Derived() "<<std::endl;
    };
    int date3;
};

int main()
{
    Derived d(0,1,2,3);
    std::cout<<"d.date: "<<d.date
             <<" d.date1: "<<d.date1
             <<" d.date1: "<<d.date2
             <<" d.date3: "<<d.date3
             <<std::endl;
    return 0;
}

/*
using Base::Base() 
using Base1::Base1() 
using Base2::Base2() 
using Derived::Derived() 
d.date: 0 d.date1: 1 d.date1: 2
using Derived::~Derived() 
using Base2::~Base2() 
using Base1::~Base1() 
using Base::~Base() 
*/