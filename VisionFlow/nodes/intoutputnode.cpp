#include "intoutputnode.h"
#include "flownodeautoregister.h"
#include "flow/flowtypes.h"

#include <memory>

// 返回 NodeData 形式的 int，供下游节点接收
std::shared_ptr<QtNodes::NodeData> IntOutputNode::outData(QtNodes::PortIndex)
{
    // 需要自己封装一个 NodeData 类型
    return std::make_shared<IntNodeData>(value);
}

REGISTER_NODE(IntOutputNode, "IntOutputNode")
