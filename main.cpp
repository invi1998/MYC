#include <iostream>
#include <memory>     // 用于智能指针
#include <bitset>      // 用于输出二进制表示
#include <iomanip>
#include <cstring>
#include <vector>
#include "solution1/Complex.h"
#include "solution1/Date.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif

namespace test {
    void testFunction() {
        Complex c1;
        std::cin >> c1;
        std::cout << "你输入的复数是: " << c1 << std::endl;

        Complex c2;
        std::cin >> c2;
        std::cout << "另一个复数是: " << c2 << std::endl;

        Complex sum = c1 + c2;
        Complex difference = c1 - c2;
        Complex product = c1 * c2;
        Complex quotient = c1 / c2;
        std::cout << "和: " << sum << std::endl;
        std::cout << "差: " << difference << std::endl;
        std::cout << "积: " << product << std::endl;
        std::cout << "商: " << quotient << std::endl;
    }

    void testDate() {
        Date date;
        std::cin >> date;
        if (std::cin) {
            std::cout << "你输入的日期是: " << date << std::endl;
        } else {
            std::cout << "输入的日期格式不正确!" << std::endl;
        }
    }
}

void dumpBytes(const void* data, size_t size)
{
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    // low->high
    std::cout << "Raw bytes (low->high): ";
    for (size_t i = 0; i < size; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(bytes[i]) << ' ';
    }
    std::cout << std::dec << '\n';

    std::cout << "Binary bytes (low->high): ";
    for (size_t i = 0; i < size; ++i) {
        std::cout << std::bitset<8>(bytes[i]) << ' ';
    }
    std::cout << "\n\n";

    // high->low
    std::cout << "\nRaw bytes (high->low): ";
    for (size_t i = size; i > 0; --i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(bytes[i - 1]) << ' ';
    }
    std::cout << std::dec << '\n';

    std::cout << "Binary bytes (high->low): ";
    for (size_t i = size; i > 0; --i) {
        std::cout << std::bitset<8>(bytes[i - 1]) << ' ';
    }
    std::cout << "\n\n";
}

namespace test2{

    class NoVirtual {
        int a;
    };

    class WithVirtual {
        int a;
    public:
        virtual void f() {}
    };

    struct BitField
    {
        int a : 3;
        int b : 5;
        int c : 10;
        int d : 14;

    };

    union MyUnion {
        char c[5];
        int i;
    };

    struct Field
    {
        unsigned int a;
        unsigned int b;
    };

    struct BField
    {
        unsigned int a : 3;
        unsigned int b : 5;
        // 在没有添加额外位域的情况下，a 和 b 共占用 8 位（1 字节），但由于对齐要求，4字节对齐的情况下，整个结构体的大小可能会增加到 4 字节（32 位）。因此，BField 的大小可能是 4 字节。
        // 在添加一个额外的位域后，结构体的大小可能会增加到 8 字节（64 位），因为编译器可能会为新的位域分配一个新的存储单元，并且整个结构体的对齐要求可能会增加。
        char c : 1; // 添加一个额外的位域以增加结构体的大小
    };

    struct S {
        char a;
        double b;
        int c;
    };

    struct S2 {
        char c1;
        double d;
        char c2;
    };
    // 调整一下成员的顺序，看看是否能减少内存占用
    struct S3 {
        double d;
        char c1;
        char c2;
    };

    struct S4 {
        int a;
        char b[4];
        double c;
    };

    struct Inner {
        int i;
        short s;
    };

    struct Outer {
        char c;
        Inner inner;
        double d;
    };

    struct X {
        short s;
        double d;
    };

    struct Y {
        char c;
        X x;
        int i;
    };

    struct InnerArray {
        int arr[3];
        char ch;
    };

    struct Outer2 {
        double d;
        InnerArray ia;
        short s;
    };

    class Base {
        int a;
        virtual void f() {}
    };

    class Derived : public Base {
        char c;
    };

