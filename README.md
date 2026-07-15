# C++ Threading Practice: Producer-Consumer Examples

Educational C++ examples demonstrating the **Producer-Consumer** pattern with `std::condition_variable`, `std::mutex`, and `std::thread`.

Each example is a separate executable so you can build and run them independently.

## Project layout

```
producer-consumer/
├── CMakeLists.txt
└── examples/
    ├── condition_variable/main.cpp   # single producer, single consumer
    └── bounded_buffer/main.cpp       # bounded queue, multiple producers/consumers
```

## Examples

### 1. `condition_variable`

A minimal producer-consumer setup:

- One **producer** pushes numbers 1–10 into a shared queue
- One **consumer** waits for data and processes it safely
- A single `std::condition_variable` avoids busy-waiting

**Key components:** `std::queue`, `std::mutex`, `std::condition_variable`, `std::unique_lock`

### 2. `bounded_buffer`

A fixed-capacity buffer with multiple threads:

- `BoundedBuffer<T>` template class with a max queue size
- Two **producers** and two **consumers**
- Producers block when the buffer is full (`notFull`)
- Consumers block when the buffer is empty (`notEmpty`)
- Items are encoded as `producer_id * 100 + index` so output is easy to trace

**Key components:** two condition variables (`notFull`, `notEmpty`), predicate-based `wait()`, synchronized logging via a dedicated `log_mutex`

## Build & Run

### Prerequisites

- C++17 or later
- CMake
- A compiler with threading support (g++, clang++, MSVC)

### Using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
./condition_variable   # example 1
./bounded_buffer       # example 2
```

## Notes

### Why must the mutex be locked before `cv.wait()`?

In the `condition_variable` example, the consumer uses:

```cpp
std::unique_lock<std::mutex> lock(buffer_mutex);
cv.wait(lock, [] { return !bufferQueue.empty(); });
```

What happens during `cv.wait()`:

1. The lock is already held when you enter `wait()`
2. `cv.wait()` atomically releases the lock and puts the thread to sleep
3. When another thread calls `cv.notify_one()`, this thread wakes up
4. It re-acquires the lock before exiting `wait()`
5. It then checks the predicate (`!bufferQueue.empty()`)

This cycle prevents a race between checking the queue and going to sleep.

### Why is console output synchronized in `bounded_buffer`?

`std::cout` is not thread-safe. Without a logging mutex, lines from different threads can interleave and produce garbled output. That is a display issue, not a buffer bug — the `BoundedBuffer` synchronization remains correct either way.
