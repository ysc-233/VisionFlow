#ifndef FLOW_TYPES_H
#define FLOW_TYPES_H

#pragma once


#include <QString>
#include <QVariant>
#include <QMap>
#include <QVector>


using NodeId = QString;
using PortId = QString;


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
