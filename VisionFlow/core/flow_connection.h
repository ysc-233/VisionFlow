#ifndef FLOW_CONNECTION_H
#define FLOW_CONNECTION_H

#pragma once


#include "flow_types.h"


class FlowConnection
{
public:
    FlowConnection() = default;
    FlowConnection(const PortId& from, const PortId& to)
        : m_from(from), m_to(to)
    {}

    const PortId& fromPort() const { return m_from; }
    const PortId& toPort() const { return m_to; }

private:
    PortId m_from;
    PortId m_to;
};
#endif // FLOW_CONNECTION_H
