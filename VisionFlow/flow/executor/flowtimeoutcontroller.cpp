#include "flowtimeoutcontroller.h"
#include <QDebug>

FlowTimeoutController::FlowTimeoutController(int timeoutMs, QObject* parent)
    : QObject(parent)
    , m_timer(nullptr)
    , m_timeoutMs(timeoutMs)
{
    // 创建定时器对象
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);  // 单次触发
    m_timer->setTimerType(Qt::PreciseTimer);  // 精确计时

    // 连接定时器超时信号
    connect(m_timer, &QTimer::timeout, this, [this]() {
        qDebug() << "[FlowTimeoutController] Timeout triggered after" << m_timeoutMs << "ms";
        emit timeoutReached();
    });
}

FlowTimeoutController::~FlowTimeoutController()
{
    // QTimer 是 QObject，由父对象自动管理，不需要手动删除
}

void FlowTimeoutController::start()
{
    if (m_timer) {
        if (m_timer->isActive()) {
            m_timer->stop();
        }
        m_timer->start(m_timeoutMs);
        qDebug() << "[FlowTimeoutController] Timer started with timeout:" << m_timeoutMs << "ms";
    }
}

void FlowTimeoutController::stop()
{
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        qDebug() << "[FlowTimeoutController] Timer stopped";
    }
}
