#ifndef NODELIBRARYREGISTRY_H
#define NODELIBRARYREGISTRY_H
#pragma once
#include <QString>
#include <QList>
#include <functional>
#include <memory>
#include "flow/graph/flownode.h"

struct LibraryNodeInfo
{
    QString name;
    QString category;
    std::function<std::unique_ptr<FlowNode>()> creator;
};

class NodeLibraryRegistry
{
public:
    static NodeLibraryRegistry& instance()
    {
        static NodeLibraryRegistry inst;
        return inst;
    }

    void addNode(const LibraryNodeInfo& info)
    {
        m_nodes.push_back(info);
    }

    const QList<LibraryNodeInfo>& nodes() const
    {
        return m_nodes;
    }

private:
    QList<LibraryNodeInfo> m_nodes;
};
#endif // NODELIBRARYREGISTRY_H