    void TestBitField()
    {
        std::cout << "NoVirtual 类的大小: " << sizeof(NoVirtual) << " 字节" << std::endl;
        std::cout << "WithVirtual 类的大小: " << sizeof(WithVirtual) << " 字节" << std::endl;

        std::cout << "BitField 结构体的大小: " << sizeof(BitField) << " 字节" << std::endl;

        std::cout << "MyUnion 联合体的大小: " << sizeof(MyUnion) << " 字节" << std::endl;
        MyUnion u;
        ::memset(&u, 0, sizeof(MyUnion)); // 将联合体内存初始化为0
        std::cout << "After memset:\n";
        unsigned int* pu = reinterpret_cast<unsigned int*>(&u);
        std::cout << "Raw bits: " << std::hex << *pu << std::dec << std::endl; // 输出原始二进制数据
        std::cout << "Binary: " << std::bitset<sizeof(MyUnion) * 8>(*pu) << "\n\n"; // 输出二进制表示
        dumpBytes(&u, sizeof(MyUnion));

        u.i = 0x12345678; // 给联合体的整数成员赋值
        std::cout << "After i = 0x12345678:\n";
        std::cout << "Raw bits: " << std::hex << *pu << std::dec << std::endl; // 输出原始二进制数据
        std::cout << "Binary: " << std::bitset<sizeof(MyUnion) * 8>(*pu) << "\n\n"; // 输出二进制表示
        dumpBytes(&u, sizeof(MyUnion));

        u.c[0] = 'A'; // 给联合体的字符数组成员赋值
        std::cout << "After c[0] = 'A':\n";
        std::cout << "Raw bits: " << std::hex << *pu << std::dec << std::endl; // 输出原始二进制数据
        std::cout << "Binary: " << std::bitset<sizeof(MyUnion) * 8>(*pu) << "\n\n"; // 输出二进制表示
        dumpBytes(&u, sizeof(MyUnion));

        u.c[1] = 'B'; // 给联合体的字符数组成员赋值
        std::cout << "After c[1] = 'B':\n";
        std::cout << "Raw bits: " << std::hex << *pu << std::dec << std::endl; // 输出原始二进制数据
        std::cout << "Binary: " << std::bitset<sizeof(MyUnion) * 8>(*pu) << "\n\n"; // 输出二进制表示
        dumpBytes(&u, sizeof(MyUnion));

        u.c[4] = 'E'; // 给联合体的字符数组成员赋值
        std::cout << "After c[4] = 'E':\n";
        std::cout << "Raw bits: " << std::hex << *pu << std::dec << std::endl; // 输出原始二进制数据
        std::cout << "Binary: " << std::bitset<sizeof(MyUnion) * 8>(*pu) << "\n\n"; // 输出二进制表示
        dumpBytes(&u, sizeof(MyUnion));

        std::cout << "Field 结构体的大小: " << sizeof(Field) << " 字节" << std::endl;
        std::cout << "BField 结构体的大小: " << sizeof(BField) << " 字节" << std::endl;

        BField bf;
        ::memset(&bf, 0, sizeof(BField)); // 将结构体内存初始化为0
        bf.a = 5; // 5 的二进制表示为 0000 0101，适合3位
        std::cout << "After a = 5:\n";
        unsigned int* p = reinterpret_cast<unsigned int*>(&bf);
        std::cout << "Raw bits: " << std::hex << *p << std::dec << std::endl; // 输出原始二进制数据
        std::cout << "Binary: " << std::bitset<sizeof(BField) * 8>(*p) << "\n\n"; // 输出二进制表示
        dumpBytes(&bf, sizeof(BField));

        bf.b = 10; // 10 的二进制表示为 0000 1010，适合5位
        std::cout << "After b = 10:\n";
        std::cout << "Raw bits: " << std::hex << *p << std::dec << std::endl; // 输出原始二进制数据
        std::cout << "Binary: " << std::bitset<sizeof(BField) * 8>(*p) << "\n\n"; // 输出二进制表示
        dumpBytes(&bf, sizeof(BField));

        bf.c = 1; // 1 的二进制表示为 1，适合1位
        std::cout << "After c = 1:\n";
        std::cout << "Raw bits: " << std::hex << *p << std::dec << std::endl; // 输出原始二进制数据
        std::cout << "Binary: " << std::bitset<sizeof(BField) * 8>(*p) << "\n\n"; // 输出二进制表示
        dumpBytes(&bf, sizeof(BField));
    }

