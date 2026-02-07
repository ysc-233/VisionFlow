#ifndef FLOWEXECUTORWORKER_H
#define FLOWEXECUTORWORKER_H
#pragma once
#include <QObject>
#include "flow/graph/flowgraph.h"

class FlowExecutorWorker : public QObject
{
    Q_OBJECT

public slots:
    void run(FlowGraph &graph);

signals:
    void finished();
    void nodeUpdated(int nodeId);

private:
    static QVector<FlowNodeId> topologicalSort(FlowGraph& graph);
};

#endif // FLOWEXECUTORWORKER_H
