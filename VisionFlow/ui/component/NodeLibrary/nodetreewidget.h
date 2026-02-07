#ifndef NODETREEWIDGET_H
#define NODETREEWIDGET_H
#pragma once
#include <QTreeWidget>

class NodeTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    using QTreeWidget::QTreeWidget;

protected:
    void startDrag(Qt::DropActions supportedActions) override;
};
#endif // NODETREEWIDGET_H
