#include "flowview.h"
#include <QtNodes/BasicGraphicsScene>
#include <QMenu>
#include <QContextMenuEvent>
void FlowView::contextMenuEvent(QContextMenuEvent *event)
{
    auto s =
        dynamic_cast<QtNodes::BasicGraphicsScene*>(scene());

    if (!s)
        return;

    QPointF scenePos = mapToScene(event->pos());

    QMenu* menu = s->createSceneMenu(scenePos);

    if (menu)
        menu->exec(event->globalPos());
}
