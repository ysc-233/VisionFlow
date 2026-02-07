#include "nodelibrarywidget.h"
#include <QVBoxLayout>
#include "nodes/flownodeautoregister.h"
#include <QMimeData>
#include <QDrag>
NodeLibraryWidget::NodeLibraryWidget(QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);

    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("Search node...");

    m_tree = new NodeTreeWidget;
    m_tree->setHeaderHidden(true);
    m_tree->setDragEnabled(true);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);

    layout->addWidget(m_searchEdit);
    layout->addWidget(m_tree);

    rebuildTree();

    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &NodeLibraryWidget::rebuildTree);
}

void NodeLibraryWidget::rebuildTree(const QString& filter)
{
    m_tree->clear();

    QMap<QString, QTreeWidgetItem*> categories;

    auto& nodes = NodeLibraryRegistry::instance().nodes();
    for (auto& node : nodes)
    {
        if (!filter.isEmpty() &&
            !node.name.contains(filter, Qt::CaseInsensitive))
            continue;

        if (!categories.contains(node.category))
        {
            auto catItem = new QTreeWidgetItem(m_tree);
            catItem->setText(0, node.category);
            categories[node.category] = catItem;
        }

        auto nodeItem = new QTreeWidgetItem(categories[node.category]);
        nodeItem->setText(0, node.name);
        nodeItem->setData(0, Qt::UserRole, node.name);
    }

    m_tree->expandAll();
}

