#ifndef IMAGELOADNODE_H
#define IMAGELOADNODE_H
#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include "imagedata.h"
#include "ui/flowexecutioncontext.h"

class ImageLoadNode : public QtNodes::NodeDelegateModel
{
    Q_OBJECT

public:
    ImageLoadNode();

    QString caption() const override { return "Image Load"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return "ImageLoad"; }

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
    void setInData(std::shared_ptr<QtNodes::NodeData>,QtNodes::PortIndex) override {}
    QWidget* embeddedWidget() override { return _widget; }

    void compute();

private:
    QWidget* _widget;
    QLabel* _label;
    QImage _image;
};
#endif // IMAGELOADNODE_H
