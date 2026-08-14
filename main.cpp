#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("igorf7");
    app.setOrganizationDomain("igorf7.com");
    app.setApplicationName("HartDeviceManager");

    MainWindow w;
    w.resize(900, 600);
    w.show();
    return QApplication::exec();
}
