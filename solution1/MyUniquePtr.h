#ifndef __MY_UNIQUE_PTR_H__
#define __MY_UNIQUE_PTR_H__

template<typename T>
class MyUniquePtr
{
private:
	T* ptr; // 独占的原始指针

public:
	// 构造函数
	explicit MyUniquePtr(T* p = nullptr) : ptr(p) {}

	// 禁止拷贝构造
	MyUniquePtr(const MyUniquePtr<T>& other) = delete;

	// 禁止拷贝赋值
	MyUniquePtr& operator=(const MyUniquePtr<T>& other) = delete;

	// 移动构造
	MyUniquePtr(MyUniquePtr<T>&& other) noexcept : ptr(other.ptr)
	{
		other.ptr = nullptr;
	}

	// 移动赋值
	MyUniquePtr& operator=(MyUniquePtr<T>&& other) noexcept
	{
		if (this != &other)
		{
			delete ptr;
			ptr = other.ptr;
			other.ptr = nullptr;
		}
		return *this;
	}

	// 析构函数
	~MyUniquePtr()
	{
		delete ptr;
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

	// 获取原始指针
	T* get() const
	{
		return ptr;
	}

	// 释放所有权，返回原始指针
	T* release()
	{
		T* old = ptr;
		ptr = nullptr;
		return old;
	}

	// 重置管理对象
	void reset(T* p = nullptr)
	{
		if (ptr != p)
		{
			delete ptr;
			ptr = p;
		}
	}

	// 交换两个 unique ptr
	void swap(MyUniquePtr<T>& other) noexcept
	{
		T* temp = ptr;
		ptr = other.ptr;
		other.ptr = temp;
	}

	// 判空
	explicit operator bool() const
	{
		return ptr != nullptr;
	}
};



#endif // __MY_UNIQUE_PTR_H__
