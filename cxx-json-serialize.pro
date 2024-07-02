QT -= gui
TARGET = cxx_json_serialize

CONFIG += c++11 console
CONFIG -= app_bundle
CONFIG += c11

QMAKE_CXXFLAGS = -fpermissive
!win32-msvc*:QMAKE_CXXFLAGS = -save-temps

win32-msvc*:QMAKE_CXXFLAGS += /utf-8
win32-msvc*:QMAKE_CXXFLAGS += /wd"4819"
win32-msvc*:QMAKE_CXXFLAGS += /FS

QMAKE_CXXFLAGS_RELEASE -= -g
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CXXFLAGS_RELEASE += -O0

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp    \
        utils/json/cJSON.c \
        utils/json/cxxJson.cpp

HEADERS += \
        utils/json/cJSON.h \
        utils/json/cxxJson.h

# test
SOURCES += \
        test/lsblk/tstlsblk.cpp \
        test/base/tstbase.cpp

HEADERS += \
        test/lsblk/infolsblk.h   \
        test/lsblk/tstlsblk.h \
        test/base/tstbase.h

win32{
    CONFIG(debug, debug|release) {
        CONFIG += console

        win32-msvc*:exists("C:/VLD/include/vld.h") {
            INCLUDEPATH += C:/VLD/include
            DEPENDPATH += C:/VLD/include
            LIBS += -LC:/VLD/lib/Win64 -lvld
            DEFINES += HAS_WIN32_VLD
        }

    }
}



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
