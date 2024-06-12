QT       += core gui
DESTDIR = bin

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    framebuffer.cpp \
    main.cpp \
    mainwindow.cpp \
    videodecoder.cpp \
    videodisplay.cpp \
    videosaver.cpp

HEADERS += \
    framebuffer.h \
    mainwindow.h \
    videodecoder.h \
    videodisplay.h \
    videosaver.h


FORMS += \
    mainwindow.ui
win32 {
    LIBS += -L$$PWD/lib/SDL2/lib/x86 \
        -L$$PWD/lib/ffmpeg-4.2.1-win32-dev/lib \
        -lSDL2 \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lswresample \
        -lswscale

    INCLUDEPATH += src \
        lib/SDL2/include \
        lib/ffmpeg-4.2.1-win32-dev/include
    }


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
