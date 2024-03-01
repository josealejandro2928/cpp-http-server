//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_CONTROLLERBASE_H
#define HTTP_SERVER_CONTROLLERBASE_H


#include <utility>
#include "http_server/Server.h"
#include "string"

class ControllerBase {
protected:
    HttpServer::Router *router = nullptr;
    std::string basePath;
public:

    ControllerBase() = delete;

    ControllerBase(HttpServer::Router *router, std::string basePath) : basePath(std::move(basePath)), router(router) {}

    virtual void registerEndpoints() = 0;
};


#endif //HTTP_SERVER_CONTROLLERBASE_H
