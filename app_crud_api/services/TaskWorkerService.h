//
// Created by pepe on 3/14/24.
//

#ifndef HTTP_SERVER_TASKWORKERSERVICE_H
#define HTTP_SERVER_TASKWORKERSERVICE_H

#include "http_server/concurrency/ThreadPool.h"

namespace hs = HttpServer;

class TaskWorkerService {
private:
    hs::ThreadPool pool{8};

    TaskWorkerService() = default;

public:
    static TaskWorkerService &getInstance();

    TaskWorkerService(TaskWorkerService &) = delete;

    TaskWorkerService(TaskWorkerService &&) = delete;

    TaskWorkerService &operator=(TaskWorkerService &) = delete;

    TaskWorkerService &operator=(TaskWorkerService &&) = delete;

    [[nodiscard]] hs::ThreadPool &getPool();


};


#endif //HTTP_SERVER_TASKWORKERSERVICE_H
