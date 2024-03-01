//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_MODELS_H
#define HTTP_SERVER_MODELS_H

#include "vector"
#include "string"
#include "nlohmann/json.hpp"

struct User {
    int id;
    std::string name;
    std::string email;
    std::string password;

    User(std::string name, std::string email, std::string password);
};

enum class TaskStatus {
    PENDING,
    IN_PROGRESS,
    DONE
};

struct Task {
    int id;
    std::string title;
    std::string description;
    TaskStatus status;
    User &creator;

    Task(std::string title, std::string description, TaskStatus status, User &creator);
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(User, id, name, email, password)

#endif //HTTP_SERVER_MODELS_H
