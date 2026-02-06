#include "flowgraphexporter.h"
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/DataFlowGraphModel>
using namespace QtNodes;
FlowGraph FlowGraphExporter::exportGraph(
    DataFlowGraphModel& model)
{
    FlowGraph graph;

    QHash<NodeId, FlowNodeId> idMap;
    FlowNodeId nextId = 1;

    // ---------- nodes ----------
    for (auto nodeId : model.allNodeIds())
    {
        auto* delegate =
            model.delegateModel<NodeDelegateModel>(nodeId);

        if (!delegate)
            continue;

        FlowNodeId fid = nextId++;
        idMap[nodeId] = fid;

        FlowNodeInstance inst;
        inst.id = fid;
        inst.model = delegate;

        // 关键：获取执行接口
        inst.flowNode =
            dynamic_cast<FlowNode*>(delegate);

        graph.nodes.push_back(inst);
    }

    // ---------- connections ----------
    for (auto nodeId : model.allNodeIds())
    {
        auto conns =
            model.allConnectionIds(nodeId);

        for (auto const& c : conns)
        {
            // 只处理 out → in
            if (c.outNodeId != nodeId)
                continue;

            if (!idMap.contains(c.outNodeId) ||
                !idMap.contains(c.inNodeId))
                continue;

            FlowConnection fc;

            fc.fromNode = idMap[c.outNodeId];
            fc.fromPort = c.outPortIndex;

            fc.toNode = idMap[c.inNodeId];
            fc.toPort = c.inPortIndex;

            graph.connections.push_back(fc);
        }
    }

    graph.buildAdjacency();

    return graph;
}

