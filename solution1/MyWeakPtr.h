#ifndef __MY_WEAK_PTR_H__
#define __MY_WEAK_PTR_H__

#include "MyShared_ptr.h"

template<typename T>
class MyWeakPtr
{
private:
	T* ptr; // 仅用于 lock() 成功后构造共享指针
	typename MySharedPtr<T>::ControlBlock* control; // 指向共享控制块

	void release()
	{
		if (!control)
		{
			ptr = nullptr;
			return;
		}

		// 弱计数减1，若减到0且强计数也为0，才释放控制块
		if (control->weakCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			if (control->strongCount.load(std::memory_order_acquire) == 0)
			{
				delete control;
			}
		}

		ptr = nullptr;
		control = nullptr;
	}

public:
	// 默认构造：空弱引用
	MyWeakPtr() : ptr(nullptr), control(nullptr) {}

	// 从共享指针构造弱引用
	MyWeakPtr(const MySharedPtr<T>& shared) : ptr(shared.ptr), control(shared.control)
	{
		if (control)
		{
			control->weakCount.fetch_add(1, std::memory_order_relaxed);
		}
	}

	// 拷贝构造
	MyWeakPtr(const MyWeakPtr<T>& other) : ptr(other.ptr), control(other.control)
	{
		if (control)
		{
			control->weakCount.fetch_add(1, std::memory_order_relaxed);
		}
	}

	// 移动构造
	MyWeakPtr(MyWeakPtr<T>&& other) noexcept : ptr(other.ptr), control(other.control)
	{
		other.ptr = nullptr;
		other.control = nullptr;
	}

	// 析构
	~MyWeakPtr()
	{
		release();
	}

	// 拷贝赋值
	MyWeakPtr& operator=(const MyWeakPtr<T>& other)
	{
		if (this != &other)
		{
			release();
			ptr = other.ptr;
			control = other.control;
			if (control)
			{
				control->weakCount.fetch_add(1, std::memory_order_relaxed);
			}
		}
		return *this;
	}

	// 移动赋值
	MyWeakPtr& operator=(MyWeakPtr<T>&& other) noexcept
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

	// 从共享指针赋值
	MyWeakPtr& operator=(const MySharedPtr<T>& shared)
	{
		release();
		ptr = shared.ptr;
		control = shared.control;
		if (control)
		{
			control->weakCount.fetch_add(1, std::memory_order_relaxed);
		}
		return *this;
	}

	// 强引用是否已失效
	bool expired() const
	{
		return !control || control->strongCount.load(std::memory_order_acquire) == 0;
	}

	// 当前强引用计数
	size_t use_count() const
	{
		return control ? control->strongCount.load(std::memory_order_relaxed) : 0;
	}

	// 尝试提升为共享指针
	MySharedPtr<T> lock() const
	{
		if (!control)
		{
			return MySharedPtr<T>();
		}

		size_t current = control->strongCount.load(std::memory_order_acquire);
		while (current != 0)
		{
			if (control->strongCount.compare_exchange_weak(
					current,
					current + 1,
					std::memory_order_acq_rel,
					std::memory_order_acquire))
			{
				return MySharedPtr<T>(ptr, control, false);
			}
		}

		return MySharedPtr<T>();
	}
};

#endif // __MY_WEAK_PTR_H__
