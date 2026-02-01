#ifndef FLOW_CONTEXT_H
#define FLOW_CONTEXT_H
#pragma once


#include "flow_types.h"


class FlowContext
{
public:
    FlowContext() = default;


    void setData(const PortId& portId, const QVariant& value);
    QVariant data(const PortId& portId) const;
    bool hasData(const PortId& portId) const;
    void clear();


private:
    QHash<PortId, QVariant> m_data;
};
#endif // FLOW_CONTEXT_H
