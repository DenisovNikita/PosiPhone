cmake_minimum_required(VERSION 3.16)
project(my_zmq LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.51.0 COMPONENTS system serialization REQUIRED)

find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY NAMES zmq)

add_executable(client src/client.cpp src/my_utils.cpp)
add_executable(server src/server.cpp src/my_utils.cpp)

target_include_directories(client PRIVATE
    include/
    ${ZMQ_INCLUDE_DIR}
)
target_include_directories(server PRIVATE
    include/
    ${ZMQ_INCLUDE_DIR}
)

target_link_libraries(client PRIVATE
    boost_serialization
    pthread
    ${ZMQ_LIBRARY}
)
target_link_libraries(server PRIVATE
    boost_serialization
    pthread
    ${ZMQ_LIBRARY}
)
