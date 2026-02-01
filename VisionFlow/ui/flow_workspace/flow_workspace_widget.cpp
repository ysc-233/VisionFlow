#include "flow_workspace_widget.h"
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QScrollBar>
#include <QApplication>
#include <QMessageBox>

FlowWorkspaceWidget::FlowWorkspaceWidget(FlowGraph* graph, QWidget *parent)
    : QWidget(parent)
    , m_flowGraph(graph)
    , m_showGrid(true)
    , m_gridSize(20.0)
    , m_zoomLevel(1.0)
    , m_isModified(false)
    , m_isRunning(false)
    , m_ctrlPressed(false)
    , m_shiftPressed(false)
    , m_isDraggingView(false)
{
    setupUI();
    setupContextMenu();

    // Enable drop events
    setAcceptDrops(false);
    m_view->viewport()->setAcceptDrops(false);

    // Initialize scene with existing nodes
    if (m_flowGraph) {
        // TODO: Add existing nodes to scene
        auto nodes = m_flowGraph->nodes();
        for (const auto& node : nodes) {
            // Create visual representation for each node
            createNodeGraphicsItem(node, QPointF(0, 0));
        }
    }
}

FlowWorkspaceWidget::~FlowWorkspaceWidget()
{
    delete m_contextMenu;
}

void FlowWorkspaceWidget::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create graphics scene and view
    setupScene();
    setupView();

    layout->addWidget(m_view);
}

void FlowWorkspaceWidget::setupScene()
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-2000, -2000, 4000, 4000);

    // 设置单选模式 - 这是关键！
//    m_scene->setSelectionMode(QGraphicsScene::SingleSelection);

    // 连接场景选择变化信号
    connect(m_scene, &QGraphicsScene::selectionChanged,
            this, &FlowWorkspaceWidget::updateSelection);
}

void FlowWorkspaceWidget::setupView()
{
    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setRenderHint(QPainter::TextAntialiasing);
    m_view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    m_view->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    // 设置拖拽模式为单选矩形选择
    m_view->setDragMode(QGraphicsView::RubberBandDrag);

    // 设置单选 - 点击空白区域清空选择
    m_view->setRubberBandSelectionMode(Qt::ContainsItemShape);

    m_view->setMouseTracking(true);
    m_view->setBackgroundBrush(QBrush(QColor(45, 45, 48)));

    // 确保视图接受拖放事件
    m_view->setAcceptDrops(true);

    // 设置视口属性
    m_view->viewport()->setAcceptDrops(true);
    m_view->viewport()->setMouseTracking(true);
}

void FlowWorkspaceWidget::setupContextMenu()
{
    m_contextMenu = new QMenu(this);

    QAction* deleteAction = new QAction("Delete", this);
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, this, &FlowWorkspaceWidget::deleteSelected);

    QAction* copyAction = new QAction("Copy", this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &FlowWorkspaceWidget::copySelected);

    QAction* pasteAction = new QAction("Paste", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, this, &FlowWorkspaceWidget::paste);

    QAction* selectAllAction = new QAction("Select All", this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, this, &FlowWorkspaceWidget::selectAll);

    m_contextMenu->addAction(deleteAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(copyAction);
    m_contextMenu->addAction(pasteAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(selectAllAction);
}

void FlowWorkspaceWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // 添加调试信息
    qDebug() << "dragEnterEvent - MIME formats:" << event->mimeData()->formats();

    // 检查是否包含我们的MIME类型或文本
    if (event->mimeData()->hasFormat("application/x-visionflow-module")) {
        qDebug() << "接受自定义MIME类型";
        event->acceptProposedAction();
        setCursor(Qt::DragCopyCursor);
    } else if (event->mimeData()->hasText()) {
        qDebug() << "接受文本MIME类型";
        event->acceptProposedAction();
        setCursor(Qt::DragCopyCursor);
    } else {
        qDebug() << "拒绝拖拽事件";
        event->ignore();
    }
}

void FlowWorkspaceWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-visionflow-module") ||
        event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void FlowWorkspaceWidget::dropEvent(QDropEvent *event)
{
    // 重置光标
       setCursor(Qt::ArrowCursor);

       qDebug() << "=== dropEvent 被调用 ===";
       qDebug() << "dropEvent - 事件位置:" << event->pos();
       qDebug() << "dropEvent - MIME类型:" << event->mimeData()->formats();

       // 检查是否为我们的MIME类型
       if (!event->mimeData()->hasFormat("application/x-visionflow-module") &&
           !event->mimeData()->hasText()) {
           qDebug() << "dropEvent: Not a supported MIME type, ignore";
           event->ignore();
           return;
       }

       // 获取模块类型
       QString moduleType;
       if (event->mimeData()->hasFormat("application/x-visionflow-module")) {
           QByteArray data = event->mimeData()->data("application/x-visionflow-module");
           moduleType = QString::fromUtf8(data);
           qDebug() << "从自定义MIME获取模块类型:" << moduleType;
       } else if (event->mimeData()->hasText()) {
           moduleType = event->mimeData()->text();
           qDebug() << "从文本获取模块类型:" << moduleType;
       }

       if (moduleType.isEmpty()) {
           qDebug() << "无法获取模块类型";
           event->ignore();
           return;
       }

       // 转换坐标 - 修复坐标转换
       QPoint widgetPos = event->pos();
       QPoint viewPos = m_view->mapFromParent(widgetPos);
       QPointF scenePos = m_view->mapToScene(viewPos);

       qDebug() << "坐标转换:";
       qDebug() << "  事件位置 (widget):" << widgetPos;
       qDebug() << "  视图位置 (view):" << viewPos;
       qDebug() << "  场景位置 (scene):" << scenePos;

       // 检查视图是否有效
       if (!m_view->viewport()->rect().contains(viewPos)) {
           qDebug() << "The drag-and-drop position is not in the viewport";
           event->ignore();
           return;
       }

       // 对齐到网格
       if (m_showGrid) {
           scenePos = snapToGrid(scenePos);
           qDebug() << "对齐到网格后:" << scenePos;
       }

       // 创建节点
       createNodeFromMimeData(moduleType, scenePos);

       event->acceptProposedAction();
       qDebug() << "=== dropEvent 处理完成 ===";
}

void FlowWorkspaceWidget::createNodeFromMimeData(const QString& moduleType, const QPointF& scenePos)
{
    // 生成唯一节点ID
    static int nodeCounter = 0;
    QString nodeId = QString("node_%1").arg(++nodeCounter);

    try {
        // 创建节点
        FlowNode node = FlowNodeFactory::createNode(nodeId, moduleType);

        // 添加到图
        if (m_flowGraph) {
            if (!m_flowGraph->addNode(node)) {
                emit logMessage(QString("Failed to add node %1 to graph (duplicate ID?)").arg(nodeId),
                               QtWarningMsg);
                return;
            }
        }

        // 创建图形项
        createNodeGraphicsItem(node, scenePos);

        // 标记已修改
        m_isModified = true;
        emit modifiedChanged(true);

        // 修正日志格式化 - 使用 QString::arg 正确格式化
        QString logMsg = QString("Added node: %1 (%2) at (%3, %4)")
                        .arg(nodeId)
                        .arg(moduleType)
                        .arg(scenePos.x(), 0, 'f', 1)
                        .arg(scenePos.y(), 0, 'f', 1);
        emit logMessage(logMsg, QtInfoMsg);

        qDebug() << logMsg;

    } catch (const std::exception& e) {
        emit logMessage(QString("Failed to create node: %1").arg(e.what()),
                       QtCriticalMsg);
    }
}

void FlowWorkspaceWidget::createNodeGraphicsItem(const FlowNode& node, const QPointF& pos)
{
    // Create custom node graphics item
    NodeGraphicsItem* nodeItem = new NodeGraphicsItem(node);
    nodeItem->setPos(pos);
    m_scene->addItem(nodeItem);

    // Make it selectable and movable
    nodeItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    nodeItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    nodeItem->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    // Store reference
    m_nodeItems[node.id()] = nodeItem;

    // Connect signals - 注意这里需要修正
    // 我们需要捕获 nodeItem 的地址，而不是 node 的引用
    // 因为 lambda 捕获的 node 引用可能会失效
    connect(nodeItem, &NodeGraphicsItem::positionChanged,
            [this, nodeId = node.id()](const QPointF& oldPos) {
                Q_UNUSED(oldPos);
                m_isModified = true;
                emit modifiedChanged(true);
            });

    connect(nodeItem, &NodeGraphicsItem::nodeSelected,
            [this, nodeItem](NodeGraphicsItem* item) {
                Q_UNUSED(item);
                // 通过 nodeItem 获取节点
                if (m_flowGraph) {
                    const FlowNode* graphNode = m_flowGraph->node(nodeItem->nodeId());
                    if (graphNode) {
                        emit nodeSelected(graphNode);
                    }
                }
            });
}

void FlowWorkspaceWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Control:
        m_ctrlPressed = true;
        break;
    case Qt::Key_Shift:
        m_shiftPressed = true;
        break;
    case Qt::Key_Delete:
        deleteSelected();
        break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        if (m_ctrlPressed) {
            zoomIn();
        }
        break;
    case Qt::Key_Minus:
        if (m_ctrlPressed) {
            zoomOut();
        }
        break;
    case Qt::Key_0:
        if (m_ctrlPressed) {
            zoomReset();
        }
        break;
    case Qt::Key_F:
        if (m_ctrlPressed) {
            fitView();
        }
        break;
    }

    QWidget::keyPressEvent(event);
}

void FlowWorkspaceWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Control:
        m_ctrlPressed = false;
        break;
    case Qt::Key_Shift:
        m_shiftPressed = false;
        break;
    }

    QWidget::keyReleaseEvent(event);
}

