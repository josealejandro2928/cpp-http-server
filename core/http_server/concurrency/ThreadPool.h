//
// Created by pepe on 3/10/24.
//

#ifndef HTTP_SERVER_THREADPOOL_H
#define HTTP_SERVER_THREADPOOL_H

#include "string"
#include "vector"
#include "thread"
#include "queue"
#include "mutex"
#include <condition_variable>
#include "functional"
#include "tuple"
#include "any"
#include "memory"

namespace HttpServer {
    class TaskThread {
    private:
        enum class TaskThreadStatus {
            SUCCESS, IN_PROGRESS, FAILED, CANCELLED
        };
        static int GlobalTaskId;
        std::mutex mx;
        int taskId;
        std::condition_variable condition;
        TaskThreadStatus status;
        int threadId;
        std::function<std::any()> functionCb = nullptr;
        std::any result;
        std::exception err;
        std::function<void(TaskThread *)> onFinishCb = nullptr;

    public:
        template<typename Func, typename... Args>
        TaskThread(int threadId, Func &&func, Args &&... args) {
            functionCb = [func = std::forward<Func>(func), args = std::make_tuple(
                    std::forward<Args>(args)...)]() mutable -> std::any {
                return std::apply(func, args);
            };
            this->threadId = threadId;
            this->taskId = GlobalTaskId++;
            this->status = TaskThreadStatus::IN_PROGRESS;
        }

        TaskThread(TaskThread &&other) = delete;
        TaskThread &operator=(TaskThread &&other) = delete;
        TaskThread(TaskThread &) = delete;
        TaskThread &operator=(TaskThread &) = delete;

        void invokeFunction();

        template<typename T>
        T get() {
            std::unique_lock<std::mutex> lock(mx);
            condition.wait(lock, [this]() {
                return status != TaskThreadStatus::IN_PROGRESS;
            });
            if (status != TaskThreadStatus::IN_PROGRESS) {
                if (status == TaskThreadStatus::SUCCESS) return std::any_cast<T>(result);
                if (status == TaskThreadStatus::FAILED) throw err;
                if (status == TaskThreadStatus::CANCELLED) throw std::runtime_error("Task Has been cancelled");
            }
            return {};
        }

        void addOnFinishCallback(const std::function<void(TaskThread *)> &fn);

        friend std::ostream& operator <<(std::ostream& os, TaskThread& t);
    };

    class ThreadPool {
    public:
        ThreadPool(int size, const std::string &name);
        explicit ThreadPool(int size);

        ThreadPool(ThreadPool &&other) = delete;
        ThreadPool &operator=(ThreadPool &&other) = delete;
        ThreadPool &operator=(ThreadPool &) = delete;

        ThreadPool(ThreadPool &) = delete;
        ~ThreadPool();
        template<typename Func, typename... Args>
        std::shared_ptr<TaskThread> submit(const Func &func, const Args &... args) {
            std::unique_lock<std::mutex> lock(mx);
            auto taskPtr = std::shared_ptr<TaskThread>(new TaskThread(threadId, func, args...));
            createThread();
            taskQueue.push_back(taskPtr);
            cv.notify_all();
            lock.unlock();
            return taskPtr;
        }
        void shutdown();
        [[nodiscard]] const std::deque<std::shared_ptr<TaskThread>>& getTaskQueueState()const;
        [[nodiscard]] const std::vector<std::thread>& getThreadsState() const;

    private:
        std::shared_ptr<TaskThread> popTask();

        void createThread();

        void runWorker();


    private:
        int size;
        std::condition_variable cv;
        std::string name;
        static int GlobalThreadId;
        std::vector<std::thread> threadsPool;
        int threadId;
        std::mutex mx;
        std::deque<std::shared_ptr<TaskThread>> taskQueue;
        std::atomic<bool> shutdownFlag = false;
    };


}


#endif //HTTP_SERVER_THREADPOOL_H
