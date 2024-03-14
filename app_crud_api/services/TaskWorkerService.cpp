//
// Created by pepe on 3/14/24.
//

#include "TaskWorkerService.h"

TaskWorkerService &TaskWorkerService::getInstance() {
    static TaskWorkerService instance = TaskWorkerService();
    return instance;
}

hs::ThreadPool &TaskWorkerService::getPool(){
    return pool;
}
