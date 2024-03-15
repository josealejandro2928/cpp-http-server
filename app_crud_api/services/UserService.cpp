//
// Created by pepe on 3/1/24.
//

#include "vector"
#include "UserService.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include "models/Models.h"
#include "data/DataStorage.h"
#include "http_server/exceptions/Exceptions.h"
#include "AuthService.h"

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

void UserService::deleteUser(const std::string &id) {
    auto &dataStorage = DataStorage::getInstance("");
    auto users = dataStorage.getUsers();
    int index = -1;
    for (int i = 0; i < users.size(); i++) {
        if (std::to_string(users[i].id) == id) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        throw hs::NotFoundException("User not found with id:" + id);
    }
    dataStorage.deleteUser(index);
}

User UserService::updateUser(const std::string &id, const UpdateUserRequest &request) {
    auto &dataStorage = DataStorage::getInstance("");
    auto users = dataStorage.getUsers();
    int index = -1;
    for (int i = 0; i < users.size(); i++) {
        if (std::to_string(users[i].id) == id) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        throw hs::NotFoundException("User not found with id:" + id);
    }
    auto user = users[index];
    user.name = request.name;
    user.password = request.password;
    dataStorage.addUser(user, index);
    AuthService::logoutUser(user.email);
    return user;
}


