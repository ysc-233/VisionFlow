#ifndef FLOW_GRAPH_VALIDATOR_H
#define FLOW_GRAPH_VALIDATOR_H

#pragma once

#include "flow_graph.h"
#include <QString>

/**
 * @brief FlowGraphValidator
 *
 * 职责：
 *  - 在执行前对 FlowGraph 进行完整一致性校验
 *  - 保证 Executor 可以“无脑跑”
 *
 * 校验内容：
 *  - Node / Port 存在性
 *  - 连接方向合法性（Output -> Input）
 *  - DataType 兼容性
 *  - 必填输入是否已连接
 *  - 图是否为 DAG
 */
class FlowGraphValidator
{
public:
    /**
     * @brief validate
     * @param graph       被校验的 FlowGraph
     * @param errorMsg    可选错误输出
     * @return true 表示图可执行
     */
    static bool validate(const FlowGraph& graph, QString* errorMsg = nullptr);

private:
    static bool validateConnections(
        const FlowGraph& graph,
        QString* errorMsg);

    static bool validateRequiredInputs(
        const FlowGraph& graph,
        QString* errorMsg);

    static bool validateDAG(
        const FlowGraph& graph,
        QString* errorMsg);

    static void setError(QString* errorMsg, const QString& msg);
};

#endif // FLOW_GRAPH_VALIDATOR_H
