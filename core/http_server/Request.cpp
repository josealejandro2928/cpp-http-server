//
// Created by pepe on 2/27/24.
//

#include "Request.h"
#include <iostream>
#include <string>
#include <sstream>
#include "http_server/utils/utils.h"
#include <sys/socket.h>
#include <unistd.h>
#include "nlohmann/json.hpp"
#include "http_server/exceptions/Exceptions.h"

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

    static void validateRequest(Request &req) {
        static const std::string validMethods[] = {"GET", "POST", "PUT", "DELETE", "PATCH", "OPTIONS", "HEAD"};
        if (std::find(std::begin(validMethods), std::end(validMethods), req.getMethod()) == std::end(validMethods)) {
            throw BadRequestException("Unsupported HTTP method");
        }

        if (req.getPath().empty() || req.getPath().front() != '/') {
            throw BadRequestException("Invalid request path");
        }

        if (req.getMethod() == "POST" || req.getMethod() == "PUT" || req.getMethod() == "PATCH") {
            std::string contentType = req.getHeader("Content-Type");
            std::string contentLength = req.getHeader("Content-Length");

            if (contentType.empty()) {
                throw BadRequestException("Missing Content-Type header");
            }

            if (contentLength.empty() || !std::all_of(contentLength.begin(), contentLength.end(), ::isdigit)) {
                throw BadRequestException("Invalid or missing Content-Length header");
            }
        }
        if(req.getFullPath().empty()) {
            throw BadRequestException("Invalid request path");
        }
        if(req.getHeader("Host").empty()) {
            throw BadRequestException("Missing Host header");
        }
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
                } else if (!line.empty()) {
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
        while (req.body.back() == '\n') {
            req.body.pop_back();
        }
        validateRequest(req);
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
            if (!message.empty()) {
                try {
                    auto json = json::parse(message);
                    response += json.dump();
                } catch (const json::parse_error &e) {
                    response += "{}";
                }
            }
        } else if (contentType == ContentType::TEXT) {
            response += message;
        } else {
            throw HttpException("Unsupported content type: " + contentType);
        }
        hasSendResponseBeenCalled = true;
        for (auto &cb: registerCallbacksOnFinish) {
            cb(statusCode);
        }
        send(req.getNewFD(), response.data(), response.length(), 0);
        close(req.getNewFD());
    }

    void Request::sendText(Request &req, int statusCode, const std::string &data) {
        sendResponse(req, statusCode, data, ContentType::TEXT);
    }

    std::map<std::string, std::string> &Request::getAllRequestParams() {
        return requestParams;
    }

    std::string &Request::getRequestParam(const std::string &key) {
        if (requestParams.find(key) != requestParams.end()) {
            return requestParams[key];
        }
        throw UnprocessableEntityException("Request param not found: " + key);
    }

    void Request::onRequestFinish(const std::function<void(int statusCode)> &cb) {
        registerCallbacksOnFinish.push_back(cb);
    }

    void Request::setRequestAttribute(const std::string &key, const std::any &value) {
        requestAttributes[key] = value;
    }

    std::any &Request::getRequestAttribute(const std::string &key) {
        if (requestAttributes.find(key) != requestAttributes.end()) {
            return requestAttributes[key];
        }
        throw UnprocessableEntityException("Request attribute not found: " + key);
    }
}
