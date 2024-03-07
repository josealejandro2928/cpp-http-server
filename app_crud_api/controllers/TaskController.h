//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_TASKCONTROLLER_H
#define HTTP_SERVER_TASKCONTROLLER_H

#include "http_server/exceptions/Exceptions.h"
#include "http_server/utils/utils.h"
#include "iostream"
#include <vector>
#include "ControllerBase.h"
#include "dto/RequestDtos.h"
#include "services/AuthService.h"
#include "middlewares/Middlewares.h"
#include "nlohmann/json.hpp"
#include "services/TaskService.h"

using json = nlohmann::json;

class TaskController : public ControllerBase {
public:
    static std::shared_ptr<Task> checkTaskOwnership(int taskId, int userId) {
        auto task = TaskService::getTask(taskId);
        if (!task) {
            throw HttpServer::NotFoundException("Task not found");
        }
        if (task->userId != userId) {
            throw HttpServer::UnauthorizedException("You are not the owner of this task");
        }
        return task;
    }

    static void createTask(HttpServer::Request &req) {
        auto createTaskRequest = req.getBodyObject<CreateTaskRequest>();
        createTaskRequest.validate();
        auto loggedInUser = any_cast<std::shared_ptr<User>>(req.getRequestAttribute("loggedInUser"));
        Task task = TaskService::createTask(createTaskRequest, *loggedInUser);
        json response;
        response["data"] = task.toDto();
        req.sendJson(req, 201, response);
    }

    static void findAllTask(HttpServer::Request &req) {
        auto loggedInUser = any_cast<std::shared_ptr<User>>(req.getRequestAttribute("loggedInUser"));
        RequestFilterTask filter;
        if (req.getQuery().count("title") > 0) {
            filter.title = req.getQuery()["title"];
        }
        if (req.getQuery().count("status") > 0) {
            auto parts = HttpServer::strSplit(req.getQuery()["status"], ',');
            filter.status = parts;
        }
        req.sendJson<vector<Task>>(req, 200, TaskService::getTasks(*loggedInUser, filter));
    }

    static void getTask(hs::Request &req) {
        string &taskId = req.getRequestParam("taskId");
        auto loggedInUser = any_cast<std::shared_ptr<User>>(req.getRequestAttribute("loggedInUser"));
        auto task = checkTaskOwnership(std::stoi(taskId), loggedInUser->id);
        json response;
        response["data"] = task->toDto();
        req.sendJson(req, 200, response);
    }

    static void updateTask(hs::Request &req) {
        string &taskId = req.getRequestParam("taskId");
        auto requestBody = req.getBodyObject<CreateTaskRequest>();
        requestBody.validate();
        auto loggedInUser = any_cast<std::shared_ptr<User>>(req.getRequestAttribute("loggedInUser"));
        auto task = checkTaskOwnership(std::stoi(taskId), loggedInUser->id);
        auto newTask = TaskService::updateTask(task->id, requestBody);
        json response;
        response["data"] = newTask.toDto();
        req.sendJson(req, 201, response);
    }

    static void deleteTask(hs::Request &req) {
        string &taskId = req.getRequestParam("taskId");
        auto loggedInUser = any_cast<std::shared_ptr<User>>(req.getRequestAttribute("loggedInUser"));
        auto task = checkTaskOwnership(std::stoi(taskId), loggedInUser->id);
        TaskService::deleteTask(std::stoi(taskId));
        req.sendJson(req, 204);
    }


public:
    explicit TaskController(HttpServer::Router *router, const string &basePath = "/") : ControllerBase(router,
                                                                                                       basePath) {}

    void registerEndpoints() override {
        router->postMethod(basePath, {Middlewares::usersAuthentication, createTask});
        router->getMethod(basePath, {Middlewares::usersAuthentication, findAllTask});
        router->getMethod(basePath + "/:taskId", {Middlewares::usersAuthentication, getTask});
        router->putMethod(basePath + "/:taskId", {Middlewares::usersAuthentication, updateTask});
        router->deleteMethod(basePath + "/:taskId", {Middlewares::usersAuthentication, deleteTask});
    }
};

#endif //HTTP_SERVER_TASKCONTROLLER_H
