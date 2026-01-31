#include "flow_executor.h"
// =======================================================
// core/flow_executor.cpp（修改版）
// =======================================================
#include "flow_executor.h"

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

        // -------- input ports --------
        for (const FlowPort& port : node.inputPorts())
        {
            const QString targetKey = port.contextKey(); // e.g. "n2.in"

            for (const FlowConnection& c : m_graph.connections())
            {
                if (c.toPort() == targetKey)
                {
                    // input("in") → 读取 fromPort 的值
                    binding.inputs.insert(port.id(), c.fromPort());
                    break;
                }
            }
        }

        // -------- output ports --------
        for (const FlowPort& port : node.outputPorts())
        {
            // output("out") → 写入自己的 key
            binding.outputs.insert(port.id(), port.contextKey());
        }

        return binding;
}


bool FlowExecutor::run()
{
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
