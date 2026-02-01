#include "node_graphics_item.h"

#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

NodeGraphicsItem::NodeGraphicsItem(const FlowNode& node, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_flowNode(node)
    , m_isSelected(false)
    , m_isHighlighted(false)
    , m_isRunning(false)
    , m_hasError(false)
    , m_isHovered(false)
    , m_width(150)
    , m_height(100)
    , m_titleHeight(30)
    , m_cornerRadius(8)
    , m_shadowEffect(nullptr)
{
    setupUI();
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable, true);
    setFlag(ItemIsMovable, true);
    setFlag(ItemSendsScenePositionChanges, true);
}

NodeGraphicsItem::~NodeGraphicsItem()
{
    if (m_shadowEffect) {
        delete m_shadowEffect;
    }
}

void NodeGraphicsItem::setupUI()
{
    // Set up shadow effect (可选)
     m_shadowEffect = new QGraphicsDropShadowEffect();
     m_shadowEffect->setBlurRadius(10);
     m_shadowEffect->setColor(QColor(0, 0, 0, 80));
     m_shadowEffect->setOffset(2, 2);
     this->setGraphicsEffect(m_shadowEffect);

    // Initialize ports from flow node
    auto inputPorts = m_flowNode.inputPorts();
    auto outputPorts = m_flowNode.outputPorts();

    // Calculate port positions
    qreal portSpacing = 20;
    qreal currentY = m_titleHeight + 10;

    for (const auto& port : inputPorts) {
        PortInfo info;
        info.name = port.id().port;
        info.isInput = true;
        info.position = QPointF(0, currentY);
        m_ports.append(info);
        currentY += portSpacing;
    }

    currentY = m_titleHeight + 10;
    for (const auto& port : outputPorts) {
        PortInfo info;
        info.name = port.id().port;
        info.isInput = false;
        info.position = QPointF(m_width, currentY);
        m_ports.append(info);
        currentY += portSpacing;
    }

    // Update height based on ports
    qreal maxHeight = qMax(inputPorts.size(), outputPorts.size()) * portSpacing + m_titleHeight + 20;
    if (maxHeight > m_height) {
        m_height = maxHeight;
    }

    updateColors();
}

QRectF NodeGraphicsItem::boundingRect() const
{
    // Add padding for selection and ports
    return QRectF(-10, -10, m_width + 20, m_height + 20);
}

QPainterPath NodeGraphicsItem::shape() const
{
    QPainterPath path;
    path.addRoundedRect(0, 0, m_width, m_height, m_cornerRadius, m_cornerRadius);
    return path;
}

void NodeGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                            QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    // Draw node body
    QPainterPath bodyPath;
    bodyPath.addRoundedRect(0, 0, m_width, m_height, m_cornerRadius, m_cornerRadius);

    // Fill body
    painter->fillPath(bodyPath, QBrush(getBodyColor()));

    // Draw title bar
    QPainterPath titlePath;
    titlePath.addRoundedRect(0, 0, m_width, m_titleHeight, m_cornerRadius, m_cornerRadius);
    titlePath.addRect(0, m_cornerRadius, m_width, m_titleHeight - m_cornerRadius);

    painter->fillPath(titlePath, QBrush(getTitleColor()));

    // Draw border
    QPen borderPen(m_borderColor, m_isSelected ? 2.0 : 1.0);
    if (m_isHighlighted) {
        borderPen.setColor(QColor(255, 200, 0));
        borderPen.setWidth(2);
    }
    if (m_hasError) {
        borderPen.setColor(Qt::red);
        borderPen.setWidth(2);
    }
    if (m_isRunning) {
        borderPen.setColor(QColor(0, 200, 0));
        borderPen.setWidth(2);
    }

    painter->setPen(borderPen);
    painter->drawPath(bodyPath);

    // Draw title text
    painter->setPen(m_textColor);
    painter->setFont(QFont("Arial", 9, QFont::Bold));
    QString title = m_flowNode.moduleType();
    if (title.length() > 15) {
        title = title.left(12) + "...";
    }
    painter->drawText(QRectF(5, 0, m_width - 10, m_titleHeight),
                     Qt::AlignCenter, title);

    // Draw node ID
    painter->setFont(QFont("Arial", 7));
    painter->drawText(QRectF(5, m_titleHeight, m_width - 10, 15),
                     Qt::AlignLeft, m_flowNode.id());

    // Draw ports
    painter->setFont(QFont("Arial", 8));
    for (const auto& port : m_ports) {
        // Draw port circle
        QPointF portPos = port.position;
        painter->setBrush(QBrush(Qt::white));
        painter->setPen(QPen(Qt::black, 1));
        painter->drawEllipse(portPos, 5, 5);

        // Draw port label
        QRectF labelRect;
        if (port.isInput) {
            labelRect = QRectF(portPos.x() + 10, portPos.y() - 10, 60, 20);
            painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, port.name);
        } else {
            labelRect = QRectF(portPos.x() - 70, portPos.y() - 10, 60, 20);
            painter->drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, port.name);
        }
    }

    // Draw selection highlight
    if (m_isSelected) {
        QPen selectionPen(QColor(0, 120, 215), 2, Qt::DashLine);
        painter->setPen(selectionPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(-3, -3, m_width + 6, m_height + 6,
                                m_cornerRadius + 2, m_cornerRadius + 2);
    }
}

