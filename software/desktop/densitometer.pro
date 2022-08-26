#-------------------------------------------------------------------------------
# Qt and Make options
#-------------------------------------------------------------------------------

QT += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

#-------------------------------------------------------------------------------
# Compiler options
#-------------------------------------------------------------------------------

!win32 {
    QMAKE_CXXFLAGS += -Wno-deprecated-copy
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#-------------------------------------------------------------------------------
# Deploy options
#-------------------------------------------------------------------------------

win32* {
    RC_FILE = deploy/windows/resources/info.rc
}

macx* {
    ICON = deploy/macOS/icon.icns
    RC_FILE = deploy/macOS/icon.icns
    QMAKE_INFO_PLIST = deploy/macOS/info.plist
    CONFIG += sdk_no_version_check # To avoid warnings with Big Sur
    HEADERS += src/MacUtil.h
    OBJECTIVE_SOURCES += src/MacUtil.mm
}

linux:!android {
    TARGET = densitometer

    target.path = /usr/bin
    icon.path = /usr/share/pixmaps
    desktop.path = /usr/share/applications
    icon.files += deploy/linux/*.png
    desktop.files += deploy/linux/*.desktop

    INSTALLS += target desktop icon
}

#-------------------------------------------------------------------------------
# Import source code
#-------------------------------------------------------------------------------

SOURCES += \
    src/connectdialog.cpp \
    src/denscalvalues.cpp \
    src/denscommand.cpp \
    src/densinterface.cpp \
    src/floatitemdelegate.cpp \
    src/gaincalibrationdialog.cpp \
    src/headlesstask.cpp \
    src/logger.cpp \
    src/logwindow.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/remotecontroldialog.cpp \
    src/settingsexporter.cpp \
    src/settingsimportdialog.cpp \
    src/slopecalibrationdialog.cpp \
    src/util.cpp \
    src/qsimplesignalaggregator.cpp

HEADERS += \
    src/connectdialog.h \
    src/denscalvalues.h \
    src/denscommand.h \
    src/densinterface.h \
    src/floatitemdelegate.h \
    src/gaincalibrationdialog.h \
    src/headlesstask.h \
    src/logger.h \
    src/logwindow.h \
    src/mainwindow.h \
    src/remotecontroldialog.h \
    src/settingsexporter.h \
    src/settingsimportdialog.h \
    src/slopecalibrationdialog.h \
    src/util.h \
    src/qsignalaggregator.h \
    src/qsimplesignalaggregator.h

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

#-------------------------------------------------------------------------------
# Deploy files
#-------------------------------------------------------------------------------

OTHER_FILES += \
    deploy/linux/* \
    deploy/macOS/* \
    deploy/windows/nsis/* \
    deploy/windows/resources/*
