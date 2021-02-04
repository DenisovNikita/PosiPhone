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

SOURCES += ./sioclient/src/sio_client.cpp
SOURCES += ./sioclient/src/internal/sio_packet.cpp

HEADERS  += ./sioclient/src/sio_client.h
HEADERS  += ./sioclient/src/sio_message.h

INCLUDEPATH += ./sioclient/lib/rapidjson/include
INCLUDEPATH += ./sioclient/lib/websocketpp
INCLUDEPATH += ./sioclient/lib/asio/asio/include
INCLUDEPATH += ./sioclient/lib/catch

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
