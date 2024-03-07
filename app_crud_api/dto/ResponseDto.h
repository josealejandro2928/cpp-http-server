//
// Created by pepe on 3/6/24.
//

#ifndef HTTP_SERVER_RESPONSEDTO_H
#define HTTP_SERVER_RESPONSEDTO_H

#include <nlohmann/json.hpp>

struct UserResponseDto {
    int id;
    std::string name;
    std::string email;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UserResponseDto, id, name, email)

#endif //HTTP_SERVER_RESPONSEDTO_H
