## Installation

The project uses CMake to build the project. The following steps show how to build the project:

1. Clone the repository
2. Go to the root directory of the project
   3Execute the following commands:

```bash
sudo bash build.sh
```

This command will create a build directory and build the project.

```bash
mkdir -p build && cd build
cmake ..
make
sudo make install
```

## Dependencies

The project relies on the following dependencies:

- nlohmann/json: JSON for Modern C++

### Adding <nlohmann/json>

The project uses the `FetchContent` module to add the `nlohmann/json` library. The following code snippet shows how to
add the library to the project:

```cmake
include(FetchContent)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_BRANCH master
)
FetchContent_MakeAvailable(json)
target_link_libraries(your_project_name PRIVATE nlohmann_json::nlohmann_json)
```

## Example Usage
Everyone that has been working as a backend developer knows somehow the well know Javascript library to create 
Http rest api `expressjs`. This project is a C++ version of `expressjs` architecture based on middlewares. To in a
esy way create endpoints to respond http calls.

```cpp
#include <iostream>
#include "http_server/Server.h"
#include "http_server/utils/Logging.h"

namespace hs = HttpServer;
using logger = hs::Logging;

// A custom middleware function, we will use for a post to the endpoint /example
void requestPostHandler(hs::Request &req) {
    json body = std::any_cast<json>(req.getRequestAttribute("loggedInUser"));
    std::cout << "Logged in user: " << body["username"] << std::endl;
    // Return a json response with 200 as status code
    req.sendJson(req, 200, body);
}

int main() {
    int port = 3000;
    // Build the server instance to listen on port 3000
    hs::Server server(port);
    // Get the router instance ref to create endpoints
    auto &router = server.getRouter();
    
    // Create a global middleware to log all requests
    router.use([](hs::Request &req) {
        std::cout << "Received " << req.getMethod() << " request for " << req.getFullPath() << std::endl;
    });

    // Create an GET request endpoint path / that will return a response with status code 200 and Content-Type text/plain
    router.getMethod("/", [&](hs::Request &req) {
        std::string response = "Hello World, im listening request on http://127.0.0.1:" + std::to_string(port);
        req.sendText(req, 200, response);
    });

    // Add a hook function to be called when the server starts
    server.onServerStart([]() {
        logger::info("Cpp Server has started");
    });
    
    /// You can create your custom middleware as a lambda function
    auto middlewareFn = [](hs::Request &req) {
        std::cout << ("Custom middleware to print the body and headers of the request") << std::endl;
        // Here you pass the type of the body you are expecting, you can define your own structures and classes, but they must be serializable to json, for that use the nlohmann/json library
        json body = req.getBodyObject<json>();
        auto headers = req.getHeaders();
        std::cout << "Body: " << body << std::endl;
        std::cout << "Headers: ";
        for (const auto &[key, val]: headers) {
            std::cout << key << " : " << val << ", ";
        }
        std::cout << std::endl;
        // Then manipulate the request body object and set an state to be passes to the next middleware function
        body["isLogged"] = true;
        body["username"] = "admin@admin";
        req.setRequestAttribute("loggedInUser", body);
    };
    // Then we just supply for the route /example the chain of middlewares
    router.postMethod("/example", {middlewareFn, requestPostHandler});
    
    server.startListening();
    return 0;
}
```

In the above simple snippet when we hit the endpoint `/example` with a POST request  like this:

```bash
curl --location 'http://localhost:3000/example' \
--header 'Content-Type: application/json' \
--data '{
    "name":"Jose Alejandro Concepcion",
    "lastName":"Concepcion Alvarez"
}'
```

We will get the following output in the console:

```bash
Received POST request for /example
Custom middleware to print the body and headers of the request
Body: {"lastName":"Concepcion Alvarez","name":"Jose Alejandro Concepcion"}
Headers: Accept : */*, Content-Length : 79, Content-Type : application/json, Host : localhost:3000, User-Agent : curl/8.2.1, 
Logged in user: "admin@admin"
```

That was the result of the execution of the global middleware that we declared with `router.use` and then the chain execution
for the post route `/example`.

in the project directory you can find  a example of crud api `app_crud_api`.

If you want to define a global custom error handler you can do it like this:

```cpp
    server.setGlobalExceptionHandler([](std::exception &exc, hs::Request &req) {
        hs::ErrorResponseData errorResponseData(500, "Internal Server Error" + string(exc.what()));
        req.sendJson<hs::ErrorResponseData>(req, errorResponseData.code, errorResponseData);
    });
```

This will catch all the exceptions that are thrown in the middlewares and the route handlers, and return
a custom error response class provided by the library with the status code and the message. For example if you throw an exception like this:

```cpp
    router.getMethod("/error", [](hs::Request &req) {
        throw std::runtime_error("This is a custom error");
    });
```
You will get the following response:

```bash
{
    "code": 500,
    "errors": [],
    "message": "This is a custom error"
}
```

By the moment the project is in early stages of development and only allow to return json responses, as Content-Type application/json and
text/plain. In the future will be added more features like file uploads, and more content types.
For the post, put and patch the content types allowed to be serialized by the library (behind the scenes the library relies on `nlohmann/json` library) are `application/json` and `application/x-www-form-urlencoded`.

## Custom Thread Pool Executor
The project uses a custom thread pool executor to handle the requests, you can define the number of threads that the server will use to handle the requests concurrently.
But it exposes a set of functionalities that might be useful for you.

```cpp
#include "http_server/concurrency/Executor.h"
namespace hs = HttpServer;
// Define a function
long sumUpTo(int n) {
    long sum = 0;
    for (int i = 0; i <= n; i++) {
        sum += i;
    }
    return sum;
}

int main(){
    // Create a thread pool executor with 4 threads, by default the number of threads is the number of cores in the machine
    hs::ThreadPoolExecutor executor(4);
    // passing a lambda function to the submit method
    executor.submit([]() {
        std::cout << "This is a task" << std::endl;
        return 0;
    }).get();
    
    // Submit a task to the executor, that return a future object
    std::vector<std::future<long>> futures;
    for (int i = 0; i < 50; i++) {
        futures.push_back(std::move(executor.submit(sumUpTo, 1000000)));
    }
    // Wait for the futures to be ready
    for (auto &future: futures) {
        auto result = future.get();
        std::cout << "Result: " << result << std::endl;
    }
    return 0;
}

```
Also, you can use the `ThreadPoolExecutor` to run task, and call pass a callback to be executed when the task is done.

```cpp
#include "http_server/concurrency/Executor.h"
namespace hs = HttpServer;

void onTaskDone(future<long>& f) {
    std::cout << "Task is done" << std::endl;
    std::cout << "Result: " << f.get() << std::endl;
}

int main(){
    hs::ThreadPoolExecutor executor();
    executor.run([]() {
        std::cout << "This is a task" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return 42;
    }, onTaskDone);
    return 0;
}
```

Another feature is the ability to run task periodically, you can use the `runSchedule`. Also, You can run a function in the future after a delay a number of times.

```cpp
#include "http_server/concurrency/Executor.h"
namespace hs = HttpServer;

int main(){
    hs::ThreadPoolExecutor executor();
    /// Run a task every 2 seconds with initial delay of 1 second
    pool.runSchedule([]() {
        std::cout << "This is a scheduled task that will be run every 2 seconds" << std::endl;
    }, std::chrono::seconds(2), std::chrono::seconds(1));
    
      pool.runFunctionNthTimes([]() {
        std::cout << "This is a task that will be executed 2 times after 4 seconds" << std::endl;
    }, std::chrono::seconds(4), 2);
      
      std::cin.get();
    return 0;
}
```
