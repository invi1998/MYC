#ifndef __MY_THREAD_H__
#define __MY_THREAD_H__

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

void ThreadFunc()
{
    for (int i = 0; i < 5; ++i) {
        std::cout << "线程函数执行: " << i << std::endl;
    }
}

class Task
{
    int& m_value;
public:
    Task(int& value) : m_value(value) {
        std::cout << "Task 的构造函数被调用，m_value: " << m_value << std::endl;
    }

    ~Task() {
        std::cout << "Task 的析构函数被调用，m_value: " << m_value << std::endl;
    }

    // 拷贝构造函数
    Task(const Task& other) : m_value(other.m_value) {
        std::cout << "Task 的拷贝构造函数被调用" << std::endl;
    }

    void operator()() const
    {
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 模拟一些工作,每次循环等待1秒
            std::cout << "Task 对象调用 operator(): " << i << ", m_value: " << m_value << std::endl;
        }
    }
};

void TestThread()
{
    // 1：使用常规函数创建线程
    // std::thread t(ThreadFunc); // 创建一个线程，执行 ThreadFunc 函数
    // if (t.joinable()) {
    //     std::cout << "等待线程执行完毕..." << std::endl;
    //     t.join(); // 等待线程执行完毕
    // }
    // if (t.joinable()) {
    //     std::cout << "线程仍然可连接，可能未正确 join。" << std::endl;
    //     t.detach(); // 如果线程仍然可连接，尝试分离线程
    // } else {
    //     std::cout << "线程已成功 join。" << std::endl;
    // }

    // 2：使用可调用类对象创建线程
    // int v = 42;
    // Task task(v); // 创建一个 Task 对象，传入一个整数引用
    // std::thread t2(task); // 创建一个线程，执行 Task 对象的 operator() 函数
    // if (t2.joinable()) {
    //     std::cout << "等待线程执行完毕..." << std::endl;
    //     t2.detach(); // 分离线程，允许它独立运行

    //     // 在detach之后，主线程继续执行，而子线程可能仍在运行。
    //     // 由于子线程访问了外部变量 v，如果主线程在子线程完成之前修改或销毁了 v，可能会导致未定义行为。
    //     // 因此，在使用 detach 时需要确保被线程访问的资源在子线程完成之前不会被修改或销毁。

    //     // 但是同样的问题，为什么对于这个task这个对象来说，主线程结束了，
    //     // 按理说这个task对象也会被栈回收掉，为什么子线程还能访问到这个对象呢？
    //     // 这是因为在创建线程时，Task 对象被复制了一份（通过调用 Task 的拷贝构造函数），
    //     // 并且这个副本被传递给了线程函数。因此，即使主线程中的 Task 对象被销毁了，子线程仍然持有一个有效的 Task 对象副本，可以继续访问它的成员变量 m_value。
    //     // 所以，只要Task对象里没有引用或者指针类型的成员变量，或者这些成员变量指向的资源在子线程完成之前不会被修改或销毁，就不会导致未定义行为。

    //     // t2.join(); // 等待线程执行完毕
    // }


    // 3：使用 lambda 表达式创建线程
    std::thread t3([]() {
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 模拟一些工作,每次循环等待1秒
            std::cout << "Lambda 表达式线程执行: " << i << std::endl;
        }
    });

    if (t3.joinable()) {
        std::cout << "等待线程执行完毕..." << std::endl;
        t3.join(); // 等待线程执行完毕
    }

}

