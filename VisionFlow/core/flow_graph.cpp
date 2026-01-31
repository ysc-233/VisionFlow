#include "flow_graph.h"

// =======================================================
// core/flow_graph.cpp
// FlowGraph 的核心实现：结构校验 + 拓扑排序
// =======================================================

#include <QSet>
#include <QQueue>

// ------------------------------
// 添加 / 删除节点
// ------------------------------

bool FlowGraph::addNode(const FlowNode& node)
{
    if (m_nodes.contains(node.id()))
        return false;

    m_nodes.insert(node.id(), node);
    return true;
}

bool FlowGraph::removeNode(const NodeId& nodeId)
{
    if (!m_nodes.contains(nodeId))
        return false;

    // 删除与该节点相关的所有连接
    auto it = m_connections.begin();
    while (it != m_connections.end())
    {
        const FlowConnection& c = *it;
        bool related = false;

        for (const auto& node : m_nodes)
        {
            Q_UNUSED(node);
        }

        // 简化判断：端口 ID 以 NodeId 作为前缀（约定）
        if (c.fromPort().startsWith(nodeId) ||
                c.toPort().startsWith(nodeId))
        {
            related = true;
        }

        if (related)
            it = m_connections.erase(it);
        else
            ++it;
    }

    m_nodes.remove(nodeId);
    return true;
}

// 返回指针，如果不存在则返回nullptr
const FlowNode *FlowGraph::node(const QString &nodeId) const {
    auto it = m_nodes.find(nodeId);
    return (it != m_nodes.end()) ? &(*it) : nullptr;
}

// ------------------------------
// 添加 / 删除连接
// ------------------------------

bool FlowGraph::addConnection(const FlowConnection& connection)
{
    // 不允许重复连接
    for (const auto& c : m_connections)
    {
        if (c.fromPort() == connection.fromPort() &&
                c.toPort() == connection.toPort())
        {
            return false;
        }
    }

    m_connections.push_back(connection);
    return true;
}

bool FlowGraph::removeConnection(const PortId& from, const PortId& to)
{
    auto it = m_connections.begin();
    while (it != m_connections.end())
    {
        if (it->fromPort() == from && it->toPort() == to)
        {
            it = m_connections.erase(it);
            return true;
        }
        ++it;
    }
    return false;
}

// ------------------------------
// 访问器
// ------------------------------

const QMap<NodeId, FlowNode>& FlowGraph::nodes() const
{
    return m_nodes;
}

const QVector<FlowConnection>& FlowGraph::connections() const
{
    return m_connections;
}

// ------------------------------
// 流程合法性校验
// ------------------------------

bool FlowGraph::validate(QString* errorMessage) const
{
    // 1. 检查连接端口是否存在
    QSet<PortId> allPorts;
    for (const auto& node : m_nodes)
    {
        for (const auto& port : node.ports())
        {
            allPorts.insert(port.id());
        }
    }

    for (const auto& c : m_connections)
    {
        if (!allPorts.contains(c.fromPort()) ||
                !allPorts.contains(c.toPort()))
        {
            if (errorMessage)
                *errorMessage = "Connection refers to non-existent port.";
            return false;
        }
    }

    // 2. 检查是否存在环（通过拓扑排序验证）
    bool ok = false;
    topologicalSort(&ok);
    if (!ok)
    {
        if (errorMessage)
            *errorMessage = "FlowGraph contains cycle.";
        return false;
    }

    return true;
}

// ------------------------------
// 拓扑排序（Kahn 算法）
// ------------------------------

QVector<NodeId> FlowGraph::topologicalSort(bool* ok) const
{
    QVector<NodeId> result;

    // 1. 构建入度表
    QMap<NodeId, int> inDegree;
    QMap<NodeId, QVector<NodeId>> adjacency;

    for (const auto& nodeId : m_nodes.keys())
    {
        inDegree[nodeId] = 0;
    }

    // 简化假设：通过端口前缀解析所属 NodeId
    auto portToNode = [](const PortId& pid) -> NodeId {
        int idx = pid.indexOf(":");
        return idx > 0 ? pid.left(idx) : NodeId();
    };

    for (const auto& c : m_connections)
    {
        NodeId fromNode = portToNode(c.fromPort());
        NodeId toNode   = portToNode(c.toPort());

        if (fromNode.isEmpty() || toNode.isEmpty())
            continue;

        adjacency[fromNode].push_back(toNode);
        inDegree[toNode]++;
    }

    // 2. 初始化队列（入度为 0 的节点）
    QQueue<NodeId> queue;
    for (auto it = inDegree.begin(); it != inDegree.end(); ++it)
    {
        if (it.value() == 0)
            queue.enqueue(it.key());
    }

    // 3. Kahn
    while (!queue.isEmpty())
    {
        NodeId n = queue.dequeue();
        result.push_back(n);

        for (const auto& next : adjacency[n])
        {
            inDegree[next]--;
            if (inDegree[next] == 0)
                queue.enqueue(next);
        }
    }

    // 4. 是否成功
    if (ok)
        *ok = (result.size() == m_nodes.size());

    return result;
}
