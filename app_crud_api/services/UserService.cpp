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

namespace fs = std::filesystem;
namespace hs = HttpServer;
using json = nlohmann::json;

std::vector<User> UserService::users = {};

User UserService::createUser(CreateUserRequest &request) {
    User user(request.name, request.email, request.password);
    users.push_back(user);
    return user;
}

User *UserService::findUserByEmail(const std::string &email) {
    auto it = std::find_if(users.begin(), users.end(), [&](User &user) {
        return user.email == email;
    });
    return it == users.end() ? nullptr : it.base();
}

std::vector<User> &UserService::findAll() {
    std::sort(users.begin(), users.end(), [&](auto &a, auto &b) {
        return a.id < b.id;
    });
    return users;
}

User *UserService::findUserById(const std::string &id) {
    auto it = std::find_if(users.begin(), users.end(), [&](User &user) {
        return std::to_string(user.id) == id;
    });
    return it == users.end() ? nullptr : it.base();
}

void UserService::readUsersFromFile() {
    fs::path basePath = fs::current_path().parent_path();
    fs::path fullPath = basePath / "app_crud_api" / "data" / "users.json";
    hs::Logging::info(("Reading users from file: " + fullPath.string()).c_str());
    std::ifstream file(fullPath);
    if (!file) {
        throw std::runtime_error("Unable to open the file: " + fullPath.string());
    }
    auto jsonData = json::parse(file);
    for (const auto &userJson: jsonData) {
        User user = userJson.get<User>();
        UserService::users.push_back(user);
    }

}


