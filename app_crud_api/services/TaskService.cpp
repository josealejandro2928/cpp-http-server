//
// Created by pepe on 3/6/24.
//

#include "TaskService.h"
#include "http_server/utils/utils.h"
#include "data/DataStorage.h"
#include "http_server/exceptions/Exceptions.h"

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
        if (!filter.title.empty()) {
            res &= (task.title.find(filter.title) != std::string::npos);
        }
        if (!filter.status.empty()) {
            res &= (std::find(filter.status.begin(), filter.status.end(), task.status) != filter.status.end());
        }
        return res;
    });
    std::sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
        return a.id < b.id;
    });
    return tasks;
}

std::shared_ptr<Task> TaskService::getTask(int id) {
    auto &dataStorage = DataStorage::getInstance("");
    auto tasks = dataStorage.getTasks();
    auto task = hs::findFn(tasks, [id](const Task &task) {
        return task.id == id;
    });
    if (task == nullptr) throw hs::NotFoundException("Task not found with id:" + std::to_string(id));
    return std::make_shared<Task>(*task);
}

Task TaskService::updateTask(int id, CreateTaskRequest &requestUpdateBody) {
    auto &dataStorage = DataStorage::getInstance("");
    int index = -1;
    auto tasks = dataStorage.getTasks();
    for (int i = 0; i < tasks.size(); i++) {
        if (dataStorage.getTasks()[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) throw hs::NotFoundException("Task not found with id:" + std::to_string(id));
    auto task = tasks[index];
    task.title = requestUpdateBody.title;
    task.description = requestUpdateBody.description;
    task.status = requestUpdateBody.status;
    dataStorage.addTask(task, index);
    return task;
}

void TaskService::deleteTask(int id) {
    auto &dataStorage = DataStorage::getInstance("");
    int index = -1;
    auto tasks = dataStorage.getTasks();
    for (int i = 0; i < tasks.size(); i++) {
        if (dataStorage.getTasks()[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) throw hs::NotFoundException("Task not found with id:" + std::to_string(id));
    dataStorage.deleteTask(index);
}


