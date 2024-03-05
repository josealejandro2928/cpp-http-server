cmake_minimum_required(VERSION 3.27)
project(http_server)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(core/external/nlohmann_json)

add_library(http_server
        core/http_server/Server.h
        core/http_server/Server.cpp
        core/http_server/Request.h
        core/http_server/Request.cpp
        core/http_server/utils/utils.h
        core/http_server/utils/utils.cpp
        core/http_server/Router.h
        core/http_server/Router.cpp
        core/http_server/exceptions/Exceptions.h
        core/http_server/utils/types.cpp
        core/http_server/utils/Logging.h
        core/http_server/utils/Logging.cpp)

# Set the include directories for http_server target
target_include_directories(http_server PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
        $<INSTALL_INTERFACE:core> # for install targets
)

target_link_libraries(http_server PUBLIC nlohmann_json::nlohmann_json)

add_executable(server main.cpp)
target_link_libraries(server PRIVATE http_server)

add_executable(app app_crud_api/app.cpp
        app_crud_api/controllers/UserController.h
        app_crud_api/services/UserService.cpp
        app_crud_api/models/Models.h
        app_crud_api/models/Models.cpp
        app_crud_api/dto/RequestDtos.h
        app_crud_api/services/UserService.h
        app_crud_api/middlewares/Middlewares.h
        app_crud_api/middlewares/Middlewares.cpp)

target_link_libraries(app PRIVATE http_server)

target_include_directories(app PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/app_crud_api
)

# Specify include directories
#target_include_directories(http_server PUBLIC http_server)
#target_include_directories(server PRIVATE http_server)
#target_include_directories(app PRIVATE http_server)
