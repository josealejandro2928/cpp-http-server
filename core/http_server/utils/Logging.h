//
// Created by pepe on 3/5/24.
//

#ifndef HTTP_SERVER_LOGGING_H
#define HTTP_SERVER_LOGGING_H

#include "http_server/Request.h"

namespace HttpServer {
    class Logging {
    public:
        enum class Level {
            DEBUG,
            INFO,
            WARN,
            ERROR
        };

        static void log(const Level level, const char *msg);

        static void httpRequestLog(Request &request);

        static void info(const char *msg);

        static void debug(const char *msg);

        static void warn(const char *msg);

        static void error(const char *msg);

    private:
        static bool enable;
        static bool enableLogHttpRequests;
        static Level logLevelForHttpRequest;
        static std::string hostname;
        static std::string hostIP;
    };

}


#endif //HTTP_SERVER_LOGGING_H
