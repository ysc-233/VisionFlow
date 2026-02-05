#include "flowrunner.h"
#include "flowexecutioncontext.h"
#include <QtNodes/NodeDelegateModel>
#include <QElapsedTimer>
#include <QDebug>
using namespace QtNodes;

void FlowRunner::run(DataFlowGraphModel& model)
{
    FlowExecutionContext::running = true;

    QElapsedTimer totalTimer;
    totalTimer.start();

    QMap<QString, qint64> nodeCost;

    for (auto nodeId : model.allNodeIds())
    {
        NodeDelegateModel* node =
            model.delegateModel<NodeDelegateModel>(nodeId);

        if (!node)
            continue;

        QElapsedTimer timer;
        timer.start();

        // 触发节点执行
        Q_EMIT node->dataUpdated(0);

        qint64 cost = timer.elapsed();
        nodeCost[node->name()] += cost;
    }

    FlowExecutionContext::running = false;

    qDebug() << "===== Node Cost =====";
    for (auto it = nodeCost.begin(); it != nodeCost.end(); ++it)
        qDebug() << it.key() << ":" << it.value() << "ms";

    qDebug() << "Total:" << totalTimer.elapsed() << "ms";
}
