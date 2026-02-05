#ifndef IMAGEDATA_H
#define IMAGEDATA_H
#pragma once
#include <QtNodes/NodeData>
#include <QImage>

class ImageData : public QtNodes::NodeData
{
public:
    ImageData() {}
    ImageData(const QImage& img) : _image(img) {}

    QtNodes::NodeDataType type() const override
    {
        return {"image", "Image"};
    }

    QImage image() const { return _image; }

private:
    QImage _image;
};
#endif // IMAGEDATA_H
