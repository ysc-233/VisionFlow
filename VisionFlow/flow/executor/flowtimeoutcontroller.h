#ifndef FLOWTIMEOUTCONTROLLER_H
#define FLOWTIMEOUTCONTROLLER_H
#include <QObject>
#include <QTimer>

class FlowTimeoutController : public QObject
{
    Q_OBJECT

public:
    explicit FlowTimeoutController(int timeoutMs, QObject* parent = nullptr);
    ~FlowTimeoutController() override;

public slots:
    void start();
    void stop();

signals:
    void timeoutReached();

private:
    QTimer* m_timer;
    int m_timeoutMs;
};


#endif // FLOWTIMEOUTCONTROLLER_H
