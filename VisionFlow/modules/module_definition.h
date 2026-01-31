#ifndef MODULE_DEFINITION_H
#define MODULE_DEFINITION_H

// =======================================================
// modules/module_definition.h
// 模块的“静态描述信息”（供 UI / Core 使用）
// =======================================================
#pragma once


#include <QString>
#include <QVector>
#include <QMap>
#include <QVariant>

#include "core/flow_types.h"


struct ModulePortDef
{
    QString name;
    PortDirection direction;
    DataType type;
};


struct ModuleDefinition
{
    QString moduleType; // 唯一标识（如 ImageLoad）
    QString displayName; // UI 显示名称

    QVector<ModulePortDef> ports;
    QMap<QString, QVariant> defaultParameters;
};
#endif // MODULE_DEFINITION_H
