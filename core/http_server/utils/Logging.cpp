//
// Created by pepe on 3/5/24.
//
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <thread>
#include "Logging.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace HttpServer {

    std::string getHostname() {
        std::array<char, HOST_NAME_MAX> hostname;
        hostname.fill(0);
        gethostname(hostname.data(), hostname.size());
        return hostname.data();
    }

    std::string getHostIPAddress(const std::string &hostname) {
        struct addrinfo hints, *info;
        std::string hostIP;

        std::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET; // For IPv4
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_CANONNAME;

        if (getaddrinfo(hostname.c_str(), nullptr, &hints, &info) == 0) {
            for (struct addrinfo *p = info; p != nullptr; p = p->ai_next) {
                char ip[INET_ADDRSTRLEN];
                if (inet_ntop(AF_INET, &((struct sockaddr_in *) p->ai_addr)->sin_addr, ip, sizeof(ip))) {
                    hostIP = ip;
                    break; // Take the first IP
                }
            }
            freeaddrinfo(info);
        }
        return hostIP;
    }

    std::mutex Logging::logMutex = {};

    void Logging::log(const Level level, const char *msg) {
        std::lock_guard<std::mutex> lock(logMutex);
        if (!Logging::enable) return;
        std::string levelStr;
        switch (level) {
            case Level::DEBUG:
                levelStr = "DEBUG";
                break;
            case Level::INFO:
                levelStr = "INFO";
                break;
            case Level::WARN:
                levelStr = "WARN";
                break;
            case Level::ERROR:
                levelStr = "ERROR";
                break;
        }
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "[%Y-%m-%d %X]");
        ss << " [" << getpid() << "]";
        ss << " " << levelStr << " - " << msg;
        std::cout << ss.str() << std::endl;

    }

    void Logging::httpRequestLog(Request &request) {
        if (!Logging::enableLogHttpRequests) return;
        auto start = std::chrono::system_clock::now();
        request.onRequestFinish([&request, start](int statusCode) {
            std::stringstream ss;
            ss << "Request: " << "[" << request.getMethod() << "] " << Logging::hostIP
               << request.getFullPath();
            auto end = std::chrono::system_clock::now();
            auto elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            ss << " - " << statusCode << " " << elapsed_seconds << "ms";
            log(Logging::logLevelForHttpRequest, ss.str().c_str());
        });
    }

    void Logging::info(const char *msg) {
        Logging::log(Level::INFO, msg);
    }

    void Logging::debug(const char *msg) {
        Logging::log(Level::DEBUG, msg);
    }

    void Logging::warn(const char *msg) {
        Logging::log(Level::WARN, msg);
    }

    void Logging::error(const char *msg) {
        Logging::log(Level::ERROR, msg);
    }

    bool Logging::enable = true;
    Logging::Level Logging::logLevelForHttpRequest = Level::INFO;
    bool Logging::enableLogHttpRequests = true;
    std::string Logging::hostname = getHostname();
    std::string Logging::hostIP = getHostIPAddress(getHostname());
}
