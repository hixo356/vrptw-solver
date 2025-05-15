
add_executable(vrptw)

target_include_directories(vrptw PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCES src/*.cpp)

target_sources(vrptw
 PRIVATE
  main.cpp ${SOURCES})