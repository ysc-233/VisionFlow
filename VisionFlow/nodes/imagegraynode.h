#ifndef IMAGEGRAYNODE_H
#define IMAGEGRAYNODE_H
#pragma once

#include <QtNodes/NodeDelegateModel>
#include "flow/executor/flowexecutioncontext.h"
#include "flow/graph/flownode.h"
#include "flow/flowtypes.h"
class ImageGrayNode : public QtNodes::NodeDelegateModel, public FlowNode
{
    Q_OBJECT

public:
    static QString staticNodeName;
    ImageGrayNode (){setStatus(FlowStatus::NodeStatus::Idle);}
    QString caption() const override { return "Gray"; }
    bool captionVisible() const override { return true; }
    QString name() const override {return staticNodeName; }

    unsigned int nPorts(QtNodes::PortType) const override
    {
        return 1;
    }

    QtNodes::NodeDataType dataType(
        QtNodes::PortType,
        QtNodes::PortIndex) const override
    {
        return {"image","Image"};
    }

    void setInData(std::shared_ptr<QtNodes::NodeData> data,
                   QtNodes::PortIndex) override;

    std::shared_ptr<QtNodes::NodeData>
    outData(QtNodes::PortIndex) override;

    QWidget* embeddedWidget() override { return nullptr; }
    QVariant getOutput(int port) override;
    void setInput(int port, const QVariant &data) override;
    void compute() override;
    std::vector<cv::Mat> outputImages() const override
    {
        if (!_out || _out->empty())
            return {};
        return { *_out };
        //multy
        //return { img1, img2, img3 };
    }
private:
    MatPtr _input;
    MatPtr _out;
};

#endif // IMAGEGRAYNODE_H
