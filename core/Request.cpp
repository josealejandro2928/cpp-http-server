//
// Created by pepe on 2/27/24.
//

#include "Request.h"
#include <iostream>
#include <string>
#include <sstream>
#include "utils/utils.h"
#include <sys/socket.h>
#include <unistd.h>

namespace HttpServer {
    void Request::addHeader(const std::string &key, const std::string &value) {
        headers[key] = value;
    }

    std::map<std::string, std::string> &Request::getHeaders() {
        return headers;
    }

    std::string Request::getHeader(const std::string &key) {
        if (headers.find(key) != headers.end()) {
            return headers[key];
        }
        return "";
    }

    Request Request::makeRequest(std::string &request) {
        Request req;
        std::istringstream requestStream(request);
        std::string line;
        bool headerSection = true;

        while (std::getline(requestStream, line) && line != "\r") {
            if (headerSection) {
                auto colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string headerName = line.substr(0, colonPos);
                    std::string headerValue = line.substr(colonPos + 2,
                                                          line.size() - colonPos - 3); // Adjust for ": " and "\r"
                    req.addHeader(headerName, headerValue);
                } else {
                    auto parsedMethodQueryAndPath = processRequestPathMethodAndQueryParams(line);
                    req.method = parsedMethodQueryAndPath.method;
                    req.path = parsedMethodQueryAndPath.path;
                    req.fullPath = parsedMethodQueryAndPath.fullPath;
                    req.query = parsedMethodQueryAndPath.query;
                }
            } else {
                req.body += line + "\n";
            }
            if (line == "\r") {
                headerSection = false;
            }
        }
        // Add the rest of the request as the body
        std::string bodyLine;
        while (std::getline(requestStream, bodyLine)) {
            req.body += bodyLine + "\n";
        }
        return req;
    }

    std::string Request::getBody() {
        return body;
    }

    std::string Request::getMethod() const { return HttpMethodToString(method); }

    std::string Request::getPath() const { return path; }

    std::map<std::string, std::string> Request::getQuery() const { return query; }

    std::string Request::getFullPath() const { return fullPath; }

    std::ostream &operator<<(std::ostream &os, Request &req) {
        os << "Headers:\n";
        for (auto &[key, value]: req.getHeaders()) {
            os << key << ": " << value << "\n";
        }
        os << "Full Path: " << req.getFullPath() << "\n";
        os << "Method: " << req.getMethod() << "\n";
        os << "Path: " << req.getPath() << "\n";
        os << "Query: " << "\n";
        for (const auto &[key, value]: req.getQuery()) {
            os << key << ": " << value << "\n";
        }
        os << "Body:\n" << req.getBody() << "\n";
        return os;
    }

    void Request::sendResponse(Request &req, int statusCode = 200,
                               const std::string message = "{\"message\": \"Hello from server\"}") {
        std::vector<std::string> parts = {"HTTP/1.1 ", std::to_string(statusCode), " ",
                                          getHttpStatusResponseStrFromStatus(statusCode),
                                          "\r\nContent-Type: application/nlohmann_json\r\n\r\n", message};

        std::string response = strJoin(parts, "");
        hasSendResponseBeenCalled = true;
        send(req.getNewFD(), response.data(), response.length(), 0);
        close(req.getNewFD());
    }
}
