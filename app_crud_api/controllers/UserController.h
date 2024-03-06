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

class UserController : public ControllerBase {

public:
    static void createUser(HttpServer::Request &req) {
        auto createUserBody = req.getBodyObject<CreateUserRequest>();
        auto userFound = UserService::findUserByEmail(createUserBody.email);
        if (userFound != nullptr) throw HttpServer::BadRequestException("The email is already taken");
        auto user = UserService::createUser(createUserBody);
        auto responseEntity = ResponseEntity<UserResponseDto>{user.toDto()};
        req.sendJson<ResponseEntity<UserResponseDto>>(req, 201, responseEntity);
    }

    static void findAllUsers(HttpServer::Request &req) {
        std::cout << "Find All users" << std::endl;
        auto users = HttpServer::mapFn<std::vector<User>, UserResponseDto>(UserService::findAll(),
                                                                           [](const User &user) {
                                                                               return user.toDto();
                                                                           });
        req.sendJson<ResponseEntity<vector<UserResponseDto>>>(req, 200, ResponseEntity<vector<UserResponseDto>>{users});
    }

    static void findUser(HttpServer::Request &req) {
        std::cout << "Find user By Id" << std::endl;
        string &userId = req.getRequestParam("userId");
        User *user = UserService::findUserById(userId);
        if (user == nullptr) throw HttpServer::NotFoundException("User not found with id:" + userId);
        req.sendJson<ResponseEntity<UserResponseDto>>(req, 200, ResponseEntity<UserResponseDto>{user->toDto()});
    }

    static void loginUser(HttpServer::Request &req) {
        auto userLoginRequest = req.getBodyObject<LoginRequest>();
        User *user = UserService::findUserByEmail(userLoginRequest.email);
        if (!user) throw HttpServer::UnauthorizedException("User not found");
        if (user->password != userLoginRequest.password) throw HttpServer::UnauthorizedException("Invalid password");
        auto tokenData = AuthService::generateToken(*user);
        AuthService::setToken(tokenData.token, &tokenData);
        json response;
        response["token"] = tokenData.token;
        response["user"] = user->toDto();
        req.sendJson(req, 200, response);
    }

public:
    explicit UserController(HttpServer::Router *router, const string &basePath = "/") : ControllerBase(router,
                                                                                                       basePath) {}

    void registerEndpoints() override {
        router->postMethod(basePath + "/login", loginUser);
        router->getMethod(basePath, findAllUsers);
        router->postMethod(basePath, {Middlewares::usersAuthentication, createUser});
        router->getMethod(basePath + "/:userId",
                          {Middlewares::usersAuthentication, findUser});
    }
};

#endif //HTTP_SERVER_USERCONTROLLER_H
