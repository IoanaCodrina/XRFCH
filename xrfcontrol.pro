######################################################################
# Automatically generated by qmake (2.01a) Thu Dec 19 10:27:13 2013
######################################################################

TEMPLATE = app
TARGET = xrfcontrol
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += /usr/include
LIBS += -L/usr/lib -lserial
OBJECTS_DIR = $(PWD)/obj

# Input
HEADERS += src/XRFMotor.h

FORMS += src/XRFGui.ui
SOURCES += src/XRFConnections.cpp \
           src/XRFInitialization.cpp \
           src/XRFMain.cpp \
           src/XRFMotor.cpp \
           src/XRFMovement.cpp 
