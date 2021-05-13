cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_COMPILER g++-10)

project(my_zmq)

find_package(Boost 1.55 REQUIRED system serialization)

set(CMAKE_CXX_STANDARD 17)

add_executable(single_message main.cpp)
add_executable(server src/server.cpp src/my_utils.cpp)
add_executable(client src/client.cpp src/my_utils.cpp)

include_directories(include)

target_include_directories(single_message PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_include_directories(server PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_include_directories(client PUBLIC ${ZeroMQ_INCLUDE_DIR})

target_link_libraries(single_message PUBLIC ${ZeroMQ_LIBRARY})
target_link_libraries(server PUBLIC ${ZeroMQ_LIBRARY})
target_link_libraries(client PUBLIC ${ZeroMQ_LIBRARY})
target_link_libraries(server PUBLIC boost_serialization)
target_link_libraries(client PUBLIC boost_serialization)

