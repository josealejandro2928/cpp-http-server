//
// Created by pepe on 3/14/24.
//

#ifndef HTTP_SERVER_COMPUTATIONCONTROLLER_H
#define HTTP_SERVER_COMPUTATIONCONTROLLER_H

#include "http_server/Router.h"
#include "ControllerBase.h"
#include "dto/RequestDtos.h"
#include "services/ComputationService.h"
#include "nlohmann/json.hpp"
#include "services/TaskWorkerService.h"
#include <future>

namespace hs = HttpServer;
using json = nlohmann::json;

class ComputationController : public ControllerBase {
public:
    explicit ComputationController(HttpServer::Router *router, const string &basePath = "/computation")
            : ControllerBase(router,
                             basePath) {}

private:
    static std::atomic<long> backgroundTaskCounter;

    static void computePrimesFactor(hs::Request &req) {
        hs::Logging::info("Running the computePrimesFactor");
        auto requestBody = req.getBodyObject<ComputeNPrimesRequest>();
        if (requestBody.primesToCompute.empty()) {
            throw hs::BadRequestException("At least one item");
        }
        json result = json::object();
        auto maxItem = requestBody.primesToCompute.at(0);
        for (auto &el: requestBody.primesToCompute) {
            maxItem = std::max(maxItem, el);
        }
        auto primesNumber = ComputationService::computePrimesUpToN(maxItem);

        for (const auto &item: requestBody.primesToCompute) {
            auto partial = ComputationService::computePrimeFactors(item, primesNumber);
            result[std::to_string(item)] = std::move(partial);
        }
        json response;
        response["status"] = "OK";
        response["computation"] = result;
        req.sendJson(req, 201, response);
    }

    static void computePrimesFactorConcurrent(hs::Request &req) {
        hs::Logging::info("Running the computePrimesFactorConcurrent");
        auto requestBody = req.getBodyObject<ComputeNPrimesRequest>();
        if (requestBody.primesToCompute.empty()) {
            throw hs::BadRequestException("At least one item");
        }
        json result;
        auto &pool = TaskWorkerService::getInstance().getPool();
        std::cout << pool << std::endl;
        std::vector<std::shared_ptr<hs::TaskThread>> futuresTask;
        auto maxItem = requestBody.primesToCompute.at(0);
        for (auto &el: requestBody.primesToCompute) {
            maxItem = std::max(maxItem, el);
        }
        auto primesNumber = ComputationService::computePrimesUpToN(maxItem);

        for (const auto &item: requestBody.primesToCompute) {
            auto taskThread = pool.submit([&item, &primesNumber]() {
                auto partial = ComputationService::computePrimeFactors(item, primesNumber);
                return std::make_pair(std::to_string(item), std::move(partial));
            });
            futuresTask.push_back(std::move(taskThread));
        }
        for (auto &task: futuresTask) {
            auto res = task->get<std::pair<string, std::vector<std::pair<long, long>>>>();
            result[res.first] = std::move(res.second);
        }

        json response;
        response["status"] = "OK";
        response["computation"] = result;
        req.sendJson(req, 201, response);
    }

    static void createBackgroundTask(hs::Request &req) {
        auto &pool = TaskWorkerService::getInstance().getPool();
        pool.submit([]() {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            return make_pair(backgroundTaskCounter++, random());
        })->addOnFinishCallback([](hs::TaskThread *task) {
            try {
                auto res = task->get<pair<long, long>>();
                auto message = string("Background task: " + std::to_string(res.first) + " completed" + " result= " +
                                      std::to_string(res.second));
                hs::Logging::info(message.c_str());
            } catch (std::exception &exc) {
                hs::Logging::error("Error on background task:");
                hs::Logging::error(exc.what());
            }
        });
        req.sendJson(req, 201, json::object());
    }

    static void getPrimeFactorization(hs::Request &req) {
        int number = std::stoi(req.getQuery()["number"]);
        auto primesNumber = ComputationService::computePrimesUpToN(number);
        auto result = ComputationService::computePrimeFactors(number, primesNumber);
        json response;
        response["result"] = result;
        response["number"] = number;
        req.sendJson(req, 201, response);
    }

public:

    void registerEndpoints() override {
        router->postMethod(basePath + "/primes-factor", computePrimesFactor);
        router->postMethod(basePath + "/primes-factor-concurrent", computePrimesFactorConcurrent);
        router->postMethod(basePath + "/background", createBackgroundTask);
        router->getMethod(basePath + "/prime-factorization", getPrimeFactorization);
    }
};

std::atomic<long> ComputationController::backgroundTaskCounter = 0;
#endif //HTTP_SERVER_COMPUTATIONCONTROLLER_H
