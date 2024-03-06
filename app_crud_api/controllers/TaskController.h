//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_TASKCONTROLLER_H
#define HTTP_SERVER_TASKCONTROLLER_H

#include "http_server/exceptions/Exceptions.h"
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
    static std::string possibleStatus[3];

public:
    static void createTask(HttpServer::Request &req) {
        auto createTaskRequest = req.getBodyObject<CreateTaskRequest>();
        if (std::find(TaskController::possibleStatus, TaskController::possibleStatus + 3, createTaskRequest.status) ==
            TaskController::possibleStatus + 3) {
            throw HttpServer::BadRequestException("Invalid status");
        }
        auto loggedInUser = any_cast<User *>(req.getRequestAttribute("loggedInUser"));
        Task task = TaskService::createTask(createTaskRequest, *loggedInUser);
        req.sendJson<Task>(req, 201, task);
    }

    static void findAllTask(HttpServer::Request &req) {
        auto loggedInUser = any_cast<User *>(req.getRequestAttribute("loggedInUser"));
        req.sendJson<vector<Task>>(req, 200, TaskService::getTasks(*loggedInUser));
    }


public:
    explicit TaskController(HttpServer::Router *router, const string &basePath = "/") : ControllerBase(router,
                                                                                                       basePath) {}

    void registerEndpoints() override {
        router->postMethod(basePath, {Middlewares::usersAuthentication, createTask});
        router->getMethod(basePath, {Middlewares::usersAuthentication, findAllTask});
    }
};

std::string TaskController::possibleStatus[3] = {"PENDING", "IN_PROGRESS", "DONE"};

#endif //HTTP_SERVER_TASKCONTROLLER_H
