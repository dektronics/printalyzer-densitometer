#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#ifdef Q_OS_MACX
#include "MacUtil.h"
#endif

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

    MainWindow w;
    w.show();
    return a.exec();
}
