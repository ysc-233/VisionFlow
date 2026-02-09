#include "customtabwidget.h"
#include <QStyle>
#include <QStyleOptionTab>

CustomTabBar::CustomTabBar(QWidget *parent)
    : QTabBar(parent)
{
    m_addButton = new QToolButton(this);
    m_addButton->setText("+");
    m_addButton->setToolTip("Add New Tab");
    m_addButton->setFixedSize(24, 24);
    m_addButton->setStyleSheet(R"(
        QToolButton {
            border: 1px solid #cccccc;
            border-radius: 3px;
            background-color: #f0f0f0;
        }
        QToolButton:hover {
            background-color: #e0e0e0;
        }
        QToolButton:pressed {
            background-color: #d0d0d0;
        }
    )");

    connect(m_addButton, &QToolButton::clicked,
            this, &CustomTabBar::addButtonClicked);

    updateAddButtonPosition();
}

void CustomTabBar::resizeEvent(QResizeEvent *event)
{
    QTabBar::resizeEvent(event);
    updateAddButtonPosition();
}

void CustomTabBar::tabLayoutChange()
{
    QTabBar::tabLayoutChange();
    updateAddButtonPosition();
}

void CustomTabBar::updateAddButtonPosition()
{
    if (count() == 0) {

        m_addButton->move(5, (height() - m_addButton->height()) / 2);
        return;
    }

    QRect lastTabRect = tabRect(count() - 1);

    int x = lastTabRect.right() + 5;
    int y = (height() - m_addButton->height()) / 2;

    int maxX = width() - m_addButton->width() - 5;
    if (x > maxX) {
        x = maxX;
    }

    m_addButton->move(x, y);
    m_addButton->show();
}

CustomTabWidget::CustomTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    m_customTabBar = new CustomTabBar();
    QTabWidget::setTabBar(m_customTabBar);

    connect(m_customTabBar, &CustomTabBar::addButtonClicked,
            this, &CustomTabWidget::addButtonClicked);
}

void CustomTabWidget::resizeEvent(QResizeEvent *event)
{
    QTabWidget::resizeEvent(event);

    m_customTabBar->updateAddButtonPosition();
}
