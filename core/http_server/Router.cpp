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
#include <iostream>
#include <boost/stacktrace.hpp>

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

    void Router::registerRoute(HttpMethod method, const string &path, const std::vector<Middleware> &middlewares) {
        for (const auto &midd: middlewares) {
            switch (method) {
                case HttpMethod::GET:
                    GetRoutes[path].push_back(midd);
                    break;
                case HttpMethod::POST:
                    PostRoutes[path].push_back(midd);
                    break;
                case HttpMethod::DELETE:
                    DeleteRoutes[path].push_back(midd);
                    break;
                case HttpMethod::PATCH:
                    PatchRoutes[path].push_back(midd);
                    break;
                case HttpMethod::PUT:
                    PutRoutes[path].push_back(midd);
                    break;
                default:
                    throw std::invalid_argument("Unknown HTTP VERB");
            }
        }
    }

    void Router::use(const Middleware &middleware) {
        globalMiddlewares["*"].push_back(middleware);
    }

    void Router::use(const string &prefixPath, const Middleware &middleware) {
        globalMiddlewares[prefixPath].push_back(middleware);
    }

    static void printStackTrace(std::exception &e) {
        std::cerr << "Exception: " << e.what() << '\n'
                  << "Stack trace:\n" << boost::stacktrace::stacktrace();
    }

    static void processErrors(Request &req, std::exception &e) {
        if (req.exceptionHandler) {
            req.exceptionHandler(e, req);
            if (!req.hasSendResponseBeenCalled) {
                ErrorResponseData errorResponseData(400,
                                                    "You must call sendResponse in your middleware at some point.");
                req.sendJson<ErrorResponseData>(req, 400, errorResponseData);
            }
            return;
        } else {
            auto *ptr = dynamic_cast<HttpException *>(&e);
            if (ptr) {
                ErrorResponseData errorResponseData(ptr->getCode(), ptr->what());
                req.sendJson<ErrorResponseData>(req, errorResponseData.code, errorResponseData);
                return;
            }
            printStackTrace(e);
            ErrorResponseData errorResponseData(500, "Internal Server Error" + string(e.what()));
            req.sendJson<ErrorResponseData>(req, errorResponseData.code, errorResponseData);
        }
    }

    void Router::switchRouter(Request &req) {
        try {
            string path = req.getPath();
            if (req.getMethod() == "GET") {
                return processCallStacks(path, req, GetRoutes);
            }
            if (req.getMethod() == "POST") {
                return processCallStacks(path, req, PostRoutes);
            }
            if (req.getMethod() == "PUT") {
                return processCallStacks(path, req, PutRoutes);
            }
            if (req.getMethod() == "PATCH") {
                return processCallStacks(path, req, PatchRoutes);
            }
            if (req.getMethod() == "DELETE") {
                return processCallStacks(path, req, DeleteRoutes);
            }
            throw UnprocessableEntityException("Unsupported METHOD at the moment");
        } catch (std::exception &exc) {
            processErrors(req, exc);
        }
    }

    void Router::switchRouter(std::shared_ptr<Request>& req) {
        switchRouter(*req);
    }


    static bool
    processCallbacksSequence(Request &req, vector<Middleware> &middlewares, bool isShouldReturnAtTheEnd = true) {
        for (auto &middleware: middlewares) {
            try {
                middleware(req);
                if (req.hasSendResponseBeenCalled) {
                    return true;
                }
            } catch (std::exception &e) {
                processErrors(req, e);
                return true;
            } catch (...) {
                ErrorResponseData errorResponseData(500, "Internal Server Error");
                req.sendJson<ErrorResponseData>(req, 500, errorResponseData);
                return true;
            }
        }
        if (!req.async && isShouldReturnAtTheEnd && !req.hasSendResponseBeenCalled) {
            ErrorResponseData errorResponseData(400, "You must call sendResponse in your middleware at some point.");
            req.async = true;
            req.sendJson<ErrorResponseData>(req, 400, errorResponseData);
        }
        return false;
    }

    void Router::processCallStacks(string &path, Request &req, map<string, vector<Middleware>> &Routes) {
        // Process first any global middlewares
        if (globalMiddlewares.find("*") != globalMiddlewares.end()) {
            bool res = processCallbacksSequence(req, globalMiddlewares["*"], false);
            if (res) return;
        }
        // Then process the global middlewares for prefixPath, the ones used with use function that takes a prefixPath
        for (auto &[keyPath, middlewares]: globalMiddlewares) {
            if (keyPath == "*") continue;
            if (path.find(keyPath) == 0) {
                bool res = processCallbacksSequence(req, middlewares, false);
                if (res) return;
            }
        }
        // Then Process the normal routes middlewares
        for (auto &[key, middlewaresStack]: Routes) {
            std::string processedPath = key;
            if (processedPath[processedPath.size() - 1] == '/') {
                processedPath = processedPath.substr(0, processedPath.size() - 1);
            }
            if (key == path || processedPath == path) {
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
                if (isMatch) {
                    requestParams = newRequestParams;
                    processCallbacksSequence(req, middlewaresStack);
                    return;
                }
            }

        }
        throw NotFoundException("Route " + req.getFullPath() + " not found!");
    }

    void Router::getMethod(const string &path, const Middleware &middleware) {
        registerRoute(HttpMethod::GET, path, middleware);
    }

    void Router::getMethod(const string &path, const vector<Middleware> &middlewares) {
        registerRoute(HttpMethod::GET, path, middlewares);
    }

    void Router::postMethod(const string &path, const Middleware &middleware) {
        registerRoute(HttpMethod::POST, path, middleware);
    }

    void Router::postMethod(const string &path, const vector<Middleware> &middlewares) {
        registerRoute(HttpMethod::POST, path, middlewares);
    }

    void Router::putMethod(const string &path, const Middleware &middleware) {
        registerRoute(HttpMethod::PUT, path, middleware);
    }

    void Router::putMethod(const string &path, const vector<Middleware> &middlewares) {
        registerRoute(HttpMethod::PUT, path, middlewares);
    }

    void Router::patchMethod(const string &path, const Middleware &middleware) {
        registerRoute(HttpMethod::PATCH, path, middleware);
    }

    void Router::patchMethod(const string &path, const vector<Middleware> &middlewares) {
        registerRoute(HttpMethod::PATCH, path, middlewares);
    }

    void Router::deleteMethod(const string &path, const Middleware &middleware) {
        registerRoute(HttpMethod::DELETE, path, middleware);
    }

    void Router::deleteMethod(const string &path, const vector<Middleware> &middlewares) {
        registerRoute(HttpMethod::DELETE, path, middlewares);
    }


}
