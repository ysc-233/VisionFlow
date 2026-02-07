#ifndef INTOUTPUTNODE_H
#define INTOUTPUTNODE_H
#pragma once
#include <QtNodes/NodeDelegateModel>
#include "flow/graph/flownode.h"
#include <QSpinBox>
#include <QVBoxLayout>

class IntOutputNode : public QtNodes::NodeDelegateModel, public FlowNode
{
    Q_OBJECT

public:
    static QString staticNodeName;
    IntOutputNode(int initValue = 0)
        : FlowNode(), value(initValue)
    {
        setStatus(FlowStatus::NodeStatus::Idle);
        _widget = new QWidget();
        auto layout = new QVBoxLayout(_widget);
        _spinBox = new QSpinBox();
        _spinBox->setRange(0, 100);
        _spinBox->setValue(value);

        layout->addWidget(_spinBox);

        connect(_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v){
            value = v;
            Q_EMIT dataUpdated(0);
        });
    }

    QString caption() const override { return "Int Output"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return staticNodeName; }

    unsigned int nPorts(QtNodes::PortType type) const override
    {
        return type == QtNodes::PortType::Out ? 1 : 0;
    }

    QtNodes::NodeDataType dataType(
        QtNodes::PortType type,
        QtNodes::PortIndex) const override
    {
        return {"int", "Integer"};
    }

    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex) override {}

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;

    QWidget* embeddedWidget() override { return _widget; }

    QVariant getOutput(int) override { return QVariant::fromValue(value); }

    void setInput(int, const QVariant&) override {}
    void compute() override;

private:
    int value;
    QSpinBox* _spinBox = nullptr;
    QWidget* _widget = nullptr;
};
#endif // INTOUTPUTNODE_H
