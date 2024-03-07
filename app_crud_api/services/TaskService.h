//
// Created by pepe on 3/6/24.
//

#ifndef HTTP_SERVER_TASKSERVICE_H
#define HTTP_SERVER_TASKSERVICE_H


#include "dto/RequestDtos.h"

class TaskService {
public:
    static Task createTask(CreateTaskRequest &, User &);

    static std::vector<Task> getTasks(User &, RequestFilterTask &);

    static void getTask();

    static void updateTask();

    static void deleteTask();

};


#endif //HTTP_SERVER_TASKSERVICE_H
