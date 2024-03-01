//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_EXCEPTIONS_H
#define HTTP_SERVER_EXCEPTIONS_H

#include <exception>
#include <string>
#include <utility>

namespace HttpServer {
    class HttpException : public std::exception {
    private:
        std::string message;
        int code;

    public:
        HttpException(std::string msg, int errorCode = 500) : message(std::move(msg)), code(errorCode) {}

        const char *what() const noexcept override {
            return message.c_str();
        }

        int getCode() const noexcept {
            return code;
        }
    };

    class BadRequestException : public HttpException {
    public:
        BadRequestException(std::string msg) : HttpException(std::move(msg), 400) {}
    };

    class UnprocessableEntityException : public HttpException {
    public:
        UnprocessableEntityException(std::string msg) : HttpException(std::move(msg), 400) {}
    };

    class NotFoundException : public HttpException {
    public:
        NotFoundException(std::string msg) : HttpException(std::move(msg), 404) {}
    };

}


#endif //HTTP_SERVER_EXCEPTIONS_H
