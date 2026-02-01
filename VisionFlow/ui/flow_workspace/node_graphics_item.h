#ifndef NODE_GRAPHICS_ITEM_H
#define NODE_GRAPHICS_ITEM_H
#include <QGraphicsItem>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include "core/flow_node.h"

// QGraphicsItem 不能有 Q_OBJECT 宏，但我们需要信号
// 解决方案：使用 QObject 作为基类，并组合 QGraphicsItem
class NodeGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    explicit NodeGraphicsItem(const FlowNode& node, QGraphicsItem *parent = nullptr);
    ~NodeGraphicsItem();

    // QGraphicsItem 接口
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    // Custom type
    enum { Type = UserType + 1 };
    int type() const override { return Type; }

    // Node information
    const FlowNode& flowNode() const { return m_flowNode; }
    NodeId nodeId() const { return m_flowNode.id(); }
    QString moduleType() const { return m_flowNode.moduleType(); }

    // State
    void setSelected(bool selected);
    void setHighlighted(bool highlighted);
    void setRunning(bool running);
    void setError(bool error);

signals:
    void positionChanged(const QPointF& oldPos);
    void nodeSelected(NodeGraphicsItem* item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    void setupUI();
    void updateColors();
    QColor getTitleColor() const;
    QColor getBodyColor() const;

private:
    FlowNode m_flowNode;

    // Appearance
    QColor m_titleColor;
    QColor m_bodyColor;
    QColor m_borderColor;
    QColor m_textColor;

    // State
    bool m_isSelected;
    bool m_isHighlighted;
    bool m_isRunning;
    bool m_hasError;
    bool m_isHovered;

    // Size
    qreal m_width;
    qreal m_height;
    qreal m_titleHeight;
    qreal m_cornerRadius;

    // Port information
    struct PortInfo {
        QString name;
        bool isInput;
        QPointF position;
    };
    QList<PortInfo> m_ports;

    // Effects
    QGraphicsDropShadowEffect* m_shadowEffect;
};


#endif // NODE_GRAPHICS_ITEM_H
