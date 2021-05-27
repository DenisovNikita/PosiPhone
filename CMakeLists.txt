cmake_minimum_required(VERSION 3.16)
project(PosiPhone LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    include/GUI/audio.h
    include/GUI/login_widget.h
    include/GUI/message.h
    include/GUI/mock.h
    include/GUI/model_fwd.h
    include/GUI/model.h
    include/GUI/move_item.h
    include/GUI/producer.h
    include/GUI/user.h
    include/GUI/view.h
    include/Network/client.h
    include/Network/client_fwd.h
    include/Network/server.h
    include/Network/server_fwd.h
    include/GUI/view.ui
    resources/resources.qrc
    src/GUI/audio.cpp
    src/GUI/login_widget.cpp
    src/GUI/main.cpp
    src/GUI/message.cpp
    src/GUI/mock.cpp
    src/GUI/model.cpp
    src/GUI/move_item.cpp
    src/GUI/producer.cpp
    src/GUI/user.cpp
    src/GUI/view.cpp
    src/Network/client.cpp
    src/Network/common_server.cpp
    server/network_utils.cpp
)

qt5_use_modules(${PROJECT_NAME} Widgets Multimedia)

target_include_directories(${PROJECT_NAME} PRIVATE
    include/GUI/
    include/Network/
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
