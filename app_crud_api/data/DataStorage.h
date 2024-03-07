//
// Created by pepe on 3/7/24.
//

#ifndef HTTP_SERVER_DATASTORAGE_H
#define HTTP_SERVER_DATASTORAGE_H


#include "models/Models.h"
#include "filesystem"

namespace fs = std::filesystem;

class DataStorage {
private:
    static DataStorage *instance;
    std::vector<User> users;
    std::vector<Task> tasks;
    fs::path dataPath;

    DataStorage() = default;

    mutable std::mutex usersMutex;
    mutable std::mutex tasksMutex;
    static std::mutex instanceMutex;

public:
    DataStorage(const DataStorage &) = delete;

    ~DataStorage();

    DataStorage &operator=(const DataStorage &) = delete;


    std::vector<User> getUsers();

    std::vector<Task> getTasks();

    void addUser(User &user, int index = -1);

    void addTask(Task &task, int index = -1);

    void deleteTask(int index);

    void deleteUser(int index);

    static DataStorage &getInstance(const fs::path &path);

    void syncUsers();

    void syncTasks();

    void readUsersFromFile();

    void readTasksFromFile();

};


#endif //HTTP_SERVER_DATASTORAGE_H
