#include "testdemo.h"
#include "core/flow_graph.h"
#include "core/flow_executor.h"
void registerTestModules();
testdemo::testdemo()
{
    registerTestModules();
    FlowGraph graph;
    // 构建 node / port / connection
    // ---------- ConstInt ----------
    FlowNode constNode("n1", "ConstInt");
    constNode.setParameter("value", 1);
    constNode.addPort(
        FlowPort(
            "out",
            PortDirection::Output,
            DataType::Any,
            constNode.id()
        )
    );
    graph.addNode(constNode);

    // ---------- AddOne ----------
    FlowNode addNode("n2", "AddOne");
    addNode.addPort(
        FlowPort(
            "in",
            PortDirection::Input,
            DataType::Any,
            addNode.id()
        )
    );
    addNode.addPort(
        FlowPort(
            "out",
            PortDirection::Output,
            DataType::Any,
            addNode.id()
        )
    );
    graph.addNode(addNode);

    // ---------- Print ----------
    FlowNode printNode("n3", "Print");
    printNode.addPort(
        FlowPort(
            "in",
            PortDirection::Input,
            DataType::Any,
            printNode.id()
        )
    );
    graph.addNode(printNode);

    // ---------- Connections ----------
    graph.addConnection(
        FlowConnection(
            "n1.out",
            "n2.in"
        )
    );

    graph.addConnection(
        FlowConnection(
            "n2.out",
            "n3.in"
        )
    );
    FlowExecutor executor(graph);

    bool ok = executor.run();
}
