#ifndef IMAGESAVENODE_H
#define IMAGESAVENODE_H
#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include "flow/executor/flowexecutioncontext.h"
#include "flow/graph/flownode.h"
#include "flow/flowtypes.h"
class ImageSaveNode : public QtNodes::NodeDelegateModel,public FlowNode
{
    Q_OBJECT

public:
    ImageSaveNode();
    QString caption() const override { return "Image Save"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return flowNodeName; }

    unsigned int nPorts(QtNodes::PortType type) const override
    {
        return type == QtNodes::PortType::In ? 1 : 0;
    }

    QtNodes::NodeDataType dataType(
        QtNodes::PortType,
        QtNodes::PortIndex) const override
    {
        return {"image","Image"};
    }
    std::shared_ptr<QtNodes::NodeData>outData(QtNodes::PortIndex) override{return nullptr;};
    void setInData(std::shared_ptr<QtNodes::NodeData> data,
                   QtNodes::PortIndex) override;

    QWidget* embeddedWidget() override { return _widget; }
    void setInput(int port, const QVariant &data) override;
    QVariant getOutput(int port) override{ return NULL;};
    void compute() override;
private:
    QWidget* _widget;
    QLabel* _label;
    QString _path;
    MatPtr _input;
};
#endif // IMAGESAVENODE_H
