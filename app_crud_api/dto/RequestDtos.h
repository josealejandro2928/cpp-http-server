//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_REQUESTDTOS_H
#define HTTP_SERVER_REQUESTDTOS_H


#include <string>
#include "models/Models.h"
#include <nlohmann/json.hpp>

struct CreateUserRequest {
    std::string name;
    std::string email;
    std::string password;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreateUserRequest, name, email, password)

struct UpdateUserRequest {
    std::string name;
    std::string password;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UpdateUserRequest, name, password)

struct LoginRequest {
    std::string email;
    std::string password;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LoginRequest, email, password)

struct CreateTaskRequest {
    std::string title;
    std::string description;
    std::string status = "PENDING";
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreateTaskRequest, title, description, status)

struct RequestFilterTask {
    std::string title;
    std::vector<std::string> status;
};

#endif //HTTP_SERVER_REQUESTDTOS_H
