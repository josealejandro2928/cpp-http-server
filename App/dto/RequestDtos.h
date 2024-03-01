//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_REQUESTDTOS_H
#define HTTP_SERVER_REQUESTDTOS_H


#include <string>
#include <nlohmann/json.hpp>

struct CreateUserRequest {
    std::string name;
    std::string email;
    std::string password;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CreateUserRequest, name, email, password)

#endif //HTTP_SERVER_REQUESTDTOS_H
