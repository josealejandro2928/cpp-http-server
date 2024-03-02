//
// Created by pepe on 2/27/24.
//

#include "utils.h"
#include <vector>
#include <string>
#include <sstream>
#include "types.h"

using namespace std;
namespace HttpServer {
    HttpMethod StringToHttpMethod(const std::string &method) {
        if (method == "GET") return HttpMethod::GET;
        else if (method == "POST") return HttpMethod::POST;
        else if (method == "PUT") return HttpMethod::PUT;
        else if (method == "DELETE") return HttpMethod::DELETE;
        else if (method == "PATCH") return HttpMethod::PATCH;
        else if (method == "OPTIONS") return HttpMethod::OPTIONS;
        else if (method == "HEAD") return HttpMethod::HEAD;
        else if (method == "CONNECT") return HttpMethod::CONNECT;
        else if (method == "TRACE") return HttpMethod::TRACE;
        else throw std::invalid_argument("Unknown HTTP method: " + method);
    }

    std::string HttpMethodToString(HttpMethod method) {
        switch (method) {
            case HttpMethod::GET:
                return "GET";
            case HttpMethod::POST:
                return "POST";
            case HttpMethod::PUT:
                return "PUT";
            case HttpMethod::DELETE:
                return "DELETE";
            case HttpMethod::PATCH:
                return "PATCH";
            case HttpMethod::OPTIONS:
                return "OPTIONS";
            case HttpMethod::HEAD:
                return "HEAD";
            case HttpMethod::CONNECT:
                return "CONNECT";
            case HttpMethod::TRACE:
                return "TRACE";
            default:
                throw std::invalid_argument("Unknown HTTP VERB");
        }
    }

    vector<string> strSplit(string &data, char limiter) {
        vector<string> result;
        stringstream ss(data);
        string chunk;
        while (getline(ss, chunk, limiter)) {
            result.push_back(chunk);
        }
        return result;
    }

    string strJoin(vector<string> &data, string &&limiter) {
        string result;
        for (int i = 0; i < data.size(); i++) {
            result += data[i];
            if (i != data.size() - 1) {
                result += limiter;
            }
        }
        return result;
    }

    string strReplace(string &data, string &oldStr, string &newStr) {
        size_t pos = 0;
        while ((pos = data.find(oldStr, pos)) != string::npos) {
            data.replace(pos, oldStr.length(), newStr);
            pos += newStr.length();
        }
        return data;
    }


    PathMethodAndQueryParams processRequestPathMethodAndQueryParams(string &request) {
        vector<string> parts = strSplit(request, ' ');
        PathMethodAndQueryParams result;
        result.method = StringToHttpMethod(parts[0]);
        result.fullPath = parts[1];
        vector<string> pathAndQuery = strSplit(parts[1], '?');
        result.path = pathAndQuery[0];
        if (pathAndQuery.size() > 1) {
            vector<string> queryParts = strSplit(pathAndQuery[1], '&');
            for (auto &queryPart: queryParts) {
                vector<string> queryParam = strSplit(queryPart, '=');
                result.query[queryParam[0]] = queryParam[1];
            }
        }
        return result;
    }

    std::string getHttpStatusResponseStrFromStatus(int status) {
        switch (status) {
            case 200:
                return "OK";
            case 201:
                return "Created";
            case 202:
                return "Accepted";
            case 204:
                return "No Content";
            case 400:
                return "Bad Request";
            case 401:
                return "Unauthorized";
            case 403:
                return "Forbidden";
            case 404:
                return "Not Found";
            case 405:
                return "Method Not Allowed";
            case 500:
                return "Internal Server Error";
            case 501:
                return "Not Implemented";
            case 502:
                return "Bad Gateway";
            case 503:
                return "Service Unavailable";
            default:
                return "Unknown";
        }

    }



}
