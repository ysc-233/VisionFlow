#include "module_registry.h"

// =======================================================
// modules/module_registry.cpp
// =======================================================


ModuleRegistry& ModuleRegistry::instance()
{
    static ModuleRegistry inst;
    return inst;
}


bool ModuleRegistry::registerModule(const ModuleDefinition& def, Creator creator)
{
    if (m_definitions.contains(def.moduleType))
        return false;


    m_definitions.insert(def.moduleType, def);
    m_creators.insert(def.moduleType, creator);
    return true;
}


std::unique_ptr<BaseModule> ModuleRegistry::createModule(const QString& moduleType) const
{
    if (!m_creators.contains(moduleType))
        return nullptr;

    return m_creators.value(moduleType)();
}


const ModuleDefinition* ModuleRegistry::getDefinition(const QString& moduleType) const
{
    auto it = m_definitions.find(moduleType);
    if (it == m_definitions.end())
        return nullptr;


    return &it.value();
}


const QMap<QString, ModuleDefinition>& ModuleRegistry::allDefinitions() const
{
    return m_definitions;
}