    void testStructAlignment()
    {
        std::cout << "S 结构体的大小: " << sizeof(S) << " 字节" << std::endl;
        S s = {'A', 3.14, 42};
        dumpBytes(&s, sizeof(S));

        std::cout << "S2 结构体的大小: " << sizeof(S2) << " 字节" << std::endl;
        S2 s2 = {'B', 2.718, 'C'};
        dumpBytes(&s2, sizeof(S2));

        std::cout << "S3 结构体的大小: " << sizeof(S3) << " 字节" << std::endl;
        S3 s3 = {1.618, 'D', 'E'};
        dumpBytes(&s3, sizeof(S3));

        std::cout << "S4 结构体的大小: " << sizeof(S4) << " 字节" << std::endl;

        char a[5] = {1, 2, 3, 4, 5};
        std::cout << "数组 a 的大小: " << sizeof(a) << " 字节" << std::endl;

        std::cout << "Outer 结构体的大小: " << sizeof(Outer) << " 字节" << std::endl;

        std::cout << "Y 结构体的大小: " << sizeof(Y) << " 字节" << std::endl;
        std::cout << "InnerArray 结构体的大小: " << sizeof(InnerArray) << " 字节" << std::endl;
        std::cout << "Outer2 结构体的大小: " << sizeof(Outer2) << " 字节" << std::endl;

        std::cout << "Base 类的大小: " << sizeof(Base) << " 字节" << std::endl;
        std::cout << "Derived 类的大小: " << sizeof(Derived) << " 字节" << std::endl;

    }

    // 测试打印普通菱形继承和虚继承的内存布局以及内存大小
    class A {
    public:
        int a;
    };

    class B : public A {
    public:
        int b;
    };

    class C : public A {
    public:
        int c;
    };

    class D : public B, public C {
    public:
        int d;
    };

    class A_Virtual {
    public:
        int a;
        virtual ~A_Virtual() = default;
    };

    class B_Virtual : virtual public A_Virtual {
    public:
        int b;
    };

    class C_Virtual : virtual public A_Virtual {
    public:
        int c;
    };

    class D_Virtual : public B_Virtual, public C_Virtual {
    public:
        int d;
    };

    void testDiamondInheritance() {
        std::cout << "普通菱形继承:" << std::endl;
        std::cout << "A 的大小: " << sizeof(A) << " 字节" << std::endl;
        std::cout << "B 的大小: " << sizeof(B) << " 字节" << std::endl;
        std::cout << "C 的大小: " << sizeof(C) << " 字节" << std::endl;
        std::cout << "D 的大小: " << sizeof(D) << " 字节" << std::endl;

        std::cout << "\n虚继承:" << std::endl;
        std::cout << "A_Virtual 的大小: " << sizeof(A_Virtual) << " 字节" << std::endl;
        std::cout << "B_Virtual 的大小: " << sizeof(B_Virtual) << " 字节" << std::endl;
        std::cout << "C_Virtual 的大小: " << sizeof(C_Virtual) << " 字节" << std::endl;
        std::cout << "D_Virtual 的大小: " << sizeof(D_Virtual) << " 字节" << std::endl;
    }

}

namespace test3 {
    // RTTI（Run-Time Type Information）是C++的一项特性，允许程序在运行时获取对象的类型信息。RTTI主要通过typeid操作符和dynamic_cast运算符来实现。
    class Base {};
    class Derived : public Base {};

