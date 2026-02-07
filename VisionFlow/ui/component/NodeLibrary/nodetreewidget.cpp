#include "nodetreewidget.h"
#include <QDrag>
#include <QMimeData>
#include <QDebug>

void NodeTreeWidget::startDrag(Qt::DropActions)
{
    auto item = currentItem();
    if (!item || item->childCount() > 0)
        return;

    QString nodeName = item->data(0, Qt::UserRole).toString();

    QMimeData* mime = new QMimeData;
    mime->setText(nodeName);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mime);

    drag->exec(Qt::CopyAction);
}
