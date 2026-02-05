#include "flowscene.h"
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/Definitions>

#include <QMenu>
#include <QWidgetAction>
#include "nodesearchwidget.h"
using namespace QtNodes;

FlowScene::FlowScene(
    AbstractGraphModel& model,
    std::shared_ptr<NodeDelegateModelRegistry> registry,
    QObject* parent)
    : BasicGraphicsScene(model, parent),
      m_registry(registry)
{
}

QMenu* FlowScene::createSceneMenu(QPointF const scenePos)
{
    using namespace QtNodes;

    QMenu* menu = new QMenu;

    auto& graph =
        static_cast<DataFlowGraphModel&>(graphModel());

    QStringList modelNames;

    for (auto const& kv :
         m_registry->registeredModelCreators())
    {
        modelNames << kv.first;
    }

    auto* widget =
        new NodeSearchWidget(modelNames);

    QWidgetAction* action = new QWidgetAction(menu);
    action->setDefaultWidget(widget);

    menu->addAction(action);

    connect(widget, &NodeSearchWidget::nodeSelected,
                [&, scenePos, menu](QString name)
    {
        NodeId id = graph.addNode(name);

        graph.setNodeData(
            id,
            NodeRole::Position,
            scenePos);

        menu->close();
    });

    return menu;
}
