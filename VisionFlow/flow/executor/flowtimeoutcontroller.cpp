#include "flowtimeoutcontroller.h"
#include <QDebug>
#include "utils/logmanager.h"
FlowTimeoutController::FlowTimeoutController(int timeoutMs, QObject* parent)
    : QObject(parent)
    , m_timer(nullptr)
    , m_timeoutMs(timeoutMs)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setTimerType(Qt::PreciseTimer);

    connect(m_timer, &QTimer::timeout, this, [this]()
    {
        qDebug() << "[FlowTimeoutController] Timeout triggered after" << m_timeoutMs << "ms";
        LOG_DEBUG(QString("[FlowTimeoutController] Timeout triggered after: %1 ms").arg(m_timeoutMs));
        emit timeoutReached();
    });
}

FlowTimeoutController::~FlowTimeoutController()
{

}

void FlowTimeoutController::start()
{
    if (m_timer) {
        if (m_timer->isActive()) {
            m_timer->stop();
        }
        m_timer->start(m_timeoutMs);
        qDebug() << "[FlowTimeoutController] Timer started with timeout:" << m_timeoutMs << "ms";
        LOG_DEBUG(QString("[FlowTimeoutController] Timer started with timeout: %1 ms").arg(m_timeoutMs));
    }
}

void FlowTimeoutController::stop()
{
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        qDebug() << "[FlowTimeoutController] Timer stopped";
        LOG_DEBUG("[FlowTimeoutController] Timer stopped");
    }
}
