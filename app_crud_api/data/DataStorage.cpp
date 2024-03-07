//
// Created by pepe on 3/7/24.
//

#include "DataStorage.h"
#include <fstream>
#include "http_server/utils/Logging.h"
#include "nlohmann/json.hpp"

namespace hs = HttpServer;
using json = nlohmann::json;

std::vector<User> DataStorage::getUsers() {
    std::lock_guard<std::mutex> guard(usersMutex);
    return users;
}

std::vector<Task> DataStorage::getTasks() {
    std::lock_guard<std::mutex> guard(tasksMutex);
    return tasks;
}

void DataStorage::addUser(User &user, int index) {
    std::lock_guard<std::mutex> guard(usersMutex);
    if (index == -1) {
        users.push_back(user);
    } else {
        if (index >= users.size()) {
            throw std::invalid_argument("Index out of bounds");
        }
        users[index] = user;
    }
    syncUsers();

}

void DataStorage::addTask(Task &task, int index) {
    std::lock_guard<std::mutex> guard(tasksMutex);
    if (index == -1) {
        tasks.push_back(task);
    } else {
        if (index >= tasks.size()) {
            throw std::invalid_argument("Index out of bounds");
        }
        tasks[index] = task;
    }
    syncTasks();

}

DataStorage &DataStorage::getInstance(const fs::path &path = "") {
    std::lock_guard<std::mutex> guard(instanceMutex);
    if (instance == nullptr) {
        hs::Logging::info("Creating new instance of DataStorage");
        instance = new DataStorage();
        instance->dataPath = path;
        instance->readUsersFromFile();
        instance->readTasksFromFile();
    }
    return *instance;
}

void DataStorage::syncUsers() {
    auto userPath = dataPath / "users.json";
    std::ofstream file(userPath);
    nlohmann::json j = users;
    file << j;
}

void DataStorage::syncTasks() {
    auto taskPath = dataPath / "tasks.json";
    std::ofstream file(taskPath);
    nlohmann::json j = tasks;
    file << j;
}


void DataStorage::readUsersFromFile() {
    auto userPath = dataPath / "users.json";
    hs::Logging::info(("Reading users from file: " + userPath.string()).c_str());
    std::ifstream file(userPath);
    if (!file) {
        hs::Logging::warn(("The file does not exist: " + userPath.string()).c_str());
        return;
    }
    auto jsonData = json::parse(file);
    for (const auto &userJson: jsonData) {
        User user = userJson.get<User>();
        users.push_back(user);
    }
    std::sort(users.begin(), users.end(), [&](auto &a, auto &b) {
        return a.id < b.id;
    });
    User::globalUserId = users.empty() ? 1 : users.back().id + 1;
}

void DataStorage::readTasksFromFile() {
    auto taskPath = dataPath / "tasks.json";
    hs::Logging::info(("Reading tasks from file: " + taskPath.string()).c_str());
    std::ifstream file(taskPath);
    if (!file) {
        hs::Logging::warn(("The file does not exist: " + taskPath.string()).c_str());
        return;
    }
    auto jsonData = json::parse(file);
    for (const auto &taskJson: jsonData) {
        Task task = taskJson.get<Task>();
        tasks.push_back(task);
    }
    std::sort(tasks.begin(), tasks.end(), [&](auto &a, auto &b) {
        return a.id < b.id;
    });
    Task::globalTaskId = tasks.empty() ? 1 : tasks.back().id + 1;
}

DataStorage *DataStorage::instance = nullptr;
std::mutex DataStorage::instanceMutex = std::mutex();

void DataStorage::deleteTask(int index) {
    std::lock_guard<std::mutex> guard(tasksMutex);
    if (index >= tasks.size()) {
        throw std::invalid_argument("Index out of bounds");
    }
    tasks.erase(tasks.begin() + index);
    syncTasks();
}

void DataStorage::deleteUser(int index) {
    std::lock_guard<std::mutex> guard(usersMutex);
    if (index >= users.size()) {
        throw std::invalid_argument("Index out of bounds");
    }
    users.erase(users.begin() + index);
    syncUsers();
}

DataStorage::~DataStorage() {
    syncUsers();
    syncTasks();
    delete instance;
}

