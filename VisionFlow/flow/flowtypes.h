#ifndef FLOWTYPES_H
#define FLOWTYPES_H
#pragma once

#include <QMetaType>
#include <QtNodes/NodeData>
#include <opencv2/opencv.hpp>
#include <memory>
#include <QMetaEnum>
using MatPtr = std::shared_ptr<cv::Mat>;
Q_DECLARE_METATYPE(MatPtr)

class FlowStatus : public QObject
{
    Q_OBJECT
public:
    enum NodeStatus {
        Idle,
        Waiting,
        Done,
        Timeout,
        Running
    };
    Q_ENUM(NodeStatus)

    static QString ConverToString(NodeStatus status)
    {
        const QMetaObject &mo = FlowStatus::staticMetaObject;
        int index = mo.indexOfEnumerator("NodeStatus");
        if (index < 0)
            return {};
        QMetaEnum metaEnum = mo.enumerator(index);
        return metaEnum.valueToKey(static_cast<int>(status));
    }
};

class ImageData : public QtNodes::NodeData
{
public:
    ImageData() = default;

    ImageData(std::shared_ptr<cv::Mat> img)
        : _image(img)
    {}

    QtNodes::NodeDataType type() const override
    {
        return {"image", "Image"};
    }

    std::shared_ptr<cv::Mat> matPtr()
    {
        return _image;
    }

    std::shared_ptr<const cv::Mat> matPtr() const
    {
        return _image;
    }

private:
    std::shared_ptr<cv::Mat> _image;
};

class IntNodeData : public QtNodes::NodeData
{
public:
    IntNodeData(int v)
        : _value(std::make_shared<int>(v)) {}

    QtNodes::NodeDataType type() const override
    {
        return {"int", "Integer"};
    }

    std::shared_ptr<int> ptr() { return _value; }

private:
    std::shared_ptr<int> _value;
};

#endif // FLOWTYPES_H
