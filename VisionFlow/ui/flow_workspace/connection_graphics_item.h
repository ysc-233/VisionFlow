#ifndef CONNECTION_GRAPHICS_ITEM_H
#define CONNECTION_GRAPHICS_ITEM_H

#include <QGraphicsPathItem>
#include <QPen>
#include "core/flow_connection.h"

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

class PortGraphicsItem;

enum class ConnectionState {
    Normal,
    Selected,
    Highlighted,
    Error,
    Disabled
};

class ConnectionGraphicsItem : public QGraphicsPathItem
{
public:
    explicit ConnectionGraphicsItem(const FlowConnection& connection,
                                    PortGraphicsItem* startPort = nullptr,
                                    PortGraphicsItem* endPort = nullptr,
                                    QGraphicsItem *parent = nullptr);
    ~ConnectionGraphicsItem();

    // QGraphicsItem 接口
    int type() const override { return Type; }
    enum { Type = UserType + 2 };

    // 连接信息
    FlowConnection flowConnection() const { return m_connection; }
    PortId fromPortId() const { return m_connection.fromPort(); }
    PortId toPortId() const { return m_connection.toPort(); }

    // 端口连接
    PortGraphicsItem* startPort() const { return m_startPort; }
    PortGraphicsItem* endPort() const { return m_endPort; }
    void setStartPort(PortGraphicsItem* port);
    void setEndPort(PortGraphicsItem* port);
    void disconnectPorts();

    // 状态控制
    ConnectionState state() const { return m_state; }
    void setState(ConnectionState state);

    // 重写 QGraphicsItem 的 setSelected - 注意：不是虚函数，不要用 override
    void setSelected(bool selected);

    void setHighlighted(bool highlighted);

    // 路径更新
    void updatePath();
    QPainterPath createPath(const QPointF& start, const QPointF& end) const;

    // 样式
    void setLineWidth(qreal width);
    void setLineColor(const QColor& color);
    void setHighlightColor(const QColor& color);
    void setLineStyle(Qt::PenStyle style);

signals:
    void connectionClicked(ConnectionGraphicsItem* item);
    void connectionHoverEnter(ConnectionGraphicsItem* item);
    void connectionHoverLeave(ConnectionGraphicsItem* item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    // 正确的 paint 函数签名
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    void setupStyle();
    void updateColors();
    void updateZValue();
    QColor stateToColor(ConnectionState state) const;
    void drawArrow(QPainter* painter, const QPointF& end, qreal angle);

private:
    FlowConnection m_connection;
    PortGraphicsItem* m_startPort;
    PortGraphicsItem* m_endPort;
    ConnectionState m_state;

    // 样式
    QPen m_pen;
    QColor m_lineColor;
    QColor m_highlightColor;
    QColor m_selectionColor;
    qreal m_lineWidth;
    qreal m_highlightWidth;
    Qt::PenStyle m_lineStyle;

    // 箭头
    bool m_showArrow;
    qreal m_arrowSize;

    bool m_isHovered;
};

#endif // CONNECTION_GRAPHICS_ITEM_H