void NodeGraphicsItem::setSelected(bool selected)
{
    m_isSelected = selected;
    QGraphicsItem::setSelected(selected);
    update();

    // 发送选中信号
    if (selected) {
        emit nodeSelected(this);
    }
}

void NodeGraphicsItem::setHighlighted(bool highlighted)
{
    m_isHighlighted = highlighted;
    update();
}

void NodeGraphicsItem::setRunning(bool running)
{
    m_isRunning = running;
    update();
}

void NodeGraphicsItem::setError(bool error)
{
    m_hasError = error;
    update();
}
#include <QGraphicsScene>
void NodeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 清空场景中其他项目的选中状态
        QList<QGraphicsItem*> items = scene()->items();
        for (QGraphicsItem* item : items) {
            if (item != this && item->isSelected()) {
                item->setSelected(false);
            }
        }
        // 选中当前项
        setSelected(true);
        // 发送选中信号
        emit nodeSelected(this);
    }

    QGraphicsItem::mousePressEvent(event);
}

void NodeGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void NodeGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void NodeGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
    // TODO: 可以在这里添加双击事件处理
}

void NodeGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = true;
    updateColors();
    setCursor(Qt::PointingHandCursor);
    QGraphicsItem::hoverEnterEvent(event);
}

void NodeGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = false;
    updateColors();
    setCursor(Qt::ArrowCursor);
    QGraphicsItem::hoverLeaveEvent(event);
}

QVariant NodeGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        QPointF oldPos = pos();
        QPointF newPos = value.toPointF();
        if (oldPos != newPos) {
            emit positionChanged(oldPos);
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

void NodeGraphicsItem::updateColors()
{
    // Set colors based on module type and state
    if (m_flowNode.moduleType().contains("Const", Qt::CaseInsensitive)) {
        m_titleColor = QColor(70, 130, 180);  // Steel blue
        m_bodyColor = QColor(240, 248, 255);  // Alice blue
    } else if (m_flowNode.moduleType().contains("Add", Qt::CaseInsensitive)) {
        m_titleColor = QColor(60, 179, 113);  // Medium sea green
        m_bodyColor = QColor(240, 255, 240);  // Honeydew
    } else if (m_flowNode.moduleType().contains("Print", Qt::CaseInsensitive)) {
        m_titleColor = QColor(210, 105, 30);  // Chocolate
        m_bodyColor = QColor(255, 248, 220);  // Cornsilk
    } else {
        m_titleColor = QColor(105, 105, 105); // Dim gray
        m_bodyColor = QColor(245, 245, 245);  // White smoke
    }

    // Adjust colors for hover state
    if (m_isHovered) {
        m_titleColor = m_titleColor.lighter(120);
        m_bodyColor = m_bodyColor.lighter(105);
    }

    // Adjust colors for selection state
    if (m_isSelected) {
        m_titleColor = m_titleColor.darker(110);
        m_bodyColor = m_bodyColor.darker(105);
    }

    m_borderColor = m_titleColor.darker(150);
    m_textColor = Qt::black;
}

QColor NodeGraphicsItem::getTitleColor() const
{
    return m_titleColor;
}

QColor NodeGraphicsItem::getBodyColor() const
{
    return m_bodyColor;
}
