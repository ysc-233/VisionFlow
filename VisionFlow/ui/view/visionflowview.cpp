#include "visionflowview.h"
#include "ui_visionflowview.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>
#include "flow/executor/flowrunner.h"
#include "ui/component/NodeLibrary/nodelibrarywidget.h"

VisionFlowView::VisionFlowView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisionFlowView)
{
    ui->setupUi(this);

    qDebug()<<__FUNCTION__<< QThread::currentThreadId() << "Start...";
    m_flowEditorWidget = new FlowEditorWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(ui->gpb_workspace);
    lay->addWidget(m_flowEditorWidget);

    NodeLibraryWidget *m_nodeLibraryWidget = new NodeLibraryWidget(this);
    QVBoxLayout *lay_nodeLib = new QVBoxLayout(ui->gpb_nodeLibrary);
    lay_nodeLib->addWidget(m_nodeLibraryWidget);
        createMenu();
    setConnections();
}

VisionFlowView::~VisionFlowView()
{
    delete ui;
}

void VisionFlowView::setConnections()
{
    connect(ui->btn_run, &QPushButton::clicked,this, [this]()
    {
        auto page = m_flowEditorWidget->currentPage();
        if (page && page->model) {
            FlowRunner::run(*page->model);
        }
    });

    connect(ui->btn_Stop, &QPushButton::clicked,this, [this]()
    {
        FlowRunner::stop();
    });
}

void VisionFlowView::createMenu()
{
    m_menuBar = new QMenuBar(this);

    // File 菜单
    m_fileMenu = m_menuBar->addMenu(tr("File"));
    m_actNew    = new QAction(tr("New"), this);
    m_actOpen   = new QAction(tr("Open"), this);
    m_actSave   = new QAction(tr("Save"), this);
    m_actSaveAs = new QAction(tr("Save As"), this);

    m_fileMenu->addAction(m_actNew);
    m_fileMenu->addAction(m_actOpen);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_actSave);
    m_fileMenu->addAction(m_actSaveAs);

    // 添加到预留的菜单容器
    ui->layout_menu->layout()->addWidget(m_menuBar);

    // 设置菜单栏样式，使其占满宽度
    m_menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(m_actNew, &QAction::triggered, this, [this]() {
        m_flowEditorWidget->newFlow();
        m_currentFile.clear();
    });

    connect(m_actOpen, &QAction::triggered, this, [this]() {
        QString file = QFileDialog::getOpenFileName(
            this, "Open Workflow", "", "Workflow (*.json)");
        if (!file.isEmpty()) {
             m_flowEditorWidget->loadFlow(file);
            m_currentFile = file;
        }
    });

    connect(m_actSave, &QAction::triggered, this, [this]() {
        if (m_currentFile.isEmpty()) {
            m_actSaveAs->trigger();
            return;
        }
         m_flowEditorWidget->saveFlow(m_currentFile);
    });

    connect(m_actSaveAs, &QAction::triggered, this, [this]() {
        QString file = QFileDialog::getSaveFileName(
            this, "Save Workflow", "", "Workflow (*.json)");
        if (!file.isEmpty()) {
             m_flowEditorWidget->saveFlow(file);
            m_currentFile = file;
        }
    });
}
