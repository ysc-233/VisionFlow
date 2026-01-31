#include "flow_connection.h"

// =======================================================
// core/flow_connection.cpp
// =======================================================
#include "flow_connection.h"


FlowConnection::FlowConnection(const PortId& from, const PortId& to)
    : m_fromPortId(from)
    , m_toPortId(to)
{
}


const PortId& FlowConnection::fromPort() const
{
    return m_fromPortId;
}


const PortId& FlowConnection::toPort() const
{
    return m_toPortId;
}
