//
// Created by pepe on 2/27/24.
//
#include "controllers/UserController.h"
#include "http_server/utils/Logging.h"
#include "thread"
#include "middlewares/Middlewares.h"

namespace hs = HttpServer;

int main() {
    hs::Server server("9000", 10);

    server.getRouter().registerRoute(hs::HttpMethod::GET, "/", [](hs::Request &request) {
        request.sendText(request, 200, "Hello World");
    });

    // Defining controllers
    UserController userController(&server.getRouter(), "/users");
    userController.registerEndpoints();

    // Defining middlewares
    server.getRouter().use(hs::Logging::httpRequestLog);
    server.getRouter().use(Middlewares::apiAuthentication);

    server.onServerStart([]() {
        hs::Logging::info("<<<<<>>>>>Server started<<<<<>>>>>");
        UserService::readUsersFromFile();
        hs::Logging::info("<<<<<>>>>>Users loaded from file<<<<<>>>>>");
    });
    server.startListening();
    return 0;
}
