#include "mainwindow.h"

#include <QApplication>
#include <modules/test/testdemo.h>
#include "ui/main_editor/main_editor_widget.h"
#include "modules/test/register_test_modules.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    testdemo test;
    // 设置应用程序信息
    a.setApplicationName("VisionFlow");
    a.setOrganizationName("VisionFlow");

    // 注册测试模块
    registerTestModules();

    // 创建主窗口
    MainEditorWidget mainWindow;
    mainWindow.setWindowTitle("VisionFlow 流程图编辑器");
    mainWindow.resize(1400, 900);
    mainWindow.show();

    return a.exec();
}
