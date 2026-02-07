#ifndef CENTERCROPNODE_H
#define CENTERCROPNODE_H

#pragma once
#include <QtNodes/NodeDelegateModel>
#include "flow/executor/flowexecutioncontext.h"
#include "flow/graph/flownode.h"
#include <memory>
#include <flow/flowtypes.h>
#include <QSpinBox>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QElapsedTimer>
class CenterCropNode : public QtNodes::NodeDelegateModel, public FlowNode
{
    Q_OBJECT
public:
    static QString staticNodeName;

    CenterCropNode();

    QString caption() const override { return "Center Crop"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return staticNodeName; }

    unsigned int nPorts(QtNodes::PortType type) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex port) override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;

    QVariant getOutput(int port) override;
    void setInput(int port, const QVariant &data) override;
    void compute() override;

    QWidget* embeddedWidget() override { return _widget; }

private:
    QMutex _inputMutex;
    MatPtr _inputImg = nullptr;
    std::shared_ptr<int> _inputInt = nullptr;
    MatPtr _output = nullptr;

    QWidget* _widget = nullptr;
    QSpinBox* _timeoutSpin = nullptr;
    int timeoutMs = 0;
    int counter = 0;
    QElapsedTimer waitTimer;
};


#endif // CENTERCROPNODE_H
