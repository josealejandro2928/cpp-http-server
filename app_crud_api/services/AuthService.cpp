//
// Created by pepe on 3/6/24.
//

#include <fstream>
#include "AuthService.h"
#include "nlohmann/json.hpp"
#include "http_server/utils/Logging.h"
#include "http_server/utils/utils.h"

namespace hs = HttpServer;

std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period> AuthService::TTL = std::chrono::hours(
        1);
std::unordered_map<std::string, UserTokenData> AuthService::authTokens = {};
std::mutex AuthService::authTokenMutex = {};
fs::path AuthService::dataPath = fs::current_path().parent_path() / "app_crud_api" / "data" / "auth_tokens.json";

void AuthService::syncData() {
    std::ofstream file(AuthService::dataPath);
    nlohmann::json j = AuthService::authTokens;
    file << j;
}

UserTokenData AuthService::generateToken(User &user) {
    std::hash<std::string> hasher;
    auto hashValue = hasher(std::to_string(user.id) + ":" + user.email + ":" + user.password + ":" +
                            std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
    auto token = std::to_string(hashValue);
    std::scoped_lock<std::mutex> lock{AuthService::authTokenMutex};
    auto exp = std::chrono::system_clock::now() + AuthService::TTL;
    auto expSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(exp.time_since_epoch()).count();
    UserTokenData userTokenData{user.email, expSinceEpoch, token};
    return userTokenData;
}

void AuthService::setToken(std::string &token, UserTokenData *userTokenData) {
    std::scoped_lock<std::mutex> lock{AuthService::authTokenMutex};
    if (userTokenData == nullptr) {
        AuthService::authTokens.erase(token);
    } else {
        auto tokesForUser = hs::filterFn(AuthService::authTokens, [userTokenData](const auto &entry) {
            return entry.second.email == userTokenData->email;
        });
        if (tokesForUser.size() > 1) {
            for (int i = 1; i < tokesForUser.size(); i++) {
                AuthService::authTokens.erase(tokesForUser[i].first);
            }
        }
        AuthService::authTokens[token] = *userTokenData;
    }
    syncData();
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

void AuthService::readTokensFromFile() {
    std::ifstream file(AuthService::dataPath);
    if (!file) {
        hs::Logging::warn(("Unable to open the file: " + AuthService::dataPath.string()).c_str());
        return;
    }
    auto jsonData = nlohmann::json::parse(file);
    for (const auto &tokenData: jsonData) {
        UserTokenData userTokenData = tokenData.get<UserTokenData>();
        AuthService::authTokens[userTokenData.token] = userTokenData;
    }
}

void AuthService::logoutUser(const std::string &email) {
    for (auto it = authTokens.begin(); it != authTokens.end();) {
        if (it->second.email == email) {
            it = authTokens.erase(it);
        } else {
            ++it;
        }
    }

}




