QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    rocket.cpp \
    asio_server.cpp \
    client_session.cpp \
    client_request.cpp \
    client_request_parser.cpp \
    client_response.cpp \
    server_config.cpp \
    server_config_virtualhost.cpp \
    server_config_parser_base.cpp \
    server_config_json_parser.cpp \
    server_config_virtualhost_portinfo.cpp \
    http_response_templates.cpp \
    cache.cpp \
    cache_key.cpp \
    cache_content.cpp \
    cache_remove.cpp

# boost libraries build guide
# - download boost
# - depending on yout selected Qt compiler, open the visual studio command prompt and navigate into the boost directory
# - For static linking on windows x86, msvc2015 type:
# - b2.exe --toolset=msvc-15.0 address-model=32 --build-type=complete
# - For faster build times you can also choose to build only boost_system.
# - Update the related INCLUDEPATH and LIBS variables bellow
windows:INCLUDEPATH += "C:/Downloads/boost_1_66_0/"
windows:LIBS += "-LC:/Downloads/boost_1_66_0/stage/lib/"

mac:INCLUDEPATH += "/Users/yiannis/Projects/boost_1_66_0"
mac:LIBS += -L/Users/yiannis/Projects/boost_1_66_0/bin.v2/libs/system/build/darwin-gnu-4.2.1/release/threadapi-pthread/threading-multi -lboost_system

# readelf -d open-web-server-asio to view the executable dependencies
# if boost libraries are not installed you should install them
# method 1: sudo apt-get install libboost-all-dev
# #############
# method 2:
# - Downloads boost
# - build boost by typing:
# ./bootstrap.sh
# ./b2 install
# - For faster build times you can also choose to build only boost_system.
# - Update the related INCLUDEPATH and LIBS variables bellow
linux:INCLUDEPATH += "/home/pi/boost_1_66/boost_1_66_0" #for my raspberri pi
linux:LIBS += -L/home/pi/boost_1_66/boost_1_66_0/stage/lib/ #for my raspberri pi
linux:LIBS += -L/usr/lib/x86_64-linux-gnu -lboost_system

HEADERS += \
    rocket.h \
    asio_server.h \
    client_session.h \
    client_request.h \
    client_request_parser.h \
    client_response.h \
    server_config.h \
    server_config_virtualhost.h \
    qstring_hash_specialization.h \
    server_config_parser_base.h \
    server_config_json_parser.h \
    server_config_virtualhost_portinfo.h \
    http_response_templates.h \
    cache.h \
    cache_key.h \
    cache_content.h \
    cache_remove.h

