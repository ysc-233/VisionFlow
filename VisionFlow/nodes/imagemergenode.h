#ifndef IMAGEMERGENODE_H
#define IMAGEMERGENODE_H
#pragma once

#include <QtNodes/NodeDelegateModel>
#include "flow/executor/flowexecutioncontext.h"

#include <QComboBox>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>
#include <opencv2/opencv.hpp>
#include "flow/graph/flownode.h"
#include "flow/flowtypes.h"
class ImageMergeNode : public QtNodes::NodeDelegateModel,public FlowNode
{
    Q_OBJECT

public:
    static QString staticNodeName;
    ImageMergeNode();

    QString caption() const override { return "Image Merge"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return staticNodeName; }

    unsigned int nPorts(QtNodes::PortType type) const override
    {
        return (type == QtNodes::PortType::In) ? 2 : 1;
    }

    QtNodes::NodeDataType dataType(
        QtNodes::PortType,
        QtNodes::PortIndex) const override
    {
        return {"image", "Image"};
    }

    void setInData(std::shared_ptr<QtNodes::NodeData> data,
                   QtNodes::PortIndex port) override;

    std::shared_ptr<QtNodes::NodeData>
    outData(QtNodes::PortIndex) override;

    QWidget* embeddedWidget() override { return _widget; }
    void setInput(int port, const QVariant &data) override;
    QVariant getOutput(int port) override;
    void compute() override;

private:
    MatPtr _inputs[2];
    bool _ready[2] = {false, false};
    MatPtr _out;

    QWidget* _widget = nullptr;
    QComboBox* _modeBox = nullptr;
};
#endif // IMAGEMERGENODE_H
