//
// Created by pepe on 3/1/24.
//

#include "Models.h"

#include <utility>


int User::globalUserId = 1;
int Task::globalTaskId = 1;

User::User(std::string name, std::string email, std::string password) : name(std::move(name)), email(std::move(email)),
                                                                        password(std::move(password)) {
    id = globalUserId++;
}

UserResponseDto User::toDto() const {
    return UserResponseDto{id, name, email};
}

Task::Task(std::string title, std::string description, std::string status, User &creator) : title(std::move(title)),
                                                                                            description(std::move(
                                                                                                    description)),
                                                                                            status(std::move(status)),
                                                                                            userId(creator.id),
                                                                                            user(creator) {
    id = globalTaskId++;
}

TaskResponseDto Task::toDto() const {
    return TaskResponseDto(id, title, description, status, userId, user.toDto());
}