void myprint(const int& i, char* buf, const std::string& str)
{
    // 我们通过打印i的地址和主线程中ref的地址来验证它们是否指向同一个内存位置。由于i是一个引用，指向主线程中的一个整数变量，因此它们的地址应该是相同的。
    std::cout << "i 的地址: " << &i << std::endl;
    std::cout << "buf 的地址: " << static_cast<void*>(buf) << std::endl;

    // 打印结果显示虽然i是引用，但是线程在传递参数时，确是进行了值传递，也就是说线程函数接收的i是主线程中整数变量的一个副本，而不是一个引用。
    // 因此，线程函数中的i和主线程中的整数变量是两个不同的对象，它们的地址也不同。

    // 在这个函数中，i 是一个 const int& 类型的参数，表示它是一个对整数的常量引用。buf 是一个 char* 类型的参数，表示它是一个指向字符数组的指针。
    // 这里的 i 是一个引用，指向主线程中的一个整数变量，而 buf 是一个字符数组。
    // 由于在创建线程时，i 和 buf 都被复制了一份（通过调用它们的拷贝构造函数），
    // 并且这个副本被传递给了线程函数，因此即使主线程中的 i 和 buf 被销毁了，
    // 子线程仍然持有一个有效的 i 和 buf 副本，可以继续访问它们的值。
    
    // 对于buf来说，就不像i那样是一个引用，而是一个指针类型的参数。
    // 而对于指针类型的参数来说，线程函数接收的是指针的值，也就是指针所指向的内存地址。
    // 因此，线程函数中的buf和主线程中的buf指向同一个内存地址，它们的地址是相同的。
    // 那么这种情况下，如果主线程detach了线程，主线程继续执行并且修改了buf指向的内存内容，那么子线程在访问buf时就可能会看到修改后的内容，这可能会导致未定义行为。
    // 所以，在给线程传递参数的时候，指针类型绝对是及其高危的操作

    // 然后对于str来说，虽然它是一个 const std::string& 类型的参数，表示它是一个对字符串的常量引用，
    // 但是在创建线程时，str 也被复制了一份（通过调用 std::string 的拷贝构造函数），
    // 所以，按理来说，就算主线程中的buf被提前销毁了，因为引用传参在创建线程时是进行了值传递的，
    // 所以线程函数接收的str也是主线程中字符串变量的一个副本，在场景线程的时候会进行隐式类型转换将字符串字面量转换为std::string对象，
    // 因此线程函数中的str和主线程中的字符串变量是两个不同的对象，它们的地址也不同。
    // 但是，这里就意味着这种情况下是线程安全的做法吗？
    // 不一定，有可能出现问题的点在于：传递参数时的隐式类型转换的时机
    // 如果我们隐式类型转换需要花费的时间较长，那么在detach之后，
    // 主线程迅速执行完毕，销毁了字符串变量，那么在字符串字面量转换为std::string对象的过程中，可能会访问到已经被销毁的字符串变量，从而导致未定义行为。

    // std::thread t(myprint, ref, buf, std::string(buf)); // 创建一个线程，执行 myprint 函数，并传递参数
    // 而如果我们在传递参数的时候，显式地将字符串字面量转换为std::string对象，
    // 那么就不会存在隐式类型转换的时机问题了，因为在创建线程时，std::string对象已经被创建好了，并且这个对象的生命周期至少和线程函数一样长，所以即使主线程中的字符串变量被销毁了，线程函数中的std::string对象仍然是有效的，可以继续访问它的值。
    std::cout << "i: " << i << ", buf: " << buf << ", str: " << str << std::endl;
}

void testThread2()
{
    int i = 42;
    char buf[20] = "Hello, World!";
    int &ref = i;
    // 打印 ref 的地址和 buf 的地址来验证它们是否指向同一个内存位置。由于 ref 是一个引用，指向主线程中的一个整数变量，因此它的地址应该与主线程中的 i 的地址相同。buf 是一个字符数组，它的地址应该是数组的起始地址。
    std::cout << "ref 的地址: " << &ref << ", 主线程中 i 的地址: " << &i << std::endl;
    std::cout << "buf 的地址: " << static_cast<void*>(buf) << std::endl;
    std::thread t(myprint, ref, buf, buf); // 创建一个线程，执行 myprint 函数，并传递参数
    if (t.joinable()) {
        // t.join(); // 等待线程执行完毕
        t.detach(); // 分离线程，允许它独立运行
    }
    std::cout << "主线程继续执行..." << std::endl;
}

class Task2
{
public:
    int value;
    Task2(int val) : value(val)
    {
        // 打印构建当前对象的线程ID，来验证是哪个线程在创建这个对象的。
        std::cout << "Task2 对象创建线程ID: " << std::this_thread::get_id() << std::endl;
        std::cout << "Task2 的构造函数被调用，value: " << value << std::endl;
    }
    Task2(const Task2& other) : value(other.value) {
        std::cout << "Task2 的拷贝构造函数被调用，线程ID：" << std::this_thread::get_id() << std::endl;
    }
    ~Task2() {
        std::cout << "Task2 的析构函数被调用，线程ID：" << std::this_thread::get_id() << ", value: " << value << std::endl;
    }

    void threadFunc(int val) const
    {
        std::cout << "Task2 对象调用 threadFunc: value: " << val << std::endl;
    }
};

void TestTask(const Task2& task)
{
    std::cout << "TestTask 执行线程ID: " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 模拟一些工作,等待1秒
    std::cout << "Task2 对象调用 TestTask: value: " << task.value << std::endl;
}

