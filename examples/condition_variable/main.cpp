#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

// Shared queue
std::queue<int> bufferQueue;
// Mutex that protects access to bufferQueue. Only one thread can lock it at a time.
std::mutex buffer_mutex;
// Allows the consumer to wait until the producer adds data.
std::condition_variable cv;
bool finished = false;

const int MAX_ITEMS = 10;

void producer() {
    for (int i = 1; i <= MAX_ITEMS; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Locks the mutex so no other thread can access bufferQueue while we modify it.
        // std::unique_lock is used here because it’s required by std::condition_variable::wait().
        std::unique_lock<std::mutex> lock(buffer_mutex);
        bufferQueue.push(i);
        std::cout << "Produced: " << i << std::endl;
        // Optional: Explicitly releases the lock before notifying.
        // It is good practice, because it ensures the consumer
        // doesn’t block trying to re-lock while notification happens.
        lock.unlock();
        // Wakes up one waiting thread (the consumer) that is blocked on cv.wait().
        cv.notify_one();
    }
    {
        std::unique_lock<std::mutex> lock(buffer_mutex);
        finished = true;
    }
    cv.notify_one();
}

void consumer() {
    while (true) {
        // Locks the mutex before checking or modifying the queue.
        std::unique_lock<std::mutex> lock(buffer_mutex);
        // Wait until queue is not empty or we're done
        cv.wait(lock, [] {
            return !bufferQueue.empty() || finished;
        });

        if (bufferQueue.empty() && finished) {
            break; // No more work
        }

        int value = bufferQueue.front();
        bufferQueue.pop();
        std::cout << "Consumed: " << value << std::endl;
        lock.unlock();
    }
}

int main()
{
    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    producer_thread.join();
    consumer_thread.join();

    std::cout << "Done!" << std::endl;
    return 0;
}
