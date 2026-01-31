#include "flow_context.h"

// =======================================================
// core/flow_context.cpp
// =======================================================
#include "flow_context.h"


void FlowContext::setData(const PortId& portId, const QVariant& value)
{
    m_data.insert(portId, value);
}


QVariant FlowContext::data(const PortId& portId) const
{
    return m_data.value(portId);
}


bool FlowContext::hasData(const PortId& portId) const
{
    return m_data.contains(portId);
}


void FlowContext::clear()
{
    m_data.clear();
}
