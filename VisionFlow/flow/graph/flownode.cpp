#include "flownode.h"


FlowStatus::NodeStatus FlowNode::getStatus() const
{
    return nodeStatus.load();
}

void FlowNode::setStatus(FlowStatus::NodeStatus s)
{
    nodeStatus.store(s);
}
