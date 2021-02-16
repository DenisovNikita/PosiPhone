cmake_minimum_required(VERSION 3.9)
project(chat_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
set(CMAKE_CXX_FLAGS "-fcoroutines-ts --stdlib=libc++")# -Xclang")

find_package(Boost COMPONENTS system)

include_directories(
        ${BOOST_INCLUDE_DIRS}
)


add_executable(chat_server chat_server.cpp)

message(${Boost_LIBRARIES})

target_link_libraries(chat_server
        ${Boost_LIBRARIES}
        pthread
        )
