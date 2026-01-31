#ifndef MODULE_REGISTRY_H
#define MODULE_REGISTRY_H


// =======================================================
// modules/module_registry.h
// 模块注册表（工厂 + 元信息中心）
// =======================================================
#pragma once


#include <QMap>
#include <functional>


#include "module_definition.h"
#include "base_module.h"


class ModuleRegistry
{
public:
//    using Creator = std::function<BaseModule*()>;


    static ModuleRegistry& instance();


//    bool registerModule(const ModuleDefinition& def, Creator creator);
    using Creator = std::function<std::unique_ptr<BaseModule>()>;

    bool registerModule(const ModuleDefinition& def, Creator creator);
    std::unique_ptr<BaseModule> createModule(const QString& moduleType) const;


    const ModuleDefinition* getDefinition(const QString& moduleType) const;


    const QMap<QString, ModuleDefinition>& allDefinitions() const;


private:
    ModuleRegistry() = default;


    QMap<QString, ModuleDefinition> m_definitions;
    QMap<QString, Creator> m_creators;
};

#endif // MODULE_REGISTRY_H
