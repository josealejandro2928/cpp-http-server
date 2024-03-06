//
// Created by pepe on 3/6/24.
//

#include "AuthService.h"

UserTokenData AuthService::generateToken(User &user) {
    std::hash<std::string> hasher;
    auto hashValue = hasher(std::to_string(user.id) + ":" + user.email + ":" + user.password + ":" +
                            std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
    auto token = std::to_string(hashValue);
    std::scoped_lock<std::mutex> lock{AuthService::authTokenMutex};
    auto exp = std::chrono::system_clock::now() + AuthService::TTL;
    auto expSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(exp.time_since_epoch()).count();
    UserTokenData userTokenData{user.id, user.email, expSinceEpoch, token};
    return userTokenData;
}

void AuthService::setToken(std::string &token, UserTokenData *userTokenData) {
    std::scoped_lock<std::mutex> lock{AuthService::authTokenMutex};
    if (userTokenData == nullptr) {
        AuthService::authTokens.erase(token);
    } else {
        AuthService::authTokens[token] = *userTokenData;
    }
}

UserTokenData *AuthService::getTokenData(const std::string &token) {
    if (AuthService::authTokens.find(token) == AuthService::authTokens.end()) return nullptr;
    return &AuthService::authTokens[token];
}

UserTokenData *AuthService::validateToken(std::string &token) {
    auto tokenData = AuthService::getTokenData(token);
    if (tokenData == nullptr) throw InvalidTokenException("Token not found");
    long now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    long ttl = tokenData->exp;
    if (now > ttl) throw InvalidTokenException("Token expired");
    return tokenData;
}


std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period> AuthService::TTL = std::chrono::hours(
        1);

std::unordered_map<std::string, UserTokenData> AuthService::authTokens = {};
std::mutex AuthService::authTokenMutex = {};

