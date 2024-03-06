//
// Created by pepe on 3/6/24.
//

#ifndef HTTP_SERVER_RESPONSEDTO_H
#define HTTP_SERVER_RESPONSEDTO_H

#include <nlohmann/json.hpp>

template<class T>
class ResponseEntity {
public:
    T &data;

    explicit ResponseEntity(T &data) : data(data) {}
    explicit ResponseEntity(T &&data) : data(data) {}

};

template<class T>
void from_json(const nlohmann::json &j, ResponseEntity<T> &res) {
    res.data = j.get<T>();
}

template<class T>
void to_json(nlohmann::json &j, const ResponseEntity<T> &res) {
    j["data"] = res.data;
}

struct UserResponseDto {
    int id;
    std::string name;
    std::string email;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UserResponseDto, id, name, email)

#endif //HTTP_SERVER_RESPONSEDTO_H
