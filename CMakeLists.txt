cmake_minimum_required(VERSION 3.5)
project(concurrency LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
add_executable(concurrency src/main.cpp)

target_sources(concurrency PRIVATE 
    src/thread_pool/thread_pool.hpp 
    src/thread_pool/thread_pool.cpp
    src/thread_pool/task.hpp
    src/thread_pool/queue.hpp)

add_subdirectory(function2)

target_link_libraries(concurrency function2)