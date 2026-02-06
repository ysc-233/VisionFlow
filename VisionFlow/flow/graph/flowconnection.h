#ifndef FLOWCONNECTION_H
#define FLOWCONNECTION_H

#pragma once
#include "flownode.h"

struct FlowConnection
{
    FlowNodeId fromNode;
    int fromPort;

    FlowNodeId toNode;
    int toPort;
};

#endif // FLOWCONNECTION_H
