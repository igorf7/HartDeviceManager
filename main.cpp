#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow* mainWindow = new MainWindow();

    mainWindow->resize(886, 575);
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);
    mainWindow->show();

    return a.exec();
}
