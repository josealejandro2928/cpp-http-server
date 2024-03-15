//
// Created by pepe on 3/10/24.
//

#include <algorithm>
#include "ThreadPool.h"
#include "ranges"

namespace HttpServer {
    int ThreadPool::GlobalThreadId = 1;
    int TaskThread::GlobalTaskId = 1;


    void TaskThread::invokeFunction() {
        std::unique_lock<std::mutex> lock(mx);
        if (status != TaskThreadStatus::IN_PROGRESS) return;
        try {
            result = functionCb();
            status = TaskThreadStatus::SUCCESS;
            condition.notify_all();
            lock.unlock();
            if (onFinishCb) {
                onFinishCb(this);
            }
        } catch (...) {
            err = std::current_exception();
            status = TaskThreadStatus::FAILED;
            condition.notify_all();
            lock.unlock();
            if (onFinishCb) {
                onFinishCb(this);
            }
        }

    }

    TaskThread *TaskThread::addOnFinishCallback(const std::function<void(TaskThread *)> &fn) {
        onFinishCb = fn;
        if (status != TaskThreadStatus::IN_PROGRESS) {
            onFinishCb(this);
        }
        return this;
    }

    std::ostream &operator<<(std::ostream &os, TaskThread &t) {
        std::string statusString;
        switch (t.status) {
            case TaskThread::TaskThreadStatus::SUCCESS:
                statusString = "SUCCESS";
                break;
            case TaskThread::TaskThreadStatus::IN_PROGRESS:
                statusString = "IN_PROGRESS";
                break;
            case TaskThread::TaskThreadStatus::FAILED:
                statusString = "FAILED";
                break;
            case TaskThread::TaskThreadStatus::CANCELLED:
                statusString = "CANCELLED";
                break;
            default:
                statusString = "UNKNOWN";
        }
        os << "TaskThread [TaskID: " << t.taskId << ", ThreadID: " << t.threadId
           << ", Status: " << statusString << "]";
        return os;
    }

    void TaskThread::setThreadId(std::thread::id id) {
        std::scoped_lock<std::mutex> lock(this->mx);
        threadId = id;
    }

    TaskThread::TaskThreadStatus TaskThread::getStatus() const {
        return this->status;
    }

    ////////////////////////////////// Thread Pool Implementation /////////////////////////////////
    ThreadPool::ThreadPool(int size, const std::string &name) : size(size) {
        threadId = GlobalThreadId++;
        this->name = name + "_" + std::to_string(threadId);

    }

    ThreadPool::ThreadPool(int size) : size(size) {
        threadId = GlobalThreadId++;
        this->name = "Custom Thread pool_" + std::to_string(threadId);
    }


    std::shared_ptr<TaskThread> ThreadPool::popTask() {
        std::unique_lock<std::mutex> lock(mx);
        cv.wait(lock, [&]() {
            return !taskQueue.empty() || shutdownFlag;
        });
        if (shutdownFlag) {
            for (; !taskQueue.empty();) {
                taskQueue.pop_front();
            }
            return nullptr;
        }
        auto task = taskQueue.front();
        taskQueue.pop_front();
        return task;
    }

    void ThreadPool::runWorker() {
        while (true) {
            auto task = popTask();
            if (task) {
                task->setThreadId(std::this_thread::get_id());
                task->invokeFunction();
            } else {
                return;
            }
        }
    }


    void ThreadPool::createThread() {
        if (threadsPool.size() < size) {
            threadsPool.push_back(std::move(std::thread([this]() { this->runWorker(); })));
        }
    }

    void ThreadPool::shutdown() {
        shutdownFlag = true;
        cv.notify_all();
        for (auto &thread: threadsPool) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    ThreadPool::~ThreadPool() {
        this->shutdown();
    }

    const std::deque<std::shared_ptr<TaskThread>> &ThreadPool::getTaskQueueState() const {
        return taskQueue;
    }

    const std::vector<std::thread> &ThreadPool::getThreadsState() const {
        return threadsPool;
    }

    std::atomic<bool> &ThreadPool::taskShouldTerminate() {
        return shutdownFlag;
    }

}
