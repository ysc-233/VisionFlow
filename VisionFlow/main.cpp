#include "mainwindow.h"

#include <QApplication>
#include "flow/flowtypes.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<MatPtr>("MatPtr");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
