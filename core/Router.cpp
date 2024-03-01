//
// Created by pepe on 2/27/24.
//

#include <stdexcept>
#include "vector"
#include "string"
#include "map"
#include "Router.h"
#include "utils/types.h"

namespace HttpServer {

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
        req.sendResponse(req, 404, "Not Found", ContentType::TEXT);
    }

    void Router::processCallStacks(string &path, Request &req, map<string, vector<Middleware>> &Routes) {
        if (Routes.find(path) != Routes.end()) {
            for (auto &middleware: Routes[path]) {
                try {
                    middleware(req);
                    if (req.hasSendResponseBeenCalled) {
                        return;
                    }
                } catch (std::exception &e) {
                    req.sendResponse(req, 500, e.what(), ContentType::TEXT);
                    return;
                }
            }
            if (!req.hasSendResponseBeenCalled) {
                req.sendResponse(req, 400, "You must call sendResponse in your middleware at some point.",
                                 ContentType::TEXT);
            }
        }
        req.sendResponse(req, 404, "Not Found", ContentType::TEXT);
    }

}
