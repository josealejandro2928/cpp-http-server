//
// Created by pepe on 3/5/24.
//

#ifndef HTTP_SERVER_MIDDLEWARES_H
#define HTTP_SERVER_MIDDLEWARES_H

#include "http_server/Request.h"

class Middlewares {
    static std::string apiToken;
public:
    static void apiAuthentication(HttpServer::Request &req);

    static void usersAuthentication(HttpServer::Request &req);
};


#endif //HTTP_SERVER_MIDDLEWARES_H
