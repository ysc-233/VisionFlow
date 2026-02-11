#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H
#include <QTabWidget>
#include <QTabBar>

class CustomTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit CustomTabBar(QWidget *parent = nullptr);

signals:
    void addButtonClicked();
protected:
    void tabInserted(int index) override;
    void tabRemoved(int index) override;
    void tabMoved(int from, int to);

private slots:
    void onTabBarClicked(int index);

private:
    void ensureAddTabLast();   // 确保“+”标签页存在且在最后
    bool isAddTab(int index) const; // 判断是否为“+”标签页
    int findAddTab() const;   // 查找“+”标签页的索引，未找到返回-1

    bool m_updatingAddTab;    // 防止递归更新
};

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit CustomTabWidget(QWidget *parent = nullptr);

signals:
    void addButtonClicked();

private:
    CustomTabBar *m_customTabBar;
};

#endif // CUSTOMTABWIDGET_H
