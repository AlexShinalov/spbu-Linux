cmake_minimum_required(VERSION 3.10)

project(Backup)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs)

set (CMAKE_CXX_FLAGS "-lstdc++fs -std=c++17")
