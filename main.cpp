#include "mainwindow.h"

#include <QCoreApplication>
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Mapcrafter");
    QCoreApplication::setOrganizationDomain("mapcrafter.org");
    QCoreApplication::setApplicationName("Mapcrafter GUI");
    QCoreApplication::setApplicationVersion("0.1");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
