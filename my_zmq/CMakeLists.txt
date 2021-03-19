cmake_minimum_required(VERSION 3.16)
project(my_zmq)

find_package(cppzmq)

set(CMAKE_CXX_STANDARD 17)

add_executable(single_message main.cpp)
add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(single_message cppzmq)
target_link_libraries(server cppzmq)
target_link_libraries(client cppzmq)
