#include <iostream>
#include "core/Server.h"
#include <thread>

int main() {
    std::cout << "Hello, World!" << std::endl;
    HttpServer::Server server("9000", 10);
    HttpServer::Router &router = server.getRouter();
    auto middleware = [](HttpServer::Request &request) {
//        std::this_thread::sleep_for(std::chrono::seconds(10));
        long sum = 0;
        for (int i = 0; i < 1000000000; i++) {
            sum += i;
        }
        request.sendResponse(request, 200, "Then computation was: " + std::to_string(sum));
    };
    router.registerRoute(HttpServer::HttpMethod::GET, "/hello", middleware);

    router.registerRoute(HttpServer::HttpMethod::GET, "/", [](auto &request) -> void {
        request.sendResponse(request, 200, "Hello World");
    });

    if (server.start() < 0) {
        std::cerr << "Error while starting server\n";
    }
    return 0;
}
