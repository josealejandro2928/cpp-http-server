//
// Created by pepe on 2/27/24.
//
#include "Server.h"
#include <cstring>
#include <iostream>
#include <string>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <future>
#include "http_server/utils/Logging.h"

namespace HttpServer {

    Server::Server(const char *portNum, unsigned int backLog) {
        this->portNum = (char *) portNum;
        this->backLog = backLog;
    }

    Server::Server(const char *portNum) {
        this->portNum = (char *) portNum;
        this->backLog = SOMAXCONN;
    }

    Server::Server(const int portNum) {
        auto port = std::to_string(portNum);
        this->portNum = new char[port.size() + 1];
        memcpy(this->portNum, port.c_str(), port.size() + 1);
        this->backLog = SOMAXCONN;
    }

    static void defaultServerErrorHandler(const std::exception &e, int newFD) {
        std::cerr << "Error while processing request: " << e.what() << "\n";
        Request request;
        request.setNewFD(newFD);
        request.sendResponse(request, 500, "Internal server error: " + std::string(e.what()), ContentType::TEXT);
    }

    static ssize_t getContentLengthFromHeaders(std::string &data) {
        const std::string contentLength = "Content-Length: ";
        const std::string contentLength_2 = "content-length: ";
        size_t pos = std::string::npos;
        pos = data.find(contentLength);
        if (pos == std::string::npos) {
            pos = data.find(contentLength_2);
            if (pos == std::string::npos) return 0;
        }
        std::string sizeBytes;
        int index = pos + contentLength.size();
        while (data[index] != '\r' && data[index] != '\n') {
            if (data[index]) {
                sizeBytes += data[index];
            }
            index++;
        }
        return std::stoi(sizeBytes);
    }

    void Server::processIncomingConnections(int newFD) {
        try {
            char buffer[4096];
            ssize_t bytesRead = recv(newFD, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead < 0) {
                std::cerr << "Error reading from socket\n";
                close(newFD);
                return;
            }
            buffer[bytesRead] = '\0';
            std::string requestStr(buffer);
            ssize_t contentLength = getContentLengthFromHeaders(requestStr);
            ssize_t totalBytesRead = bytesRead;

            while (totalBytesRead < contentLength && bytesRead > 0) {
                bytesRead = recv(newFD, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    requestStr.append(buffer);
                    totalBytesRead += bytesRead;
                }
            }
            Request request = Request::makeRequest(requestStr);
            if (request.hasSendResponseBeenCalled) return;
            request.setPort(this->portNum);
            request.exceptionHandler = this->globalExceptionHandler;
            request.setNewFD(newFD);
            router.switchRouter(request);
        } catch (std::exception &e) {
            defaultServerErrorHandler(e, newFD);
        }
    }

    static void cleanFuturesThreadData(std::vector<std::future<void>> &futures) {
        for (int i = 0; i < futures.size(); i++) {
            if (futures[i].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                futures.erase(futures.begin() + i);
            }
        }
    }

    int Server::start() {
        addrinfo hints{}, *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        int gAddRes = getaddrinfo(nullptr, portNum, &hints, &res);
        if (gAddRes != 0) {
            std::cerr << gai_strerror(gAddRes) << "\n";
            return -2;
        }

        int sockFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockFD == -1) {
            std::cerr << "Error while creating socket\n" << strerror(errno) << "\n";
            freeaddrinfo(res);
            return -4;
        }
        int yes = 1;
        if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            std::cerr << "Error setting socket options\n";
            close(sockFD);
            return -1; // Or handle the error as per your error handling scheme
        }

        int bindR = bind(sockFD, res->ai_addr, res->ai_addrlen);
        if (bindR == -1) {
            std::cerr << "Error while binding socket\n" << strerror(errno) << "\n";
            close(sockFD);
            freeaddrinfo(res);
            return -5;
        }

        int listenR = listen(sockFD, (int) backLog);
        if (listenR == -1) {
            std::cerr << "Error while Listening on socket\n" << strerror(errno) << "\n";
            close(sockFD);
            freeaddrinfo(res);
            return -6;
        }
        std::string message = "Server listening on port " + string(portNum);
        Logging::debug(message.c_str());

        sockaddr_storage client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        std::vector<std::future<void>> futures;

        // Running the onStart Callbacks
        for (auto &cb: startCallbacks) {
            cb();
        }

        try {
            while (true) {
                int newFD = accept(sockFD, (sockaddr *) &client_addr, &client_addr_size);
                if (newFD == -1) {
                    std::cerr << "Error while Accepting on socket\n";
                    continue;
                }
                cleanFuturesThreadData(futures);
                std::future<void> response = std::async(std::launch::async, &Server::processIncomingConnections, this,
                                                        newFD);
                futures.push_back(std::move(response));
                cleanFuturesThreadData(futures);
            }

        } catch (std::exception &e) {
            std::cerr << "Server crashed: " << e.what() << "\n";
            close(sockFD);
            freeaddrinfo(res);
            return 0;
        }
    }


    Router &Server::getRouter() {
        return router;
    }

    void Server::startListening() {
        if (start() < 0) {
            std::cerr << "Error while starting server\n";
        }
    }

    void Server::onServerStart(const std::function<void()> &cb) {
        startCallbacks.push_back(cb);
    }

    void Server::setGlobalExceptionHandler(const function<void(std::exception &, Request &)> &fn) {
        this->globalExceptionHandler = fn;
    }

};
