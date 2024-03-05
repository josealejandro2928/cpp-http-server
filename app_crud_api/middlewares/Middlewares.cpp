//
// Created by pepe on 3/5/24.
//

#include "Middlewares.h"
#include "http_server/exceptions/Exceptions.h"
#include "http_server/utils/utils.h"
#include "services/UserService.h"

std::string Middlewares::apiToken = "1234567890__!!";

void Middlewares::apiAuthentication(HttpServer::Request &req) {
    std::string data = req.getHeader("X-API-TOKEN");
    if (data.empty()) throw HttpServer::UnauthorizedException("Missing token header");
    if (apiToken != data) throw HttpServer::UnauthorizedException("Invalid token");
}

void Middlewares::usersAuthentication(HttpServer::Request &req) {
    std::string data = req.getHeader("Authentication");
    if (data.empty()) throw HttpServer::UnauthorizedException("Missing token authentication");
    if (data.find("Bearer ") != 0)
        throw HttpServer::UnauthorizedException("Invalid token authentication, should starts with Bearer");
    std::string tokenUser = HttpServer::strSplit(data, ' ')[1];
    auto parts = HttpServer::strSplit(tokenUser, ':');
    if (parts.size() > 2)
        throw HttpServer::UnauthorizedException("Invalid token authentication, should be user:password");
    std::string &email = parts.at(0);
    std::string &password = parts.at(1);
    auto user = UserService::findUserByEmail(email);
    if (user == nullptr) throw HttpServer::UnauthorizedException("User not found");
    if (user->password != password) throw HttpServer::UnauthorizedException("Invalid password");
    req.setRequestAttribute("loggedInUser",  user);
}
