TEMPLATE = app

QT += qml quick widgets quick-private widgets gui multimedia

SOURCES += main.cpp \
    bone.cpp \
    bonetransform.cpp \
    ikjoint.cpp \
    rendercmdscache.cpp \
    spinemeshitem.cpp \
    spinemeshrenderer.cpp \
    spineskinnedmeshitem.cpp \
    bone.cpp \
    bonetransform.cpp \
    ikjoint.cpp \
    rendercmdscache.cpp \
    spinemeshitem.cpp \
    spinemeshrenderer.cpp \
    spineskinnedmeshitem.cpp

INCLUDEPATH += c:\Qt\5.3\msvc2010_opengl\include\QtQuick\5.3.1\QtQuick\
INCLUDEPATH += c:\Qt\5.3\msvc2010_opengl\include\QtQml\5.3.1\QtQml\
INCLUDEPATH += c:\Qt\5.3\msvc2010_opengl\include\QtCore\5.3.1\
INCLUDEPATH += c:\Qt\5.3\msvc2010_opengl\include\QtCore\5.3.1\QtCore\

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    bone.h \
    bonetransform.h \
    ikjoint.h \
    rendercmdscache.h \
    screenrecorder.h \
    spinemeshitem.h \
    spinemeshrenderer.h \
    spineskinnedmeshitem.h \
    bone.h \
    bonetransform.h \
    ikjoint.h \
    rendercmdscache.h \
    spinemeshitem.h \
    spinemeshrenderer.h \
    spineskinnedmeshitem.h

