#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui/view/visionflowview.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    VisionFlowView *view = new VisionFlowView(this);
    setCentralWidget(view);
    resize(1280,720);
}

MainWindow::~MainWindow()
{
    delete ui;
}

