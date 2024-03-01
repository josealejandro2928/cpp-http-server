//
// Created by pepe on 3/1/24.
//

#include "vector"
#include "UserService.h"
#include <algorithm>


namespace userService {
    std::vector<User> UserService::users = {};
    User UserService::createUser(CreateUserRequest &request) {
        User user(request.name, request.email, request.password);
        users.push_back(user);
        return user;
    }

    User *UserService::findUserByEmail(const std::string &email) {
        auto it = std::find_if(users.begin(), users.end(), [=](User &user) {
            return user.email == email;
        });
        return it == users.end() ? nullptr : it.base();
    }

    std::vector<User> &UserService::findAll() {
        std::sort(users.begin(), users.end(), [=](auto &a, auto &b) {
            return a.id < b.id;
        });
        return users;
    }

    User *UserService::findUserById(const std::string &id) {
        auto it = std::find_if(users.begin(), users.end(), [=](User &user) {
            return std::to_string(user.id) == id;
        });
        return it == users.end() ? nullptr : it.base();
    }

}

