//
// Created by pepe on 3/1/24.
//
#include "string"
#include "types.h"

namespace HttpServer {
    const std::string ContentType::TEXT = "text/plain";
    const std::string ContentType::JSON = "application/json";
    const std::string ContentType::FORM_URL_ENCODED = "application/x-www-form-urlencoded";
    const std::string ContentType::HTML = "text/html";
    const std::string ContentType::XML = "application/xml";
    const std::string ContentType::FORM = "multipart/form-data";
}
