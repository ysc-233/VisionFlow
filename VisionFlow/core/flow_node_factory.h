#ifndef FLOW_NODE_FACTORY_H
#define FLOW_NODE_FACTORY_H
#pragma once

#include "flow_node.h"
#include <QString>

/**
 * @brief FlowNodeFactory
 *
 * 职责：
 *  - 将 ModuleDefinition 实例化为 FlowNode
 *  - 保证 Node / Port / Parameter 结构与 ModuleDefinition 完全一致
 *
 * 这是 ModuleDefinition → FlowNode 的唯一合法入口
 */
class FlowNodeFactory
{
public:
    /**
     * @brief createNode
     * @param nodeId       节点唯一 ID（由 Graph / UI / Loader 提供）
     * @param moduleType   模块类型（必须已注册）
     * @return 构造完成的 FlowNode
     *
     * 失败策略：
     *  - 未注册 moduleType：Q_ASSERT
     */
    static FlowNode createNode(
        const NodeId& nodeId,
        const QString& moduleType
    );
};
#endif // FLOW_NODE_FACTORY_H
