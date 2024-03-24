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

hs::ThreadPoolExecutor &TaskWorkerService::getPool() {
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
    pool = (new hs::ThreadPoolExecutor(size))->setName("TaskWorkerExecutorService");

}
