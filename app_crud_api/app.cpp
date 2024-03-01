//
// Created by pepe on 2/27/24.
//

#include <iostream>
#include "controllers/UserController.h"

int main() {
    HttpServer::Server server("9000", 10);

    server.getRouter().registerRoute(HttpServer::HttpMethod::GET, "/", [](HttpServer::Request &request) {
        request.sendText(request, 200, "Hello World");
    });

    // Defining controllers
    UserController userController(&server.getRouter(), "/users");
    userController.registerEndpoints();

    if (server.start() < 0) {
        std::cerr << "Error while starting server\n";
    }
    return 0;
}
