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

add_executable(server server.cpp ../src/Network/client.cpp)

target_include_directories(server PRIVATE
        ../include/GUI/
        ../include/Network/
        ${ZMQ_INCLUDE_DIR}
        ${FOLLY_INCLUDE_DIR}
        )

target_link_libraries(server PRIVATE
        ${Boost_LIBRARIES}
        pthread
        ${QT_LIBRARIES}
        ${ZMQ_LIBRARY}
        ${FOLLY_LIBRARIES}
        )
