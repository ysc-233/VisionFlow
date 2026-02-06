#ifndef FLOWGRAPHEXPORTER_H
#define FLOWGRAPHEXPORTER_H
#pragma once

#include "flow/graph/flowgraph.h"
#include <QtNodes/DataFlowGraphModel>

class FlowGraphExporter
{
public:
    static FlowGraph exportGraph(
        QtNodes::DataFlowGraphModel& model);
};

#endif // FLOWGRAPHEXPORTER_H
