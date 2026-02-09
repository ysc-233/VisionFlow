#include "flowview.h"
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMimeData>
#include <QDrag>
using namespace QtNodes;

FlowView::FlowView(QtNodes::BasicGraphicsScene* scene,
                   QWidget* parent)
    : GraphicsView(scene, parent)
{
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
}

void FlowView::contextMenuEvent(QContextMenuEvent *event)
{
    auto s = dynamic_cast<QtNodes::BasicGraphicsScene*>(scene());

    if (!s)
        return;

    QPointF scenePos = mapToScene(event->pos());

    QMenu* menu = s->createSceneMenu(scenePos);

    if (menu)
        menu->exec(event->globalPos());
}

bool FlowView::viewportEvent(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::DragEnter:
    {
        auto* e = static_cast<QDragEnterEvent*>(event);
        e->acceptProposedAction();
        return true;
    }

    case QEvent::DragMove:
    {
        auto* e = static_cast<QDragMoveEvent*>(event);
        e->acceptProposedAction();
        return true;
    }

    case QEvent::Drop:
    {
        auto* e = static_cast<QDropEvent*>(event);
        auto scene = nodeScene();
        if (!scene)
            return false;

        QString nodeName = e->mimeData()->text();
        auto& graph = static_cast<DataFlowGraphModel&>(scene->graphModel());

        NodeId id = graph.addNode(nodeName);

        graph.setNodeData(
            id,
            NodeRole::Position,
            mapToScene(e->pos()));

        e->acceptProposedAction();
        return true;
    }

    default:
        break;
    }

    return GraphicsView::viewportEvent(event);
}
