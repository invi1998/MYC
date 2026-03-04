#ifndef __MY_CONDITION_VARIABLE_H__
#define __MY_CONDITION_VARIABLE_H__

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

class ProducerConsumer
{
private:
    std::queue<int> dataQueue; // 用于存储生产的数据（queue 双端队列）
    std::mutex mtx; // 互斥锁，用于保护共享数据的访问
    std::condition_variable cv; // 条件变量，用于线程间的通知和等待
    const int maxCount; // 最大生产数量
    int produced_count; // 生产的数据计数器
    int consumed_count; // 消费的数据计数器
    bool production_done;                   // 生产是否已完成
    
    std::atomic<int> next_id{0}; // 用于生成唯一的生产数据ID

public:
    ProducerConsumer(int total_count)
        : maxCount(total_count), produced_count(0), consumed_count(0), production_done(false) {}

    // 工厂提供两个函数，分别用于创造和消费数据

    // 生产者：尝试生产一个数据，成功返回true，失败返回false（例如达到生产上限时）
    bool produce()
    {
        // 首先尝试获取一个唯一的数据ID，原子操作
        int data = next_id.fetch_add(1, std::memory_order_relaxed);
        if (data >= maxCount) {
            // 已经生产了足够的数据，标记生产完成
            {
                std::lock_guard<std::mutex> lock(mtx);
                production_done = true;
            }
            cv.notify_all(); // 通知所有等待的消费者线程
            return false; // 生产失败，达到上限
        }

        {
            std::lock_guard<std::mutex> lock(mtx); // 加锁，保护共享数据的访问
            dataQueue.push(data); // 将生产的数据放入队列
            ++produced_count; // 增加生产计数器
            std::cout << "ID: " << std::this_thread::get_id() << " 生产者线程生产了数据: " << data << std::endl;
        } 

        cv.notify_one(); // 通知一个等待的消费者线程有新数据可用
        return true; // 生产成功
    }

    // 消费者线程调用：尝试消费一个数据，成功返回true，失败返回false（例如生产完成且没有数据可消费时）
    bool consume(int& outdata)
    {
        std::unique_lock<std::mutex> lock(mtx); // 加锁，保护共享数据的访问
        // 等待条件：要么有数据可消费，要么生产已经完成
        cv.wait(lock, [this]() -> bool {
            return !dataQueue.empty() || (production_done && dataQueue.empty());
        });

        if (!dataQueue.empty()) {
            outdata = dataQueue.front(); // 获取队列中的数据
            dataQueue.pop(); // 从队列中移除数据
            ++consumed_count; // 增加消费计数器
            std::cout << "ID: " << std::this_thread::get_id() << " 消费者线程消费了数据: " << outdata << std::endl;
            return true; // 消费成功
        }
        return false; // 其他情况，消费失败

    }


};


void testProducerConsumer()
{
    const int total_count = 100; // 总共要生产的数据数量
    ProducerConsumer pc(total_count);

    std::vector<std::thread> producerThreads;
    std::vector<std::thread> consumerThreads;
    for (int i = 0; i < 5; ++i) {
        producerThreads.emplace_back([&pc]() {
            while (pc.produce()) {
                // 生产成功，继续生产
            }
        });
        consumerThreads.emplace_back([&pc]() {
            int data;
            while (pc.consume(data)) {
                // 消费成功，继续消费
            }
        });
    }

    for (auto& t : producerThreads) {
        t.join(); // 等待所有生产者线程完成
    }
    for (auto& t : consumerThreads) {
        t.join(); // 等待所有消费者线程完成
    }

    std::cout << "生产和消费已完成。" << std::endl;
}

#endif // __MY_CONDITION_VARIABLE_H__
