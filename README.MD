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
