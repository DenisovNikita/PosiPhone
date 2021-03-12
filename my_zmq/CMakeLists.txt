cmake_minimum_required(VERSION 3.16)
project(my_zmq)

find_package(cppzmq)

set(CMAKE_CXX_STANDARD 17)

add_executable(my_zmq main.cpp)

target_link_libraries(my_zmq cppzmq)