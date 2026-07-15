// Practice: bounded buffer with multiple producers and consumers.
//
// Goals:
// - Fixed-capacity queue (producer blocks when full)
// - Multiple producer threads, multiple consumer threads
// - Thread-safe via mutex + condition_variable(s)

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

// Showing bounded buffer implementation with condition variable
// WITHOUT CONDITION VARIABLE
// 1. Busy-Waiting (Wasted CPU Cycles)
// PRODUCER                           TIME
//  put(item)
//  acquire lock (FULL)    ─┐        0ms
//  release lock           │
//  sleep 10ms             ├────────── 10ms
//  acquire lock (FULL)    │
//  release lock           ├────────── 20ms
//  sleep 10ms             │
//  acquire lock (FULL)    │
//  release lock           ├────────── 30ms
//  sleep 10ms             │
//  acquire lock (OK!)     ├────────── 35ms
//  push + return          ─┘

const size_t BUFFER_CAPACITY = 10;

std::mutex log_mutex;

void log(const std::string& message)
{
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cout << message << std::endl;
}

template<typename T>
class BoundedBuffer {
    public:
     BoundedBuffer(size_t capacity) : capacity(capacity) {}

     void push(const T& item) {
       std::unique_lock<std::mutex> lock(mutex);
       notFull.wait(lock, [this]() { return queue.size() < capacity; });
       queue.push(item);
       // Notify waiting consumers (we know that buffer already contains something)
       notEmpty.notify_one();
    }

    T take() {
        std::unique_lock<std::mutex> lock(mutex);
        notEmpty.wait(lock, [this]() { return !queue.empty(); });
        T item = queue.front();
        queue.pop();
        // Notify waiting producers (we know that buffer has space)
        notFull.notify_one();
        return item;
    }

    private:
        std::queue<T> queue;
       size_t capacity;
       mutable std::mutex mutex;
       std::condition_variable notFull;
       std::condition_variable notEmpty;
};

void producer(BoundedBuffer<int>& buffer, int id, int items) {
    log("Producer " + std::to_string(id) + " started");
    for (int i = 0; i < items; i++) {
        buffer.push(i);
        log("Producer " + std::to_string(id) + " pushed " + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void consumer(BoundedBuffer<int>& buffer, int id, int items) {
    log("Consumer " + std::to_string(id) + " started");
    for (int i = 0; i < items; i++) {
        const int item = buffer.take();
        log("Consumer " + std::to_string(id) + " took " + std::to_string(item));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

int main()
{
    std::vector<std::thread> threads;
    // Declare buffer with capacity of 10
    BoundedBuffer<int> buffer(BUFFER_CAPACITY);
    // adding threads to the vector
    threads.push_back(std::thread(producer, std::ref(buffer), 1, 20));
    threads.push_back(std::thread(consumer, std::ref(buffer), 1, 20));
    threads.push_back(std::thread(producer, std::ref(buffer), 2, 20));
    threads.push_back(std::thread(consumer, std::ref(buffer), 2, 20));

    // joining threads
    for (auto& thread : threads) {
        thread.join();
    }

    log("Done!");
    return 0;
}
