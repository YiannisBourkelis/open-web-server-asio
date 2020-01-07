#should check gcc Profile guided optimization
#https://en.wikipedia.org/wiki/Profile-guided_optimization

QT -= gui

CONFIG += c++17 console
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

#Disables ssl
#DEFINES += NO_ENCRYPTION

SOURCES += main.cpp \
    rocket.cpp \
    client_request.cpp \
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
    cache_remove.cpp \
    client_session_base.cpp \
    client_session_plain.cpp \
    client_session_encrypted.cpp \
    asio_server_base.cpp \
    asio_server_plain.cpp \
    asio_server_encrypted.cpp \
    cgi_service.cpp

# boost libraries build guide
# - download boost
# - depending on yout selected Qt compiler, open the visual studio command prompt and navigate into the boost directory
# - For static linking on windows x86, msvc2015 type:
# - b2.exe --toolset=msvc-15.0 address-model=32 --build-type=complete
# - For faster build times you can also choose to build only boost_system.
# - Update the related INCLUDEPATH and LIBS variables bellow
windows:INCLUDEPATH += "C:/Downloads/boost_1_66_0/"
windows:LIBS += "-LC:/Downloads/boost_1_66_0/stage/lib/"

#mac:INCLUDEPATH += "/Users/yiannis/Projects/boost_1_66_0"
#mac:LIBS += -L/Users/yiannis/Projects/boost_1_66_0/bin.v2/libs/system/build/darwin-gnu-4.2.1/release/threadapi-pthread/threading-multi -lboost_system
mac:INCLUDEPATH += "/usr/local/Cellar/boost/1.72.0/include/"
mac:LIBS += -L/usr/local/Cellar/boost/1.72.0/lib/ -lboost_system
mac:LIBS += -L/usr/local/Cellar/boost/1.72.0/lib/ -lboost_thread-mt

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
    client_request.h \
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
    cache_remove.h \
    client_session_base.h \
    client_session_plain.h \
    client_session_encrypted.h \
    asio_server_base.h \
    asio_server_plain.h \
    asio_server_encrypted.h \
    cgi_service.h \
    json.hpp \
    parser_enums.h


#***************************************************************
#openssl library

# osx compile guide:
# (https://gist.github.com/tmiz/1441111)
# open a terminal window in OpenSSL-x.x folder
# osx build commands:
# ./Configure darwin64-x86_64-cc -shared
# make clean
# make
#macx:LIBS += -L/Users/yiannis/Projects/poll-echo-client-server-gihub/poll-echo-client-server/src/OpenSSL-1.1.0g/ -lcrypto
#macx:LIBS += -L/Users/yiannis/Projects/poll-echo-client-server-gihub/poll-echo-client-server/src/OpenSSL-1.1.0g/ -lssl
#macx:INCLUDEPATH += /Users/yiannis/Projects/poll-echo-client-server-gihub/poll-echo-client-server/src/OpenSSL-1.1.0g/include/
macx:LIBS += -L/usr/local/Cellar/openssl@1.1/1.1.1d/lib/ -lcrypto
macx:LIBS += -L/usr/local/Cellar/openssl@1.1/1.1.1d/lib/ -lssl
macx:INCLUDEPATH += /usr/local/Cellar/openssl@1.1/1.1.1d/include/


# windows compile guide:
# install activeperl - https://www.activestate.com/activeperl
# install nasm - http://www.nasm.us/
# go inside OpenSSL-x.x folder from visual studio 2010 command line prompt
# type :
# 1)    perl.exe Configure VC-WIN32
# 2)    nmake clean
# 3)    nmake
# this should compile openssl on windows
#windows:LIBS += -LC:/Users/Yiannis/Documents/GitHub/poll-echo-client-server/src/OpenSSL-1.1.0g -llibcrypto
#windows:LIBS += -LC:/Users/Yiannis/Documents/GitHub/poll-echo-client-server/src/OpenSSL-1.1.0g -llibssl
#windows:INCLUDEPATH += "C:/Users/Yiannis/Documents/GitHub/poll-echo-client-server/src/OpenSSL-1.1.0g/include/"

#windows:LIBS += -L$$PWD/../OpenSSL-1.1.0g/ -llibcrypto
#windows:LIBS += -L$$PWD/../OpenSSL-1.1.0g/ -llibssl
#windows:INCLUDEPATH += $$PWD/../OpenSSL-1.1.0g/include/

windows:LIBS += -LC:/Users/Yiannis/Documents/GitHub/OpenSSL-1.1.0g/ -llibcrypto
windows:LIBS += -LC:/Users/Yiannis/Documents/GitHub/OpenSSL-1.1.0g/ -llibssl
windows:INCLUDEPATH += C:/Users/Yiannis/Documents/GitHub/OpenSSL-1.1.0g/include/

# linux compile guide:
# go inside OpenSSL-x.x folder using the terminal
# type:
# 1)    ./config
# 2)    make clean
# 3)    make
# this should compile openssl on linux
linux:LIBS += -L$$PWD/../OpenSSL-1.1.0g -lssl
linux:LIBS += -L$$PWD/../OpenSSL-1.1.0g -lcrypto
linux:INCLUDEPATH += $$PWD/../OpenSSL-1.1.0g/include

linux:LIBS += -ldl
#***************************************************************
