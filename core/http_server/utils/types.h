//
// Created by pepe on 2/27/24.
//

#ifndef HTTP_SERVER_TYPES_H
#define HTTP_SERVER_TYPES_H

#include "string"
#include "map"

namespace HttpServer {
    enum class HttpMethod {
        GET,
        POST,
        PUT,
        DELETE,
        PATCH,
        OPTIONS,
        HEAD,
        CONNECT,
        TRACE
    };
    enum class HttpStatus {
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NO_CONTENT = 204,
        MOVED_PERMANENTLY = 301,
        FOUND = 302,
        SEE_OTHER = 303,
        NOT_MODIFIED = 304,
        TEMPORARY_REDIRECT = 307,
        PERMANENT_REDIRECT = 308,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        REQUEST_TIMEOUT = 408,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503,
        GATEWAY_TIMEOUT = 504
    };

    struct ContentType {
        static const std::string TEXT;
        static const std::string JSON;
        static const std::string FORM_URL_ENCODED;
        static const std::string HTML;
        static const std::string XML;
        static const std::string FORM;
    };
    struct PathMethodAndQueryParams {
        std::string path;
        std::string fullPath;
        HttpMethod method;
        std::map<std::string, std::string> query;
    };
}
#endif //HTTP_SERVER_TYPES_H
