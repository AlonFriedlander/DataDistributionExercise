#pragma once
#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>

// Declaration of ThreadPool class
class ThreadPool {
public:
    using Task = std::function<void()>;

    ThreadPool(size_t numThreads);
    ~ThreadPool();
    void enqueue(Task);

private:
    std::vector<std::thread> workers;
    std::queue<Task> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    //bool stop;
    std::atomic<bool> stop;
};
