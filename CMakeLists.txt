cmake_minimum_required(VERSION 3.20)
project(caissa)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} src/main.cpp src/board.cpp src/movegen.cpp src/move_application.cpp src/tests.cpp src/evalutation.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
