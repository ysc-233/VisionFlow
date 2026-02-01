#ifndef MODULE_PORT_BINDING_H
#define MODULE_PORT_BINDING_H
#pragma once

#include <QMap>
#include <QString>
#include "flow_types.h"
/**
 * @brief ModulePortBinding
 *
 * 语义端口名 → FlowContext Key 的映射
 *
 * 例如：
 *  inputs["in"]  = "n1.out"
 *  outputs["out"] = "n2.out"
 */
struct ModulePortBinding
{
    QMap<QString, PortId> inputs;
    QMap<QString, PortId> outputs;
};

#endif // MODULE_PORT_BINDING_H
