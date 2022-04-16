cmake_minimum_required(VERSION 3.0.0)
project(Pinecast VERSION 0.1.0)
set(CMAKE_C_COMPILER /usr/bin/g++)
include(CTest)
find_package(PkgConfig)
include(CPack)
include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
add_executable(Pinecast main.cpp)
set (CMAKE_VERBOSE_MAKEFILE true)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Iinclude -lcurl -Wall")

target_include_directories(Pinecast PUBLIC "${PROJECT_BINARY_DIR}")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(curlpp REQUIRED curlpp)
# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
# Add other flags to the compiler
add_definitions(${GTK_CFLAGS_OTHER})
# Link the target to the GTK+ libraries
target_link_libraries(Pinecast ${GTK_LIBRARIES})

include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
# Add other flags to the compiler
add_definitions(${GTK_CFLAGS_OTHER})
# Link the target to the GTK+ libraries
target_link_libraries(Pinecast ${GTK_LIBRARIES})


include_directories(${curlpp_INCLUDE_DIRS})
link_directories(${curlpp_LIBRARY_DIRS})
# Add other flags to the compiler
add_definitions(${curlpp_CFLAGS_OTHER})
include_directories(${curlpp_INCLUDE_DIRS})
link_directories(curlpp_LIBRARY_DIRS})
# Add other flags to the compiler
add_definitions(${curlpp_CFLAGS_OTHER})
# Link the target to the GTK+ libraries
target_link_libraries(Pinecast ${curlpp_LIBRARIES})