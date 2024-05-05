#include "ThreadPool.h"

ThreadPool::ThreadPool() : stop(false) {}

ThreadPool::~ThreadPool() {
    stop = true;
    condition.notify_all();
    for (std::thread& worker : workers) {
        worker.join();
    }
}

void ThreadPool::startThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                Task task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] {return stop || !tasks.empty(); });

                    if (stop && tasks.empty()) return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
            });
    }
}

void ThreadPool::enqueue(Task task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(move(task));
    }
    condition.notify_one();
}