void FlowWorkspaceWidget::wheelEvent(QWheelEvent *event)
{
    if (m_ctrlPressed) {
        // Zoom with Ctrl + Wheel
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}

void FlowWorkspaceWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QPointF scenePos = m_view->mapToScene(m_view->mapFromParent(event->pos()));
    emit contextMenuRequested(event->globalPos(), scenePos);

    if (m_contextMenu) {
        m_contextMenu->exec(event->globalPos());
    }
}

void FlowWorkspaceWidget::runFlow()
{
    emit logMessage("Starting flow execution...", QtInfoMsg);
    m_isRunning = true;
    emit executionStarted();

    // TODO: Actual flow execution

    // Simulate execution completion
    QTimer::singleShot(1000, [this]() {
        emit logMessage("Execution completed", QtInfoMsg);
        m_isRunning = false;
        emit executionFinished(true);
    });
}

void FlowWorkspaceWidget::stopFlow()
{
    emit logMessage("Stopping execution", QtWarningMsg);
    m_isRunning = false;
    emit executionFinished(false);
}

void FlowWorkspaceWidget::addNode(const FlowNode& node, const QPointF& pos)
{
    createNodeFromMimeData(node.moduleType(), pos);
}

void FlowWorkspaceWidget::clearScene()
{
    m_scene->clear();
    m_nodeItems.clear();
    m_isModified = false;
    emit modifiedChanged(false);
}

void FlowWorkspaceWidget::zoomIn()
{
    m_zoomLevel *= 1.2;
    m_view->scale(1.2, 1.2);
    emit logMessage(QString("Zoom: %1%").arg(int(m_zoomLevel * 100)), QtDebugMsg);
}

void FlowWorkspaceWidget::zoomOut()
{
    m_zoomLevel /= 1.2;
    m_view->scale(1/1.2, 1/1.2);
    emit logMessage(QString("Zoom: %1%").arg(int(m_zoomLevel * 100)), QtDebugMsg);
}

void FlowWorkspaceWidget::zoomReset()
{
    m_view->resetTransform();
    m_zoomLevel = 1.0;
    emit logMessage("Zoom reset to 100%", QtDebugMsg);
}

void FlowWorkspaceWidget::fitView()
{
    m_view->fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    emit logMessage("Fitted view to content", QtDebugMsg);
}

void FlowWorkspaceWidget::showGrid(bool show)
{
    m_showGrid = show;
    m_scene->update();
    emit logMessage(show ? "Grid shown" : "Grid hidden", QtDebugMsg);
}

void FlowWorkspaceWidget::deleteSelected()
{
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
    if (selectedItems.isEmpty()) return;

    for (QGraphicsItem* item : selectedItems) {
        NodeGraphicsItem* nodeItem = dynamic_cast<NodeGraphicsItem*>(item);
        if (nodeItem) {
            QString nodeId = nodeItem->nodeId();

            // Remove from graph
            if (m_flowGraph) {
                m_flowGraph->removeNode(nodeId);
            }

            // Remove from our map
            m_nodeItems.remove(nodeId);

            // Remove from scene
            m_scene->removeItem(item);
            delete item;

            emit logMessage(QString("Deleted node: %1").arg(nodeId), QtInfoMsg);
        }
    }

    m_isModified = true;
    emit modifiedChanged(true);
}

void FlowWorkspaceWidget::copySelected()
{
    // TODO: Implement copy
    emit logMessage("Copy not implemented yet", QtWarningMsg);
}

void FlowWorkspaceWidget::paste()
{
    // TODO: Implement paste
    emit logMessage("Paste not implemented yet", QtWarningMsg);
}

void FlowWorkspaceWidget::selectAll()
{
    for (auto item : m_nodeItems) {
        item->setSelected(true);
    }
}

void FlowWorkspaceWidget::clearSelection()
{
    m_scene->clearSelection();
}

QPointF FlowWorkspaceWidget::snapToGrid(const QPointF& pos) const
{
    if (!m_showGrid || m_gridSize <= 0) {
        return pos;
    }

    qreal x = round(pos.x() / m_gridSize) * m_gridSize;
    qreal y = round(pos.y() / m_gridSize) * m_gridSize;

    return QPointF(x, y);
}

void FlowWorkspaceWidget::updateView()
{
    m_view->viewport()->update();
}

void FlowWorkspaceWidget::updateSelection()
{
    // Update node selection in property editor
    QList<QGraphicsItem*> selected = m_scene->selectedItems();
    if (!selected.isEmpty()) {
        NodeGraphicsItem* nodeItem = dynamic_cast<NodeGraphicsItem*>(selected.first());
        if (nodeItem && m_flowGraph) {
            const FlowNode* node = m_flowGraph->node(nodeItem->nodeId());
            if (node) {
                emit nodeSelected(node);
            }
        }
    } else {
        emit nodeSelected(nullptr);
    }
}
