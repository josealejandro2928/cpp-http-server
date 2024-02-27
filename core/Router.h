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

    using Middleware = function<ResponseMiddleware *(Request &)>;

    class Router {
    private:
        map <string, vector<Middleware>> GetRoutes;
    public:
        void registerRoute(HttpMethod method, string &&path, const Middleware &middleware);

        void switchRouter(Request &req);

    private:
        static void processCallStacks(string &, Request &, map <string, vector<Middleware>> &);
    };
}


#endif //HTTP_SERVER_ROUTER_H
