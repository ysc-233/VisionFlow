#ifndef MODULE_PORT_BINDING_H
#define MODULE_PORT_BINDING_H
// =======================================================
// core/module_port_binding.h
// =======================================================
#pragma once


#include <QMap>
#include <QString>
#include "flow_types.h" // PortId


struct ModulePortBinding
{
    // 语义端口名 -> Flow PortId
    QMap<QString, PortId> inputs;
    QMap<QString, PortId> outputs;
};

#endif // MODULE_PORT_BINDING_H