void testThread3()
{
    int value = 100;
    // 打印主线程id
    std::cout << "主线程ID: " << std::this_thread::get_id() << std::endl;
    // std::thread t(TestTask, value); // 创建一个线程，执行 TestTask 函数，并传递一个整数参数
    std::thread t(TestTask, Task2(value)); // 创建一个线程，执行 TestTask 函数，并传递一个 Task2 对象参数
    // 传递整形时，因为Task2存在类型转换构造函数，所以会先将整数参数转换为一个临时的 Task2 对象，然后这个临时对象会被传递给线程函数 TestTask。

    t.detach(); // 分离线程，允许它独立运行

    Task2 task(value); // 在主线程中创建一个 Task2 对象
    std::cout << "主线程继续执行..." << std::endl;
    std::thread t2(TestTask, std::ref(task)); // 创建一个线程，执行 TestTask 函数，并传递一个 Task2 对象的引用参数
    // std::ref明确告知编译器在传递参数时使用引用传递，而不是值传递。这样，线程函数 TestTask 接收的就是主线程中创建的 Task2 对象的引用，而不是一个副本。
    // 安全按值、危险std::ref、危险char*、安全join
    t2.detach(); // 分离线程，允许它独立运行

}

// 线程函数接收智能指针参数
void ThreadFuncWithUniquePtr(std::unique_ptr<Task2> taskPtr)
{
    std::cout << "ThreadFuncWithUniquePtr 执行线程ID: " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 模拟一些工作,等待1秒
    std::cout << "Task2 对象调用 ThreadFuncWithUniquePtr: value: " << taskPtr->value << std::endl;
}

void ThreadFuncWithSharedPtr(std::shared_ptr<Task2> taskPtr)
{
    std::cout << "ThreadFuncWithSharedPtr 执行线程ID: " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 模拟一些工作,等待1秒
    std::cout << "Task2 对象调用 ThreadFuncWithSharedPtr: value: " << taskPtr->value << std::endl;
}

void testThreadWithSharedPtr()
{
    // unique_ptr 是独占所有权的智能指针，不能被复制，只能被移动。
    // 因此，在创建线程时，我们需要使用 std::move 将 unique_ptr 转移到线程函数中，
    // 这样线程函数就获得了对 Task2 对象的所有权。
    std::unique_ptr<Task2> uniqueTaskPtr = std::make_unique<Task2>(200); // 创建一个 Task2 对象，并使用 unique_ptr 管理它
    std::thread t1(ThreadFuncWithUniquePtr, std::move(uniqueTaskPtr)); // 创建一个线程，执行 ThreadFuncWithUniquePtr 函数，并传递 unique_ptr 参数
    t1.detach(); // 分离线程，允许它独立运行

    std::shared_ptr<Task2> taskPtr = std::make_shared<Task2>(200); // 创建一个 Task2 对象，并使用智能指针管理它
    std::thread t(ThreadFuncWithSharedPtr, taskPtr); // 创建一个线程，执行 ThreadFuncWithSharedPtr 函数，并传递智能指针参数
    t.detach(); // 分离线程，允许它独立运行

    // 成员函数作为线程执行函数
    Task2 task(300); // 创建一个 Task2 对象
    std::thread taskThread(&Task2::threadFunc, std::ref(task), 300); // 创建一个线程，执行 Task2 的成员函数 threadFunc，并传递一个 Task2 对象和一个整数参数
    taskThread.detach(); // 分离线程，允许它独立运行(传递真引用的时候，detach是非常危险的)
}

