#ifndef FLOW_PORT_H
#define FLOW_PORT_H
#pragma once


#include "flow_types.h"


class FlowPort
{
public:
    FlowPort() = default;
    FlowPort(const PortId& id,
             PortDirection direction,
             DataType type,
             const NodeId& ownerNodeId);

    const PortId& id() const;
    PortDirection direction() const;
    DataType dataType() const;
    const NodeId& ownerNodeId() const;
    QString contextKey() const;

private:
    PortId m_id;
    PortDirection m_direction { PortDirection::Input };
    DataType m_type { DataType::Any };
    NodeId m_ownerNodeId;
};

#endif // FLOW_PORT_H
