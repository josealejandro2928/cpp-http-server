//
// Created by pepe on 2/27/24.
//
#include "controllers/UserController.h"
#include "http_server/utils/Logging.h"
#include "middlewares/Middlewares.h"
#include "controllers/TaskController.h"
#include "services/AuthService.h"
#include "data/DataStorage.h"

namespace hs = HttpServer;

int main() {
    hs::Server server("9000");

    server.getRouter().registerRoute(hs::HttpMethod::GET, "/", [](hs::Request &request) {
        request.sendText(request, 200, "Hello World");
    });

    // Defining controllers
    UserController userController(&server.getRouter(), "/users");
    TaskController taskController(&server.getRouter(), "/tasks");
    userController.registerEndpoints();
    taskController.registerEndpoints();

    // Defining middlewares
    server.getRouter().use(hs::Logging::httpRequestLog);
//    server.getRouter().use(Middlewares::apiAuthentication);

    server.onServerStart([]() {
        hs::Logging::info("<<<<<>>>>>Server started<<<<<>>>>>");
        hs::Logging::info("<<<<<>>>>>Loading data from dataStorage files<<<<<>>>>>");
        DataStorage::getInstance(fs::current_path().parent_path() / "app_crud_api" / "data");
        AuthService::readTokensFromFile();
        hs::Logging::info("<<<<<>>>>>Tokens loaded from file<<<<<>>>>>");
    });
    server.startListening();
    return 0;
}
