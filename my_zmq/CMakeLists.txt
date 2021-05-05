cmake_minimum_required(VERSION 3.16)
project(my_zmq)

find_package(cppzmq)
find_package(Boost 1.55 REQUIRED system serialization)

set(CMAKE_CXX_STANDARD 17)

add_executable(single_message main.cpp)
add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(single_message cppzmq)
target_link_libraries(server cppzmq)
target_link_libraries(client cppzmq)
target_link_libraries(server boost_serialization)
target_link_libraries(client boost_serialization)
