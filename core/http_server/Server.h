//
// Created by pepe on 2/27/24.
//


#include "functional"
#include "Router.h"
#include "concurrency/Executor.h"

namespace HttpServer {
    class Server {
    private:
        char *portNum;
        ThreadPoolExecutor *executor;
        unsigned int backLog;
        Router router;

        int start();

        void processIncomingConnections(int);

        std::vector<std::function<void()>> startCallbacks;
    public:
        std::function<void(std::exception &, Request &)> globalExceptionHandler = nullptr;

        Server(const char *portNum, unsigned int backLog);

        explicit Server(const char *portNum);

        explicit Server(int portNum);

        explicit Server(int portNum, unsigned int backLog);

        Router &getRouter();

        void startListening();

        void onServerStart(const std::function<void()> &);

        void setGlobalExceptionHandler(const std::function<void(std::exception &, Request &)> &fn);
        ~Server();
        static size_t SERVER_THREAD_POOL_SIZE;

    };
}

