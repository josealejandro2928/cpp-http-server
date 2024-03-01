//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_USERSERVICE_H
#define HTTP_SERVER_USERSERVICE_H

#include "models/Models.h"
#include "dto/RequestDtos.h"

extern std::vector<User> users;

namespace userService {
    class UserService {
    public:
        static std::vector<User> users;

        static User createUser(CreateUserRequest &request);

        static User *findUserByEmail(const std::string &email);

        static User *findUserById(const std::string &id);

        static std::vector<User> &findAll();

    };

}

#endif //HTTP_SERVER_USERSERVICE_H


