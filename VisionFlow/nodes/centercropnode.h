#ifndef CENTERCROPNODE_H
#define CENTERCROPNODE_H
#pragma once
#include <QtNodes/NodeDelegateModel>
#include "flow/executor/flowexecutioncontext.h"
#include "flow/graph/flownode.h"
#include <memory>
#include <flow/flowtypes.h>
#include <QSpinBox>
class CenterCropNode : public QtNodes::NodeDelegateModel, public FlowNode
{
    Q_OBJECT
public:
    CenterCropNode();

    QString caption() const override { return "Center Crop"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return flowNodeName; }

    unsigned int nPorts(QtNodes::PortType type) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port) override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;

    QVariant getOutput(int port) override;
    void setInput(int port, const QVariant &data) override;
    void compute() override;

    QWidget* embeddedWidget() override { return _widget; }

private:
    MatPtr _inputImg = nullptr;
    std::shared_ptr<int> _inputInt = nullptr;
    MatPtr _output = nullptr;

    QWidget* _widget = nullptr;
    QSpinBox* _timeoutSpin = nullptr; // UI 输入框
    int timeoutMs = 0;                // 节点内部存储
};


#endif // CENTERCROPNODE_H
