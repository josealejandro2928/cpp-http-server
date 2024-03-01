//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_CONTROLLERBASE_H
#define HTTP_SERVER_CONTROLLERBASE_H


#include <utility>

#include "../../core/Server.h"

class ControllerBase {
protected:
    HttpServer::Router *router = nullptr;
    string basePath;
public:

    ControllerBase() = delete;

    ControllerBase(HttpServer::Router *router, string basePath) : basePath(std::move(basePath)), router(router) {}

    virtual void registerEndpoints() = 0;
};


#endif //HTTP_SERVER_CONTROLLERBASE_H
