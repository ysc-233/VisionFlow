#include "flowgraph.h"

void FlowGraph::buildAdjacency()
{
    adjacency.clear();

    for (auto& c : connections)
    {
        adjacency[c.fromNode].push_back(c.toNode);
    }
}
