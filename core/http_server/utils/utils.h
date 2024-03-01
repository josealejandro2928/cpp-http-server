//
// Created by pepe on 2/27/24.
//

#ifndef HTTP_SERVER_UTILS_H
#define HTTP_SERVER_UTILS_H

#include <vector>
#include <string>
#include <sstream>
#include "types.h"

using namespace std;
namespace HttpServer {
    vector <string> strSplit(string &data, char limiter);
    string strJoin(vector <string> &data, string&& limiter);
    string strReplace(string &data, string &oldStr, string &newStr);
    PathMethodAndQueryParams processRequestPathMethodAndQueryParams(string &request);
    HttpMethod StringToHttpMethod(const std::string &method);
    std::string HttpMethodToString(HttpMethod method);
    std::string getHttpStatusResponseStrFromStatus(int status);
}


#endif //HTTP_SERVER_UTILS_H
