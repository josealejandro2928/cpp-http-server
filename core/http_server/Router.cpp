//
// Created by pepe on 2/27/24.
//

#include <stdexcept>
#include "vector"
#include "string"
#include "map"
#include "Router.h"
#include "http_server/utils/types.h"
#include "http_server/utils/utils.h"
#include "exceptions/Exceptions.h"
#include <execinfo.h>
#include <iostream>
#include <cstdlib>

namespace HttpServer {

    void Router::registerRoute(HttpMethod method, const string &path, const Middleware &middleware) {
        switch (method) {
            case HttpMethod::GET:
                GetRoutes[path].push_back(middleware);
                break;
            case HttpMethod::POST:
                PostRoutes[path].push_back(middleware);
                break;
            case HttpMethod::DELETE:
                DeleteRoutes[path].push_back(middleware);
                break;
            case HttpMethod::PATCH:
                PatchRoutes[path].push_back(middleware);
                break;
            case HttpMethod::PUT:
                PutRoutes[path].push_back(middleware);
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
            return Router::processCallStacks(path, req, PostRoutes);
        }
        if (req.getMethod() == "PUT") {
            return Router::processCallStacks(path, req, PutRoutes);
        }
        if (req.getMethod() == "PATCH") {
            return Router::processCallStacks(path, req, PatchRoutes);
        }
        if (req.getMethod() == "DELETE") {
            return Router::processCallStacks(path, req, DeleteRoutes);
        }
        req.sendResponse(req, 404, "Not Found", ContentType::TEXT);
    }

    static void printStackTrace() {
        void *addrlist[64];
        int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void *));

        if (addrlen == 0) {
            std::cerr << "  <empty, possibly corrupt>\n";
            return;
        }

        char **symbollist = backtrace_symbols(addrlist, addrlen);

        // Print the stack trace
        for (int i = 4; i < addrlen; i++) {
            std::cerr << symbollist[i] << "\n";
        }

        free(symbollist);
    }

    static void processErrors(Request &req, std::exception &e) {
        auto *ptr = dynamic_cast<HttpException *>(&e);
        if (ptr) {
            ErrorResponseData errorResponseData(ptr->getCode(), ptr->what());
            req.sendJson<ErrorResponseData>(req, errorResponseData.code, errorResponseData);
            return;
        }
        ErrorResponseData errorResponseData(500, "Internal Server Error" + std::string(e.what()));
        req.sendJson<ErrorResponseData>(req, errorResponseData.code, errorResponseData);
    }

    static void processCallbacksSequence(Request &req, vector<Middleware> &middlewares) {
        for (auto &middleware: middlewares) {
            try {
                middleware(req);
                if (req.hasSendResponseBeenCalled) {
                    return;
                }
            } catch (std::exception &e) {
//                    printStackTrace();
                processErrors(req, e);
                return;
            }
        }
        if (!req.hasSendResponseBeenCalled) {
            ErrorResponseData errorResponseData(400, "You must call sendResponse in your middleware at some point.");
            req.sendJson<ErrorResponseData>(req, 400, errorResponseData);
        }
    }

    void Router::processCallStacks(string &path, Request &req, map<string, vector<Middleware>> &Routes) {
        for (auto &[key, middlewaresStack]: Routes) {
            if (key == path) {
                processCallbacksSequence(req, middlewaresStack);
                return;
            } else {
                auto requestedPathParts = strSplit(path, '/');
                string registeredPath = key;
                auto registeredPathParts = strSplit(registeredPath, '/');
                if (requestedPathParts.size() != registeredPathParts.size()) {
                    continue;
                }
                bool isMatch = true;
                auto &requestParams = req.getAllRequestParams();
                map<string, string> newRequestParams;
                for (int i = 0; i < requestedPathParts.size(); i++) {
                    if (registeredPathParts[i] != requestedPathParts[i] && registeredPathParts[i][0] != ':') {
                        isMatch = false;
                        break;
                    }
                    if (registeredPathParts[i][0] == ':') {
                        newRequestParams[registeredPathParts[i].substr(1)] = requestedPathParts[i];
                    }
                }
                if(isMatch) {
                    requestParams = newRequestParams;
                    processCallbacksSequence(req, middlewaresStack);
                    return;
                }
            }

        }
        req.sendResponse(req, 404, "Not Found", ContentType::TEXT);
    }


}
