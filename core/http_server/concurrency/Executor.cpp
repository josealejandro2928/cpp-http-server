//
// Created by pepe on 3/24/24.
//

#include "Executor.h"

namespace HttpServer {
    ThreadPoolExecutor::ThreadPoolExecutor(int threads) {
        pool = new ThreadPool(threads);
    }

    ThreadPoolExecutor::ThreadPoolExecutor() {
        pool = new ThreadPool((int) std::thread::hardware_concurrency());
    }

    void ThreadPoolExecutor::shutdown() {
        pool->shutdown();
    }

    void ThreadPoolExecutor::execute(std::function<void()> &&task) {
        pool->submit([task]() mutable {
            task();
            return 0;
        });
    }

    ThreadPoolExecutor::~ThreadPoolExecutor() {
        delete pool;
        pool = nullptr;
    }


    void ThreadPoolExecutor::runSchedule(std::function<void()> &&task, std::chrono::duration<double> delay,
                                         std::chrono::duration<double> initialDelay) {
        pool->submit([this, task = std::forward<std::function<void()>>(task), delay, initialDelay]() {
            std::this_thread::sleep_for(initialDelay);
            while (this->isPoolRunning()) {
                try {
                    task();
                } catch (std::exception &e) {
                    std::cerr << "Error running schedule task: " << e.what() << std::endl;
                }
                std::this_thread::sleep_for(delay);
            }
            return 0;
        });

    }

    bool ThreadPoolExecutor::isPoolRunning() {
        return !pool->taskShouldTerminate();
    }

    std::ostream &operator<<(std::ostream &os, ThreadPoolExecutor &t) {
        auto &threadPool = t.pool;
        os << "ThreadPoolExecutor [Name: " << t.getName() << ", Size: " << threadPool->getCapacity() << ", Threads: "
           << threadPool->getThreadsState().size()
           << ", Tasks: " << threadPool->getTaskQueueState().size() << "]";
        return os;
    }

    void ThreadPoolExecutor::runFunctionNthTimes(std::function<void()> &&task,
                                                 std::chrono::duration<double> delay,
                                                 long numberOfTimes) {
        pool->submit([this, task = std::forward<std::function<void()>>(task), delay, numberOfTimes]() {
            for (int i = 0; i < numberOfTimes && this->isPoolRunning(); i++) {
                std::this_thread::sleep_for(delay);
                try {
                    task();
                } catch (std::exception &e) {
                    std::cerr << "Error running runFunctionNthTimes task: " << e.what() << std::endl;
                }
            }
            return 0;
        });
    }
}
