#ifndef FLOW_CONNECTION_H
#define FLOW_CONNECTION_H

#pragma once


#include "flow_types.h"


class FlowConnection
{
public:
    FlowConnection() = default;
    FlowConnection(const PortId& from, const PortId& to);


    const PortId& fromPort() const;
    const PortId& toPort() const;


private:
    PortId m_fromPortId;
    PortId m_toPortId;
};

#endif // FLOW_CONNECTION_H
