//
// Created by pepe on 2/27/24.
//

#include <iostream>
#include "../core/Server.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    HttpServer::Server server("9000", 10);
    auto &router = server.getRouter();
    router.registerRoute(HttpServer::HttpMethod::GET, "/", [](auto &request) -> HttpServer::ResponseMiddleware * {
        request.sendResponse(request, 200, "Hello World");
        return nullptr;
    });

    if (server.start() < 0) {
        std::cerr << "Error while starting server\n";
    }
    return 0;
}
