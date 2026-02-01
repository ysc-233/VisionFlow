#ifndef FLOW_WORKSPACE_WIDGET_H
#define FLOW_WORKSPACE_WIDGET_H
#include <QWidget>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QMenu>

#include "core/flow_graph.h"
#include "core/flow_node.h"
#include "core/flow_node_factory.h"

// Forward declaration
#include "ui/flow_workspace/node_graphics_item.h"

class FlowWorkspaceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FlowWorkspaceWidget(FlowGraph* graph = nullptr, QWidget *parent = nullptr);
    ~FlowWorkspaceWidget();

    FlowGraph* flowGraph() const { return m_flowGraph; }
QGraphicsView* view() const{return m_view;}
    void runFlow();
    void stopFlow();
    void addNode(const FlowNode& node, const QPointF& pos);
    void clearScene();

    // View operations
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void fitView();
    void showGrid(bool show);

    // Selection operations
    void deleteSelected();
    void copySelected();
    void paste();
    void selectAll();
    void clearSelection();

signals:
    void modifiedChanged(bool modified);
    void nodeSelected(const FlowNode* node);
    void executionStarted();
    void executionFinished(bool success);
    void logMessage(const QString& message, QtMsgType type);
    void contextMenuRequested(const QPoint& globalPos, const QPointF& scenePos);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void setupUI();
    void setupView();
    void setupScene();
    void setupContextMenu();
    void createNodeGraphicsItem(const FlowNode& node, const QPointF& pos);
    QPointF snapToGrid(const QPointF& pos) const;
    void drawGrid();
    void updateView();
    void updateSelection();
    void createNodeFromMimeData(const QString& moduleType, const QPointF& scenePos);

    // Grid drawing
    void drawBackground(QPainter* painter, const QRectF& rect);

private:
    FlowGraph* m_flowGraph;
    QGraphicsScene* m_scene;
    QGraphicsView* m_view;

    // Node management
    QMap<NodeId, NodeGraphicsItem*> m_nodeItems;

    // View settings
    bool m_showGrid;
    qreal m_gridSize;
    qreal m_zoomLevel;

    // State
    bool m_isModified;
    bool m_isRunning;

    // Context menu
    QMenu* m_contextMenu;

    // Keyboard state
    bool m_ctrlPressed;
    bool m_shiftPressed;

    // Drag state
    bool m_isDraggingView;
    QPoint m_dragStartPos;
};

#endif // FLOW_WORKSPACE_WIDGET_H
