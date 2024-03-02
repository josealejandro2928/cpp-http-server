//
// Created by pepe on 2/27/24.
//

#ifndef HTTP_SERVER_UTILS_H
#define HTTP_SERVER_UTILS_H

#include <vector>
#include <string>
#include <sstream>
#include "types.h"
#include "functional"

namespace HttpServer {
    PathMethodAndQueryParams processRequestPathMethodAndQueryParams(std::string &request);

    HttpMethod StringToHttpMethod(const std::string &method);

    std::string HttpMethodToString(HttpMethod method);

    std::string getHttpStatusResponseStrFromStatus(int status);

    ///////// FUNCTIONS ////////////
    std::vector<std::string> strSplit(std::string &data, char limiter);

    std::string strJoin(std::vector<std::string> &data, std::string &&limiter);

    std::string strReplace(std::string &data, std::string &oldStr, std::string &newStr);

    template<typename T, typename K>
    std::vector<K> mapFn(const T &iterable, const std::function<K(const typename T::value_type &)> &predicate) {
        std::vector<K> res;
        for (const auto el: iterable) {
            K output = predicate(el);
            res.push_back(output);
        }
        return res;
    }

    template<typename T>
    std::vector<typename T::value_type> filterFn(const T &iterable, const std::function<bool(const typename T::value_type &)> &predicate) {
        std::vector<typename T::value_type> res;
        for (const auto el: iterable) {
            if (predicate(el)) {
                res.push_back(el);
            }
        }
        return res;
    }
}


#endif //HTTP_SERVER_UTILS_H
