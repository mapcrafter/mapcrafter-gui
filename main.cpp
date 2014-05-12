#include "mainwindow.h"

#include <QCoreApplication>
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("MapcrafterDev");
    QCoreApplication::setOrganizationDomain("mapcrafter.org");
    QCoreApplication::setApplicationName("MapcrafterSimpleGui");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
