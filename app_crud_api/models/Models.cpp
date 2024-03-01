//
// Created by pepe on 3/1/24.
//

#include "Models.h"

#include <utility>

int globalUserId = 1;
int globalTaskId = 1;

User::User(std::string name, std::string email, std::string password) : name(std::move(name)), email(std::move(email)),
                                                                        password(std::move(password)) {
    id = globalUserId++;
}

Task::Task(std::string title, std::string description, TaskStatus status, User &creator) : title(std::move(title)),
                                                                                           description(std::move(
                                                                                                   description)),
                                                                                           status(status),
                                                                                           creator(creator) {
    id = globalTaskId++;
}
