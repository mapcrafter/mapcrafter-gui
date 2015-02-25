#include "mainwindow.h"

#include <mapcraftercore/util.h>

#include <QCoreApplication>
#include <QApplication>
#include <QtCore/QMetaType>

using namespace mapcrafter;
Q_DECLARE_METATYPE(util::LogMessage);

int main(int argc, char *argv[])
{
    qRegisterMetaType<util::LogMessage>("util::LogMessage");

    QCoreApplication::setOrganizationName("Mapcrafter");
    QCoreApplication::setOrganizationDomain("mapcrafter.org");
    QCoreApplication::setApplicationName("Mapcrafter GUI");
    QCoreApplication::setApplicationVersion("0.1");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
