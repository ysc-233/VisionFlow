#ifndef FLOWSCENE_H
#define FLOWSCENE_H
#pragma once

#include <QtNodes/BasicGraphicsScene>
#include <memory>

namespace QtNodes {
class NodeDelegateModelRegistry;
}

class FlowScene : public QtNodes::BasicGraphicsScene
{
    Q_OBJECT
public:
    FlowScene(
        QtNodes::AbstractGraphModel& model,
        std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry,
        QObject* parent = nullptr);

protected:
    QMenu* createSceneMenu(QPointF const scenePos) override;

private:
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> m_registry;
};
#endif // FLOWSCENE_H