// 消费线程和生产线程共享数据的示例
class Counter
{
private:
    std::vector<int> sharedData; // 共享数据容器
    std::mutex mtx; // 互斥锁，用于保护共享数据的访问
    std::mutex mtx2; // 另一个互斥锁，用于保护共享数据的访问

public:
    void Producer()
    {
        for (int i = 0; i < 10000; ++i) {
            // std::lock(mtx, mtx2); // 加锁，保护共享数据的访问
            // // std::adopt_lock 告诉 lock_guard 不要在构造时自动加锁，因为我们已经使用 std::lock 同时锁定了多个互斥锁。
            // std::lock_guard<std::mutex> lock(mtx, std::adopt_lock); // 使用 lock_guard 自动管理锁的生命周期，确保在作用域结束时自动释放锁
            // std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock); // 使用另一个 lock_guard 来保护对共享数据的访问，确保在输出时不会与消费者线程发生竞争条件
            // sharedData.push_back(i); // 向共享数据中添加一个整数
            // std::cout << "ID: " << std::this_thread::get_id() << " 生产者线程生产了数据: " << i << std::endl;
            // // mtx.unlock(); // 解锁，允许其他线程访问共享数据
            // // mtx2.unlock(); // 解锁，允许其他线程访问共享数据

            std::unique_lock<std::mutex> lock(mtx, std::defer_lock); // 创建一个 unique_lock 对象，但不立即加锁
            std::unique_lock<std::mutex> lock2(mtx2, std::defer_lock); // 创建另一个 unique_lock 对象，但不立即加锁
            // std::defer_lock 的使用前提是我们需要在后续的代码中根据某些条件来决定是否加锁，以及加锁的顺序。
            // 而且在std:::unique_lock之前不能对同一个互斥锁事先进行加锁，否则会导致死锁。
            // lock.lock(); // 加锁，保护共享数据的访问
            // lock2.lock(); // 加锁，保护共享数据的访问
            if (lock2.try_lock()) { // 尝试加锁，如果成功获得锁
                if (lock.try_lock()) { // 尝试加锁，如果成功获得锁
                    sharedData.push_back(i); // 向共享数据中添加一个整数
                    std::cout << "ID: " << std::this_thread::get_id() << " 生产者线程生产了数据: " << i << std::endl;
                }
                else {
                    std::cout << "ID: " << std::this_thread::get_id() << " 生产者线程无法获得 mtx 锁，跳过本次生产..." << std::endl;
                }
            }
            else {
                std::cout << "ID: " << std::this_thread::get_id() << " 生产者线程无法获得 mtx2 锁，跳过本次生产..." << std::endl;
            }

        }
    }

    void Consumer()
    {
        for (int i = 0; i < 10000; ++i) {
            std::unique_lock<std::mutex> lock(mtx, std::try_to_lock); // 尝试加锁，保护共享数据的访问
            if (lock.owns_lock()) { // 如果成功获得锁
                std::unique_lock<std::mutex> lock2(mtx2, std::try_to_lock); // 尝试加锁，保护共享数据的访问
                if (lock2.owns_lock()) { // 如果成功获得锁
                    if (!sharedData.empty()) {
                        int data = sharedData.back(); // 从共享数据中获取一个整数
                        sharedData.pop_back(); // 从共享数据中移除这个整数
                        std::cout << "ID: " << std::this_thread::get_id() << " 消费者线程消费了数据: " << data << std::endl;
                    } else {
                        std::cout << "ID: " << std::this_thread::get_id() << " 消费者线程等待数据..." << std::endl;
                    }
                } else {
                    std::cout << "ID: " << std::this_thread::get_id() << " 消费者线程无法获得 mtx2 锁，跳过本次消费..." << std::endl;
                }
            } else {
                std::cout << "ID: " << std::this_thread::get_id() << " 消费者线程无法获得 mtx 锁，跳过本次消费..." << std::endl;
            }
            // // std::lock(mtx2, mtx); // 加锁，保护共享数据的访问
            // // mtx.lock(); // 加锁，保护共享数据的访问
            // // std::lock_guard<std::mutex> lock(mtx); // 使用 lock_guard 自动管理锁的生命周期，确保在作用域结束时自动释放锁
            // if (!sharedData.empty()) {
            //     int data = sharedData.back(); // 从共享数据中获取一个整数
            //     sharedData.pop_back(); // 从共享数据中移除这个整数
            //     // std::lock_guard<std::mutex> lock2(mtx2); // 使用另一个 lock_guard 来保护对共享数据的访问，确保在输出时不会与生产者线程发生竞争条件
            //     std::cout << "ID: " << std::this_thread::get_id() << " 消费者线程消费了数据: " << data << std::endl;
            // } else {
            //     std::cout << "ID: " << std::this_thread::get_id() << " 消费者线程等待数据..." << std::endl;
            // }
            // // mtx2.unlock(); // 解锁，允许其他线程访问共享数据
            // // mtx.unlock(); // 解锁，允许其他线程访问共享数据
        }
    }
};


// 创建多个线程，数据共享问题分析
void testMultipleThreads()
{   
    Counter sharedData; // 创建一个 Counter 对象，作为生产者和消费者线程共享的数据容器
    std::vector<std::thread> threads; // 创建一个线程容器，用于存储多个线程对象
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(&Counter::Producer, &sharedData); // 创建一个生产者线程，执行 Counter 的 Producer 函数，并将 sharedData 作为参数传递
        threads.emplace_back(&Counter::Consumer, &sharedData); // 创建一个消费者线程，执行 Counter 的 Consumer 函数，并将 sharedData 作为参数传递
    }
    for (auto& t : threads) {
        t.join(); // 等待所有线程执行完毕
    }
}


#endif // __MY_THREAD_H__