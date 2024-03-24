#include <iostream>
#include "vector"
#include "list"
#include "http_server/concurrency/ThreadPool.h"
#include "http_server/concurrency/Executor.h"

using namespace HttpServer;

long sumUpTo(int n) {
    long sum = 0;
    for (int i = 0; i <= n; i++) {
        sum += i;
    }
    return sum;
}

int main() {
    ThreadPoolExecutor pool(10);
    pool.runFunctionNthTimes([]() {
        std::cout << "This is a task that will be executed 1 time after 4 seconds" << std::endl;
    }, std::chrono::seconds(4), 1);

    pool.submit([&pool]() {
        while (pool.isPoolRunning()) {
            std::cout << "Long task running..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        return 0;
    });
    pool.runSchedule([]() {
        std::cout << "This is a scheduled task" << std::endl;
    }, std::chrono::seconds(1), std::chrono::seconds(1));
    // Start measuring time
    auto start = std::chrono::steady_clock::now();
    std::vector<std::future<long>> taskFutures;
    for (int i = 0; i < 10; i++) {
        auto t1 = pool.submit(sumUpTo, 1000000000);
        taskFutures.push_back(std::move(t1));
    }
    std::cout << "Waiting for all the task result" << std::endl;
    int index = 0;
    for (auto &t: taskFutures) {
        auto res = t.get();
        std::cout << "Result of the task" << index << " Blocking: " << res << std::endl;
        index++;
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "All tasks completed in " << elapsed << " milliseconds." << std::endl;
    pool.submit([]() {
        std::cout << "This is another stuff" << std::endl;
        return 0;
    });

    pool.run([]() {
        return sumUpTo(500);
    }, [](auto &f) {
        std::cout << "This is the final " << f.get() << std::endl;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    return 0;
}
