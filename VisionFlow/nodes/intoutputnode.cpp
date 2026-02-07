#include "intoutputnode.h"
#include "flownodeautoregister.h"
#include "flow/flowtypes.h"

#include <memory>
QString IntOutputNode::staticNodeName = "IntOutput";
// 返回 NodeData 形式的 int，供下游节点接收
std::shared_ptr<QtNodes::NodeData> IntOutputNode::outData(QtNodes::PortIndex)
{
    return std::make_shared<IntNodeData>(value);
}

void IntOutputNode::compute()
{
    qDebug()<<__FUNCTION__<<value;
    setStatus( FlowStatus::NodeStatus::Done);
}

REGISTER_NODE(IntOutputNode, "Constant")
