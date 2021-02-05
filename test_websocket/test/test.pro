QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += no_keywords

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp


HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += ./socket.io-client-cpp/lib/asio/asio/include
INCLUDEPATH += ./socket.io-client-cpp/lib/websocketpp
INCLUDEPATH += ./socket.io-client-cpp/lib/rapidjson/include
INCLUDEPATH += ./socket.io-client-cpp/src

SOURCES += ./socket.io-client-cpp/src/sio_client.cpp \
    ./socket.io-client-cpp/src/sio_socket.cpp \
    ./socket.io-client-cpp/src/internal/sio_client_impl.cpp \
    ./socket.io-client-cpp/src/internal/sio_packet.cpp

HEADERS += ./socket.io-client-cpp/src/sio_client.h \
    ./socket.io-client-cpp/src/sio_socket.h \
    ./socket.io-client-cpp/src/sio_message.h \
    ./socket.io-client-cpp/src/internal/sio_client_impl.h \
    ./socket.io-client-cpp/src/internal/sio_packet.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
