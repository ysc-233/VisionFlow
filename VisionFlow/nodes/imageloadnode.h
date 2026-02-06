#ifndef IMAGELOADNODE_H
#define IMAGELOADNODE_H
#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include "flow/executor/flowexecutioncontext.h"
#include "flow/graph/flownode.h"
#include "flow/flowtypes.h"
class ImageLoadNode : public QtNodes::NodeDelegateModel,public FlowNode
{
    Q_OBJECT

public:
    ImageLoadNode();

    QString caption() const override { return "Image Load"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return flowNodeName; }

    unsigned int nPorts(QtNodes::PortType type) const override
    {
        return type == QtNodes::PortType::Out ? 1 : 0;
    }

    QtNodes::NodeDataType dataType(
        QtNodes::PortType,
        QtNodes::PortIndex) const override
    {
        return {"image","Image"};
    }

    std::shared_ptr<QtNodes::NodeData>
    outData(QtNodes::PortIndex) override;

    void setInData(
        std::shared_ptr<QtNodes::NodeData>,
        QtNodes::PortIndex) override {}

    QWidget* embeddedWidget() override { return _widget; }

    QVariant getOutput(int port) override;
    void setInput(int port, const QVariant &data) override{return;}
    void compute() override;

private:
    QWidget* _widget;
    QLabel* _label;
    MatPtr  _image;
};
#endif // IMAGELOADNODE_H
