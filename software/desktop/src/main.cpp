#include <iostream>

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QSerialPortInfo>
#include <QTimer>
#include <QDebug>

#ifdef Q_OS_MACX
#include "MacUtil.h"
#endif

#include "mainwindow.h"
#include "headlesstask.h"

namespace
{
HeadlessTask::Command headlessCommand = HeadlessTask::CommandUnknown;
QString headlessArg;
QString connectPort;
}

bool handleCommandLine(const QCoreApplication &app)
{
    // Setup the command line parser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    // Add specific command line options
    QCommandLineOption listOption(QStringList() << "l" << "list",
                                  QCoreApplication::translate("main", "List attached devices."));
    parser.addOption(listOption);

    QCommandLineOption portOption(QStringList() << "p" << "port",
                                  QCoreApplication::translate("main", "Connect to device at the selected port."),
                                  QCoreApplication::translate("main", "port"));
    parser.addOption(portOption);

    QCommandLineOption infoOption(QStringList() << "i" << "info",
                                  QCoreApplication::translate("main", "Query device system info."));
    parser.addOption(infoOption);

    QCommandLineOption exportOption(QStringList() << "export",
                                    QCoreApplication::translate("main", "Export device settings to file."),
                                    QCoreApplication::translate("main", "file"));
    parser.addOption(exportOption);

    // Parse the command line
    parser.process(app);

    if (parser.isSet(listOption)) {
        bool hasDevices = false;
        const auto infos = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &info : infos) {
            // Filter the list to only contain devices that match the VID/PID
            // actually assigned to the Printalyzer Densitometer
            if (!(info.vendorIdentifier() == 0x16D0 && info.productIdentifier() == 0x10EB)) {
                continue;
            }
            std::cout << info.portName().toStdString()
                      << " " << info.description().toStdString()
                      << " [" << info.serialNumber().toStdString() << "]"
                      << std::endl;
            hasDevices = true;
        }
        if (!hasDevices) {
            std::cout << "No attached devices." << std::endl;
        }

        return true;
    }

    QString portValue = parser.value(portOption);
    if (!portValue.isEmpty()) {
        std::cout << "Connecting to " << portValue.toStdString() << std::endl;
        connectPort = portValue;
    }

    if (parser.isSet(infoOption) && headlessCommand == HeadlessTask::CommandUnknown) {
        headlessCommand = HeadlessTask::CommandSystemInfo;
    }

    if (parser.isSet(exportOption) && headlessCommand == HeadlessTask::CommandUnknown) {
        headlessCommand = HeadlessTask::CommandExportSettings;
        headlessArg = parser.value(exportOption);
    }

    return false;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Printalyzer Densitometer Desktop");
    QApplication::setApplicationVersion("0.2");
    QApplication::setOrganizationName("Dektronics, Inc.");
    QApplication::setOrganizationDomain("dektronics.com");
    QApplication::setWindowIcon(QIcon(":/icons/appicon.png"));

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "densitometer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

#ifdef Q_OS_MACX
    /* Force light theme for UI consistency */
    if (macIsInDarkTheme()) {
        macSetToLightTheme();
    }
#endif

    if (handleCommandLine(a)) {
        return 0;
    }

    if (headlessCommand != HeadlessTask::CommandUnknown) {
        HeadlessTask *task = new HeadlessTask(&a);
        task->setPort(connectPort);
        task->setCommand(headlessCommand, headlessArg);
        QTimer::singleShot(0, task, &HeadlessTask::run);
        QObject::connect(task, &HeadlessTask::finished, &a, &QCoreApplication::quit);
        return a.exec();
    } else {
        MainWindow w;
        w.show();

        if (!connectPort.isEmpty()) {
            w.connectToPort(connectPort);
        }
        return a.exec();
    }
}
