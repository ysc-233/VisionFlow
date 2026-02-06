#ifndef NODESEARCHWIDGET_H
#define NODESEARCHWIDGET_H
#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>

class NodeSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NodeSearchWidget(
        const QStringList& models,
        QWidget* parent = nullptr);

signals:
    void nodeSelected(QString name);

private:
    QLineEdit* m_edit;
    QListWidget* m_list;

    QStringList m_allModels;

private slots:
    void filterText(const QString& text);
    void chooseItem();
};


#endif // NODESEARCHWIDGET_H
