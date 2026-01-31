#include "mainwindow.h"

#include <QApplication>
#include <modules/test/testdemo.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    testdemo test;

    return a.exec();
}
