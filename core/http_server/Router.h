//
// Created by pepe on 2/27/24.
//

#ifndef HTTP_SERVER_ROUTER_H
#define HTTP_SERVER_ROUTER_H

#include "vector"
#include "string"
#include "map"
#include "Request.h"
#include "functional"

using namespace std;

namespace HttpServer {

    using Middleware = function<void(Request &)>;

    class Router {
    private:
        map <string, vector<Middleware>> GetRoutes;
        map <string, vector<Middleware>> PostRoutes;
        map <string, vector<Middleware>> PatchRoutes;
        map <string, vector<Middleware>> PutRoutes;
        map <string, vector<Middleware>> DeleteRoutes;
        map <string, vector<Middleware>> globalMiddlewares;
    public:
        Router() = default;

        Router(const Router &) = delete;

        Router operator=(const Router &) = delete;

        /**
         * @brief This method is used to register a route with a specific method and a middleware
         * @param method The HTTP method
         * @param path The path of the route
         * @param middleware The middleware to be executed, Function that takes a Request and returns void
         */
        void registerRoute(HttpMethod method, const string &path, const Middleware &);

        /**
          * @brief This method is used to register a route with a specific method and a vector of middlewares
          * @param method The HTTP method
          * @param path The path of the route
          * @param middlewares The middlewares to be executed, Function that takes a Request and returns void
          */
        void registerRoute(HttpMethod method, const string &path, const std::vector<Middleware> &);

        /**
         * @brief This method register global middlewares to be executed before the route handlers can be suitable for authentication or logging
         * @param middleware The middleware to be executed, Function that takes a Request and returns void
         */
        void use(const Middleware &);

        /**
         * @brief This method register global middlewares to be executed to any route that starts with the prefixPath for all verbs, can be suitable for authentication or logging for a specific group of routes
         * @param middlewares The middlewares to be executed, Function that takes a Request and returns void
         */
        void use(const string &prefixPath, const Middleware &);

        void switchRouter(Request &req);

        void getMethod(const string& path, const Middleware &);
        void getMethod(const string& path, const std::vector<Middleware> &);
        void postMethod(const string& path, const Middleware &);
        void postMethod(const string& path, const std::vector<Middleware> &);
        void putMethod(const string& path, const Middleware &);
        void putMethod(const string& path, const std::vector<Middleware> &);
        void patchMethod(const string& path, const Middleware &);
        void patchMethod(const string& path, const std::vector<Middleware> &);
        void deleteMethod(const string& path, const Middleware &);
        void deleteMethod(const string& path, const std::vector<Middleware> &);

    private:
        void processCallStacks(string &, Request &, map <string, vector<Middleware>> &);
    };
}


#endif //HTTP_SERVER_ROUTER_H
