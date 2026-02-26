#include "mainwindow.h"

#include <QApplication>
#include "flow/flowtypes.h"
#include "utils/logmanager.h"
int main(int argc, char *argv[])
{
    qRegisterMetaType<MatPtr>("MatPtr");
    qRegisterMetaType<LogLevel>("LogLevel");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
