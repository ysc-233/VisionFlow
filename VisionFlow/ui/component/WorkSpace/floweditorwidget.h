#ifndef FLOWEDITORWIDGET_H
#define FLOWEDITORWIDGET_H
#pragma once

#include <QWidget>
#include <QTabWidget>
#include <memory>

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
    ~FlowEditorWidget();

    void newFlow();
    void loadFlow(const QString& file);
    void saveFlow(const QString& file);

    struct FlowPage
    {
        QtNodes::DataFlowGraphModel* model;
        QtNodes::BasicGraphicsScene* scene;
        QtNodes::GraphicsView* view;
        QWidget* container;
    };

    FlowPage* createFlowPage(QString name);
    FlowPage* currentPage();
    void removeTab(int index);
    void setupTabContextMenu();

private:
    QTabWidget* m_tabWidget;

public:
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> m_registry;
};

#endif // FLOWEDITORWIDGET_H
