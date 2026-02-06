#ifndef FLOWNODE_H
#define FLOWNODE_H
#pragma once
#include <QObject>
using FlowNodeId = quint64;

class FlowNode
{
public:
    virtual ~FlowNode() = default;

    virtual QVariant getOutput(int port) = 0;

    virtual void setInput(int port,const QVariant& data) = 0;

    virtual void compute() = 0;

    QString flowNodeName;
};
#endif // FLOWNODE_H
