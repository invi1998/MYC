#ifndef __MY_SHARED_PTR_H__
#define __MY_SHARED_PTR_H__

// 需要实现接口：
// 1. 构造函数：接受一个原始指针，并初始化引用计数为 1。
// 2. 拷贝构造函数：增加引用计数。
// 3. 移动构造函数：转移所有权，不增加引用计数。
// 4. 析构函数：减少引用计数，如果引用计数为 0，则删除原始指针。
// 5. 重载 * 和 -> 运算符：提供对原始指针的访问。
// 6. get() 方法：返回原始指针。
// 7. use_count() 方法：返回当前的引用计数。


#include <atomic>
#include <cstddef>

template<typename T>
class MyWeakPtr;

template<typename T>
class MySharedPtr
{
private:
    struct ControlBlock
    {
        std::atomic<size_t> strongCount;
        std::atomic<size_t> weakCount;

        ControlBlock() : strongCount(1), weakCount(0) {}
    };

    T* ptr; // 原始指针
    ControlBlock* control; // 控制块：强引用计数 + 弱引用计数

    friend class MyWeakPtr<T>;

    // 供 MyWeakPtr::lock() 使用的内部构造函数
    MySharedPtr(T* p, ControlBlock* cb, bool addStrongRef) : ptr(p), control(cb)
    {
        if (addStrongRef && control)
        {
            control->strongCount.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void release()
    {
        if (!control)
        {
            ptr = nullptr;
            return;
        }

        if (control->strongCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            delete ptr; // 删除原始指针
            ptr = nullptr;

            if (control->weakCount.load(std::memory_order_acquire) == 0)
            {
                delete control;
            }
        }

        ptr = nullptr;
        control = nullptr;
    }

public:
    // 构造函数
    explicit MySharedPtr(T* p = nullptr) : ptr(p), control(p ? new ControlBlock() : nullptr)
    {
        // 构造函数里，如果传入的指针不为 nullptr，则初始化强引用计数为 1；如果传入 nullptr，则 control 也设置为 nullptr。
        // 同时，构造函数使用 explicit 关键字来防止隐式转换，这样可以避免一些潜在的错误，例如 MySharedPtr<int> p = new int(5); 这种写法会被禁止，必须显式调用构造函数 MySharedPtr<int> p(new int(5));。
        
        // 在 C++ 中，std::atomic 的构造函数不接收内存序参数。
        // 这是因为内存序（memory_order）是用于控制原子操作（如 load、store、fetch_add 等）在多线程环境中的同步和排序行为，
        // 而对象的构造过程通常发生在该对象被多个线程共享之前，因此不存在并发访问的风险，也就不需要指定内存序。
    }

    // 析构函数
    ~MySharedPtr()
    {
        // 强引用计数减1，并且如果引用计数为0，则删除原始指针
        release();
    }

    // 拷贝构造函数
    MySharedPtr(const MySharedPtr<T>& other) : ptr(other.ptr), control(other.control)
    {
        if (control)
        {
            // 如果引用计数不为 nullptr，则增加引用计数。这里使用 std::memory_order_relaxed 来增加引用计数，
            // 因为在拷贝构造函数中，对引用计数这个原子操作的顺序要求不高，使用 relaxed 可以提高性能。
            // 为什么说顺序要求不高？因为对于引用计数来说，增加计数的操作本身不需要与其他操作进行严格的同步，只要确保增加计数的操作是原子的即可。
            // 仅将计数值原子地加 1，不涉及资源释放或对其他线程的依赖，因此 relaxed 内存顺序是足够的。
            // 操作本身不会触发资源销毁，也不会对其他线程的访问照成直接影响
            control->strongCount.fetch_add(1, std::memory_order_relaxed);
        }
    }

    // 移动构造函数
    MySharedPtr(MySharedPtr&& other) noexcept : ptr(other.ptr), control(other.control)
    {
        // 移动构造函数将资源的所有权从other转移到了当前对象上，同时将other的ptr和control设置为nullptr，表示other不再拥有资源。
        other.ptr = nullptr;
        other.control = nullptr;
    }

    // 拷贝赋值运算符
    MySharedPtr& operator=(const MySharedPtr<T>& other)
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            control = other.control;
            if (control)
            {
                control->strongCount.fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    // 移动赋值运算符
    MySharedPtr& operator=(MySharedPtr<T>&& other) noexcept
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            control = other.control;
            other.ptr = nullptr;
            other.control = nullptr;
        }
        return *this;
    }

    // 重载 * 运算符
    T& operator*() const
    {
        return *ptr;
    }

    // 重载 -> 运算符
    T* operator->() const
    {
        return ptr;
    }

    // get() 方法
    T* get() const
    {
        return ptr;
    }

    // use_count() 方法
    size_t use_count() const
    {
        // 同样使用 std::memory_order_relaxed 来读取引用计数，因为这个操作只是为了获取当前的引用计数值，并不涉及资源的释放或者其他线程的同步。
        return control ? control->strongCount.load(std::memory_order_relaxed) : 0;
    }

};



#endif // __MY_SHARED_PTR_H__