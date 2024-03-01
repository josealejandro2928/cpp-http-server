#include <iostream>
#include "core/Server.h"
#include <thread>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

struct Response {
    long sum;
};

struct RequestData {
    std::string name;
    std::string lastName;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Response, sum)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RequestData, name, lastName)

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
        auto data = request.getBodyObject<RequestData>();
        string name, lastName;
        name = data.name;
        lastName = data.lastName;
        std::cout << name << " " << lastName << std::endl;
        Response response{sum};
        request.sendJson<Response>(request, 200, response);
    };
    router.registerRoute(HttpServer::HttpMethod::GET, "/hello", middleware);

    router.registerRoute(HttpServer::HttpMethod::GET, "/", [](HttpServer::Request &request) -> void {
        request.sendText(request, 200, "Hello World");
    });

    if (server.start() < 0) {
        std::cerr << "Error while starting server\n";
    }
    return 0;
}
