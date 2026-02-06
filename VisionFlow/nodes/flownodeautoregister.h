#ifndef FLOWNODEAUTOREGISTER_H
#define FLOWNODEAUTOREGISTER_H
#pragma once
#include <QtNodes/NodeDelegateModelRegistry>
#include <memory>
#include <vector>
#include <functional>
#include <QThread>
class NodeAutoRegistry
{
public:
    using RegisterFunc =
        std::function<void(QtNodes::NodeDelegateModelRegistry&)>;

    static NodeAutoRegistry& instance()
    {
        static NodeAutoRegistry inst;
        return inst;
    }

    void add(RegisterFunc f)
    {
        _funcs.push_back(std::move(f));
    }

    void registerAll(QtNodes::NodeDelegateModelRegistry& reg)
    {
        for (auto& f : _funcs)
            f(reg);
    }

private:
    std::vector<RegisterFunc> _funcs;
};

template<typename T>
class NodeRegistrar
{
public:
    NodeRegistrar(QString category)
    {
        qDebug() << __FUNCTION__<< QThread::currentThreadId() << typeid(T).name();
        NodeAutoRegistry::instance().add(
            [category](QtNodes::NodeDelegateModelRegistry& reg)
            {
                reg.registerModel<T>(category);
            });
    }
};

#define REGISTER_NODE(ModelType, Category) \
    static NodeRegistrar<ModelType> \
        _registrar_##ModelType(Category);

#endif // FLOWNODEAUTOREGISTER_H
