#ifndef FLOW_TYPES_H
#define FLOW_TYPES_H

#pragma once


#include <QString>
#include <QVariant>
#include <QMap>
#include <QVector>


using NodeId = QString;
struct PortId
{
    NodeId node;
    QString port;
    // 添加构造函数
   PortId() = default;
   PortId(const NodeId& node_, const QString& port_)
       : node(node_), port(port_) {}

    bool operator==(const PortId& other) const
    {
        return node == other.node && port == other.port;
    }

    bool isValid() const
    {
        return !node.isEmpty() && !port.isEmpty();
    }

    QString toString() const
    {
        return node + "." + port;
    }
};
inline uint qHash(const PortId& id, uint seed = 0)
{
    return qHash(id.node, seed) ^ qHash(id.port, seed);
}

enum class PortDirection
{
    Input,
    Output
};


enum class DataType
{
    Any,
    Image,
    MetaData
};

#endif // FLOW_TYPES_H
