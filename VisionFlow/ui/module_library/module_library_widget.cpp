#include "module_library_widget.h"
#include "module_tree_widget.h"
#include <QDebug>

#include "modules/module_registry.h"
#include "modules/module_definition.h"

ModuleLibraryWidget::ModuleLibraryWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    populateModules();
}

ModuleLibraryWidget::~ModuleLibraryWidget()
{
}

void ModuleLibraryWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // 搜索框
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Search modules...");
    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &ModuleLibraryWidget::onSearchTextChanged);
    mainLayout->addWidget(m_searchEdit);

    // 模块树（使用自定义控件）
    m_treeWidget = new ModuleTreeWidget(this);
    m_treeWidget->setHeaderLabel("Modules");
    m_treeWidget->setRootIsDecorated(true);
    m_treeWidget->setAlternatingRowColors(true);

    // 连接信号
    connect(m_treeWidget, &ModuleTreeWidget::itemDoubleClicked,
            this, &ModuleLibraryWidget::onItemDoubleClicked);
    connect(m_treeWidget, &ModuleTreeWidget::itemDragStarted,
            this, &ModuleLibraryWidget::onItemDragStarted);

    mainLayout->addWidget(m_treeWidget);
}

void ModuleLibraryWidget::refreshModuleList()
{
    populateModules();
}

void ModuleLibraryWidget::populateModules()
{
    if (!m_treeWidget) return;

    m_treeWidget->clear();

    // 获取已注册模块
    auto& registry = ModuleRegistry::instance();
    auto definitions = registry.allDefinitions();

    // 创建类别项
    QTreeWidgetItem* allModulesItem = new QTreeWidgetItem(m_treeWidget);
    allModulesItem->setText(0, "All Modules");
    allModulesItem->setExpanded(true);

    // 添加模块
    for (const auto& def : definitions) {
        QTreeWidgetItem* item = new QTreeWidgetItem(allModulesItem);
        item->setText(0, def.displayName);
        item->setData(0, Qt::UserRole, def.moduleType);
        item->setToolTip(0, def.moduleType);
    }

    // 如果没有模块
    if (definitions.isEmpty()) {
        QTreeWidgetItem* item = new QTreeWidgetItem(m_treeWidget);
        item->setText(0, "No modules found");
        item->setDisabled(true);
    }
}

void ModuleLibraryWidget::onSearchTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        // 显示所有项
        for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
            QTreeWidgetItem* topItem = m_treeWidget->topLevelItem(i);
            topItem->setHidden(false);
            for (int j = 0; j < topItem->childCount(); ++j) {
                topItem->child(j)->setHidden(false);
            }
        }
    } else {
        // 过滤项
        QString searchText = text.toLower();
        for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
            QTreeWidgetItem* topItem = m_treeWidget->topLevelItem(i);
            bool hasVisibleChild = false;

            for (int j = 0; j < topItem->childCount(); ++j) {
                QTreeWidgetItem* childItem = topItem->child(j);
                QString itemText = childItem->text(0).toLower();
                if (itemText.contains(searchText)) {
                    childItem->setHidden(false);
                    hasVisibleChild = true;
                } else {
                    childItem->setHidden(true);
                }
            }

            topItem->setHidden(!hasVisibleChild);
        }
    }
}

void ModuleLibraryWidget::onItemDoubleClicked(const QString& moduleType)
{
    qDebug() << "ModuleLibraryWidget: Emitting moduleDoubleClicked:" << moduleType;
    emit moduleDoubleClicked(moduleType);
}

void ModuleLibraryWidget::onItemDragStarted(const QString& moduleType, const QPixmap& pixmap)
{
    Q_UNUSED(pixmap);
    qDebug() << "ModuleLibraryWidget: Emitting moduleDragStarted:" << moduleType;
    emit moduleDragStarted(moduleType);
}
