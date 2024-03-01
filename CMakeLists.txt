cmake_minimum_required(VERSION 3.27)
project(http_server)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(core/external/nlohmann_json)

add_library(http_server
        core/Server.h
        core/Server.cpp
        core/Request.h
        core/Request.cpp
        core/utils/utils.h
        core/utils/utils.cpp
        core/Router.h
        core/Router.cpp)

target_link_libraries(http_server PRIVATE nlohmann_json::nlohmann_json)

add_executable(server main.cpp)
target_link_libraries(server PRIVATE http_server)

add_executable(app App/app.cpp)
target_link_libraries(app PRIVATE http_server)

# Specify include directories
#target_include_directories(http_server PUBLIC http_server)
#target_include_directories(server PRIVATE http_server)
#target_include_directories(app PRIVATE http_server)
