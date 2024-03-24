//
// Created by pepe on 2/27/24.
//
#include "controllers/UserController.h"
#include "http_server/utils/Logging.h"
#include "middlewares/Middlewares.h"
#include "controllers/TaskController.h"
#include "services/AuthService.h"
#include "data/DataStorage.h"
#include "http_server/exceptions/Exceptions.h"
#include "controllers/ComputationController.h"
#include "nlohmann/json.hpp"

namespace hs = HttpServer;

void globalErrorHandler(std::exception &exc, hs::Request &req) {
    string message = "Custom Global Error Handler: " + string(exc.what());
    hs::Logging::error(message.c_str());
    auto *ptr = dynamic_cast<hs::HttpException *>(&exc);
    if (ptr) {
        hs::ErrorResponseData errorResponseData(ptr->getCode(), ptr->what());
        req.sendJson<hs::ErrorResponseData>(req, errorResponseData.code, errorResponseData);
        return;
    }
    auto *ptr2 = dynamic_cast<nlohmann::json::exception *>(&exc);
    if (ptr2) {
        hs::ErrorResponseData errorResponseData(422, ptr2->what());
        req.sendJson<hs::ErrorResponseData>(req, errorResponseData.code, errorResponseData);
        return;
    }
    hs::ErrorResponseData errorResponseData(500, "Internal Server Error" + string(exc.what()));
    req.sendJson<hs::ErrorResponseData>(req, errorResponseData.code, errorResponseData);
}

int main() {
    hs::Server server(9000);

    server.getRouter().registerRoute(hs::HttpMethod::GET, "/", [](hs::Request &request) {
        request.sendText(request, 200, "Hello World");
    });

    // Defining controllers
    UserController userController(&server.getRouter(), "/users");
    TaskController taskController(&server.getRouter(), "/tasks");
    ComputationController computationController(&server.getRouter(), "/computation");
    userController.registerEndpoints();
    taskController.registerEndpoints();
    computationController.registerEndpoints();

    // Defining middlewares
    server.getRouter().use(hs::Logging::httpRequestLog);
//    server.getRouter().use(Middlewares::apiAuthentication);

    server.onServerStart([]() {
        hs::Logging::info("<<<<<>>>>>Server started<<<<<>>>>>");
        hs::Logging::info("<<<<<>>>>>Loading data from dataStorage files<<<<<>>>>>");
        DataStorage::getInstance(fs::current_path().parent_path() / "app_crud_api" / "data");
        AuthService::readTokensFromFile();
        hs::Logging::info("<<<<<>>>>>Tokens loaded from file<<<<<>>>>>");
        hs::Logging::info("<<<<<>>>>>Created a thread Pool of workers<<<<<>>>>>");
        auto &taskWorker = TaskWorkerService::getInstance(16).getPool();
        taskWorker.runSchedule(
                []() {
                    cout << "Running a periodic task every 10 seconds" << endl;
                },
                chrono::seconds(10),
                chrono::seconds(0)
        );

        taskWorker.runSchedule(TaskService::periodicTaskComputation, chrono::seconds(20), chrono::seconds(10));
    });
    server.setGlobalExceptionHandler(globalErrorHandler);

    server.startListening();
    return 0;
}