    void testRTTI() {
        Base base;
        Derived derived;
        std::cout << "Base 对象的类型: " << typeid(base).name() << std::endl;
        std::cout << "Derived 对象的类型: " << typeid(derived).name() << std::endl;

        Base* basePtr = &derived; // 基类指针指向派生类对象
        std::cout << "通过基类指针获取的类型: " << typeid(basePtr).name() << std::endl;
        std::cout << "通过基类指针获取的类型: " << typeid(*basePtr).name() << std::endl;
    }

    class A 
    {
    public:
        std::vector<int> v;

        A()
        {
            std::cout << "调用了默认构造函数" << std::endl;
        }

        // 拷贝构造函数
        A(const A& other) : v(other.v)
        {
            std::cout << "调用了拷贝构造函数" << std::endl;
        }

        // 移动构造
        A(A&& other) noexcept : v(std::move(other.v))
        {
            std::cout << "调用了移动构造函数" << std::endl;
        }

    };

    A createA()
    {
        A a;
        a.v = {1, 2, 3, 4, 5};
        return a; // C++17 以后这里通常会发生拷贝消除（看不到拷贝/移动构造）
    }

    A createAForceMove()
    {
        A a;
        a.v = {6, 7, 8, 9, 10};
        return std::move(a); // 教学演示：显式 move，通常可观察到移动构造
    }

    A createACopy(const A& src)
    {
        return src; // 入参是 const lvalue，通常触发拷贝构造
    }

    void testRef()
    {
        std::cout << "\n[场景1] createA()：通常发生拷贝消除\n";
        A a1 = createA();
        std::cout << "a1.v 的内容: ";
        for (int num : a1.v) {
            std::cout << num << ' ';
        }
        std::cout << std::endl;

        std::cout << "\n[场景2] createAForceMove()：通常会调用移动构造\n";
        A a2 = createAForceMove();
        std::cout << "a2.v 的内容: ";
        for (int num : a2.v) {
            std::cout << num << ' ';
        }
        std::cout << std::endl;

        std::cout << "\n[场景3] createACopy(const A&)：通常会调用拷贝构造\n";
        A a3 = createACopy(a1);
        std::cout << "a3.v 的内容: ";
        for (int num : a3.v) {
            std::cout << num << ' ';
        }
        std::cout << std::endl;
    }

    // 实现了前置++，后置++运算符的模板类
    template<typename T>
    class Counter {
    private:
        T value;
    public:
        Counter(T val) : value(val) {}
        // 前置++
        Counter& operator++() {
            ++value;
            return *this;
        }
        // 后置++
        Counter operator++(int)
        {
            Counter temp = *this; // 保存当前状态
            ++value; // 增加值
            return temp; // 返回原来的状态
        }

        // 比较运算符重载
        bool operator<(const Counter& other) const {
            return value < other.value;
        }

        bool operator==(const Counter& other) const {
            return value == other.value;
        }

        // 输出运算符重载
        friend std::ostream& operator<<(std::ostream& os, const Counter& c) {
            os << c.value;
            return os;
        }


    };

    void testRef2()
    {
        std::string&& rstr = "Hello, World!";

        Counter<int> c(0);
        std::cout << "前置++" << std::endl;
        while (++c < 5) {
            std::cout << " " << c;
        }

        std::cout << "\n" << std::endl;

        Counter<int> c2(0);
        std::cout << "后置++" << std::endl;
        while (c2++ < 5) {
            std::cout << " " << c2;
        }

        std::cout << std::endl;

    }
}


int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // test::testFunction();
    // test::testDate();
    // VSCode的代码块缩进快捷键是 Ctrl + Shift + [ 或 Ctrl + Shift + ]，可以用来折叠或展开代码块。
    // 缩进快捷键（Ctrl + [ 或 Ctrl + ]）可以用来增加或减少当前行或选中行的缩进级别。

    // test2::testStructAlignment();
    test2::testDiamondInheritance();
    // test3::testRTTI();
    // test3::testRef();
    // test3::testRef2();

#ifdef _WIN32
    std::cout << "程序执行完成，按任意键退出..." << std::endl;
    _getch();
#else
    std::cout << "程序执行完成，按回车退出..." << std::endl;
    std::cin.get();
#endif

    return 0;
}
