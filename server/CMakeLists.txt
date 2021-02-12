cmake_minimum_required(VERSION 3.16.3)
project(chat_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER "clang++-10")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
set(CMAKE_CXX_FLAGS "--stdlib=libc++ -Xclang")

find_package(Boost COMPONENTS system)

include_directories(
        ${BOOST_INCLUDE_DIRS}
)

add_executable(chat_server chat_server.cpp)

target_link_libraries(chat_server
        ${Boost_LIBRARIES}
        pthread
        )