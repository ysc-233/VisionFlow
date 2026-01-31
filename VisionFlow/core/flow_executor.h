#ifndef FLOW_EXECUTOR_H
#define FLOW_EXECUTOR_H
// =======================================================
// core/flow_executor.h（修改版）
// =======================================================
#pragma once


#include "flow_graph.h"
#include "flow_context.h"
#include "modules/module_registry.h"


class FlowExecutor
{
public:
    explicit FlowExecutor(const FlowGraph& graph);

    bool run();
    void stop();

private:
    ModulePortBinding buildBinding(const FlowNode& node) const;

private:
    const FlowGraph& m_graph;
    FlowContext m_context;
    bool m_stopRequested = false;
};

#endif // FLOW_EXECUTOR_H
