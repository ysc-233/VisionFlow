#include "floweditorwidget.h"
#include <QVBoxLayout>
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/GraphicsView>
#include <QtDebug>
#include <QThread>
#include <QtNodes/NodeDelegateModelRegistry>
#include "nodes/flownodeautoregister.h"
#include "flowview.h"
#include "flowscene.h"
FlowEditorWidget::FlowEditorWidget(QWidget *parent)
    : QWidget(parent)
{
    using namespace QtNodes;
    qDebug()<<__FUNCTION__<< QThread::currentThreadId() << "Start...";
    // 创建 Registry
    m_registry = std::make_shared<NodeDelegateModelRegistry>();
    NodeAutoRegistry::instance().registerAll(*m_registry);
    // 创建 Graph Model
    m_model = new DataFlowGraphModel(m_registry);
    // Scene
    m_scene = new FlowScene(*m_model,m_registry,this);
    // View
    m_view = new FlowView(m_scene,this);
    m_view->setRenderHint(QPainter::Antialiasing);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_view);
}
