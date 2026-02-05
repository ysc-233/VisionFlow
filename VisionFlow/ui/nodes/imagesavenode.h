#ifndef IMAGESAVENODE_H
#define IMAGESAVENODE_H
#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include "imagedata.h"
#include "ui/flowexecutioncontext.h"

class ImageSaveNode : public QtNodes::NodeDelegateModel
{
    Q_OBJECT

public:
    ImageSaveNode();
    QString caption() const override { return "Image Save"; }
    bool captionVisible() const override { return true; }
    QString name() const override { return "ImageSave"; }

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

private:
    QWidget* _widget;
    QLabel* _label;
    QString _path;
};
#endif // IMAGESAVENODE_H
