#include "flowrunner.h"
#include "flowexecutioncontext.h"

#include "flow/exporter/flowgraphexporter.h"
#include "flow/executor/flowexecutorworker.h"
#include "flow/executor/flowtimeoutcontroller.h"
#include "utils/logmanager.h"
#include <QElapsedTimer>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QApplication>
using namespace QtNodes;

FlowRunner* FlowRunner::m_instance = nullptr;

FlowRunner::FlowRunner(QObject* parent)
    : QObject(parent)
{
}

FlowRunner::~FlowRunner()
{
}

FlowRunner& FlowRunner::instance()
{
    if (!m_instance) {
        m_instance = new FlowRunner();
        // qAddPostRoutine([]() { delete m_instance; });
    }
    return *m_instance;
}

void FlowRunner::run(DataFlowGraphModel& model)
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    LOG_INFO("[FlowRunner] Flow start running!");

    FlowExecutionContext::running.store(true);

    QThread* thread = new QThread;
    auto* worker = new FlowExecutorWorker;

    worker->moveToThread(thread);

    const int globalTimeoutMs = 3000;
    FlowTimeoutController* timeoutController = new FlowTimeoutController(globalTimeoutMs);

    QObject::connect(timeoutController, &FlowTimeoutController::timeoutReached, []()
    {
        LOG_WARNING("[FlowRunner] Global timeout reached!");
        FlowExecutionContext::running.store(false);
    });
    QObject::connect(thread, &QThread::started,[worker, &model]()
    {
        FlowGraph graph = FlowGraphExporter::exportGraph(model);
        worker->run(graph);
    });
    QObject::connect(worker,&FlowExecutorWorker::finished,thread,&QThread::quit);
    QObject::connect(worker,&FlowExecutorWorker::finished,worker,&QObject::deleteLater);
    QObject::connect(thread,&QThread::finished,thread,&QObject::deleteLater);
    QObject::connect(worker, &FlowExecutorWorker::finished, qApp, [this,thread, worker, timeoutController]() {
            FlowExecutionContext::running.store(false);
            timeoutController->stop();
            timeoutController->deleteLater();
            thread->quit();
            LOG_INFO("[FlowRunner] Flow finished!");
            emit sig_runFinished();
        },
        Qt::QueuedConnection
    );

    thread->start();
    timeoutController->start();
}

void FlowRunner::stop()
{
    FlowExecutionContext::running.store(false);
}
