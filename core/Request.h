//
// Created by pepe on 2/27/24.
//

#ifndef HTTP_SERVER_REQUEST_H
#define HTTP_SERVER_REQUEST_H

#include <map>
#include <string>
#include "utils/types.h"

namespace HttpServer {
    class Request {
    private:
        std::map<std::string, std::string> headers;
        std::string body;
        HttpMethod method = HttpMethod::GET;
        std::string path;
        std::map<std::string, std::string> query;
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

        void sendResponse(Request &, int, std::string);

        bool hasSendResponseBeenCalled = false;
    };
}


#endif //HTTP_SERVER_REQUEST_H
