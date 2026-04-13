cmake_minimum_required(VERSION 3.10)
project(winpilot CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADERS "include/Pilot/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(winpilot STATIC ${SOURCES} ${HEADERS})
target_include_directories(winpilot PUBLIC include)
target_link_libraries(winpilot PRIVATE wininet psapi)

option(WINPILOT_BUILD_EXAMPLE "Build example" OFF)
if(WINPILOT_BUILD_EXAMPLE)
    add_executable(winpilot_example example/main.cpp)
    target_link_libraries(winpilot_example PRIVATE winpilot)
endif()
