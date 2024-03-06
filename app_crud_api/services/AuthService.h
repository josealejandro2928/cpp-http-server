//
// Created by pepe on 3/6/24.
//

#ifndef HTTP_SERVER_AUTHSERVICE_H
#define HTTP_SERVER_AUTHSERVICE_H

#include<unordered_map>
#include <string>
#include "models/Models.h"
#include "mutex"
#include "chrono"

class AuthService {
private:
    static std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period> TTL;
    static std::mutex authTokenMutex;
    static std::unordered_map<std::string, UserTokenData> authTokens;
public:
    static UserTokenData generateToken(User &user);

    static void setToken(std::string &token, UserTokenData *userTokenData);

    static UserTokenData *getTokenData(const std::string &token);

    static UserTokenData *validateToken(std::string &);
};

class InvalidTokenException : public std::exception {
    std::string message;
public:
    explicit InvalidTokenException(std::string message) : message("Invalid token: " + std::move(message)) {
    }

    [[nodiscard]] const char *what() const noexcept override {
        return message.c_str();
    }
};

#endif //HTTP_SERVER_AUTHSERVICE_H
