cmake_minimum_required(VERSION 3.16)
project(PosiPhone LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-static)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_FOREACH")

find_package(folly REQUIRED)
find_package(gflags REQUIRED)
find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets Multimedia REQUIRED)
find_package(Boost 1.51.0 REQUIRED COMPONENTS system serialization)

find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY NAMES zmq)

add_executable(${PROJECT_NAME}
    audio.cpp
    audio.h
    audio_button.cpp
    audio_button.h
    client.cpp
    client.h
    login_widget.cpp
    login_widget.h
    main.cpp
    mock.cpp
    mock.h
    model.cpp
    model_fwd.h
    model.h
    move_item.cpp
    move_item.h
    user.cpp
    user.h
    view.cpp
    view.h
    view.ui

    ../common/message.cpp
    ../common/message.h
    ../common/network_utils.cpp
    ../common/network_utils.h

    ../resources/resources.qrc
)

qt5_use_modules(${PROJECT_NAME} Widgets Multimedia)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../common/
    ../resources/
    ${Boost_INCLUDE_DIRS}
    ${FOLLY_INCLUDE_DIR}
    ${ZMQ_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${FOLLY_LIBRARIES}
    ${QT_LIBRARIES}
    ${ZMQ_LIBRARY}
)
