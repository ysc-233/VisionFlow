#include "flowexecutorworker.h"
#include <QHash>
#include <QQueue>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include "flow/executor/flowexecutioncontext.h"

void FlowExecutorWorker::run(FlowGraph graph)
{
    qDebug() << __FUNCTION__ << QThread::currentThreadId() <<
                "Running!==================================";

    auto order = topologicalSort(graph);
    qDebug() << "Execution order:" << order;

    // 建立节点 id -> 节点实例映射
    QHash<FlowNodeId, FlowNodeInstance*> map;
    for (auto& n : graph.nodes)
        map[n.id] = &n;

    QHash<FlowNodeId, qint64> nodeCost;
    QElapsedTimer totalTimer;
    totalTimer.start();

    for (auto id : order)
    {
        if (!FlowExecutionContext::running.load())
            break; // 全局停止

        auto* inst = map.value(id, nullptr);
        if (!inst || !inst->flowNode)
            continue;

        QElapsedTimer nodeTimer;
        nodeTimer.start();

        // --------- 1️⃣ compute ----------
        inst->flowNode->compute();  // compute 内部应轮询 FlowExecutionContext::running

        qint64 computeMs = nodeTimer.elapsed();

        if (!FlowExecutionContext::running.load())
            break; // compute 内部可能触发停止

        // --------- 2️⃣ propagate ----------
        QElapsedTimer propTimer;
        propTimer.start();

        for (auto& conn : graph.connections)
        {
            if (conn.fromNode != id)
                continue;

            auto* dst = map.value(conn.toNode, nullptr);
            if (!dst || !dst->flowNode)
                continue;

            auto data = inst->flowNode->getOutput(conn.fromPort);
            dst->flowNode->setInput(conn.toPort, data);
        }

        qint64 propagateMs = propTimer.elapsed();
        qint64 totalMs = nodeTimer.elapsed();
        nodeCost[id] = totalMs;

        qDebug() << "[Node]" << inst->flowNode->flowNodeName
                 << "(id:" << id << ")"
                 << "compute:" << computeMs << "ms"
                 << "propagate:" << propagateMs << "ms"
                 << "total:" << totalMs << "ms";
    }

    qDebug() << "===== Node Cost Summary =====";
    for (auto id : order)
    {
        auto* inst = map.value(id, nullptr);
        if (!inst || !inst->flowNode)
            continue;

        QString nodeName = inst->flowNode->flowNodeName;
        qDebug() << "[Node]" << nodeName
                 << "(id:" << id << ")"
                 << "total:" << nodeCost[id] << "ms";
    }

    qDebug() << "===== Total Execution Time ====="
             << totalTimer.elapsed() << "ms";

    qDebug() << __FUNCTION__ << QThread::currentThread() <<
                "Finished!==================================";

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
