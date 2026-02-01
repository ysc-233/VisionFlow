// ui/module_library/module_tree_widget.cpp
#include "module_tree_widget.h"
#include <QPainter>
#include <QDebug>

ModuleTreeWidget::ModuleTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
    , m_draggedItem(nullptr)
{
    setDragEnabled(false); // 禁用内置拖拽
    setSelectionMode(QAbstractItemView::SingleSelection);
    setExpandsOnDoubleClick(false);
}

ModuleTreeWidget::~ModuleTreeWidget()
{
}

void ModuleTreeWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPos = event->pos();

        // 获取点击的项
        QTreeWidgetItem* item = itemAt(event->pos());

        // 只允许拖动模块项（有父项的才是模块，类别项没有父项）
        if (item && item->parent()) {
            m_draggedItem = item;
            qDebug() << "ModuleTreeWidget: Prepared to drag item:" << item->text(0);
        } else {
            m_draggedItem = nullptr;
            qDebug() << "ModuleTreeWidget: Clicked on category or empty area";
        }
    }

    QTreeWidget::mousePressEvent(event);
}

void ModuleTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        QTreeWidget::mouseMoveEvent(event);
        return;
    }

    if (!m_draggedItem) {
        QTreeWidget::mouseMoveEvent(event);
        return;
    }

    // 计算拖拽距离
    int distance = (event->pos() - m_dragStartPos).manhattanLength();
    if (distance < QApplication::startDragDistance()) {
        QTreeWidget::mouseMoveEvent(event);
        return;
    }

    qDebug() << "ModuleTreeWidget: Starting drag operation...";
    startDrag(event);

    m_draggedItem = nullptr;
}

void ModuleTreeWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QTreeWidgetItem* item = itemAt(event->pos());
    if (item && item->parent()) {
        QString moduleType = item->data(0, Qt::UserRole).toString();
        if (!moduleType.isEmpty()) {
            emit itemDoubleClicked(moduleType);
            qDebug() << "ModuleTreeWidget: Double-clicked module:" << moduleType;
        }
    }

    QTreeWidget::mouseDoubleClickEvent(event);
}

void ModuleTreeWidget::startDrag(QMouseEvent *event)
{
    if (!m_draggedItem) {
        qDebug() << "ModuleTreeWidget: No item to drag";
        return;
    }

    // 获取模块类型
    QString moduleType = m_draggedItem->data(0, Qt::UserRole).toString();
    QString moduleName = m_draggedItem->text(0);

    if (moduleType.isEmpty()) {
        qDebug() << "ModuleTreeWidget: Module type is empty";
        return;
    }

    qDebug() << "ModuleTreeWidget: Dragging module:" << moduleName << "Type:" << moduleType;

    // 创建拖拽对象
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData();

    // 使用自定义MIME类型
    mimeData->setData("application/x-visionflow-module", moduleType.toUtf8());
    mimeData->setText(moduleType); // 后备方案

    drag->setMimeData(mimeData);

    // 创建拖拽图标
    QPixmap pixmap = createDragPixmap(moduleName);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

    // 开始拖拽
    Qt::DropAction result = drag->exec(Qt::CopyAction);
    qDebug() << "ModuleTreeWidget: Drag result:" << result;

    // 发出信号
    emit itemDragStarted(moduleType, pixmap);
}

QPixmap ModuleTreeWidget::createDragPixmap(const QString& moduleName)
{
    // 创建一个漂亮的拖拽图标
    int width = 120;
    int height = 60;

    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制圆角矩形背景
    QPainterPath path;
    path.addRoundedRect(QRectF(1, 1, width-2, height-2), 8, 8);

    // 渐变背景
    QLinearGradient gradient(0, 0, 0, height);
    gradient.setColorAt(0, QColor(70, 130, 180));  // 钢蓝色
    gradient.setColorAt(1, QColor(30, 80, 150));   // 深蓝色

    painter.fillPath(path, gradient);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawPath(path);

    // 绘制模块名称
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, moduleName);

    // 绘制拖拽指示器
    painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
    painter.drawRect(10, 10, width-20, height-20);

    return pixmap;
}
