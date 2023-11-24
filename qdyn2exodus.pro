TEMPLATE = app
TARGET = qdyn2ex
CONFIG += console
CONFIG += warn_off
CONFIG+= debug
QT += widgets

# Include system-dependent variables.
include(qdyn2exodus.pri)

INCLUDEPATH += .
INCLUDEPATH += $$LIBMESH/include
INCLUDEPATH += $$LIBMESH/include/libmesh
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/openmpi/include/
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/openmpi/include/openmpi
# INCLUDEPATH += /usr/src/linux-oem-5.10-headers-5.10.0-1014/include/linux/
LIBS += -L$$LIBMESH/lib
LIBS += -L/usr/lib/x86_64-linux-gnu/openmpi/lib
LIBS+= -lmpi
LIBS += -lmesh_opt
LIBS += -ldl
LIBS += -lpthread

QMAKE_LFLAGS += -Wl,--copy-dt-needed-entries,-rpath,$$LIBMESH/lib


HEADERS += helper.h \
           geometry.h

SOURCES += main.cpp
