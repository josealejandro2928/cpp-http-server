//
// Created by pepe on 3/14/24.
//

#include "TaskWorkerService.h"

TaskWorkerService *TaskWorkerService::instance = nullptr;
std::mutex TaskWorkerService::mx{};

TaskWorkerService &TaskWorkerService::getInstance() {
    std::lock_guard<std::mutex> lock(mx);
    if (instance == nullptr) {
        instance = new TaskWorkerService();
    }
    return *instance;
}

hs::ThreadPool &TaskWorkerService::getPool() {
    return *pool;
}

TaskWorkerService &TaskWorkerService::getInstance(int size) {
    std::lock_guard<std::mutex> lock(mx);
    if (instance == nullptr) {
        instance = new TaskWorkerService(size);
    }
    return *instance;

}

TaskWorkerService::TaskWorkerService(int size) {
    pool = new hs::ThreadPool(size);
}

void TaskWorkerService::runPeriodicTask(const std::function<void()> &task, int period, int delay) {
    pool->submit([this, task, period, delay]() {
        std::this_thread::sleep_for(std::chrono::seconds(delay));
        while (!this->pool->taskShouldTerminate()) {
            task();
            std::this_thread::sleep_for(std::chrono::seconds(period));
        }
        return 0;
    });
}
