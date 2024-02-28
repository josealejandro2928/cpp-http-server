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

namespace HttpServer {

    Server::Server(const char *portNum, unsigned int backLog) {
        this->portNum = (char *) portNum;
        this->backLog = backLog;
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
            Request request = Request::makeRequest(requestStr);
            request.setNewFD(newFD);
            router.switchRouter(request);
        } catch (std::exception &e) {
            std::cerr << "Error while processing request: " << e.what() << "\n";
            Request request;
            request.setNewFD(newFD);
            request.sendResponse(request, 500,
                                 "{\"message\": \"Internal server error\",\"error\": \"" + std::string(e.what()) +
                                 "\"}");
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

        std::cout << "Server listening on port " << portNum << std::endl;

        sockaddr_storage client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        std::vector<std::future<void>> futures;

        try {
            while (true) {
                int newFD = accept(sockFD, (sockaddr *) &client_addr, &client_addr_size);
                if (newFD == -1) {
                    std::cerr << "Error while Accepting on socket\n";
                    continue;
                }
                std::cout << "New connection\n";
                cleanFuturesThreadData(futures);
                std::future<void> response = std::async(std::launch::async, &Server::processIncomingConnections, this,
                                                        newFD);
                futures.push_back(std::move(response));
                cleanFuturesThreadData(futures);
            }

        } catch (std::exception &e) {
            std::cerr << "Server crached: " << e.what() << "\n";
            close(sockFD);
            freeaddrinfo(res);
            return 0;
        }
    }


    Router &Server::getRouter() {
        return router;
    }

};
