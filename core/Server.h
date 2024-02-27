//
// Created by pepe on 2/27/24.
//

#include "./Request.h"
#include "Router.h"

namespace HttpServer {
    class Server {
    private:
        char *portNum;
        unsigned int backLog;
        Router router;
    public:
        Server(const char *portNum, unsigned int backLog);
        void processIncomingConnections(int);
        int start();
        Router& getRouter();

    };
}

