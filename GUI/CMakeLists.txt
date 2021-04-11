cmake_minimum_required(VERSION 3.5)
project(GUI LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_definitions(-DQT_NO_KEYWORDS)

find_package(Qt5 COMPONENTS Widgets Multimedia REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        gui.h
        message.h
        model.h
        moveitem.h
        gui.cpp
        model.cpp
        moveitem.cpp
        queueconsumer.h
)

set(PROJECT_LIBS
        folly
        dl
        glog
        pthread
        gflags
        event
        fmt
        double-conversion
        iberty
)

add_executable(GUI ${PROJECT_SOURCES})
target_link_libraries(GUI PRIVATE Qt5::Widgets Qt5::Multimedia ${PROJECT_LIBS})
