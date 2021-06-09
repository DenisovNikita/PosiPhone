cmake_minimum_required(VERSION 3.16)
project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-static)

find_package(folly REQUIRED)
find_package(gflags REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.51.0 REQUIRED COMPONENTS system serialization)

find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY NAMES zmq)

add_executable(${PROJECT_NAME}
    config.txt
    mixer.cpp
    mixer.h
    server.cpp
    server_network_module.h
    server_network_module.cpp

    ../common/client_fwd.h
    ../common/common_server.cpp
    ../common/message.cpp
    ../common/message.h
    ../common/network_utils.cpp
    ../common/network_utils.h
    ../common/server_fwd.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../common/
    ../client/
    ${Boost_INCLUDE_DIRS}
    ${FOLLY_INCLUDE_DIR}
    ${ZMQ_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Boost_LIBRARIES}
    ${FOLLY_LIBRARIES}
    ${ZMQ_LIBRARY}
)
