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
    static void computePrimesFactor(hs::Request &req) {
        hs::Logging::info("Running the computePrimesFactor");
        auto requestBody = req.getBodyObject<ComputeNPrimesRequest>();
        json result = json::object();
        for (const auto &item: requestBody.primesToCompute) {
            auto partial = ComputationService::computePrimeFactors(item);
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
        json result = json::object();
        auto &pool = TaskWorkerService::getInstance().getPool();
        std::cout << "Thread capacity: " << pool.getCapacity() << " Thread size: " << pool.getSize() << std::endl;
        std::vector<std::shared_ptr<hs::TaskThread>> futuresTask;

        for (const auto &item: requestBody.primesToCompute) {
            auto taskThread = pool.submit([&item]() {
                auto partial = ComputationService::computePrimeFactors(item);
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

public:

    void registerEndpoints() override {
        router->postMethod(basePath + "/primes-factor", computePrimesFactor);
        router->postMethod(basePath + "/primes-factor-concurrent", computePrimesFactorConcurrent);
    }
};

#endif //HTTP_SERVER_COMPUTATIONCONTROLLER_H
