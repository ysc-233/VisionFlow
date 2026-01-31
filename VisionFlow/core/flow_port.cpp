#include "flow_port.h"

// =======================================================
// core/flow_port.cpp
// =======================================================
#include "flow_port.h"


FlowPort::FlowPort(const PortId& id,
                   PortDirection direction,
                   DataType type,
                   const NodeId& ownerNodeId)
    : m_id(id)
    , m_direction(direction)
    , m_type(type)
    , m_ownerNodeId(ownerNodeId)
{
}


const PortId& FlowPort::id() const
{
    return m_id;
}


PortDirection FlowPort::direction() const
{
    return m_direction;
}


DataType FlowPort::dataType() const
{
    return m_type;
}


const NodeId& FlowPort::ownerNodeId() const
{
    return m_ownerNodeId;
}

QString FlowPort::contextKey() const
{
    return m_ownerNodeId + "." + m_id;
}
