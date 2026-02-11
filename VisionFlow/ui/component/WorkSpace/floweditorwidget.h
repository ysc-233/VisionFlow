#ifndef FLOWEDITORWIDGET_H
#define FLOWEDITORWIDGET_H
#pragma once

#include <QWidget>
#include <QTabWidget>
#include <memory>
#include "flow/graph/flownode.h"
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
signals:
    void sig_updateImage(QImage img);
public slots:
    void updateDisplay();
private:
    QTabWidget* m_tabWidget;
    FlowNode *m_flowNode;
public:
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> m_registry;
};

#endif // FLOWEDITORWIDGET_H
