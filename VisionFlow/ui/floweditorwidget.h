#ifndef FLOWEDITORWIDGET_H
#define FLOWEDITORWIDGET_H
#pragma once

#include <QWidget>

namespace QtNodes {
class BasicGraphicsScene;
class GraphicsView;
class DataFlowGraphModel;
class NodeDelegateModelRegistry;
}

class FlowEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FlowEditorWidget(QWidget *parent = nullptr);

    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> m_registry;
    QtNodes::DataFlowGraphModel* m_model = nullptr;
    QtNodes::BasicGraphicsScene* m_scene = nullptr;
    QtNodes::GraphicsView* m_view = nullptr;
};

#endif // FLOWEDITORWIDGET_H
