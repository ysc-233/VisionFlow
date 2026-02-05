#ifndef IMAGEMERGENODE_H
#define IMAGEMERGENODE_H
#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QImage>
#include "imagedata.h"
#include "ui/flowexecutioncontext.h"
#include <QComboBox>
#include <QVBoxLayout>
#include <QWidget>
class ImageMergeNode : public QtNodes::NodeDelegateModel
{
    Q_OBJECT

public:
    ImageMergeNode();
    QString caption() const override { return "Image Merge"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return "ImageMerge"; }

    unsigned int nPorts(QtNodes::PortType type) const override
    {
        if (type == QtNodes::PortType::In)
            return 2;
        return 1;
    }

    QtNodes::NodeDataType dataType(
        QtNodes::PortType,
        QtNodes::PortIndex) const override
    {
        return {"image","Image"};
    }

    void setInData(std::shared_ptr<QtNodes::NodeData> data,
                   QtNodes::PortIndex port) override;

    std::shared_ptr<QtNodes::NodeData>
    outData(QtNodes::PortIndex) override;

    QWidget* embeddedWidget() override { return _widget; }

private:
    void compute();

private:
    QImage _inputs[2];
    bool _ready[2] = {false, false};
    QWidget* _widget = nullptr;
    QComboBox* _modeBox = nullptr;
    QImage _out;
};
#endif // IMAGEMERGENODE_H
