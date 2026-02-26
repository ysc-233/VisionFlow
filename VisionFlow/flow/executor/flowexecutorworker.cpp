#include "flowexecutorworker.h"
#include <QHash>
#include <QQueue>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include "flow/executor/flowexecutioncontext.h"
#include "utils/logmanager.h"

void FlowExecutorWorker::run(FlowGraph &graph)
{
    // ---------- 拓扑排序 ----------
    auto order = topologicalSort(graph);
    QString strOrder = "[";
    for (int i = 0; i < order.size(); ++i) {
        strOrder += QString::number(order[i]);
        if (i < order.size() - 1) strOrder += ", ";
    }
    strOrder += "]";  // "[1, 2, 3]"
    LOG_INFO(QString("[FlowWorker] Execution order: %1").arg(strOrder));

    // ---------- 建立 id -> instance 映射 ----------
    QHash<FlowNodeId, FlowNodeInstance*> map;
    for (auto& n : graph.nodes)
        map[n.id] = &n;

    // ---------- 建立输出连接缓存 ----------
    QHash<FlowNodeId, QVector<FlowConnection>> outputMap;
    for (auto& conn : graph.connections)
        outputMap[conn.fromNode].push_back(conn);

    // ---------- 计时 ----------
    QHash<FlowNodeId, qint64> nodeCost;
    QElapsedTimer totalTimer;
    totalTimer.start();

    // ---------- 主执行循环 ----------
    while (FlowExecutionContext::running.load())
    {
        bool hasActiveNode = false;

        for (auto id : order)
        {
            auto* inst = map.value(id, nullptr);
            if (!inst || !inst->flowNode)
                continue;

            auto status = inst->flowNode->getStatus();
            if (status == FlowStatus::Timeout)
            {
                FlowExecutionContext::running.store(false);
                break;
            }

            if (status == FlowStatus::Done)
                continue;

            if (status == FlowStatus::Waiting ||
                status == FlowStatus::Running ||
                status == FlowStatus::Idle)
            {
                hasActiveNode = true;
            }

            QElapsedTimer nodeTimer;
            nodeTimer.start();

            inst->flowNode->compute();

            nodeCost[id] += nodeTimer.elapsed();

            if (inst->flowNode->getStatus() == FlowStatus::Done)
            {
                auto conns = outputMap.value(id);

                for (auto& conn : conns)
                {
                    auto* dst = map.value(conn.toNode, nullptr);
                    if (!dst || !dst->flowNode)
                        continue;

                    auto data =
                        inst->flowNode->getOutput(conn.fromPort);

                    dst->flowNode->setInput(conn.toPort, data);
                }

                emit nodeUpdated(id);
            }
        }

        if (!hasActiveNode)
            break;

        QThread::msleep(5);
    }
    LOG_INFO("[FlowWorker] Node Cost Summary:");
    for (auto id : order)
    {
        auto* inst = map.value(id, nullptr);
        if (!inst || !inst->flowNode)
            continue;
        QString outMsg = "[Node]"
                 + inst->model->name()
                 + "(id:" + QString::number(id) + ")"
                 + "total:" + QString::number(nodeCost[id]) + "ms";
        LOG_INFO(outMsg);
    }
    LOG_INFO(QString("[FlowWorker] Total Execution Time: %1 ms").arg(totalTimer.elapsed()));
    //Reset Status
    for (auto id : order)
    {
        auto* inst = map.value(id, nullptr);
        if (!inst || !inst->flowNode)
            continue;
        inst->flowNode->setStatus(FlowStatus::Idle);
    }
    emit finished();
}

QVector<FlowNodeId> FlowExecutorWorker::topologicalSort(FlowGraph &graph)
{
    QHash<FlowNodeId, int> indegree;

    for (auto& n : graph.nodes)
        indegree[n.id] = 0;

    for (auto& c : graph.connections)
        indegree[c.toNode]++;

    QQueue<FlowNodeId> q;

    for (auto it = indegree.begin();
         it != indegree.end(); ++it)
    {
        if (it.value() == 0)
            q.enqueue(it.key());
    }

    QVector<FlowNodeId> order;

    while (!q.isEmpty())
    {
        auto id = q.dequeue();
        order.push_back(id);

        for (auto next : graph.adjacency[id])
        {
            indegree[next]--;

            if (indegree[next] == 0)
                q.enqueue(next);
        }
    }

    return order;
}
