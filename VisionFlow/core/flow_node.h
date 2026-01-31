#ifndef FLOW_NODE_H
#define FLOW_NODE_H

#pragma once


#include "flow_port.h"


class FlowNode
{
public:
    FlowNode() = default;
    FlowNode(const NodeId& id, const QString& moduleType);


    const NodeId& id() const;
    const QString& moduleType() const;


    const QVector<FlowPort>& ports() const;
    QVector<FlowPort> inputPorts() const;
    QVector<FlowPort> outputPorts() const;


    void addPort(const FlowPort& port);


    QVariant parameter(const QString& key) const;
    void setParameter(const QString& key, const QVariant& value);
    const QMap<QString, QVariant>& parameters() const;


private:
    NodeId m_id;
    QString m_moduleType;
    QVector<FlowPort> m_ports;
    QMap<QString, QVariant> m_parameters;
};
#endif // FLOW_NODE_H
