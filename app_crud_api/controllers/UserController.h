//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_USERCONTROLLER_H
#define HTTP_SERVER_USERCONTROLLER_H

#include "http_server/exceptions/Exceptions.h"
#include "iostream"
#include <vector>
#include "ControllerBase.h"
#include "dto/RequestDtos.h"
#include "services/UserService.h"

class UserController : public ControllerBase {

public:
    static void createUser(HttpServer::Request &req) {
        std::cout << "Create user" << std::endl;
        auto createUserBody = req.getBodyObject<CreateUserRequest>();
        auto userFound = UserService::findUserByEmail(createUserBody.email);
        if (userFound != nullptr) throw HttpServer::BadRequestException("The email is already taken");
        auto user = UserService::createUser(createUserBody);
        req.sendJson<User>(req, 201, user);
    }

    static void findAllUsers(HttpServer::Request &req) {
        std::cout << "Find All users" << std::endl;
        req.sendJson<vector<User>>(req, 200, UserService::findAll());
    }

    static void findUser(HttpServer::Request &req) {
        std::cout << "Find user By Id" << std::endl;
        string &userId = req.getRequestParam("userId");
        User *user = UserService::findUserById(userId);
        if (user == nullptr) throw HttpServer::NotFoundException("User not found with id:" + userId);
        req.sendJson<User>(req, 200, *user);
    }

public:
    explicit UserController(HttpServer::Router *router, const string &basePath = "/") : ControllerBase(router,
                                                                                                       basePath) {}

    void registerEndpoints() override {
        router->registerRoute(HttpServer::HttpMethod::POST, basePath, createUser);
        router->registerRoute(HttpServer::HttpMethod::GET, basePath, findAllUsers);
        router->registerRoute(HttpServer::HttpMethod::GET, basePath + "/:userId", findUser);
    }
};

#endif //HTTP_SERVER_USERCONTROLLER_H
