cmake_minimum_required(VERSION 3.27.0)
project(cvrp-solver VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED_ON)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(vrptw)
