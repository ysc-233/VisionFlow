#include "flow_context.h"

// =======================================================
// core/flow_context.cpp
// =======================================================
#include "flow_context.h"
#include <QtDebug>
void FlowContext::setData(const PortId& portId, const QVariant& value)
{
    // Debug 防御：PortId 必须完整
    Q_ASSERT(portId.isValid());

    if (m_data.contains(portId))
    {
        qWarning() << "[FlowContext] overwrite data at"
                   << portId.toString()
                   << "old =" << m_data.value(portId)
                   << "new =" << value;
    }

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
