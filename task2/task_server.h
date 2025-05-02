#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <atomic>
#include <functional>

template<typename T>
class TaskServer {
public:
    using Task = std::function<T()>;

    TaskServer() : running(false), nextId(0) {}

    void start() {
        running = true;
        worker = std::thread([this]() {
            while (running || !tasks.empty()) {
                TaskEntry task;
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    cond.wait(lock, [&]() { return !tasks.empty() || !running; });
                    if (tasks.empty()) continue;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                T result = task.func();
                std::lock_guard<std::mutex> lock(mutex);
                results[task.id] = result;
            }
        });
    }

    void stop() {
        running = false;
        cond.notify_all();
        if (worker.joinable()) worker.join();
    }

    size_t add_task(Task task) {
        std::lock_guard<std::mutex> lock(mutex);
        size_t id = nextId++;
        tasks.push({id, task});
        results.push_back(T()); // Зарезервировать место
        cond.notify_one();
        return id;
    }

    T request_result(size_t id) {
        while (true) {
            std::lock_guard<std::mutex> lock(mutex);
            if (id < results.size()) return results[id];
        }
    }

private:
    struct TaskEntry {
        size_t id;
        Task func;
    };

    std::thread worker;
    std::atomic<bool> running;
    std::mutex mutex;
    std::condition_variable cond;

    std::queue<TaskEntry> tasks;
    std::vector<T> results;
    size_t nextId;
};
