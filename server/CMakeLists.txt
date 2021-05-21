cmake_minimum_required(VERSION 3.16)
project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(folly REQUIRED)
find_package(gflags REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.51.0 COMPONENTS system serialization REQUIRED)

find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY NAMES zmq)

add_executable(server
    client.cpp
    server.cpp
    server_main.cpp
    ../src/GUI/message.cpp
)

target_include_directories(server PRIVATE
    ../include/GUI/
    ../include/Network/
    ${Boost_INCLUDE_DIRS}
    ${FOLLY_INCLUDE_DIR}
    ${ZMQ_INCLUDE_DIR}
)

target_link_libraries(server PRIVATE
    ${Boost_LIBRARIES}
    ${FOLLY_LIBRARIES}
    ${ZMQ_LIBRARY}
)
