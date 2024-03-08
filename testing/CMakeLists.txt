# In testing/CMakeLists.txt
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(tests request_obj_test.cpp)
target_link_libraries(tests gtest_main http_server)
include(GoogleTest)
gtest_discover_tests(tests)
