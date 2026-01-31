#ifndef FLOW_GRAPH_H
#define FLOW_GRAPH_H
#pragma once


#include "flow_node.h"
#include "flow_connection.h"


class FlowGraph
{
public:
    FlowGraph() = default;


    bool addNode(const FlowNode& node);
    bool removeNode(const NodeId& nodeId);
    const FlowNode *node(const QString &nodeId) const ;

    bool addConnection(const FlowConnection& connection);
    bool removeConnection(const PortId& from, const PortId& to);


    const QMap<NodeId, FlowNode>& nodes() const;
    const QVector<FlowConnection>& connections() const;


    bool validate(QString* errorMessage = nullptr) const;
    QVector<NodeId> topologicalSort(bool* ok = nullptr) const;


private:
    QMap<NodeId, FlowNode> m_nodes;
    QVector<FlowConnection> m_connections;
};

#endif // FLOW_GRAPH_H
