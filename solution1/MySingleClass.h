#ifndef __MY_SINGLE_CLASS_H__
#define __MY_SINGLE_CLASS_H__

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

// 单例模式的实现(线程安全版本)
class MySingleton
{
public:

    static MySingleton* getInstancePtr()
    {
        // 如果是在多线程环境下，可能会有多个线程同时调用 getInstancePtr() 方法，从而导致创建多个 MySingleton 实例的情况。
        // 为了避免这种情况，我们可以使用双重检查锁定（Double-Checked Locking）来确保线程安全。
        // if (instance == nullptr) // 第一次检查，避免不必要的锁操作
        // {
        //     std::lock_guard<std::mutex> lock(mtx); // 加锁，确保只有一个线程能够进入创建实例的代码块
        //     if (instance == nullptr)
        //     {
        //         std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟一些工作，增加线程切换的机会，测试单例在多线程环境下的正确性
        //         instance = new MySingleton();
        //         static GC gc; // 定义一个静态的 GC 对象，负责在程序结束时自动销毁单例实例
        //         // 因为静态局部变量的初始化是线程安全的，所以在多线程环境下，这种实现方式也能保证单例实例的唯一性和线程安全性。
        //         // 而且静态成员变量的生命周期是整个程序运行期间，所以单例实例也会一直存在，直到程序结束时才会被销毁。
        //         // 程序结束时，GC 对象的析构函数会被调用，从而删除单例实例，确保资源的正确释放。
        //     }
        // }
        
        // 使用std::call_once和std::once_flag来实现线程安全的单例模式，避免了双重检查锁定的复杂性。
        static std::once_flag flag; // 定义一个 once_flag 对象，用于控制单例实例的初始化
        std::call_once(flag, []() { // 使用 call_once 来确保单例实例只被创建一次
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟一些工作，增加线程切换的机会，测试单例在多线程环境下的正确性
            instance = new MySingleton();
            static GC gc; // 定义一个静态的 GC 对象，负责在程序结束时自动销毁单例实例
        });


        return instance;
    }

private:
    static MySingleton* instance;
    static std::mutex mtx; // 互斥锁，用于保护单例实例的创建过程

    class GC
    {
    public:
        ~GC()
        {
            if (MySingleton::instance != nullptr)
            {
                delete MySingleton::instance;
                MySingleton::instance = nullptr;
            }
        }
    };

private:
    MySingleton() = default;
    ~MySingleton() = default;
    MySingleton(const MySingleton&) = delete;
    MySingleton& operator=(const MySingleton&) = delete;
};

MySingleton* MySingleton::instance = nullptr; // 初始化静态成员变量
std::mutex MySingleton::mtx; // 初始化静态成员变量

// 虽然上述写法能实现线程安全的单例模式，
// 但在 C++11 及以上版本中，我们可以利用静态局部变量的线程安全特性来简化实现，
// 避免使用 mutex 和双重检查锁定。以下是更简洁的实现方式：
class MoreElegantSingleton
{
public:
    static MoreElegantSingleton& getInstance()
    {
        static MoreElegantSingleton instance; // C++11 及以上版本保证线程安全的静态局部变量
        return instance;
    }
};



void testSingleton()
{
    // MySingleton* s1 = MySingleton::getInstancePtr();
    // MySingleton* s2 = MySingleton::getInstancePtr();

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([]() {
            MySingleton* s = MySingleton::getInstancePtr();
            std::cout << "线程 ID: " << std::this_thread::get_id() << ", 单例实例地址: " << s << std::endl;
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\n--------------使用更简洁的单例实现方式:-------------------" << std::endl;
    std::vector<std::thread> threads2;
    for (int i = 0; i < 10; ++i) {
        threads2.emplace_back([]() {
            MoreElegantSingleton& s = MoreElegantSingleton::getInstance();
            std::cout << "线程 ID: " << std::this_thread::get_id() << ", 单例实例地址: " << &s << std::endl;
        });
    }
    for (auto& t : threads2) {
        t.join();
    }
}



#endif // __MY_SINGLE_CLASS_H__