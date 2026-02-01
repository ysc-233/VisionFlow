#ifndef FLOW_VIEW_H
#define FLOW_VIEW_H
#include <QGraphicsView>
#include <QWheelEvent>
#include <QKeyEvent>
#include "core/flow_types.h"
QT_BEGIN_NAMESPACE
class QRubberBand;
QT_END_NAMESPACE

class FlowScene;

class FlowView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit FlowView(FlowScene* scene, QWidget *parent = nullptr);
    ~FlowView();

    // 视图控制
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void fitView();
    void centerOnNode(const NodeId& nodeId);

    // 网格控制
    void setShowGrid(bool show);
    void setSnapToGrid(bool snap);
    bool showGrid() const { return m_showGrid; }
    bool snapToGrid() const { return m_snapToGrid; }

    // 状态
    double zoomLevel() const { return m_zoomLevel; }

signals:
    void zoomChanged(double level);
    void viewportChanged(const QRectF& rect);
    void mouseMoved(const QPointF& scenePos);

public slots:
    void onSceneRectChanged(const QRectF& rect);
    void updateViewport();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void resizeEvent(QResizeEvent *event) override;
    void scrollContentsBy(int dx, int dy) override;

private:
    void setupView();
    void setupShortcuts();
    void updateTransform();
    QPointF snapToGridPoint(const QPointF& point) const;
    void drawGrid(QPainter* painter, const QRectF& rect);
    void drawRulers(QPainter* painter);

private:
    FlowScene* m_scene;

    // 缩放和视图
    double m_zoomLevel;
    double m_minZoom;
    double m_maxZoom;

    // 网格
    bool m_showGrid;
    bool m_snapToGrid;
    qreal m_gridSize;

    // 标尺
    bool m_showRulers;
    qreal m_rulerSize;

    // 拖拽状态
    bool m_isPanning;
    QPoint m_panStartPos;

    // 框选状态
    bool m_isRubberBandSelecting;
    QPoint m_rubberBandStartPos;
    QRubberBand* m_rubberBand;

    // 键盘状态
    bool m_spacePressed;
};

#endif // FLOW_VIEW_H
