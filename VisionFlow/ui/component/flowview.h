#ifndef FLOWVIEW_H
#define FLOWVIEW_H
#pragma once
#include <QtNodes/GraphicsView>

class FlowView : public QtNodes::GraphicsView
{
public:
    using GraphicsView::GraphicsView;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // FLOWVIEW_H
