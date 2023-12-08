TEMPLATE = app
TARGET = qdyn2ex_test
CONFIG += console
CONFIG += warn_off
CONFIG+= debug
QT += widgets


# Include system-dependent variables.
!include(qdyn2exodus_constanza.pri) {
    EXODUS_LIBMESH = true
    EXODUS_LIBRARY = false
}

# Include system-dependent variables.
include(qdyn2exodus_constanza.pri)

# This you would need to set up according to your OS system
QMAKE_MACOSX_DEPLOYMENT_TARGET = 13.5
QMAKE_APPLE_DEVICE_ARCHS = arm64


if ($$EXODUS_LIBMESH){
INCLUDEPATH += .
INCLUDEPATH += $$LIBMESH/include
INCLUDEPATH += $$LIBMESH/include/libmesh
#INCLUDEPATH += /Users/crpiceda/mambaforge3/envs/moose/include # mamba moose installation
#INCLUDEPATH += /usr/lib/x86_64-linux-gnu/openmpi/include/
#INCLUDEPATH += /usr/lib/x86_64-linux-gnu/openmpi/include/openmpi
INCLUDEPATH += /opt/homebrew/Cellar/open-mpi/5.0.0/include/
INCLUDEPATH += /opt/homebrew/Cellar/open-mpi/5.0.0/include/openmpi
# INCLUDEPATH += /usr/src/linux-oem-5.10-headers-5.10.0-1014/include/linux/
LIBS += -L$$LIBMESH/lib
# LIBS += -L/usr/lib/x86_64-linux-gnu/openmpi/lib
# mac installation
LIBS += -L/opt/homebrew/Cellar/open-mpi/5.0.0/lib
LIBS+= -lmpi
LIBS += -lmesh_opt
LIBS += -ldl
LIBS += -lpthread
QMAKE_LFLAGS += -Wl,-rpath,$$LIBMESH/lib
}else:if($$EXODUS_LIBRARY){
    INCLUDEPATH += . $$EXODUS_PATH/packages/seacas/libraries/exodus/include
    LIBS += $$EXODUS_PATH/build/packages/seacas/libraries/exodus/libexodus.2.0.dylib
    INCLUDEPATH += . $$NETCDF_PATH/include
    LIBS += -L$$NETCDF_PATH/lib -lnetcdf
}else{
  DEFINES+=NOEXODUS
}

HEADERS += helper.h \
           geometry.h

SOURCES += main.cpp
