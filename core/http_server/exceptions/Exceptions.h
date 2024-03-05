//
// Created by pepe on 3/1/24.
//

#ifndef HTTP_SERVER_EXCEPTIONS_H
#define HTTP_SERVER_EXCEPTIONS_H

#include <exception>
#include <string>
#include <utility>
#include <vector>
#include "nlohmann/json.hpp"

namespace HttpServer {
    class HttpException : public std::exception {
    private:
        std::string message;
        int code;

    public:
        explicit HttpException(std::string msg, int errorCode = 500) : message(std::move(msg)), code(errorCode) {}

        [[nodiscard]] const char *what() const noexcept override {
            return message.c_str();
        }

        [[nodiscard]] int getCode() const noexcept {
            return code;
        }
    };

    class BadRequestException : public HttpException {
    public:
        explicit BadRequestException(std::string msg) : HttpException(std::move(msg), 400) {}
    };

    class UnprocessableEntityException : public HttpException {
    public:
        explicit UnprocessableEntityException(std::string msg) : HttpException(std::move(msg), 400) {}
    };

    class NotFoundException : public HttpException {
    public:
        explicit NotFoundException(std::string msg) : HttpException(std::move(msg), 404) {}
    };

    class UnauthorizedException : public HttpException {
    public:
        explicit UnauthorizedException(std::string msg) : HttpException(std::move(msg), 401) {}
    };

    struct ErrorResponseData {
        int code;
        std::string message;
        std::vector<std::string> errors;

        ErrorResponseData(int code, std::string message, std::vector<std::string> errors) : code(code),
                                                                                            message(std::move(message)),
                                                                                            errors(std::move(errors)) {}

        ErrorResponseData(int code, std::string message) : code(code), message(std::move(message)) {}
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ErrorResponseData, code, message, errors)
}


#endif //HTTP_SERVER_EXCEPTIONS_H
