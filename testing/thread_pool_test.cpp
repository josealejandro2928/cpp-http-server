//
// Created by pepe on 3/14/24.
//

#include <gtest/gtest.h>
#include "http_server/concurrency/ThreadPool.h"

using namespace HttpServer;

int mockTask() {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 0;
}

TEST(ThreadPoolTest, ThreadShouldBeCreatedSuccessfully) {
    ThreadPool pool(2);

    ASSERT_EQ(pool.getCapacity(), 2);
    ASSERT_EQ(pool.getSize(), 0);

    // Submit a task
    auto t = pool.submit(mockTask);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(pool.getSize() == 1 || t->getStatus() == TaskThread::TaskThreadStatus::SUCCESS);

    pool.shutdown();
}

TEST(ThreadPoolTest, ShouldExecuteMultipleTasksSuccessfully) {
    ThreadPool pool(4);
    std::vector<std::shared_ptr<TaskThread>> tasks;
    for (int i = 0; i < 10; ++i) {
        tasks.push_back(pool.submit([]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            return 42;
        }));
    }
    for (auto &task: tasks) {
        EXPECT_EQ(task->get<int>(), 42);
    }

    pool.shutdown();
}

TEST(ThreadPoolTest, ShouldShutdownGracefullyWithPendingTasks) {
    ThreadPool pool(2);
    for (int i = 0; i < 100; ++i) {
        pool.submit([]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return 0;
        });
    }
    pool.shutdown();
    SUCCEED();
}

TEST(ThreadPoolTest, ShouldReuseThreadsForTasks) {
    ThreadPool pool(2);
    std::atomic<int> counter{0};
    std::set<std::thread::id> threadIds;
    std::mutex mx;
    std::vector<std::shared_ptr<TaskThread>> futureTasks;

    for (int i = 0; i < 10; ++i) {
        futureTasks.push_back(pool.submit([&counter, &threadIds, &mx]() {
            ++counter;
            {
                std::lock_guard<std::mutex> lock(mx);
                threadIds.insert(std::this_thread::get_id());
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return 0;
        }));
    }
    for (const auto &task: futureTasks) {
        task->get<int>();
    }
    pool.shutdown();

    EXPECT_EQ(counter, 10);
    EXPECT_LE(threadIds.size(), 2);
}

TEST(ThreadPoolTest, ShouldHandleTaskExceptions) {
    ThreadPool pool(2);

    auto task = pool.submit([]() -> int {
        throw std::runtime_error("Task failure");
        return 0;
    });

    EXPECT_THROW({
                     try {
                         task->get<int>();
                     } catch (const std::runtime_error &e) {
                         EXPECT_STREQ("Task failure", e.what());
                         throw;
                     }
                 }, std::runtime_error);

    pool.shutdown();
}

TEST(ThreadPoolTest, TasksShouldExecuteInOrder) {
    ThreadPool pool(1);
    const int numTasks = 10;
    std::vector<int> executionOrder;
    std::vector<int> expectedOrder;
    std::mutex orderMutex;
    std::vector<std::shared_ptr<TaskThread>> futureTasks;

    // Submit tasks
    for (int i = 0; i < numTasks; ++i) {
        expectedOrder.push_back(i);
        futureTasks.push_back(pool.submit([&executionOrder, &orderMutex, i]() {
            std::lock_guard<std::mutex> lock(orderMutex);
            executionOrder.push_back(i);
            return 0;
        }));
    }
    for (const auto &t: futureTasks) {
        t->get<int>();
    }
    pool.shutdown();
    // Verify the execution order matches the submission order
    EXPECT_EQ(executionOrder, expectedOrder);
}

TEST(ThreadPoolTest, ParallelHeavyTasksWithSharedState) {
    std::mutex counterMutex;
    std::atomic<int> sharedCounter(0);
    ThreadPool pool(5);
    const int numberOfTasks = 10;
    const auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<std::shared_ptr<TaskThread>> futureTasks;

    for (int i = 0; i < numberOfTasks; ++i) {
        futureTasks.push_back(pool.submit([&sharedCounter, &counterMutex] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            {
                std::lock_guard<std::mutex> lock(counterMutex);
                ++sharedCounter;
            }
            return 0;
        }));
    }
    for (const auto &t: futureTasks) {
        t->get<int>();
    }
    pool.shutdown();

    const auto endTime = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    EXPECT_EQ(sharedCounter.load(), numberOfTasks);

    const int expectedMaxDuration = 100 * numberOfTasks / 2;
    EXPECT_LT(duration.count(), expectedMaxDuration) << "Tasks did not execute efficiently in parallel.";
}

TEST(ThreadPoolTest, ParallelHeavyTasksWithSharedStateObjects) {
    std::mutex mx;
    struct ComputableElements {
        int result = 0;
        std::vector<int> data;
        int index = 0;
    };

    const int sizeTask = 10;

    ThreadPool pool(4);
    std::vector<std::shared_ptr<TaskThread>> futureTasks;
    std::vector<ComputableElements> result;
    for (int i = 0; i < sizeTask; i++) {
        result.push_back({0, {}, i});
    }

    for (int i = 0; i < sizeTask; ++i) {
        futureTasks.push_back(pool.submit([&mx, &result, i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            {
                std::lock_guard<std::mutex> lock(mx);
                for (int j = 0; j < result.size(); j++) {
                    auto el = j + (int) result.size() * i;
                    result[i].data.push_back(el);
                    result[i].result += el;
                }
            }
            return 0;
        }));
    }
    for (const auto &t: futureTasks) {
        t->get<int>();
    }
    pool.shutdown();
    int index = 0;
    for (auto &res: result) {
        EXPECT_EQ(res.index, index);
        std::vector<int> tempResult;
        int sum = 0;
        for (int j = 0; j < result.size(); j++) {
            auto el = j + (int) result.size() * index;
            tempResult.push_back(el);
            sum += el;
        }
        EXPECT_EQ(res.result, sum);
        EXPECT_EQ(res.data, tempResult);
        index++;
    }

}

int mockedFn1(int index) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return index * index;
}

int mockedFn2(int index) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return index * index * index;
}

TEST(ThreadPoolTest, TestNonBlockingStateOfTask) {
    ThreadPool pool(4);
    int n = 5;
    std::atomic<bool> executedFlag = false;

    auto taskFuture = pool.submit([n] { return n * n; });

    taskFuture->addOnFinishCallback([&executedFlag](TaskThread* task) {
        executedFlag = true;
        auto res = task->get<int>();
        EXPECT_EQ(res, 25);
    });

    int res = taskFuture->get<int>();
    ASSERT_TRUE(executedFlag);
    EXPECT_EQ(res, 25);

    pool.shutdown();
}
