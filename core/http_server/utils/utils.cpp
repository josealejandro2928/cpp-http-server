//
// Created by pepe on 2/27/24.
//

#include "utils.h"
#include <vector>
#include <string>
#include <sstream>
#include "types.h"
#include "http_server/exceptions/Exceptions.h"

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

    vector <string> strSplit(string &data, char limiter) {
        vector<string> result;
        stringstream ss(data);
        string chunk;
        while (getline(ss, chunk, limiter)) {
            result.push_back(chunk);
        }
        return result;
    }

    std::vector<std::string> strSplit(const std::string &s, const std::string &delimiter) {
        std::vector<std::string> parts;
        auto start = 0U;
        auto end = s.find(delimiter);
        while (end != std::string::npos) {
            parts.push_back(s.substr(start, end - start));
            start = end + delimiter.length();
            end = s.find(delimiter, start);
        }
        parts.push_back(s.substr(start, end));
        return parts;
    }


    string strJoin(vector <string> &data, string &&limiter) {
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
        if (parts.size() < 2) throw BadRequestException("Invalid request path 3");
        result.method = StringToHttpMethod(parts[0]);
        result.fullPath = parts[1];
        vector<string> pathAndQuery = strSplit(parts[1], '?');
        if (pathAndQuery.empty()) throw BadRequestException("Invalid request path 4");
        result.path = pathAndQuery[0];
        if (pathAndQuery.size() > 1) {
            vector<string> queryParts = strSplit(pathAndQuery[1], '&');
            for (auto &queryPart: queryParts) {
                vector<string> queryParam = strSplit(queryPart, '=');
                if (queryParam.size() == 1)
                    result.query[queryParam[0]] = "";
                else if (queryParam.size() == 2) {
                    result.query[queryParam[0]] = queryParam[1];
                } else {
                    throw std::invalid_argument("Invalid query param");
                }
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
            case 422:
                return "Unprocessable Entity";
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

    std::string urlDecode(const std::string &str) {
        std::string result;
        result.reserve(str.size());
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '%') {
                if (i + 2 < str.length()) {
                    std::string hex = str.substr(i + 1, 2);
                    char decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
                    result += decodedChar;
                    i += 2;
                }
            } else if (str[i] == '+') {
                result += ' ';
            } else {
                result += str[i];
            }
        }

        return result;
    }

    std::map<std::string, std::vector<std::string>> processURLEncodedFormBody(std::string &bodyStr) {
        // "name=Jose%20Alejandro%20Concepcion%20Alvarez&email=jalejandroc2928%40gmail.com&password=123456.Aa"
        auto parts = strSplit(bodyStr, '&');
        std::map<string, std::vector<std::string>> result;
        for (auto &part: parts) {
            auto keyVal = strSplit(part, '=');
            if (keyVal.empty()) continue;
            if (keyVal.size() == 1) {
                result[keyVal[0]].emplace_back("");
            } else {
                result[keyVal[0]].emplace_back(urlDecode(keyVal[1]));
            }
        }
        return result;
    }

    void parseMultipartFormData(const std::string &body, const std::string &boundary) {
        auto parts = strSplit(body, "--" + boundary + "\r\n");
        for (auto &part: parts) {
            if (part.empty() || part == "--\r\n") continue;
            auto headersEndPos = part.find("\r\n\r\n");
            if (headersEndPos != std::string::npos) {
                auto headersPart = part.substr(0, headersEndPos);
                auto bodyPart = part.substr(headersEndPos + 4, part.length() - headersEndPos -
                                                               6); // Subtract 6 to remove the trailing "\r\n" and "--"
                // Here you can parse headersPart to extract information like name, filename, etc.
                // And process bodyPart as needed
            }
        }
    }


}
