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
#include "dto/ResponseDto.h"
#include "services/UserService.h"
#include "services/AuthService.h"
#include "middlewares/Middlewares.h"
#include "nlohmann/json.hpp"
#include "http_server/utils/utils.h"

using json = nlohmann::json;
namespace hs = HttpServer;

class UserController : public ControllerBase {

public:
    static void createUser(HttpServer::Request &req) {
        auto createUserBody = req.getBodyObject<CreateUserRequest>();
        auto userFound = UserService::findUserByEmail(createUserBody.email);
        if (userFound != nullptr) throw HttpServer::BadRequestException("The email is already taken");
        json response;
        auto user = UserService::createUser(createUserBody);
        response["data"] = user.toDto();
        req.sendJson(req, 201, response);
    }

    static void findAllUsers(HttpServer::Request &req) {
//        auto users = hs::mapFn<std::vector<User>, UserResponseDto>(UserService::findAll(),
//                                                                   [](const User &user) {
//                                                                       return user.toDto();
//                                                                   });

        auto users = UserService::findAll();
        json response;
        response["data"] = users;
        req.sendJson(req, 200, response);
    }

    static void findUser(HttpServer::Request &req) {
        string &userId = req.getRequestParam("userId");
        auto user = UserService::findUserById(userId);
        if (user == nullptr) throw HttpServer::NotFoundException("User not found with id:" + userId);
        json response;
        response["data"] = user->toDto();
        req.sendJson(req, 200, response);
    }

    static void loginUser(HttpServer::Request &req) {
        auto userLoginRequest = req.getBodyObject<LoginRequest>();
        auto user = UserService::findUserByEmail(userLoginRequest.email);
        if (!user) throw HttpServer::UnauthorizedException("User not found");
        if (user->password != userLoginRequest.password) throw HttpServer::UnauthorizedException("Invalid password");
        auto tokenData = AuthService::generateToken(*user);
        AuthService::setToken(tokenData.token, &tokenData);
        json response;
        response["token"] = tokenData.token;
        response["user"] = user->toDto();
        req.sendJson(req, 200, response);
    }

    static void logoutUser(HttpServer::Request &req) {
        auto loggedInUser = any_cast<std::shared_ptr<User>>(req.getRequestAttribute("loggedInUser"));
        AuthService::logoutUser(loggedInUser->email);
        req.sendJson(req, 200, json({{"message", "Logged out"}}));
    }

    static void deleteUser(HttpServer::Request &req) {
        string &userId = req.getRequestParam("userId");
        UserService::deleteUser(userId);
        req.sendJson(req, 200, json({{"message", "User deleted"}}));
    }

    static void updateUser(HttpServer::Request &req) {
        string &userId = req.getRequestParam("userId");
        auto createUserBody = req.getBodyObject<UpdateUserRequest>();
        auto user = UserService::updateUser(userId, createUserBody);
        json response;
        response["data"] = user.toDto();
        req.sendJson(req, 200, response);
    }

public:
    explicit UserController(HttpServer::Router *router, const string &basePath = "/") : ControllerBase(router,
                                                                                                       basePath) {}

    void registerEndpoints() override {
        router->postMethod(basePath + "/login", loginUser);
        router->postMethod(basePath + "/logout", {Middlewares::usersAuthentication, logoutUser});
        router->getMethod(basePath, findAllUsers);
        router->postMethod(basePath, {Middlewares::usersAuthentication, createUser});
        router->getMethod(basePath + "/:userId",
                          {Middlewares::usersAuthentication, findUser});

        router->deleteMethod(basePath + "/:userId",
                             {Middlewares::usersAuthentication, deleteUser});

        router->putMethod(basePath + "/:userId", {Middlewares::usersAuthentication, updateUser});
    }
};

#endif //HTTP_SERVER_USERCONTROLLER_H
