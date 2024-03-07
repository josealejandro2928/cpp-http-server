//
// Created by pepe on 3/7/24.
//

#include "RequestDtos.h"

void CreateTaskRequest::validate() const {
    if (title.empty() || description.empty() || status.empty()) {
        throw HttpServer::UnprocessableEntityException("Title, description, and status must not be empty.");
    }
    auto end = std::end(possibleStatus);
    if (std::find(std::begin(possibleStatus), end, status) == end) {
        throw HttpServer::UnprocessableEntityException("Status must be one of PENDING, IN_PROGRESS, or DONE.");
    }
}

std::string CreateTaskRequest::possibleStatus[3] = {"PENDING", "IN_PROGRESS", "DONE"};
