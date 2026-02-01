#include "flow_graph_validator.h"

#include <QSet>

bool FlowGraphValidator::validate(
    const FlowGraph& graph,
    QString* errorMsg)
{
    if (!validateConnections(graph, errorMsg))
        return false;

    if (!validateRequiredInputs(graph, errorMsg))
        return false;

    if (!validateDAG(graph, errorMsg))
        return false;

    return true;
}

// ------------------------------------------------------------
// 连接合法性校验
// ------------------------------------------------------------
bool FlowGraphValidator::validateConnections(
    const FlowGraph& graph,
    QString* errorMsg)
{
    for (const FlowConnection& c : graph.connections())
    {
        const FlowPort* from = graph.port(c.fromPort().node, c.fromPort());
        const FlowPort* to   = graph.port(c.toPort().node, c.toPort());

        if (!from || !to)
        {
            setError(errorMsg, "Connection references non-existing port");
            return false;
        }

        if (from->direction() != PortDirection::Output ||
            to->direction()   != PortDirection::Input)
        {
            setError(errorMsg, "Invalid port direction (must be Output -> Input)");
            return false;
        }

        // DataType 校验（Any 视为通配）
        if (from->dataType() != DataType::Any &&
            to->dataType()   != DataType::Any &&
            from->dataType() != to->dataType())
        {
            setError(errorMsg,
                QString("DataType mismatch: %1 -> %2")
                    .arg(static_cast<int>(from->dataType()))
                    .arg(static_cast<int>(to->dataType())));
            return false;
        }
    }

    return true;
}

// ------------------------------------------------------------
// 必填输入校验（Input Port 必须有连接）
// ------------------------------------------------------------
bool FlowGraphValidator::validateRequiredInputs(
    const FlowGraph& graph,
    QString* errorMsg)
{
    QSet<PortId> connectedInputs;

    for (const FlowConnection& c : graph.connections())
        connectedInputs.insert(c.toPort());

    for (const auto& nodePair : graph.nodes())
    {
        const FlowNode& node = nodePair;

        for (const FlowPort& port : node.inputPorts())
        {
            if (!connectedInputs.contains(port.id()))
            {
                setError(errorMsg,
                    QString("Unconnected input port: %1")
                        .arg(port.id().port));
                return false;
            }
        }
    }

    return true;
}

// ------------------------------------------------------------
// DAG 校验（是否存在环）
// ------------------------------------------------------------
bool FlowGraphValidator::validateDAG(
    const FlowGraph& graph,
    QString* errorMsg)
{
    // 复用 Graph 的拓扑排序能力
    QVector<NodeId> order = graph.topologicalSort();

    if (order.size() != graph.nodes().size())
    {
        setError(errorMsg, "Graph contains cycle (not a DAG)");
        return false;
    }

    return true;
}

// ------------------------------------------------------------
void FlowGraphValidator::setError(QString* errorMsg, const QString& msg)
{
    if (errorMsg)
        *errorMsg = msg;
}
