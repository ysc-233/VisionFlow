#include "flowrunner.h"
#include "flowexecutioncontext.h"

#include "flow/exporter/flowgraphexporter.h"
#include "flow/executor/flowexecutorworker.h"
#include "flow/executor/flowtimeoutcontroller.h"
#include <QElapsedTimer>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QApplication>
using namespace QtNodes;

void FlowRunner::run(DataFlowGraphModel& model)
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    qDebug() <<__FUNCTION__<< QThread::currentThreadId() <<"Start running!";

    FlowExecutionContext::running.store(true);

    QThread* thread = new QThread;
    auto* worker = new FlowExecutorWorker;

    worker->moveToThread(thread);

    const int globalTimeoutMs = 3000;
    FlowTimeoutController* timeoutController = new FlowTimeoutController(globalTimeoutMs);

    QObject::connect(timeoutController, &FlowTimeoutController::timeoutReached, [](){
        qDebug() << "[FlowRunner] Global timeout reached!";
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
    QObject::connect(worker, &FlowExecutorWorker::finished, qApp, [thread, worker, timeoutController]() {
            FlowExecutionContext::running.store(false);
            timeoutController->stop();
            timeoutController->deleteLater();
            thread->quit();
            qDebug() << "[FlowRunner] Flow finished!";
        },
        Qt::QueuedConnection
    );

    thread->start();
    timeoutController->start();
}

void FlowRunner::stop()
{
    qDebug()<<__FUNCTION__<< QThread::currentThreadId();
    FlowExecutionContext::running.store(false);
}
