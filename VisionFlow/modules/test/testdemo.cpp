#include "testdemo.h"
#include "core/flow_graph.h"
#include "core/flow_executor.h"
#include "core/flow_node_factory.h"
#include <QtDebug>
void registerTestModules();
testdemo::testdemo()
{
    registerTestModules();

    FlowGraph graph;

    graph.addNode(FlowNodeFactory::createNode("n1", "ConstInt"));
    graph.addNode(FlowNodeFactory::createNode("n2", "AddOne"));
    graph.addNode(FlowNodeFactory::createNode("n3", "Print"));

    graph.addConnection(
        FlowConnection(PortId{"n1","out"}, PortId{"n2","in"}));

    graph.addConnection(
        FlowConnection(PortId{"n2","out"}, PortId{"n3","in"}));

    qDebug() << "[MAIN] connection count =" << graph.connections().size();
    // 必须是 2

    FlowExecutor executor(graph);
    bool ok = executor.run();
    Q_ASSERT(ok);
}
