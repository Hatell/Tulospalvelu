#-------------------------------------------------
#
# Project created by QtCreator 2012-07-01T13:39:50
#
#-------------------------------------------------

QT       += core gui sql

TARGET = Tulospalvelu
TEMPLATE = app

# Uncomment to use mysql instead of sqlie
#DEFINES += USE_MYSQL

# Uncomment to use serialport
DEFINES += USE_SERIAL

CONFIG += serialport # Qt 4

#QT += serialport     # Qt 5

# Linux 64bit
INCLUDEPATH += /usr/lib64/qt4/include


SOURCES += main.cpp\
        mainwindow.cpp \
    widget/testemitreaderwidget.cpp \
    model/emitdatamodel.cpp \
    type/rastidata.cpp \
    widget/serialemitreaderwidget.cpp \
    form/sarjatform.cpp \
    type/tapahtuma.cpp \
    type/sarja.cpp \
    type/rasti.cpp \
    type/tulos.cpp \
    type/valiaika.cpp \
    dialog/tietojadialog.cpp \
    type/tietokanta.cpp \
    dialog/tapahtumadialog.cpp \
    form/tuloksetform.cpp \
    form/tulosform.cpp \
    form/pikanappaimetform.cpp

HEADERS  += mainwindow.h \
    widget/testemitreaderwidget.h \
    model/emitdatamodel.h \
    type/rastidata.h \
    widget/serialemitreaderwidget.h \
    makrot.h \
    form/sarjatform.h \
    type/tapahtuma.h \
    type/sarja.h \
    type/rasti.h \
    type/tulos.h \
    type/valiaika.h \
    dialog/tietojadialog.h \
    type/tietokanta.h \
    dialog/tapahtumadialog.h \
    form/tuloksetform.h \
    form/tulosform.h \
    form/pikanappaimetform.h

FORMS    += mainwindow.ui \
    widget/testemitreaderwidget.ui \
    widget/serialemitreaderwidget.ui \
    form/sarjatform.ui \
    dialog/tietojadialog.ui \
    dialog/tapahtumadialog.ui \
    form/tuloksetform.ui \
    form/tulosform.ui \
    form/pikanappaimetform.ui





















































