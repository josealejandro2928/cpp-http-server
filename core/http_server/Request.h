//
// Created by pepe on 2/27/24.
//

#ifndef HTTP_SERVER_REQUEST_H
#define HTTP_SERVER_REQUEST_H

#include <map>
#include <string>
#include "http_server/utils/types.h"
#include "nlohmann/json.hpp"

namespace HttpServer {
    class Request {
    private:
        std::map<std::string, std::string> headers;
        std::string body;
        HttpMethod method = HttpMethod::GET;
        std::string path;
        std::map<std::string, std::string> query;
        std::map<std::string, std::string> requestParams;
        std::string fullPath;
        int newFD;

    public:
        Request() = default;

        void addHeader(const std::string &key, const std::string &value);

        std::string getHeader(const std::string &key);

        std::map<std::string, std::string> &getHeaders();

        static Request makeRequest(std::string &request);

        std::string getBody();

        friend std::ostream &operator<<(std::ostream &os, Request &req);

        std::string getMethod() const;

        std::string getPath() const;

        std::map<std::string, std::string> getQuery() const;

        std::string getFullPath() const;

        int getNewFD() const { return newFD; }

        void setNewFD(int _newFD) { newFD = _newFD; }

        void sendResponse(Request &, int, const std::string &, const std::string &);

        template<typename T>
        void sendJson(Request &req, int statusCode, const T &data) {
            nlohmann::json j = data;
            sendResponse(req, statusCode, j.dump(), ContentType::JSON);
        }

        void sendJson(Request &req, int statusCode, const nlohmann::json &data) {
            sendResponse(req, statusCode, data.dump(), ContentType::JSON);
        }

        void sendText(Request &, int, const std::string &);

        template<typename T>
        T getBodyObject() {
            std::string bodyStr = getBody();
            std::string contentType;
            if (!getHeader("Content-Type").empty()) {
                contentType = getHeader("Content-Type");
            } else if (!getHeader("content-type").empty()) {
                contentType = getHeader("content-type");
            } else {
                contentType = ContentType::TEXT;
            }
            if (contentType == ContentType::JSON) {
                return nlohmann::json::parse(bodyStr).get<T>();
            } else {
                throw std::runtime_error(
                        "Unsupported content type: " + contentType + " for extraction of object from body.");
            }
        }

        std::map<std::string, std::string> &getAllRequestParams();
        std::string& getRequestParam(const std::string &key);

        bool hasSendResponseBeenCalled = false;
    };
}


#endif //HTTP_SERVER_REQUEST_H
