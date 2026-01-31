#include "flow_node.h"

// =======================================================


FlowNode::FlowNode(const NodeId &id, const QString &moduleType)
    : m_id(id)
    , m_moduleType(moduleType)
{

}

const NodeId &FlowNode::id() const
{
    return m_id;
}

const QString& FlowNode::moduleType() const
{
    return m_moduleType;
}


const QVector<FlowPort>& FlowNode::ports() const
{
    return m_ports;
}


QVector<FlowPort> FlowNode::inputPorts() const
{
    QVector<FlowPort> result;
    for (const auto& p : m_ports)
    {
        if (p.direction() == PortDirection::Input)
            result.push_back(p);
    }
    return result;
}


QVector<FlowPort> FlowNode::outputPorts() const
{
    QVector<FlowPort> result;
    for (const auto& p : m_ports)
    {
        if (p.direction() == PortDirection::Output)
            result.push_back(p);
    }
    return result;
}


void FlowNode::addPort(const FlowPort& port)
{
    m_ports.push_back(port);
}


QVariant FlowNode::parameter(const QString& key) const
{
    return m_parameters.value(key);
}


void FlowNode::setParameter(const QString& key, const QVariant& value)
{
    m_parameters.insert(key, value);
}


const QMap<QString, QVariant>& FlowNode::parameters() const
{
    return m_parameters;
}
