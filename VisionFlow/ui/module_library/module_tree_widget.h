#ifndef MODULE_TREE_WIDGET_H
#define MODULE_TREE_WIDGET_H
#include <QTreeWidget>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>

class ModuleTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit ModuleTreeWidget(QWidget *parent = nullptr);
    ~ModuleTreeWidget();

signals:
    void itemDragStarted(const QString& moduleType, const QPixmap& pixmap);
    void itemDoubleClicked(const QString& moduleType);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void startDrag(QMouseEvent *event);
    QPixmap createDragPixmap(const QString& moduleName);
    QByteArray moduleMimeData(const QString& moduleType) const;

private:
    QPoint m_dragStartPos;
    QTreeWidgetItem* m_draggedItem;
};

#endif // MODULE_TREE_WIDGET_H
