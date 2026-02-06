#include "visionflowview.h"
#include "ui_visionflowview.h"
#include "flow/executor/flowrunner.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QThread>
VisionFlowView::VisionFlowView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisionFlowView)
{
    ui->setupUi(this);
    qDebug()<<__FUNCTION__<< QThread::currentThreadId() << "Start...";
    m_flowEditorWidget = new FlowEditorWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(ui->gpb_workspace);
    lay->addWidget(m_flowEditorWidget);
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
        FlowRunner::run(*m_flowEditorWidget->m_model);
    });

    connect(ui->btn_Stop, &QPushButton::clicked,this, [this]()
    {
        FlowRunner::stop();
    });
}
