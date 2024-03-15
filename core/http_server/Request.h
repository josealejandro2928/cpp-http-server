//
// Created by pepe on 2/27/24.
//

#ifndef HTTP_SERVER_REQUEST_H
#define HTTP_SERVER_REQUEST_H

#include <map>
#include <string>
#include "http_server/utils/types.h"
#include "http_server/utils/utils.h"
#include "http_server/exceptions/Exceptions.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
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
        std::vector<std::function<void(int statusCode)>> registerCallbacksOnFinish;
        std::map<std::string, std::any> requestAttributes;
        int newFD = 0;
        std::string port;
    public:
        std::function<void(std::exception &, Request &)> exceptionHandler = nullptr;

        Request() = default;

        void addHeader(const std::string &key, const std::string &value);

        std::string getHeader(const std::string &key);

        std::map<std::string, std::string> &getHeaders();

        static Request makeRequest(std::string &request);

        std::string getBody();

        friend std::ostream &operator<<(std::ostream &os, Request &req);

        [[nodiscard]] std::string getMethod() const;

        [[nodiscard]] std::string getPath() const;

        [[nodiscard]] std::map<std::string, std::string> getQuery() const;

        [[nodiscard]] std::string getFullPath() const;

        [[nodiscard]] int getNewFD() const { return newFD; }

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

        void sendJson(Request &req, int statusCode) {
            sendResponse(req, statusCode, "", ContentType::JSON);
        }

        void sendText(Request &, int, const std::string &);

        std::map<std::string, std::string> &getAllRequestParams();

        std::string &getRequestParam(const std::string &key);

        bool hasSendResponseBeenCalled = false;

        void onRequestFinish(const std::function<void(int statusCode)> &cb);

        void setRequestAttribute(const std::string &key, const std::any &value);

        std::any &getRequestAttribute(const std::string &key);

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
                return json::parse(bodyStr).get<T>();
            } else if (contentType == ContentType::FORM_URL_ENCODED) {
                auto map_body = processURLEncodedFormBody(bodyStr);
                json data;
                for (auto &[key, value]: map_body) {
                    if (value.size() > 1) {
                        data[key] = value;
                    } else {
                        data[key] = value[0];
                    }
                }
                return data.get<T>();
            } else if (contentType.find(ContentType::FORM) == 0) {
                throw std::runtime_error(
                        "Unsupported content type for the moment");
//                auto parts = strSplit(contentType, "; boundary=");
//                if (parts.size() != 2) { throw BadRequestException("Invalid form multipart body encoded"); }
//                parseMultipartFormData(bodyStr, parts.back());
//                return T();
            } else {
                throw std::runtime_error(
                        " Unsupported content type: " + contentType + " for extraction of object from body.");
            }
        }

        inline std::string &getPort() {
            return this->port;
        }

        inline void setPort(int p) {
            this->port = std::to_string(p);
        }

        inline void setPort(char *p) {
            this->port = std::string(p);
        }
    };
}


#endif //HTTP_SERVER_REQUEST_H
