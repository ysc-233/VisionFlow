#ifndef FLOW_GRAPH_MODEL_H
#define FLOW_GRAPH_MODEL_H
#include <QAbstractItemModel>
#include "core/flow_graph.h"

class FlowGraphModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit FlowGraphModel(FlowGraph* graph = nullptr, QObject *parent = nullptr);
    ~FlowGraphModel();

    // QAbstractItemModel 接口
    QModelIndex index(int row, int column,
                     const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const override;

    // 自定义角色
    enum CustomRoles {
        NodeIdRole = Qt::UserRole + 1,
        ModuleTypeRole,
        NodeObjectRole,
        PortObjectRole,
        ConnectionObjectRole
    };

    // 图操作
    void setFlowGraph(FlowGraph* graph);
    FlowGraph* flowGraph() const { return m_flowGraph; }

    // 节点操作
    void addNode(const FlowNode& node);
    void removeNode(const NodeId& nodeId);
    void updateNode(const FlowNode& node);
    QModelIndex nodeIndex(const NodeId& nodeId) const;

    // 连接操作
    void addConnection(const FlowConnection& connection);
    void removeConnection(const FlowConnection& connection);
    QModelIndex connectionIndex(const FlowConnection& connection) const;

    // 端口操作
    QModelIndex portIndex(const PortId& portId) const;

signals:
    void modelReset();
    void nodeAdded(const QModelIndex& index);
    void nodeRemoved(const NodeId& nodeId);
    void connectionAdded(const QModelIndex& index);
    void connectionRemoved(const FlowConnection& conn);

private:
    void setupModel();
    void clearModel();

private:
    FlowGraph* m_flowGraph;

    struct TreeNode {
        enum Type { Root, Node, Port, Connection };
        Type type;
        QVariant data;
        QList<TreeNode*> children;
        TreeNode* parent;
    };

    TreeNode* m_rootNode;
    QMap<NodeId, TreeNode*> m_nodeMap;
    QMap<FlowConnection, TreeNode*> m_connectionMap;
    QMap<PortId, TreeNode*> m_portMap;
};

#endif // FLOW_GRAPH_MODEL_H
