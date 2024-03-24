//
// Created by pepe on 3/24/24.
//

#ifndef HTTP_SERVER_EXECUTOR_H
#define HTTP_SERVER_EXECUTOR_H

#include <future>
#include <functional>
#include <iostream>
#include "ThreadPool.h"

namespace HttpServer {
    class Executor {
    private:
        virtual void execute(std::function<void()> &&task) = 0;

    public:
        virtual ~Executor() = default;

        /**
         * Shutdown the pool
         */
        virtual void shutdown() = 0;
        /**
         * Schedule a task in a thread of the pool to run periodically every delay with a initialDelay
         * @param task    The task to run a function that takes no arguments and returns void
         * @param delay   The delay between each run
         * @param initialDelay The delay to start the first run
         */
        virtual void runSchedule(std::function<void()> &&task,
                                 std::chrono::duration<double> delay,
                                 std::chrono::duration<double> initialDelay) = 0;

        /**
         * Run a function in a thead of the pool the numberOfTimes every delay
         * @param task The task to run a function that takes no arguments and returns void
         * @param delay The delay between each run
         * @param numberOfTimes The number of times to run the task
         */
        virtual void runFunctionNthTimes(std::function<void()> &&task,
                                 std::chrono::duration<double> delay,
                                 long numberOfTimes) = 0;


        /**
         * Submit a task to the pool and return a future object
         * @tparam F The function type
         * @tparam Args The arguments types
         * @param f the function to run
         * @param args the arguments to pass to the function
         * @return future<decltype(f(args...))>
         */
        template<class F, class... Args>
        auto submit(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
            using ReturnType = decltype(f(args...));
            auto taskPtr = std::make_shared<std::packaged_task<ReturnType()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            auto res = taskPtr->get_future();
            execute([taskPtr]() {
                (*taskPtr)();
            });
            return res;
        }
    };

    class ThreadPoolExecutor : public Executor {
    private:
        ThreadPool *pool;

        void execute(std::function<void()> &&task) override;

        std::string name;

    public:
        explicit ThreadPoolExecutor(int threads);

        explicit ThreadPoolExecutor();

        /**
         * Run a task in a thread of the pool and call the onFinishCb when the task is completed, it does not return a future object and it is non-blocking
         * @tparam F The function type
         * @param f  The function to run, it should be a void function that takes no arguments
         * @param onFinishCb The callback to call when the task is completed
         */
        template<class F, class... Args>
        void run(F &&f, Args &&... args, std::function<void(std::future<decltype(f(args...))> &)> onFinishCb) {
            using ReturnType = decltype(f(args...));

            auto taskPtr = std::make_shared<std::packaged_task<ReturnType()>>(
                    [f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable -> ReturnType {
                        return f(args...);
                    });
            pool->submit([taskPtr]() {
                (*taskPtr)();
                return 0;
            })->addOnFinishCallback([onFinishCb, taskPtr](auto *_) mutable {
                auto res = taskPtr->get_future();
                onFinishCb(res);
            });
        }

        void shutdown() override;

        void runSchedule(std::function<void()> &&task, std::chrono::duration<double> delay,
                         std::chrono::duration<double> initialDelay) override;

        bool isPoolRunning();

        ~ThreadPoolExecutor() override;

        friend std::ostream &operator<<(std::ostream &os, ThreadPoolExecutor &t);

        ThreadPoolExecutor* setName(const std::string &n) {
            this->name = n;
            return this;
        }

        [[nodiscard]] std::string getName() const {
            return name;
        }
        void runFunctionNthTimes(std::function<void ()> &&task, std::chrono::duration<double> delay, long numberOfTimes) override;
    };

}

#endif //HTTP_SERVER_EXECUTOR_H
