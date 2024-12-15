#include <memory>
#include <mutex>
#include <iostream>

class MiddlewareDataDispatch
{
private:
    MiddlewareDataDispatch();
    MiddlewareDataDispatch(const MiddlewareDataDispatch &other) = delete;
    MiddlewareDataDispatch& operator= (const MiddlewareDataDispatch& other) = delete;
public:
    /*静态成员变量是在所有对象之外单独开辟空间，只要在类中定义了静态成员变量，即使不定义对象，也为静态成员变量分配空间，它可以被类对象引用。
    它不随对象的建立而分配空间，也不随对象的撤销而释放（一般成员变量是在对象建立时分配空间，在对象撤销时释放）。
    静态成员变量是在程序编译时被分配空间的，到程序结束的时候才释放空间*/
    static  std::unique_ptr<MiddlewareDataDispatch> insDisPatch_;
    static  MiddlewareDataDispatch* GetInstance();
    virtual ~MiddlewareDataDispatch(){};
};


std::unique_ptr<MiddlewareDataDispatch> MiddlewareDataDispatch::insDisPatch_;

MiddlewareDataDispatch* MiddlewareDataDispatch::GetInstance() {
    /*std::once_flag 是一个结构体，用于记录某个函数是否已经被调用过*/
    /*
    std::once_flag 和 std::call_once 的底层实现依赖于操作系统提供的同步机制，通常使用互斥锁（mutex）和条件变量（condition variable）来确保线程安全。
    std::once_flag 内部维护一个状态，用于记录函数是否已经被调用过。这个状态通常是一个原子变量（atomic variable），确保在多线程环境下的操作是原子的。
    std::call_once 内部使用一个互斥锁来保护对 std::once_flag 状态的访问。
    如果函数尚未被调用，std::call_once 会锁定互斥锁，执行函数，然后更新 std::once_flag 的状态。
    如果函数已经被调用，std::call_once 会立即返回，不会再次执行函数。
    */
    static std::once_flag ins_flag;
    /*在new后面加std::nothrow意思就是，如果空间不足，程序不会崩溃,返回nullptr*/
    std::call_once(ins_flag, [&]
                   { insDisPatch_.reset(new (std::nothrow) MiddlewareDataDispatch); });
    return insDisPatch_.get();
}

MiddlewareDataDispatch::MiddlewareDataDispatch() 
{

}

int main()
{
    MiddlewareDataDispatch* Middleware_1 = MiddlewareDataDispatch::GetInstance();
    MiddlewareDataDispatch* Middleware_2 = MiddlewareDataDispatch::GetInstance();
    std::cout<<"&Middleware_1 = "<<Middleware_1 <<std::endl;
    std::cout<<"&Middleware_2 = "<<Middleware_2 <<std::endl;

    return 0;
}


