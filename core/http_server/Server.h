//
// Created by pepe on 2/27/24.
//

#include "http_server/Request.h"
#include "Router.h"

namespace HttpServer {
    class Server {
    private:
        char *portNum;
        unsigned int backLog;
        Router router;

        int start();

        void processIncomingConnections(int);

        std::vector<std::function<void()>> startCallbacks;
    public:
        Server(const char *portNum, unsigned int backLog);
        Server(const char *portNum);

        Router &getRouter();

        void startListening();
        void onServerStart(const std::function<void()>&);

    };
}

