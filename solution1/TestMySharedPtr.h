#include "MyShared_ptr.h"
#include "MyWeakPtr.h"
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

void testMySharedPtr() {
    MySharedPtr<int> p1(new int(5)); // 创建一个 MySharedPtr，管理一个整数值 5
    std::cout << "p1: " << *p1 << std::endl; // 输出 p1 管理的值

    MySharedPtr<int> p2 = p1; // 使用拷贝构造函数，p2 现在也管理同一个整数值
    std::cout << "p2: " << *p2 << std::endl; // 输出 p2 管理的值

    // 这种情况不合法，因为我们已经取消了隐式类型转换explicit MySharedPtr(T* p = nullptr)，所以必须显式调用构造函数
    // MySharedPtr<int> p3; // 创建一个空的 MySharedPtr
    // p3 = p1; // 使用赋值运算符，p3 现在也管理同一个整数值
    // std::cout << "p3: " << *p3 << std::endl; // 输出 p3 管理的值

    // 当函数结束时，p1、p2 和 p3 都会被销毁，引用计数会正确地减少，并且当最后一个 MySharedPtr 被销毁时，管理的整数值会被释放。
}

void testMySharedPtrMultithread() {
    MySharedPtr<int> p(new int(10)); // 创建一个 MySharedPtr，管理一个整数值 10

    auto threadFunc = [p]() mutable {
        for (int i = 0; i < 5; ++i) {
            std::cout << "Thread " << std::this_thread::get_id() << ": " << *p << std::endl;
            // 让线程休眠一段时间，以增加线程切换的机会，测试 MySharedPtr 在多线程环境下的正确性
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    std::thread t1(threadFunc);
    std::thread t2(threadFunc);

    t1.join();
    t2.join();
}

struct NodeStrongCycle {
    std::string name;
    MySharedPtr<NodeStrongCycle> next;

    explicit NodeStrongCycle(std::string n) : name(std::move(n)) {}

    ~NodeStrongCycle() {
        std::cout << "~NodeStrongCycle: " << name << std::endl;
    }
};

struct NodeWeakCycle {
    std::string name;
    MySharedPtr<NodeWeakCycle> next;
    MyWeakPtr<NodeWeakCycle> prev;

    explicit NodeWeakCycle(std::string n) : name(std::move(n)) {}

    ~NodeWeakCycle() {
        std::cout << "~NodeWeakCycle: " << name << std::endl;
    }
};

void testCycleReferenceCounter() {
    std::cout << "\n===== 场景1：纯 MySharedPtr 循环引用 =====" << std::endl;
    {
        MySharedPtr<NodeStrongCycle> a(new NodeStrongCycle("A"));
        MySharedPtr<NodeStrongCycle> b(new NodeStrongCycle("B"));

        a->next = b;
        b->next = a;

        std::cout << "a.use_count() = " << a.use_count() << std::endl;
        std::cout << "b.use_count() = " << b.use_count() << std::endl;
    }
    std::cout << "离开作用域后若看不到 ~NodeStrongCycle 输出，说明循环引用导致未释放。" << std::endl;

    std::cout << "\n===== 场景2：一侧改为 MyWeakPtr 打破环 =====" << std::endl;
    {
        MySharedPtr<NodeWeakCycle> a(new NodeWeakCycle("A"));
        MySharedPtr<NodeWeakCycle> b(new NodeWeakCycle("B"));

        a->next = b;
        b->prev = a;

        std::cout << "a.use_count() = " << a.use_count() << std::endl;
        std::cout << "b.use_count() = " << b.use_count() << std::endl;
        std::cout << "b.prev.use_count() = " << b->prev.use_count() << std::endl;
        std::cout << "b.prev.expired() = " << (b->prev.expired() ? "true" : "false") << std::endl;

        MySharedPtr<NodeWeakCycle> lockFromWeak = b->prev.lock();
        std::cout << "lock 是否成功: " << (lockFromWeak.get() ? "true" : "false") << std::endl;
        std::cout << "lock 后 a.use_count() = " << a.use_count() << std::endl;
    }
    std::cout << "离开作用域后应看到 ~NodeWeakCycle 析构输出。" << std::endl;
}
