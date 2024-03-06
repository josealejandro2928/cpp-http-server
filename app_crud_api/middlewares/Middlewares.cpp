//
// Created by pepe on 3/5/24.
//

#include "Middlewares.h"
#include "http_server/exceptions/Exceptions.h"
#include "http_server/utils/utils.h"
#include "services/UserService.h"
#include "services/AuthService.h"

std::string Middlewares::apiToken = "1234567890__!!";

void Middlewares::apiAuthentication(HttpServer::Request &req) {
    std::string data = req.getHeader("X-API-TOKEN");
    if (data.empty()) throw HttpServer::UnauthorizedException("Missing token header");
    if (apiToken != data) throw HttpServer::UnauthorizedException("Invalid token");
}

void Middlewares::usersAuthentication(HttpServer::Request &req) {
    std::string data = req.getHeader("Authorization");
    if (data.empty()) throw HttpServer::UnauthorizedException("Missing token authentication");
    if (data.find("Bearer ") != 0)
        throw HttpServer::UnauthorizedException("Invalid token authentication, should starts with Bearer");
    std::string tokenUser = HttpServer::strSplit(data, ' ')[1];
    UserTokenData* tokenData;
    try {
        tokenData = AuthService::validateToken(tokenUser);
    } catch (InvalidTokenException &e) {
        throw HttpServer::UnauthorizedException(e.what());
    }
    auto user = UserService::findUserByEmail(tokenData->email);
    if (user == nullptr) throw HttpServer::UnauthorizedException("User not found");
    req.setRequestAttribute("loggedInUser", user);
}
