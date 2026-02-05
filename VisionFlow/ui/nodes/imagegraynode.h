#ifndef IMAGEGRAYNODE_H
#define IMAGEGRAYNODE_H
#pragma once

#include <QtNodes/NodeDelegateModel>
#include "imagedata.h"
#include "ui/flowexecutioncontext.h"

class ImageGrayNode : public QtNodes::NodeDelegateModel
{
    Q_OBJECT

public:
    QString caption() const override { return "Gray"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return "ImageGray"; }

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

private:
    QImage _out;
};

#endif // IMAGEGRAYNODE_H
