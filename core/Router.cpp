//
// Created by pepe on 2/27/24.
//

#include <stdexcept>
#include "vector"
#include "string"
#include "map"
#include "Router.h"

namespace HttpServer {


//    void Router::registerRoute(HttpMethod method, string &&path, const Middleware &middleware) {
//        switch (method) {
//            case HttpMethod::GET:
//                GetRoutes[path].push_back(middleware);
//                break;
//            default:
//                throw std::invalid_argument("Unknown HTTP VERB");
//        }
//
//    }

    void Router::registerRoute(HttpMethod method, string &&path, const Middleware &middleware) {
        switch (method) {
            case HttpMethod::GET:
                GetRoutes[path].push_back(middleware);
                break;
            default:
                throw std::invalid_argument("Unknown HTTP VERB");
        }

    }

    void Router::switchRouter(Request &req) {
        string path = req.getPath();
        if (req.getMethod() == "GET") {
            return Router::processCallStacks(path, req, GetRoutes);
        }
        if (req.getMethod() == "POST") {
            return;
        }
        req.sendResponse(req, 404, "Not Found");
    }

    void Router::processCallStacks(string &path, Request &req, map<string, vector<Middleware>> &Routes) {
        ResponseMiddleware *response = nullptr;
        if (Routes.find(path) != Routes.end()) {
            for (auto &middleware: Routes[path]) {
                try {
                    delete response;
                    response = nullptr;
                    response = middleware(req);
                    if (req.hasSendResponseBeenCalled) {
                        delete response;
                        return;
                    }
                } catch (std::exception &e) {
                    delete response;
                    req.sendResponse(req, 500, e.what());
                    return;
                }
            }
            if (response != nullptr) {
                req.sendResponse(req, response->statusCode, response->data);
                delete response;
            }
        }
        req.sendResponse(req, 404, "Not Found");
    }

}
