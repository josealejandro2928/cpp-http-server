//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_USERSERVICE_H
#define HTTP_SERVER_USERSERVICE_H

#include "models/Models.h"
#include "dto/RequestDtos.h"
#include "filesystem"

namespace fs = std::filesystem;

class UserService {
public:
    static User createUser(CreateUserRequest &request);

    static std::shared_ptr<User> findUserByEmail(const std::string &email);

    static std::shared_ptr<User> findUserById(const std::string &id);

    static std::vector<User> findAll();

};


#endif //HTTP_SERVER_USERSERVICE_H


