#include "flow_node_factory.h"

#include "modules/module_registry.h"
//#include <Q_ASSERT>

FlowNode FlowNodeFactory::createNode(
    const NodeId& nodeId,
    const QString& moduleType)
{
    const ModuleDefinition* def = ModuleRegistry::instance().getDefinition(moduleType);
    Q_ASSERT(def);

    FlowNode node(nodeId, moduleType);

    for (const auto& p : def->ports)
    {
        node.addPort(FlowPort(
            /* portId */ PortId{ nodeId, p.name },
            p.direction,
            p.type,
            nodeId
        ));
    }

    for (auto it = def->defaultParameters.begin();
         it != def->defaultParameters.end(); ++it)
    {
        node.setParameter(it.key(), it.value());
    }

    return node;
}
