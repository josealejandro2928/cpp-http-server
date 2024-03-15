//
// Created by pepe on 3/7/24.
//
#include <gtest/gtest.h>
#include "http_server/Server.h"
#include "http_server/Request.h"
#include "http_server/exceptions/Exceptions.h"

namespace hs = HttpServer;

// Write a test to test the request object
TEST(RequestTest, TestingBasicRequestObjectFormation) {
    std::string httpRequest =
            "GET /tasks?status=IN_PROGRESS,PENDING HTTP/1.1\r\n"
            "Authorization: Bearer 11111111\r\n"
            "User-Agent: PostmanRuntime/7.36.3\r\n"
            "Host: localhost:9000\r\n"
            "Accept-Encoding: gzip, deflate, br\r\n"
            "Connection: keep-alive\r\n\r\n";

    hs::Request request = hs::Request::makeRequest(httpRequest);
    EXPECT_EQ(request.getMethod(), "GET");
    EXPECT_EQ(request.getPath(), "/tasks");
    EXPECT_EQ(request.getHeaders()["Host"], "localhost:9000");
    EXPECT_EQ(request.getHeaders()["User-Agent"], "PostmanRuntime/7.36.3");
    EXPECT_EQ(request.getHeaders()["Authorization"], "Bearer 11111111");
}
// write a test to test the correct queryParams are parsed
TEST(RequestTest, TestingQueryParams) {
    std::string httpRequest =
            "GET /tasks?status=IN_PROGRESS,PENDING&user=John%20Doe&priority=high HTTP/1.1\r\n"
            "Authorization: Bearer 11111111\r\n"
            "User-Agent: PostmanRuntime/7.36.3\r\n"
            "Host: localhost:9000\r\n"
            "Accept-Encoding: gzip, deflate, br\r\n"
            "Connection: keep-alive\r\n\r\n";

    hs::Request request = hs::Request::makeRequest(httpRequest);
    EXPECT_EQ(request.getMethod(), "GET");
    EXPECT_EQ(request.getPath(), "/tasks");
    EXPECT_EQ(request.getQuery()["status"], "IN_PROGRESS,PENDING");
    EXPECT_EQ(request.getQuery()["user"], "John%20Doe"); // Assuming URL decoding is not automatically applied
    EXPECT_EQ(request.getQuery()["priority"], "high");
    EXPECT_EQ(request.getHeaders()["Authorization"], "Bearer 11111111");
}

TEST(RequestTest, TestingPlainTextBody) {
    std::string httpRequest =
            "POST /tasks/create HTTP/1.1\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "Host: localhost:9000\r\n\r\n"
            "Sample text body";

    hs::Request request = hs::Request::makeRequest(httpRequest);
    EXPECT_EQ(request.getMethod(), "POST");
    EXPECT_EQ(request.getPath(), "/tasks/create");
    EXPECT_EQ(request.getHeaders()["Content-Type"], "text/plain");
    EXPECT_EQ(request.getBody(), "Sample text body");
}

TEST(RequestTest, TestingJsonBody) {
    std::string httpRequest =
            "POST /tasks/create HTTP/1.1\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 49\r\n"
            "Host: localhost:9000\r\n\r\n"
            "{\"user\":\"John Doe\",\"priority\":\"high\"}";

    hs::Request request = hs::Request::makeRequest(httpRequest);
    EXPECT_EQ(request.getMethod(), "POST");
    EXPECT_EQ(request.getPath(), "/tasks/create");
    EXPECT_EQ(request.getHeaders()["Content-Type"], "application/json");
    EXPECT_EQ(request.getBody(), "{\"user\":\"John Doe\",\"priority\":\"high\"}");
}

TEST(RequestTest, TestingTextRequestBodyWithNewlines) {
    std::string httpRequest =
            "POST /submit-comment HTTP/1.1\r\n"
            "Content-Type: text/plain\r\n"
            "Host: localhost:9000\r\n"
            "Content-Length: 51\r\n"
            "\r\n"
            "This is a test comment.\n"
            "It spans multiple lines.\n"
            "End of comment.";

    hs::Request request = hs::Request::makeRequest(httpRequest);
    EXPECT_EQ(request.getMethod(), "POST");
    EXPECT_EQ(request.getPath(), "/submit-comment");
    EXPECT_EQ(request.getHeaders()["Content-Type"], "text/plain");
    std::string expectedBody = "This is a test comment.\nIt spans multiple lines.\nEnd of comment.";
    EXPECT_EQ(request.getBody(), expectedBody);
}

TEST(RequestTest, MalformedRequestLineThrowsException) {
    std::string httpRequest = "GET  HTTP/1.1\r\nHost: localhost:9000\r\n\r\n"; // Missing path

    bool exceptionThrown = false;
    try {
        hs::Request request = hs::Request::makeRequest(httpRequest);
    } catch (const hs::BadRequestException &e) {
        exceptionThrown = true;
    } catch (...) {
        FAIL() << "Unexpected exception type thrown.";
    }
    EXPECT_TRUE(exceptionThrown) << "Expected an exception to be thrown for malformed request line.";
}

TEST(RequestTest, InvalidHttpMethodThrowsException) {
    std::string httpRequest = "INVALID /path HTTP/1.1\r\nHost: localhost:9000\r\n\r\n"; // Unsupported HTTP method

    bool exceptionThrown = false;
    try {
        hs::Request request = hs::Request::makeRequest(httpRequest);
    } catch (const hs::BadRequestException &e) {
        exceptionThrown = true;
    } catch(std::invalid_argument &e) {
        exceptionThrown = true;
    }
    catch (...) {
        FAIL() << "Unexpected exception type thrown.";
    }
    EXPECT_TRUE(exceptionThrown) << "Expected an exception to be thrown for an invalid HTTP method.";
}

TEST(RequestTest, MissingContentLengthThrowsException) {
    std::string httpRequest = "POST /submit HTTP/1.1\r\n"
                              "Content-Type: application/json\r\n" // Missing Content-Length
                              "Host: localhost:9000\r\n\r\n"
                              "{\"key\": \"value\"}";

    bool exceptionThrown = false;
    try {
        hs::Request request = hs::Request::makeRequest(httpRequest);
    } catch (const hs::BadRequestException &e) {
        exceptionThrown = true;
    } catch (...) {
        FAIL() << "Unexpected exception type thrown.";
    }
    EXPECT_TRUE(exceptionThrown) << "Expected an exception to be thrown for missing Content-Length header in POST request.";
}

TEST(RequestTest, MissingHostHeaderThrowsException) {
    std::string httpRequest = "GET /path HTTP/1.1\r\nContent-Type: application/json\r\n\r\n"; // Missing Host header

    bool exceptionThrown = false;
    try {
        hs::Request request = hs::Request::makeRequest(httpRequest);
    } catch (const hs::BadRequestException &e) {
        exceptionThrown = true;
    } catch (...) {
        FAIL() << "Unexpected exception type thrown.";
    }
    EXPECT_TRUE(exceptionThrown) << "Expected an exception to be thrown for missing Host header.";
}
