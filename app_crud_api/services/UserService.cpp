//
// Created by pepe on 3/1/24.
//

#include "vector"
#include "UserService.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include "http_server/utils/Logging.h"
#include <fstream>
#include "models/Models.h"
#include "data/DataStorage.h"

namespace fs = std::filesystem;
namespace hs = HttpServer;
using json = nlohmann::json;


User UserService::createUser(CreateUserRequest &request) {
    auto &dataStorage = DataStorage::getInstance("");
    User user(request.name, request.email, request.password);
    dataStorage.addUser(user);
    return user;
}

std::shared_ptr<User> UserService::findUserByEmail(const std::string &email) {
    auto &dataStorage = DataStorage::getInstance("");
    auto users = dataStorage.getUsers();
    auto it = std::find_if(users.begin(), users.end(), [&](User &user) {
        return user.email == email;
    });
    return it == users.end() ? nullptr : std::make_shared<User>(*it);
}

std::vector<User> UserService::findAll() {
    auto &dataStorage = DataStorage::getInstance("");
    auto users = dataStorage.getUsers();
    std::sort(users.begin(), users.end(), [&](auto &a, auto &b) {
        return a.id < b.id;
    });
    return users;
}

std::shared_ptr<User> UserService::findUserById(const std::string &id) {
    auto &dataStorage = DataStorage::getInstance("");
    auto users = dataStorage.getUsers();
    auto it = std::find_if(users.begin(), users.end(), [&](User &user) {
        return std::to_string(user.id) == id;
    });
    return it == users.end() ? nullptr : std::make_shared<User>(*it);
}


