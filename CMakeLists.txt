cmake_minimum_required(VERSION 3.5)

project(ProducerConsumerExamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(condition_variable examples/condition_variable/main.cpp)
add_executable(bounded_buffer examples/bounded_buffer/main.cpp)
