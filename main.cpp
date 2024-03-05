#include <iostream>
#include "vector"
#include "list"
#include "http_server/utils/utils.h"

int main() {
    std::cout << "Testing Map:::::::::::::" << std::endl;
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto res = HttpServer::mapFn<std::vector<int>, int>(data, [](const int &el) {
        return el * el;
    });
    auto res2 = HttpServer::mapFn<std::list<int>, std::string>({40, 50}, [](const int &el) {
        auto data = el * el;
        return "This is a string: " + std::to_string(data);
    });

    auto res3 = HttpServer::mapFn<std::vector<std::string>, char>({"hello", "World"}, [](auto &el) {
        return el[0];
    });

    for (const auto &el: res) {
        std::cout << el << " ";
    }
    std::cout << std::endl;
    for (const auto &el: res2) {
        std::cout << el << " ";
    }
    std::cout << std::endl;
    for (const auto &el: res3) {
        std::cout << el << " ";
    }
    std::cout << std::endl;
    std::cout << "Testing Filter::::::::::::::::" << std::endl;
    auto filterElements = HttpServer::filterFn<std::vector<int>>(data, [](const int &el) {
        return el % 2 == 0;
    });

    for (const auto &el: filterElements) {
        std::cout << el << " ";
    }
    std::cout << std::endl;
    std::cout << "Testing Find::::::::::::::::" << std::endl;
    auto elementFound = HttpServer::findFn<std::vector<int>>(data, [](auto &el) {
        return el == 9;
    });
    if (elementFound)
        std::cout << "Found el:" << *elementFound << std::endl;
    else
        std::cout << "Not Found el " << std::endl;

    std::cout << std::endl;
    std::cout << "Testing =Reduce::::::::::::::::" << std::endl;
    int sum = HttpServer::reduceFn<std::vector<int>, int>(data, [](int &acc, auto &el, int index) {
        acc += el;
        return acc;
    }, 0);
    std::cout << "The output of the reduce: " << sum << std::endl;

    auto elementsReduced = HttpServer::reduceFn<std::vector<int>,
            std::vector<int>>(data,
                              [&](auto &acc, auto &el, int index) {
                                  if (index % 2 == 0) {
                                      acc.push_back(data[index] *
                                                    data[index]);
                                  }
                                  return acc;
                              }, {});
    std::cout << "Reduce Elements" << std::endl;
    for (auto &el: elementsReduced) {
        std::cout << el << ",";
    }
}
