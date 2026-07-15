# C++ Threading Practice: Producer-Consumer with `std::condition_variable`

A simple and educational example demonstrating the use of `std::condition_variable`, `std::mutex`, and `std::thread` in C++ to implement a **Producer-Consumer** pattern.

This project helps understand:
- Thread synchronization
- Safe data sharing between threads
- How to avoid busy-waiting using condition variables
- Proper use of `std::unique_lock` and predicate-based `wait()`

## 🔧 Code Overview

The program consists of:
- One **producer thread** that generates numbers (1 to 10) and adds them to a shared queue.
- One **consumer thread** that waits for data and processes it safely.
- A `std::condition_variable` to notify the consumer when new data is available.

### 📌 Key Components
- `std::queue<int>` – shared buffer (protected by mutex)
- `std::mutex` – ensures thread-safe access
- `std::condition_variable` – coordinates threads efficiently (no polling)
- `std::unique_lock<std::mutex>` – RAII lock for safe locking/unlocking

## 🚀 Build & Run

### Prerequisites
- C++17 or later
- CMake (optional, for building)
- Compiler with C++ threading support (g++, clang++, MSVC)

### Using CMake
```bash
mkdir build
cd build
cmake ..
cmake --build .
./condition_variable   # single producer / single consumer
./bounded_buffer       # practice exercise (not implemented yet)

## Notes:

### 🔍 Why Must the Mutex Be Locked Before `cv.wait()`?

In the consumer thread, you see this pattern:

```cpp
std::unique_lock<std::mutex> lock(buffer_mutex);
cv.wait(lock, [] { return !bufferQueue.empty(); });
```

✅ What happens during cv.wait():

The lock is already held when you enter wait().

cv.wait() atomically:

- Releases the lock (so other threads can access the shared data)

- Puts the thread to sleep (waiting for a notify)

When another thread calls cv.notify_one():

- This thread wakes up

- Re-acquires the lock before exiting wait()

- Then checks the predicate (!bufferQueue.empty())

🔁 This cycle ensures no race condition between checking the queue and waiting.
