cmake_minimum_required(VERSION 3.27)
project(http_server)

set(CMAKE_CXX_STANDARD 20)

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

target_include_directories(http_server PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
        $<INSTALL_INTERFACE:core>
)

include(FetchContent)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_BRANCH master
)
FetchContent_MakeAvailable(json)

target_link_libraries(http_server PUBLIC nlohmann_json::nlohmann_json)


install(TARGETS http_server
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY core/http_server/
        DESTINATION include/http_server/
        FILES_MATCHING PATTERN "*.h")

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
        app_crud_api/middlewares/Middlewares.cpp
        app_crud_api/services/AuthService.h
        app_crud_api/services/AuthService.cpp
        app_crud_api/controllers/TaskController.h
        app_crud_api/services/TaskService.h
        app_crud_api/services/TaskService.cpp
        app_crud_api/dto/ResponseDtos.h
        app_crud_api/data/DataStorage.h
        app_crud_api/data/DataStorage.cpp
        app_crud_api/dto/RequestDtos.cpp)

target_link_libraries(app PRIVATE http_server)

target_include_directories(app PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/app_crud_api
)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(
        tests
        testing/request_obj_test.cpp
)
#target_link_libraries(tests PRIVATE http_server)
target_link_libraries(
        tests
        gtest_main
        http_server
)

include(GoogleTest)
gtest_discover_tests(tests)
