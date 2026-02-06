#ifndef FLOWGRAPH_H
#define FLOWGRAPH_H
#pragma once

#include <QVector>
#include <QHash>
#include <QtNodes/DataFlowGraphModel>
#include "flownode.h"
#include "flowconnection.h"

struct FlowNodeInstance
{
    FlowNodeId id;

    QtNodes::NodeDelegateModel* model = nullptr;
    FlowNode* flowNode = nullptr;
};

class FlowGraph
{
public:
    QVector<FlowNodeInstance> nodes;
    QVector<FlowConnection> connections;

    QHash<FlowNodeId, QVector<FlowNodeId>> adjacency;

public:
    void buildAdjacency();
};
#endif // FLOWGRAPH_H
