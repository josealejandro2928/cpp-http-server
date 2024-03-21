#include <iostream>
#include "vector"
#include "list"
#include "http_server/concurrency/ThreadPool.h"

using namespace HttpServer;

long sumUpTo(int n) {
    long sum = 0;
    for (int i = 0; i <= n; i++) {
        sum += i;
    }
    return sum;
}

void printPoolState(ThreadPool &pool) {
    std::cout << "Printing the State of the Thread Pool" << std::endl;
    for (const auto &task: pool.getTaskQueueState()) {
        std::cout << *task << std::endl;
    }
}

int main() {
    std::cout << "Creating a task...." << std::endl;
    ThreadPool pool(10);

    pool.submit([&pool]() {
        while (!pool.taskShouldTerminate()) {
            std::cout << "Long task running..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        return 0;
    });
    // Start measuring time
    auto start = std::chrono::steady_clock::now();
    std::vector<std::shared_ptr<TaskThread>> taskFutures;
    for (int i = 0; i < 10; i++) {
        auto t1 = pool.submit(sumUpTo, 1000000000);
        taskFutures.push_back(t1);
    }
    std::cout << "Waiting for all the task result" << std::endl;
//    printPoolState(pool);
    int index = 0;
    for (auto &t: taskFutures) {
        auto res = t->get<long>();
        std::cout << "Result of the task" << index << " Blocking: " << res << std::endl;
        index++;
    }
//    printPoolState(pool);

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "All tasks completed in " << elapsed << " milliseconds." << std::endl;
    pool.submit([]() {
        std::cout << "This is another stuff" << std::endl;
        return 0;
    });

    pool.submit(sumUpTo, 100)->addOnFinishCallback([](TaskThread *t) {
        std::cout << "This is the final " << t->get<long>() << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    return 0;
}
