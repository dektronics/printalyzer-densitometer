QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

!win32 {
    QMAKE_CXXFLAGS += -Wno-deprecated-copy
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/connectdialog.cpp \
    src/denscommand.cpp \
    src/densinterface.cpp \
    src/floatitemdelegate.cpp \
    src/gaincalibrationdialog.cpp \
    src/logger.cpp \
    src/logwindow.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/remotecontroldialog.cpp \
    src/settingsexporter.cpp \
    src/settingsimportdialog.cpp \
    src/slopecalibrationdialog.cpp \
    src/util.cpp

HEADERS += \
    src/connectdialog.h \
    src/denscommand.h \
    src/densinterface.h \
    src/floatitemdelegate.h \
    src/gaincalibrationdialog.h \
    src/logger.h \
    src/logwindow.h \
    src/mainwindow.h \
    src/remotecontroldialog.h \
    src/settingsexporter.h \
    src/settingsimportdialog.h \
    src/slopecalibrationdialog.h \
    src/util.h

FORMS += \
    src/connectdialog.ui \
    src/gaincalibrationdialog.ui \
    src/logwindow.ui \
    src/mainwindow.ui \
    src/remotecontroldialog.ui \
    src/settingsimportdialog.ui \
    src/slopecalibrationdialog.ui

RESOURCES += \
    assets/densitometer.qrc

TRANSLATIONS += \
    assets/translations/densitometer_en_US.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
