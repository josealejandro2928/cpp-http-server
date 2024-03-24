//
// Created by pepe on 3/14/24.
//

#ifndef HTTP_SERVER_TASKWORKERSERVICE_H
#define HTTP_SERVER_TASKWORKERSERVICE_H

#include <iostream>
#include "http_server/concurrency/Executor.h"

namespace hs = HttpServer;

class TaskWorkerService {
private:
    hs::ThreadPoolExecutor *pool;
    static TaskWorkerService *instance;
    static std::mutex mx;

    TaskWorkerService() : pool{new hs::ThreadPoolExecutor()} {};

    explicit TaskWorkerService(int);

public:
    static TaskWorkerService &getInstance(int);

    static TaskWorkerService &getInstance();

    TaskWorkerService(TaskWorkerService &) = delete;

    TaskWorkerService(TaskWorkerService &&) = delete;

    TaskWorkerService &operator=(TaskWorkerService &) = delete;

    TaskWorkerService &operator=(TaskWorkerService &&) = delete;

    [[nodiscard]] hs::ThreadPoolExecutor &getPool();

    void runPeriodicTask(const std::function<void()> &task, int period, int delay = 0);

    ~TaskWorkerService() {
        std::cout << "Destroying TaskWorkerService" << std::endl;
        if (pool != nullptr) {
            pool->shutdown();
            delete pool;
        }
    }


};


#endif //HTTP_SERVER_TASKWORKERSERVICE_H
