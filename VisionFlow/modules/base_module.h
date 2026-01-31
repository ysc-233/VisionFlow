#ifndef BASE_MODULE_H
#define BASE_MODULE_H
// =======================================================
// modules/base_module.h
// =======================================================
#pragma once


#include <QMap>
#include <QString>
#include <QVariant>
#include "core/module_port_binding.h"
#include "core/flow_context.h"


class BaseModule
{
public:
    virtual ~BaseModule() = default;


    // 由 Executor 在执行前统一注入
    void bind(const ModulePortBinding& binding,
              const QMap<QString, QVariant>& parameters,
              FlowContext* context)
    {
        m_binding = binding;
        m_parameters = parameters;
        m_context = context;
    }


    // 模块执行入口
    virtual bool execute() = 0;


protected:
    // 读取输入（通过语义名）
    QVariant input(const QString& name) const
    {
        if (!m_context || !m_binding.inputs.contains(name))
            return {};
        return m_context->data(m_binding.inputs.value(name));
    }


    // 写入输出（通过语义名）
    void setOutput(const QString& name, const QVariant& value)
    {
        if (!m_context || !m_binding.outputs.contains(name))
            return;
        m_context->setData(m_binding.outputs.value(name), value);
    }


    // 读取参数
    QVariant parameter(const QString& key) const
    {
        return m_parameters.value(key);
    }


private:
    ModulePortBinding m_binding;
    QMap<QString, QVariant> m_parameters;
    FlowContext* m_context = nullptr;
};

#endif // BASE_MODULE_H
