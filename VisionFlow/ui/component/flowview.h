#ifndef FLOWVIEW_H
#define FLOWVIEW_H
#pragma once
#include <QtNodes/GraphicsView>

class FlowView : public QtNodes::GraphicsView
{
public:
    FlowView(QtNodes::BasicGraphicsScene* scene,
             QWidget* parent = nullptr);
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    bool viewportEvent(QEvent* event) override;
};

#endif // FLOWVIEW_H
