#include "nodesearchwidget.h"

NodeSearchWidget::NodeSearchWidget(
    const QStringList& models,
    QWidget* parent)
    : QWidget(parent),
      m_allModels(models)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(2);

    m_edit = new QLineEdit;
    m_list = new QListWidget;

    layout->addWidget(m_edit);
    layout->addWidget(m_list);

    m_list->addItems(models);

    connect(m_edit, &QLineEdit::textChanged,
            this, &NodeSearchWidget::filterText);

    connect(m_edit, &QLineEdit::returnPressed,
            this, &NodeSearchWidget::chooseItem);

    connect(m_list, &QListWidget::itemDoubleClicked,
            this, &NodeSearchWidget::chooseItem);

    m_edit->setFocus();
}

void NodeSearchWidget::filterText(const QString& text)
{
    m_list->clear();

    for (auto const& name : m_allModels)
    {
        if (name.contains(text, Qt::CaseInsensitive))
            m_list->addItem(name);
    }

    if (m_list->count() > 0)
        m_list->setCurrentRow(0);
}

void NodeSearchWidget::chooseItem()
{
    auto item = m_list->currentItem();
    if (!item)
        return;

    emit nodeSelected(item->text());
}
