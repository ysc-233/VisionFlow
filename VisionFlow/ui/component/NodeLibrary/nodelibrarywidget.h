#ifndef NODELIBRARYWIDGET_H
#define NODELIBRARYWIDGET_H
#include <QWidget>
#include <QLineEdit>
#include <QTreeWidget>
#include "nodetreewidget.h"
class NodeLibraryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NodeLibraryWidget(QWidget* parent = nullptr);

private:
    QLineEdit* m_searchEdit;
    NodeTreeWidget* m_tree;

    void rebuildTree(const QString& filter = "");
};
#endif // NODELIBRARYWIDGET_H
