#ifndef FLOW_SCENE_H
#define FLOW_SCENE_H
#include <QGraphicsScene>
#include <QMap>
#include "core/flow_graph.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QGraphicsSceneDragDropEvent;
class QGraphicsItem;
QT_END_NAMESPACE

class NodeGraphicsItem;
class ConnectionGraphicsItem;

class FlowScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit FlowScene(FlowGraph* graph = nullptr, QObject *parent = nullptr);
    ~FlowScene();

    FlowGraph* flowGraph() const { return m_flowGraph; }
    void setFlowGraph(FlowGraph* graph);

    // 节点管理
    NodeGraphicsItem* addNodeItem(const FlowNode& node, const QPointF& pos = QPointF());
    NodeGraphicsItem* getNodeItem(const NodeId& nodeId);
    void removeNodeItem(const NodeId& nodeId);
    QList<NodeGraphicsItem*> selectedNodeItems() const;

    // 连接管理
    ConnectionGraphicsItem* addConnectionItem(const FlowConnection& connection);
    ConnectionGraphicsItem* getConnectionItem(const FlowConnection& connection);
    void removeConnectionItem(const FlowConnection& connection);
    QList<ConnectionGraphicsItem*> selectedConnectionItems() const;

    // 端口查找
    QGraphicsItem* findPortItem(const PortId& portId) const;

    // 视图操作
    void clearSelection();
    void deleteSelected();
    void selectAll();
    void copySelected();
    void paste();

    // 状态
    bool isModified() const { return m_isModified; }
    void setModified(bool modified = true);
    bool hasSelection() const;

signals:
    void nodeAdded(NodeGraphicsItem* item);
    void nodeRemoved(const NodeId& nodeId);
    void nodeSelected(NodeGraphicsItem* item);
    void nodeDoubleClicked(NodeGraphicsItem* item);
    void nodeMoved(NodeGraphicsItem* item, const QPointF& oldPos);
    void connectionAdded(ConnectionGraphicsItem* item);
    void connectionRemoved(const FlowConnection& connection);
    void connectionSelected(ConnectionGraphicsItem* item);
    void sceneModified();
    void contextMenuRequested(const QPoint& scenePos);

public slots:
    void onNodeMoved(NodeGraphicsItem* item, const QPointF& oldPos);
    void onConnectionCreated(PortId fromPort, PortId toPort);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void setupGrid();
    void updateConnections();
    void createTemporaryConnection(const QPointF& scenePos);
    void completeTemporaryConnection(const QPointF& scenePos);
    void cancelTemporaryConnection();
    NodeGraphicsItem* itemAt(const QPointF& scenePos) const;

private:
    FlowGraph* m_flowGraph;

    QMap<NodeId, NodeGraphicsItem*> m_nodeItems;
    QMap<FlowConnection, ConnectionGraphicsItem*> m_connectionItems;

    // 临时连接状态
    bool m_isCreatingConnection;
    PortId m_connectionStartPort;
    QGraphicsLineItem* m_tempConnectionLine;

    // 多选状态
    bool m_isSelecting;
    QPointF m_selectStartPos;
    QGraphicsRectItem* m_selectionRect;

    // 网格
    bool m_showGrid;
    qreal m_gridSize;

    bool m_isModified;
};
#endif // FLOW_SCENE_H
