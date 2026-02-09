#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H
#pragma once

#include <QTabWidget>
#include <QToolButton>
#include <QTabBar>
#include <QResizeEvent>

class CustomTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit CustomTabBar(QWidget *parent = nullptr);
    void updateAddButtonPosition();
signals:
    void addButtonClicked();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void tabLayoutChange() override;

private:


    QToolButton *m_addButton;
};

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit CustomTabWidget(QWidget *parent = nullptr);

signals:
    void addButtonClicked();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    CustomTabBar *m_customTabBar;
};
#endif // CUSTOMTABWIDGET_H
