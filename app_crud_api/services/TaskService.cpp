//
// Created by pepe on 3/6/24.
//

#include "TaskService.h"
#include "http_server/utils/utils.h"

std::vector<Task> TaskService::tasks = {};
std::mutex TaskService::tasksMutex = {};

Task TaskService::createTask(CreateTaskRequest &taskRequest, User &loggedUser) {
    std::lock_guard<std::mutex> lock(tasksMutex);
    Task task = Task(taskRequest.title, taskRequest.description, taskRequest.status, loggedUser);
    tasks.push_back(task);
    return task;
}

std::vector<Task> TaskService::getTasks(User &loggedUser) {
    auto data = HttpServer::filterFn(TaskService::tasks, [&loggedUser](const Task &task) {
        return task.userId == loggedUser.id;
    });
    std::sort(data.begin(), data.end(), [](const Task &a, const Task &b) {
        return a.id < b.id;
    });
    return data;
}


