cmake_minimum_required(VERSION 3.0.0)
project(Pinecast VERSION 0.1.0)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(CURLPP REQUIRED curlpp)

set(CMAKE_C_COMPILER /usr/bin/g++)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_VERBOSE_MAKEFILE true)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall")

add_definitions(${GTK_CFLAGS_OTHER})

add_executable(Pinecast main.cpp)
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
target_link_libraries(Pinecast ${GTK_LIBRARIES})
target_link_libraries(Pinecast ${CURLPP_LDFLAGS})
target_include_directories(Pinecast PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(Pinecast PRIVATE ${PROJECT_SOURCE_DIR}/Libs)
