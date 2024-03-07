//
// Created by pepe on 3/6/24.
//

#include "TaskService.h"
#include "http_server/utils/utils.h"
#include "data/DataStorage.h"
namespace hs = HttpServer;

Task TaskService::createTask(CreateTaskRequest &taskRequest, User &loggedUser) {
    auto &dataStorage = DataStorage::getInstance("");
    Task task = Task(taskRequest.title, taskRequest.description, taskRequest.status, loggedUser);
    dataStorage.addTask(task);
    return task;
}

std::vector<Task> TaskService::getTasks(User &loggedUser, RequestFilterTask &filter) {
    auto &dataStorage = DataStorage::getInstance("");
    auto tasks = dataStorage.getTasks();
    tasks = hs::filterFn(tasks, [&loggedUser, & filter](const Task &task) {
        auto res = task.userId == loggedUser.id;
        if(!filter.title.empty()){
            res&=(task.title.find(filter.title)!=std::string::npos);
        }
        if(!filter.status.empty()){
            res&=(std::find(filter.status.begin(), filter.status.end(), task.status) != filter.status.end());
        }
        return res;
    });
    std::sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
        return a.id < b.id;
    });
    return tasks;
}


