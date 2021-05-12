cmake_minimum_required(VERSION 3.16)
project(GUI LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_FOREACH")

find_package(folly REQUIRED)
find_package(gflags REQUIRED)
find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets Multimedia REQUIRED)
find_package(Boost 1.55 COMPONENTS system serialization REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    audio.cpp audio.h
    login_widget.cpp login_widget.h
    message.cpp message.h
    mock.cpp mock.h
    model.cpp model.h model_fwd.h
    move_item.cpp move_item.h
    producer.cpp producer.h
    view.cpp view.h
    user.cpp user.h
)

qt5_use_modules(${PROJECT_NAME} Widgets Multimedia)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${folly_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    ${FOLLY_LIBRARIES}
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
)
