#ifndef FLOWRUNNER_H
#define FLOWRUNNER_H
#pragma once
#include <QtNodes/DataFlowGraphModel>
#include <QObject>

class FlowRunner : public QObject
{
    Q_OBJECT

public:
    FlowRunner(const FlowRunner&) = delete;
    FlowRunner& operator=(const FlowRunner&) = delete;

    static FlowRunner& instance();

    void run(QtNodes::DataFlowGraphModel& model);
    void stop();

signals:
    void sig_runFinished();

private:
    FlowRunner(QObject* parent = nullptr);
    ~FlowRunner();

    static FlowRunner* m_instance;
};

#endif // FLOWRUNNER_H
