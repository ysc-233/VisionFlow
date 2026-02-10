#ifndef FLOWNODE_H
#define FLOWNODE_H
#pragma once
#include <QObject>
#include <QMutex>
#include <QJsonObject>
#include "flow/flowtypes.h"
using FlowNodeId = quint64;

class FlowNode
{
public:
    virtual ~FlowNode() = default;

    virtual QVariant getOutput(int port) = 0;

    virtual void setInput(int port,const QVariant& data) = 0;

    virtual void compute() = 0;

    virtual QJsonObject saveData() const
    {
        return QJsonObject();
    }

    virtual void loadData(const QJsonObject&){}
    virtual std::vector<cv::Mat> outputImages() const
    {
        return {};
    }

    FlowStatus::NodeStatus getStatus() const;
    void setStatus(FlowStatus::NodeStatus s);

    mutable QMutex _statusMutex;
    std::atomic<FlowStatus::NodeStatus> nodeStatus;
};
#endif // FLOWNODE_H
