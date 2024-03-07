//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_MODELS_H
#define HTTP_SERVER_MODELS_H

#include "vector"
#include "string"
#include "nlohmann/json.hpp"
#include "dto/ResponseDtos.h"


struct User {
    int id{};
    std::string name;
    std::string email;
    std::string password;
    static int globalUserId;

    User() = default;

    User(std::string name, std::string email, std::string password);

    [[nodiscard]] UserResponseDto toDto() const;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(User, id, name, email, password)


struct Task {
    int id;
    std::string title;
    std::string description;
    std::string status;
    int userId;
    User user;
    static int globalTaskId;

    Task() = default;

    Task(std::string title, std::string description, std::string status, User &creator);
    [[nodiscard]] TaskResponseDto toDto() const;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Task, id, title, description, status, userId, user)

struct UserTokenData {
    std::string email;
    long exp;
    std::string token;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UserTokenData, email, token, exp)

#endif //HTTP_SERVER_MODELS_H
