SOURCES += litepipe_client.cpp \
           main.cpp \
 ../../client.c \
 ../../communicator.c \
 ../../protocol.c \
 ../../server.c
HEADERS += litepipe_client.h
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt \
 debug
TARGET = litepipe_client
DESTDIR = ../bin
RESOURCES = application.qrc

INCLUDEPATH += ../..

CONFIG -= release

