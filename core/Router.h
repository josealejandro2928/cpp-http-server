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
    public:
        Router() = default;
        Router(const Router &) = delete;

        Router operator=(const Router &) = delete;

        void registerRoute(HttpMethod method, const string &path, const Middleware &middleware);

        void switchRouter(Request &req);

    private:
        static void processCallStacks(string &, Request &, map <string, vector<Middleware>> &);
    };
}


#endif //HTTP_SERVER_ROUTER_H
