#include <iostream>
#include <cstring>
#include <vector>
using namespace std;

class MyString
{
public:
    static size_t CCtor; // 统计调用拷贝构造函数的次数
    static size_t MCtor; // 统计调用移动构造函数的次数
    static size_t CAsgn; // 统计调用拷贝赋值函数的次数
    static size_t MAsgn; // 统计调用移动赋值函数的次数

public:
    // 构造函数
    MyString(const char *cstr = 0)
    {
        if (cstr)
        {
            m_data = new char[strlen(cstr) + 1];
            strcpy(m_data, cstr);
        }
        else
        {
            m_data = new char[1];
            *m_data = '\0';
        }
    }

    // 拷贝构造函数
    MyString(const MyString &str)
    {
        CCtor++;
        m_data = new char[strlen(str.m_data) + 1];
        strcpy(m_data, str.m_data);
    }
    // 移动构造函数
    MyString(MyString &&str) noexcept
        : m_data(str.m_data)
    {
        MCtor++;
        str.m_data = nullptr; // 不再指向之前的资源了
    }

    // 拷贝赋值函数 =号重载
    MyString &operator=(const MyString &str)
    {
        CAsgn++;
        if (this == &str) // 避免自我赋值!!
            return *this;

        delete[] m_data;
        m_data = new char[strlen(str.m_data) + 1];
        strcpy(m_data, str.m_data);
        return *this;
    }

    // 移动赋值函数 =号重载
    MyString &operator=(MyString &&str) noexcept
    {
        MAsgn++;
        if (this == &str) // 避免自我赋值!!
            return *this;

        delete[] m_data;
        m_data = str.m_data;
        str.m_data = nullptr; // 不再指向之前的资源了
        return *this;
    }

    ~MyString()
    {
        delete[] m_data;
    }

    char *get_c_str() const
    {
        return m_data;
    }

private:
    char *m_data;
};

size_t MyString::CCtor = 0;
size_t MyString::MCtor = 0;
size_t MyString::CAsgn = 0;
size_t MyString::MAsgn = 0;

int main()
{
    vector<MyString> vecStr;
    vecStr.reserve(100); // 先分配好100个空间
    for (int i = 0; i < 50; i++)
    {
        //而MyString("hello")是个临时对象，是个右值，优先进入移动构造函数而不是拷贝构造函数。
        //而移动构造函数与拷贝构造不同，它并不是重新分配一块新的空间，将要拷贝的对象复制过来，而是"偷"了过来，将自己的指针指向别人的资源，然后将别人的指针修改为nullptr，
        vecStr.push_back(MyString("hello"));
    }

    MyString str1("hello");         // 调用构造函数
    MyString str2("world");         // 调用构造函数
    MyString str3(str1);            // 调用拷贝构造函数
    MyString str4(std::move(str1)); // 调用移动构造函数,std::move就是将左值转换为右值
    // cout << str1.get_c_str() << endl; // 此时str1的内部指针已经失效了！不要使用
    // 注意：虽然str1中的m_dat已经称为了空，但是str1这个对象还活着，知道出了它的作用域才会析构！而不是move完了立刻析构
    MyString str5;
    str5 = str2;                    // 调用拷贝赋值函数

    cout << "CCtor = " << MyString::CCtor << endl;
    cout << "MCtor = " << MyString::MCtor << endl;
    cout << "CAsgn = " << MyString::CAsgn << endl;
    cout << "MAsgn = " << MyString::MAsgn << endl;

    return 0;
}

/* 结果
CCtor = 0
MCtor = 1000
CAsgn = 0
MAsgn = 0
*/
