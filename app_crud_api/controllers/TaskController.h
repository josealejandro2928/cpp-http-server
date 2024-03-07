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
    static std::string possibleStatus[3];

public:
    static void createTask(HttpServer::Request &req) {
        auto createTaskRequest = req.getBodyObject<CreateTaskRequest>();
        if (std::find(TaskController::possibleStatus, TaskController::possibleStatus + 3, createTaskRequest.status) ==
            TaskController::possibleStatus + 3) {
            throw HttpServer::BadRequestException("Invalid status");
        }
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
        req.sendJson<vector<Task>>(req, 200, TaskService::getTasks(*loggedInUser,filter));
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
