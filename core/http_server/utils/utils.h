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
    std::vector<std::string> strSplit(const std::string& s, const std::string& delimiter);

    std::string strJoin(std::vector<std::string> &data, std::string &&limiter);

    std::string strReplace(std::string &data, std::string &oldStr, std::string &newStr);

    template<typename T, typename K>
    std::vector<K> mapFn(const T &iterable, const std::function<K(const typename T::value_type &)> &predicate) {
        std::vector<K> res;
        for (const auto &el: iterable) {
            K output = predicate(el);
            res.push_back(output);
        }
        return res;
    }

    template<typename T>
    std::vector<typename T::value_type>
    filterFn(const T &iterable, const std::function<bool(const typename T::value_type &)> &predicate) {
        std::vector<typename T::value_type> res;
        for (const auto &el: iterable) {
            if (predicate(el)) {
                res.push_back(el);
            }
        }
        return res;
    }

    template<typename T>

    typename T::value_type *findFn(T &iterable, const std::function<bool(typename T::value_type &)> &predicate) {
        for (auto &el: iterable) {
            if (predicate(el)) {
                return &el;
            }
        }
        return nullptr;
    }

    template<typename T, typename K>
    K reduceFn(const T &iterable, const std::function<K(K &, const typename T::value_type &, int)> &predicate,
               K initValue) {
        K res = initValue;
        for (auto it = iterable.begin(); it != iterable.end(); it++) {
            res = predicate(res, *it, it - iterable.begin());
        }
        return res;
    }

    std::string urlDecode(const std::string& );
    std::map<std::string, std::vector<std::string>> processURLEncodedFormBody(std::string &);
    void parseMultipartFormData(const std::string&, const std::string&);

}


#endif //HTTP_SERVER_UTILS_H
