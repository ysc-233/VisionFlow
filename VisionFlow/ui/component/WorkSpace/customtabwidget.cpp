#include "customtabwidget.h"
#include <QStyleOptionTab>
#include <QStyle>

static const QString ADD_TAB_TEXT = QStringLiteral("+");

// ------------------------------------------------------------
// CustomTabBar
// ------------------------------------------------------------
CustomTabBar::CustomTabBar(QWidget *parent)
    : QTabBar(parent)
    , m_updatingAddTab(false)
{
    // 初始时添加一个“+”标签页
    addTab(ADD_TAB_TEXT);
    ensureAddTabLast();

    // 点击标签页时判断是否点击了“+”标签页
    connect(this, &QTabBar::tabBarClicked,
            this, &CustomTabBar::onTabBarClicked);
}

void CustomTabBar::tabInserted(int index)
{
    QTabBar::tabInserted(index);
    ensureAddTabLast();
}

void CustomTabBar::tabRemoved(int index)
{
    QTabBar::tabRemoved(index);
    ensureAddTabLast();
}

void CustomTabBar::tabMoved(int from, int to)
{
    QTabBar::tabMoved(from, to);
    ensureAddTabLast();
}

void CustomTabBar::onTabBarClicked(int index)
{
    if (isAddTab(index))
        emit addButtonClicked();
}

void CustomTabBar::ensureAddTabLast()
{
    if (m_updatingAddTab)
        return;

    m_updatingAddTab = true;

    int addIndex = findAddTab();

    // 如果没有“+”标签页，则在末尾添加一个
    if (addIndex == -1) {
        addIndex = addTab(ADD_TAB_TEXT);
    }

    // 确保“+”标签页在最后一个位置
    int lastIndex = count() - 1;
    if (addIndex != lastIndex) {
        moveTab(addIndex, lastIndex);
        addIndex = lastIndex; // 更新索引
    }

    // 设置“+”标签页的样式和行为
    setTabText(addIndex, ADD_TAB_TEXT);
    setTabToolTip(addIndex, tr("Add new tab"));

    // 固定宽度（可根据需要调整）
    setTabTextColor(addIndex, QColor(0, 120, 215)); // 蓝色，更像链接
    setTabIcon(addIndex, QIcon()); // 不使用图标

    // 禁用关闭按钮（即使 tabsClosable 为 true）
    if (tabsClosable()) {
        setTabButton(addIndex, QTabBar::RightSide, nullptr);
        setTabButton(addIndex, QTabBar::LeftSide,  nullptr);
    }

    // 可选：让“+”标签页不可移动
    // setTabEnabled(addIndex, true); // 保持可用

    m_updatingAddTab = false;
}

bool CustomTabBar::isAddTab(int index) const
{
    if (index < 0 || index >= count())
        return false;
    return tabText(index) == ADD_TAB_TEXT;
}

int CustomTabBar::findAddTab() const
{
    for (int i = 0; i < count(); ++i) {
        if (tabText(i) == ADD_TAB_TEXT)
            return i;
    }
    return -1;
}

// ------------------------------------------------------------
// CustomTabWidget
// ------------------------------------------------------------
CustomTabWidget::CustomTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    m_customTabBar = new CustomTabBar();
    setTabBar(m_customTabBar);

    // 转发“+”标签页的点击信号
    connect(m_customTabBar, &CustomTabBar::addButtonClicked,
            this, &CustomTabWidget::addButtonClicked);
}
