#include "flow_executor.h"
// =======================================================
// core/flow_executor.cpp（修改版）
// =======================================================
#include "flow_executor.h"
#include "flow_graph_validator.h"
#include <QDebug>
FlowExecutor::FlowExecutor(const FlowGraph& graph)
    : m_graph(graph)
{
}


void FlowExecutor::stop()
{
    m_stopRequested = true;
}


ModulePortBinding FlowExecutor::buildBinding(const FlowNode& node) const
{
    ModulePortBinding binding;
    const NodeId& nodeId = node.id();

    // =====================================================
    // 输出端口：端口名 -> PortId
    // =====================================================
    for (const FlowPort& port : node.outputPorts())
    {
        // 端口名是 port.id().port
        QString portName = port.id().port;
        // 创建对应的 PortId
        PortId pid(nodeId, portName);
        // 使用端口名作为键
        binding.outputs.insert(portName, pid);

        qDebug() << "[Binding DEBUG] Output binding:"
                 << portName << "->" << pid.toString();
    }

    // =====================================================
    // 输入端口：从连接中查找上游 PortId
    // =====================================================
    for (const FlowPort& port : node.inputPorts())
    {
        QString portName = port.id().port;
        bool found = false;

        for (const FlowConnection& c : m_graph.connections())
        {
            const PortId& to = c.toPort();

            // 匹配当前节点的输入端口
            if (to.node == nodeId && to.port == portName)
            {
                binding.inputs.insert(portName, c.fromPort());
                found = true;
                qDebug() << "[Binding DEBUG] Input binding found:"
                         << portName << "->" << c.fromPort().toString();
                break;
            }
        }

        if (!found)
        {
            qWarning() << "[Binding DEBUG] Input port not bound:"
                       << nodeId << "." << portName;
        }
    }

    return binding;
}

bool FlowExecutor::run()
{
    QString error;
    if (!FlowGraphValidator::validate(m_graph, &error))
    {
        qWarning() << "[FlowExecutor] Graph validation failed:" << error;
        return false;
    }

    m_context.clear();
    m_stopRequested = false;


    const auto order = m_graph.topologicalSort();


    for (const auto& nodeId : order)
    {
    if (m_stopRequested)
    return false;


    const FlowNode* node = m_graph.node(nodeId);
    if (!node)
    return false;


    auto module = ModuleRegistry::instance().createModule(node->moduleType());
    if (!module)
    return false;


    ModulePortBinding binding = buildBinding(*node);
    module->bind(binding, node->parameters(), &m_context);


    if (!module->execute())
    return false;
    }
    return true;
}
