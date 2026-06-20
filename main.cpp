#include <QApplication>
#include <QIcon>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(
            QIcon(":/images/oil.ico"));

    MainWindow w;

    w.setWindowIcon
            (QIcon(":/images/oil.ico"));

    w.show();

    return a.exec();
}
