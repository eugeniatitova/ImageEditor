#-------------------------------------------------
#
# Project created by QtCreator 2017-10-10T20:35:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tools/abstracttool.cpp \
    tools/abstractareaselecting.cpp \   
    imagescene.cpp \
    widgets/toolbar.cpp \
    widgets/colordialogbutton.cpp \
    widgets/drawtextdialog.cpp \
    widgets/resizeimagedialog.cpp \
    settingsstore.cpp \
    customundocmd.cpp \
    imageedittools.cpp \
    tools/colorpickertool.cpp \
    tools/curvelinetool.cpp \
    tools/ellipsetool.cpp \
    tools/erasertool.cpp \
    tools/filltool.cpp \
    tools/linetool.cpp \
    tools/penciltool.cpp \
    tools/rectangletool.cpp \
    tools/selectiontool.cpp \
    tools/spraytool.cpp \
    tools/texttool.cpp

HEADERS  += mainwindow.h \
    tools/abstracttool.h \
    tools/abstractareaselecting.h \    
    tools/ellipsetool.h \
    tools/colorpickertool.h \
    tools/curvelinetool.h \
    tools/erasertool.h \
    tools/filltool.h \
    tools/linetool.h \
    tools/penciltool.h \
    tools/rectangletool.h \
    tools/selectiontool.h \
    tools/spraytool.h \
    tools/texttool.h \
    params.h \
    widgets/toolbar.h \
    widgets/colordialogbutton.h \
    widgets/drawtextdialog.h \
    widgets/resizeimagedialog.h \
    settingsstore.h \
    customundocmd.h \
    imagescene.h \
    imageedittools.h
