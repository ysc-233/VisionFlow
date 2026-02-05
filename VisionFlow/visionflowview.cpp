#include "visionflowview.h"
#include "ui_visionflowview.h"
#include "ui/flowrunner.h"
#include <QVBoxLayout>
VisionFlowView::VisionFlowView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisionFlowView)
{
    ui->setupUi(this);
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
        qDebug()<<__FUNCTION__<<"Start running!";
        FlowRunner::run(*m_flowEditorWidget->m_model);
    });
}
