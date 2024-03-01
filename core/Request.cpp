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
#include "nlohmann/json.hpp"
#include "exceptions/Exceptions.h"

using json = nlohmann::json;

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

    void Request::sendResponse(Request &req, int statusCode, const std::string &message,
                               const std::string &contentType = ContentType::TEXT) {
        std::string response =
                "HTTP/1.1 " + std::to_string(statusCode) + " " + getHttpStatusResponseStrFromStatus(statusCode) +
                "\r\n";

        response += "Content-Type: " + contentType + "\r\n\r\n";

        if (contentType == ContentType::JSON) {
            try {
                auto json = json::parse(message);
                response += json.dump();
            } catch (const json::parse_error &e) {
                response += "{}";
            }
        } else if (contentType == ContentType::TEXT) {
            response += message;
        } else {
            throw HttpException("Unsupported content type: " + contentType);
        }
        hasSendResponseBeenCalled = true;
        send(req.getNewFD(), response.data(), response.length(), 0);
        close(req.getNewFD());
    }

    void Request::sendText(Request &req, int statusCode, const std::string &data) {
        sendResponse(req, statusCode, data, ContentType::TEXT);
    }
}
